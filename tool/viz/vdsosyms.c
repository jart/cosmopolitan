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
#include "libc/intrin/strace.internal.h"
#include "libc/elf/def.h"
#include "libc/elf/scalar.h"
#include "libc/elf/struct/ehdr.h"
#include "libc/elf/struct/phdr.h"
#include "libc/elf/struct/sym.h"
#include "libc/elf/struct/verdaux.h"
#include "libc/elf/struct/verdef.h"
#include "libc/intrin/kprintf.h"
#include "libc/runtime/runtime.h"
#include "libc/sysv/consts/auxv.h"

static inline void PrintDsoSymbolVersions(Elf64_Verdef *vd, int sym,
                                          char *strtab) {
  Elf64_Verdaux *aux;
  for (;; vd = (Elf64_Verdef *)((char *)vd + vd->vd_next)) {
    if (!(vd->vd_flags & VER_FLG_BASE) &&
        (vd->vd_ndx & 0x7fff) == (sym & 0x7fff)) {
      aux = (Elf64_Verdaux *)((char *)vd + vd->vd_aux);
      kprintf(" %s", strtab + aux->vda_name);
    }
    if (!vd->vd_next) {
      break;
    }
  }
}

int PrintVdsoSymbols(void) {
  void *p;
  size_t i;
  Elf64_Ehdr *ehdr;
  Elf64_Phdr *phdr;
  char *strtab = 0;
  size_t *dyn, base;
  unsigned long *ap;
  Elf64_Sym *symtab = 0;
  uint16_t *versym = 0;
  Elf_Symndx *hashtab = 0;
  Elf64_Verdef *verdef = 0;
  const char *typename, *bindname;

  for (ehdr = 0, ap = __auxv; ap[0]; ap += 2) {
    if (ap[0] == AT_SYSINFO_EHDR) {
      ehdr = (void *)ap[1];
      break;
    }
  }
  if (!ehdr) {
    kprintf("error: AT_SYSINFO_EHDR not found\n");
    return 1;
  }

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
    kprintf("error: missing program headers\n");
    return 2;
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
  if (!verdef) {
    versym = 0;
  }

  if (!strtab || !symtab || !hashtab) {
    kprintf("error: strtab/symtab/hashtab not found\n");
    return 3;
  }

  for (i = 0; i < hashtab[1]; i++) {
    if (!symtab[i].st_shndx) {
      continue;
    }

    switch (ELF64_ST_BIND(symtab[i].st_info)) {
      case STB_LOCAL:
        bindname = "locl";
        break;
      case STB_GLOBAL:
        bindname = "glob";
        break;
      case STB_WEAK:
        bindname = "weak";
        break;
      default:
        bindname = "????";
        break;
    }

    switch (ELF64_ST_TYPE(symtab[i].st_info)) {
      case STT_FUNC:
        typename = "func";
        break;
      case STT_OBJECT:
        typename = " obj";
        break;
      case STT_NOTYPE:
        typename = "none";
        break;
      default:
        typename = "????";
        break;
    }

    kprintf("%s %s %-40s", bindname, typename, strtab + symtab[i].st_name);
    PrintDsoSymbolVersions(verdef, versym[i], strtab);
    kprintf("\n");
  }

  return 0;
}

int main(int argc, char *argv[]) {
  return PrintVdsoSymbols();
}
