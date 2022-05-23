/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2022 Justine Alexandra Roberts Tunney                              │
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
#include "libc/bits/bits.h"
#include "libc/calls/calls.h"
#include "libc/elf/scalar.h"
#include "libc/elf/struct/ehdr.h"
#include "libc/elf/struct/shdr.h"
#include "libc/elf/struct/sym.h"
#include "libc/log/libfatal.internal.h"
#include "libc/runtime/runtime.h"
#include "libc/sysv/consts/auxv.h"

#define LAZY_RHEL7_RELOCATION 0xfffff

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

static char *GetDynamicStringTable(Elf64_Ehdr *e, size_t *n) {
  char *name;
  Elf64_Half i;
  Elf64_Shdr *shdr;
  for (i = 0; i < e->e_shnum; ++i) {
    shdr = GetShdr(e, i);
    name = GetSectionName(e, GetShdr(e, i));
    if (shdr->sh_type == SHT_STRTAB) {
      name = GetSectionName(e, GetShdr(e, i));
      if (name && READ64LE(name) == READ64LE(".dynstr")) {
        if (n) *n = shdr->sh_size;
        return GetSection(e, shdr);
      }
    }
  }
  return 0;
}

static Elf64_Sym *GetDynamicSymbolTable(Elf64_Ehdr *e, Elf64_Xword *n) {
  Elf64_Half i;
  Elf64_Shdr *shdr;
  for (i = e->e_shnum; i > 0; --i) {
    shdr = GetShdr(e, i - 1);
    if (shdr->sh_type == SHT_DYNSYM) {
      if (shdr->sh_entsize != sizeof(Elf64_Sym)) continue;
      if (n) *n = shdr->sh_size / shdr->sh_entsize;
      return GetSection(e, shdr);
    }
  }
  return 0;
}

/**
 * Returns Linux Kernel Virtual Dynamic Shared Object function address.
 */
void *__vdsofunc(const char *name) {
  size_t m;
  char *names;
  Elf64_Ehdr *ehdr;
  Elf64_Xword i, n;
  Elf64_Sym *symtab, *sym;
  if ((ehdr = (Elf64_Ehdr *)getauxval(AT_SYSINFO_EHDR)) &&
      (names = GetDynamicStringTable(ehdr, &m)) &&
      (symtab = GetDynamicSymbolTable(ehdr, &n))) {
    for (i = 0; i < n; ++i) {
      if (!__strcmp(names + symtab[i].st_name, name)) {
        return (char *)ehdr + (symtab[i].st_value & LAZY_RHEL7_RELOCATION);
      }
    }
  }
  return 0;
}
