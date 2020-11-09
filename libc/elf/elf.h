#ifndef COSMOPOLITAN_LIBC_ELF_H_
#define COSMOPOLITAN_LIBC_ELF_H_
#ifndef __STRICT_ANSI__
#include "libc/bits/safemacros.h"
#include "libc/elf/def.h"
#include "libc/elf/struct/ehdr.h"
#include "libc/elf/struct/phdr.h"
#include "libc/elf/struct/shdr.h"
#include "libc/elf/struct/sym.h"
#include "libc/limits.h"
#include "libc/runtime/runtime.h"
#include "libc/str/str.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

/*───────────────────────────────────────────────────────────────────────────│─╗
│ cosmopolitan § executable & linkable format                              ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/

struct MappedFile;

Elf64_Ehdr *mapelfread(const char *, struct MappedFile *);
char *GetElfStringTable(const Elf64_Ehdr *, size_t);
Elf64_Sym *GetElfSymbolTable(const Elf64_Ehdr *, size_t, Elf64_Xword *);
bool IsElf64Binary(const Elf64_Ehdr *, size_t);

forceinline void checkelfaddress(const Elf64_Ehdr *elf, size_t mapsize,
                                 intptr_t addr, size_t addrsize) {
#if !(TRUSTWORTHY + ELF_TRUSTWORTHY + 0) || ELF_UNTRUSTWORTHY + 0
  if (addr < (intptr_t)elf || addr + addrsize > (intptr_t)elf + mapsize) {
    abort();
  }
#endif
}

static inline bool iselfsymbolcontent(const Elf64_Sym *sym) {
  return sym->st_size > 0 && (ELF64_ST_TYPE(sym->st_info) == STT_FUNC ||
                              ELF64_ST_TYPE(sym->st_info) == STT_OBJECT);
}

static inline Elf64_Phdr *getelfsegmentheaderaddress(const Elf64_Ehdr *elf,
                                                     size_t mapsize,
                                                     unsigned i) {
  intptr_t addr =
      ((intptr_t)elf + (intptr_t)elf->e_phoff + (intptr_t)elf->e_phentsize * i);
  checkelfaddress(elf, mapsize, addr, elf->e_phentsize);
  return (Elf64_Phdr *)addr;
}

static inline Elf64_Shdr *getelfsectionheaderaddress(const Elf64_Ehdr *elf,
                                                     size_t mapsize,
                                                     Elf64_Half i) {
  intptr_t addr =
      ((intptr_t)elf + (intptr_t)elf->e_shoff + (intptr_t)elf->e_shentsize * i);
  checkelfaddress(elf, mapsize, addr, elf->e_shentsize);
  return (Elf64_Shdr *)addr;
}

static inline void *getelfsectionaddress(const Elf64_Ehdr *elf, size_t mapsize,
                                         const Elf64_Shdr *shdr) {
  intptr_t addr, size;
  addr = (intptr_t)elf + (intptr_t)shdr->sh_offset;
  size = (intptr_t)shdr->sh_size;
  checkelfaddress(elf, mapsize, addr, size);
  return (void *)addr;
}

static inline char *getelfsectionnamestringtable(const Elf64_Ehdr *elf,
                                                 size_t mapsize) {
  if (!elf->e_shoff || !elf->e_shentsize) return NULL;
  return getelfsectionaddress(
      elf, mapsize, getelfsectionheaderaddress(elf, mapsize, elf->e_shstrndx));
}

static inline void getelfvirtualaddressrange(const Elf64_Ehdr *elf,
                                             size_t elfsize,
                                             intptr_t *out_start,
                                             intptr_t *out_end) {
  unsigned i;
  Elf64_Phdr *phdr;
  intptr_t start, end, pstart, pend;
  start = INTPTR_MAX;
  end = 0;
  for (i = 0; i < elf->e_phnum; ++i) {
    phdr = getelfsegmentheaderaddress(elf, elfsize, i);
    if (phdr->p_type != PT_LOAD) continue;
    pstart = phdr->p_vaddr;
    pend = phdr->p_vaddr + phdr->p_memsz;
    if (pstart < start) start = pstart;
    if (pend > end) end = pend;
  }
  if (out_start) *out_start = start;
  if (out_end) *out_end = end;
}

static inline char *GetElfString(const Elf64_Ehdr *elf, size_t mapsize,
                                 const char *strtab, Elf64_Word rva) {
  intptr_t addr = (intptr_t)strtab + rva;
#if !(TRUSTWORTHY + ELF_TRUSTWORTHY + 0)
  checkelfaddress(elf, mapsize, addr, 0);
  checkelfaddress(elf, mapsize, addr,
                  strnlen((char *)addr, (intptr_t)elf + mapsize - addr) + 1);
#endif
  return (char *)addr;
}

static inline const char *GetElfSectionName(const Elf64_Ehdr *elf,
                                            size_t mapsize, Elf64_Shdr *shdr) {
  if (!elf || !shdr) return NULL;
  return GetElfString(elf, mapsize, getelfsectionnamestringtable(elf, mapsize),
                      shdr->sh_name);
}

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* !ANSI */
#endif /* COSMOPOLITAN_LIBC_ELF_H_ */
