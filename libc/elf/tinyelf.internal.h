#ifndef COSMOPOLITAN_LIBC_ELF_TINYELF_INTERNAL_H_
#define COSMOPOLITAN_LIBC_ELF_TINYELF_INTERNAL_H_
#include "libc/elf/def.h"
#include "libc/elf/struct/ehdr.h"
#include "libc/elf/struct/phdr.h"
#include "libc/elf/struct/shdr.h"
#include "libc/elf/struct/sym.h"
#include "libc/serialize.h"
#include "libc/limits.h"
#include "libc/log/libfatal.internal.h"
COSMOPOLITAN_C_START_

#define GetStr(tab, rva)     ((char *)(tab) + (rva))
#define GetSection(e, s)     ((void *)((intptr_t)(e) + (size_t)(s)->sh_offset))
#define GetShstrtab(e)       GetSection(e, GetShdr(e, (e)->e_shstrndx))
#define GetSectionName(e, s) GetStr(GetShstrtab(e), (s)->sh_name)
#define GetPhdr(e, i)                            \
  ((Elf64_Phdr *)((intptr_t)(e) + (e)->e_phoff + \
                  (unsigned)(e)->e_phentsize * (i)))
#define GetShdr(e, i)                            \
  ((Elf64_Shdr *)((intptr_t)(e) + (e)->e_shoff + \
                  (unsigned)(e)->e_shentsize * (i)))

static inline char *GetStrtab(const Elf64_Ehdr *e, size_t *n) {
  int i;
  char *name;
  Elf64_Shdr *shdr;
  for (i = 0; i < e->e_shnum; ++i) {
    shdr = GetShdr(e, i);
    if (shdr->sh_type == SHT_STRTAB) {
      name = GetSectionName(e, GetShdr(e, i));
      if (name && READ64LE(name) == READ64LE(".strtab")) {
        if (n) *n = shdr->sh_size;
        return GetSection(e, shdr);
      }
    }
  }
  return 0;
}

static inline Elf64_Sym *GetSymtab(const Elf64_Ehdr *e, Elf64_Xword *n) {
  int i;
  Elf64_Shdr *shdr;
  for (i = e->e_shnum; i-- > 0;) {
    shdr = GetShdr(e, i);
    if (shdr->sh_type == SHT_SYMTAB) {
      if (n) *n = shdr->sh_size / sizeof(Elf64_Sym);
      return GetSection(e, shdr);
    }
  }
  return 0;
}

static inline void GetImageRange(const Elf64_Ehdr *elf, intptr_t *x,
                                 intptr_t *y) {
  unsigned i;
  const Elf64_Phdr *phdr;
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

static inline bool GetElfSymbolValue(const Elf64_Ehdr *ehdr, const char *name,
                                     uint64_t *res) {
  Elf64_Xword i, n;
  const char *stab;
  const Elf64_Sym *st;
  if (!(stab = GetStrtab(ehdr, 0))) return false;
  if (!(st = GetSymtab(ehdr, &n))) return false;
  for (i = 0; i < n; ++i) {
    if (!__strcmp(GetStr(stab, st[i].st_name), name)) {
      *res = st[i].st_value;
      return true;
    }
  }
  return false;
}

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_ELF_TINYELF_INTERNAL_H_ */
