#ifndef COSMOPOLITAN_LIBC_ELF_H_
#define COSMOPOLITAN_LIBC_ELF_H_
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

Elf64_Ehdr *mapelfread(const char *filename, struct MappedFile *mf);
char *getelfstringtable(const Elf64_Ehdr *elf, size_t mapsize);
Elf64_Sym *getelfsymboltable(const Elf64_Ehdr *elf, size_t mapsize,
                             Elf64_Xword *out_count);
Elf64_Shdr *getelfsectionbyaddress(const Elf64_Ehdr *elf, size_t mapsize,
                                   void *addr);

forceinline void checkelfaddress(const Elf64_Ehdr *elf, size_t mapsize,
                                 intptr_t addr, size_t addrsize) {
#if !(TRUSTWORTHY + ELF_TRUSTWORTHY + 0)
  if (addr < (intptr_t)elf || addr + addrsize > (intptr_t)elf + mapsize) {
    abort();
  }
#endif
}

static inline bool iself64binary(const Elf64_Ehdr *elf, size_t mapsize) {
  return mapsize >= sizeof(Elf64_Ehdr) &&
         memcmp(elf->e_ident, ELFMAG, 4) == 0 &&
         (elf->e_ident[EI_CLASS] == ELFCLASSNONE ||
          elf->e_ident[EI_CLASS] == ELFCLASS64);
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
  intptr_t addr = (intptr_t)elf + (intptr_t)shdr->sh_offset;
  intptr_t size = (intptr_t)shdr->sh_size;
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
  intptr_t start = INTPTR_MAX;
  intptr_t end = 0;
  for (unsigned i = 0; i < elf->e_phnum; ++i) {
    Elf64_Phdr *phdr = getelfsegmentheaderaddress(elf, elfsize, i);
    if (phdr->p_type != PT_LOAD) continue;
    intptr_t pstart = phdr->p_vaddr;
    intptr_t pend = phdr->p_vaddr + phdr->p_memsz;
    if (pstart < start) start = pstart;
    if (pend > end) end = pend;
  }
  if (out_start) *out_start = start;
  if (out_end) *out_end = end;
}

static inline char *getelfstring(const Elf64_Ehdr *elf, size_t mapsize,
                                 const char *strtab, Elf64_Word rva) {
  intptr_t addr = (intptr_t)strtab + rva;
#if !(TRUSTWORTHY + ELF_TRUSTWORTHY + 0)
  checkelfaddress(elf, mapsize, addr, 0);
  checkelfaddress(elf, mapsize, addr,
                  strnlen((char *)addr, (intptr_t)elf + mapsize - addr) + 1);
#endif
  return (char *)addr;
}

static inline const char *getelfsectionname(const Elf64_Ehdr *elf,
                                            size_t mapsize, Elf64_Shdr *shdr) {
  if (!elf || !shdr) return NULL;
  return getelfstring(elf, mapsize, getelfsectionnamestringtable(elf, mapsize),
                      shdr->sh_name);
}

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_ELF_H_ */
