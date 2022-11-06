/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2020 Justine Alexandra Roberts Tunney                              │
│                                                                              │
│ Permission to use, copy, modify, and/or distribute this software for         │
│ any purpose with or without fee is hereby granted, provided that the         │
│ above copyright notice and this permission notice appear in all copies.      │
│                                                                              │
│ THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL                │
│ WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED                │
│ WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE             │
│ AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL         │
│ DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR        │
│ PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER               │
│ TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR             │
│ PERFORMANCE OF THIS SOFTWARE.                                                │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "libc/assert.h"
#include "libc/calls/blockcancel.internal.h"
#include "libc/calls/calls.h"
#include "libc/dce.h"
#include "libc/elf/def.h"
#include "libc/elf/scalar.h"
#include "libc/elf/struct/phdr.h"
#include "libc/elf/struct/shdr.h"
#include "libc/elf/struct/sym.h"
#include "libc/errno.h"
#include "libc/intrin/bits.h"
#include "libc/intrin/strace.internal.h"
#include "libc/limits.h"
#include "libc/log/libfatal.internal.h"
#include "libc/macros.internal.h"
#include "libc/mem/alg.h"
#include "libc/runtime/internal.h"
#include "libc/runtime/runtime.h"
#include "libc/runtime/symbols.internal.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/prot.h"
#include "libc/sysv/errfuns.h"

#define GetStr(tab, rva)     ((char *)(tab) + (rva))
#define GetSection(e, s)     ((void *)((intptr_t)(e) + (size_t)(s)->sh_offset))
#define GetShstrtab(e)       GetSection(e, GetShdr(e, (e)->e_shstrndx))
#define GetSectionName(e, s) GetStr(GetShstrtab(e), (s)->sh_name)
#define GetPhdr(e, i)                            \
  ((Elf64_Phdr *)((intptr_t)(e) + (e)->e_phoff + \
                  (size_t)(e)->e_phentsize * (i)))
#define GetShdr(e, i)                            \
  ((Elf64_Shdr *)((intptr_t)(e) + (e)->e_shoff + \
                  (size_t)(e)->e_shentsize * (i)))

static char *GetStrtab(Elf64_Ehdr *e, size_t *n) {
  char *name;
  Elf64_Half i;
  Elf64_Shdr *shdr;
  for (i = 0; i < e->e_shnum; ++i) {
    shdr = GetShdr(e, i);
    if (shdr->sh_type == SHT_STRTAB) {
      name = GetSectionName(e, GetShdr(e, i));
      if (name && !__strcmp(name, ".strtab")) {
        if (n) *n = shdr->sh_size;
        return GetSection(e, shdr);
      }
    }
  }
  return 0;
}

static Elf64_Sym *GetSymtab(Elf64_Ehdr *e, Elf64_Xword *n) {
  Elf64_Half i;
  Elf64_Shdr *shdr;
  for (i = e->e_shnum; i > 0; --i) {
    shdr = GetShdr(e, i - 1);
    if (shdr->sh_type == SHT_SYMTAB) {
      if (shdr->sh_entsize != sizeof(Elf64_Sym)) continue;
      if (n) *n = shdr->sh_size / shdr->sh_entsize;
      return GetSection(e, shdr);
    }
  }
  return 0;
}

static void GetImageRange(Elf64_Ehdr *elf, intptr_t *x, intptr_t *y) {
  unsigned i;
  Elf64_Phdr *phdr;
  intptr_t start, end, pstart, pend;
  start = INTPTR_MAX;
  end = 0;
  for (i = 0; i < elf->e_phnum; ++i) {
    phdr = GetPhdr(elf, i);
    if (phdr->p_type != PT_LOAD) continue;
    pstart = phdr->p_vaddr;
    pend = phdr->p_vaddr + phdr->p_memsz;
    if (pstart < start) start = pstart;
    if (pend > end) end = pend;
  }
  if (x) *x = start;
  if (y) *y = end;
}

static struct SymbolTable *OpenSymbolTableImpl(const char *filename) {
  int fd;
  void *map;
  long *stp;
  ssize_t filesize;
  unsigned i, j, x;
  const Elf64_Ehdr *elf;
  const char *name_base;
  struct SymbolTable *t;
  size_t n, m, tsz, size;
  const Elf64_Sym *symtab, *sym;
  ptrdiff_t names_offset, name_base_offset, stp_offset;
  map = MAP_FAILED;
  if ((fd = open(filename, O_RDONLY)) == -1) return 0;
  if ((filesize = getfiledescriptorsize(fd)) == -1) goto SystemError;
  if (filesize > INT_MAX) goto RaiseE2big;
  if (filesize < 64) goto RaiseEnoexec;
  elf = map = mmap(0, filesize, PROT_READ, MAP_PRIVATE, fd, 0);
  if (map == MAP_FAILED) goto SystemError;
  if (READ32LE(map) != READ32LE("\177ELF")) goto RaiseEnoexec;
  if (!(name_base = GetStrtab(map, &m))) goto RaiseEnobufs;
  if (!(symtab = GetSymtab(map, &n))) goto RaiseEnobufs;
  tsz = 0;
  tsz += sizeof(struct SymbolTable);
  tsz += sizeof(struct Symbol) * n;
  names_offset = tsz;
  tsz += sizeof(unsigned) * n;
  name_base_offset = tsz;
  tsz += m;
  tsz = ROUNDUP(tsz, FRAMESIZE);
  stp_offset = tsz;
  size = tsz;
  tsz += sizeof(const Elf64_Sym *) * n;
  tsz = ROUNDUP(tsz, FRAMESIZE);
  t = mmap(0, tsz, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
  if (t == MAP_FAILED) goto SystemError;
  t->magic = SYMBOLS_MAGIC;
  t->abi = SYMBOLS_ABI;
  t->size = size;
  t->mapsize = size;
  t->names_offset = names_offset;
  t->name_base_offset = name_base_offset;
  t->names = (uint32_t *)((char *)t + t->names_offset);
  t->name_base = (char *)((char *)t + t->name_base_offset);
  GetImageRange(elf, &t->addr_base, &t->addr_end);
  memcpy(t->name_base, name_base, m);
  --t->addr_end;
  stp = (long *)((char *)t + stp_offset);
  for (m = i = 0; i < n; ++i) {
    sym = symtab + i;
    if (!(sym->st_size > 0 && (ELF64_ST_TYPE(sym->st_info) == STT_FUNC ||
                               ELF64_ST_TYPE(sym->st_info) == STT_OBJECT))) {
      continue;
    }
    if (sym->st_value > t->addr_end) continue;
    if (sym->st_value < t->addr_base) continue;
    x = sym->st_value - t->addr_base;
    stp[m++] = (unsigned long)x << 32 | i;
  }
  _longsort(stp, m);
  for (j = i = 0; i < m; ++i) {
    sym = symtab + (stp[i] & 0x7fffffff);
    x = stp[i] >> 32;
    if (j && x == t->symbols[j - 1].x) --j;
    if (j && t->symbols[j - 1].y >= x) t->symbols[j - 1].y = x - 1;
    t->names[j] = sym->st_name;
    t->symbols[j].x = x;
    if (sym->st_size) {
      t->symbols[j].y = x + sym->st_size - 1;
    } else {
      t->symbols[j].y = t->addr_end - t->addr_base;
    }
    ++j;
  }
  t->count = j;
  munmap(stp, ROUNDUP(sizeof(const Elf64_Sym *) * n, FRAMESIZE));
  munmap(map, filesize);
  close(fd);
  return t;
RaiseE2big:
  errno = E2BIG;
  goto SystemError;
RaiseEnobufs:
  errno = ENOBUFS;
  goto SystemError;
RaiseEnoexec:
  errno = ENOEXEC;
SystemError:
  STRACE("OpenSymbolTable()% m");
  if (map != MAP_FAILED) {
    munmap(map, filesize);
  }
  close(fd);
  return 0;
}

/**
 * Maps debuggable binary into memory and indexes symbol addresses.
 *
 * @return object freeable with CloseSymbolTable(), or NULL w/ errno
 */
struct SymbolTable *OpenSymbolTable(const char *filename) {
  struct SymbolTable *st;
  BLOCK_CANCELLATIONS;
  st = OpenSymbolTableImpl(filename);
  ALLOW_CANCELLATIONS;
  return st;
}
