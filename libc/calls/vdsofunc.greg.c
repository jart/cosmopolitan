/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
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
#include "libc/calls/calls.h"
#include "libc/dce.h"
#include "libc/elf/def.h"
#include "libc/elf/scalar.h"
#include "libc/elf/struct/ehdr.h"
#include "libc/elf/struct/phdr.h"
#include "libc/elf/struct/shdr.h"
#include "libc/elf/struct/sym.h"
#include "libc/elf/struct/verdaux.h"
#include "libc/elf/struct/verdef.h"
#include "libc/intrin/getauxval.internal.h"
#include "libc/intrin/strace.internal.h"
#include "libc/runtime/runtime.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/auxv.h"

// needed to avoid asan restrictions on strcmp
static int StrCmp(const char *l, const char *r) {
  size_t i = 0;
  while (l[i] == r[i] && r[i]) ++i;
  return (l[i] & 255) - (r[i] & 255);
}

static inline int CheckDsoSymbolVersion(Elf64_Verdef *vd, int sym,
                                        const char *name, char *strtab) {
  Elf64_Verdaux *aux;
  for (;; vd = (Elf64_Verdef *)((char *)vd + vd->vd_next)) {
    if (!(vd->vd_flags & VER_FLG_BASE) &&
        (vd->vd_ndx & 0x7fff) == (sym & 0x7fff)) {
      aux = (Elf64_Verdaux *)((char *)vd + vd->vd_aux);
      return !StrCmp(name, strtab + aux->vda_name);
    }
    if (!vd->vd_next) {
      return 0;
    }
  }
}

/**
 * Returns address of "Virtual Dynamic Shared Object" function on Linux.
 */
void *__vdsosym(const char *version, const char *name) {
  void *p;
  size_t i;
  Elf64_Ehdr *ehdr;
  Elf64_Phdr *phdr;
  char *strtab = 0;
  size_t *dyn, base;
  Elf64_Sym *symtab = 0;
  uint16_t *versym = 0;
  Elf_Symndx *hashtab = 0;
  Elf64_Verdef *verdef = 0;
  struct AuxiliaryValue av;

  av = __getauxval(AT_SYSINFO_EHDR);
  if (!av.isfound) {
    KERNTRACE("__vdsosym() → missing AT_SYSINFO_EHDR");
    return 0;
  }

  ehdr = (void *)av.value;
  phdr = (void *)((char *)ehdr + ehdr->e_phoff);
  for (base = -1, dyn = 0, i = 0; i < ehdr->e_phnum;
       i++, phdr = (void *)((char *)phdr + ehdr->e_phentsize)) {
    switch (phdr->p_type) {
      case PT_LOAD:
        // modern linux uses the base address zero, but elders
        // e.g. rhel7 uses the base address 0xffffffffff700000
        base = (size_t)ehdr + phdr->p_offset - phdr->p_vaddr;
        break;
      case PT_DYNAMIC:
        dyn = (void *)((char *)ehdr + phdr->p_offset);
        break;
      default:
        break;
    }
  }
  if (!dyn || base == -1) {
    KERNTRACE("__vdsosym() → missing program headers");
    return 0;
  }

  for (i = 0; dyn[i]; i += 2) {
    p = (void *)(base + dyn[i + 1]);
    switch (dyn[i]) {
      case DT_STRTAB:
        strtab = p;
        break;
      case DT_SYMTAB:
        symtab = p;
        break;
      case DT_HASH:
        hashtab = p;
        break;
      case DT_VERSYM:
        versym = p;
        break;
      case DT_VERDEF:
        verdef = p;
        break;
    }
  }
  if (!strtab || !symtab || !hashtab) {
    KERNTRACE("__vdsosym() → tables not found");
    return 0;
  }
  if (!verdef) {
    versym = 0;
  }

  for (i = 0; i < hashtab[1]; i++) {
    if (ELF64_ST_TYPE(symtab[i].st_info) != STT_FUNC &&
        ELF64_ST_TYPE(symtab[i].st_info) != STT_OBJECT &&
        ELF64_ST_TYPE(symtab[i].st_info) != STT_NOTYPE) {
      continue;
    }
    if (ELF64_ST_BIND(symtab[i].st_info) != STB_GLOBAL) {
      continue;
    }
    if (!symtab[i].st_shndx) {
      continue;
    }
    if (StrCmp(name, strtab + symtab[i].st_name)) {
      continue;
    }
    if (versym && !CheckDsoSymbolVersion(verdef, versym[i], version, strtab)) {
      continue;
    }
    return (void *)(base + symtab[i].st_value);
  }

  KERNTRACE("__vdsosym() → symbol not found");
  return 0;
}
