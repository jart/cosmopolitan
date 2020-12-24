#ifndef COSMOPOLITAN_TOOL_BUILD_LIB_ELFWRITER_H_
#define COSMOPOLITAN_TOOL_BUILD_LIB_ELFWRITER_H_
#include "libc/elf/struct/ehdr.h"
#include "libc/elf/struct/rela.h"
#include "libc/elf/struct/shdr.h"
#include "libc/elf/struct/sym.h"
#include "tool/build/lib/interner.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

struct ElfWriter {
  char *path;
  char *tmppath;
  int fd;
  void *map;
  size_t mapsize;
  size_t wrote;
  size_t entsize;
  size_t addralign;
  struct Elf64_Ehdr *ehdr;
  struct {
    size_t i, n;
    Elf64_Shdr *p;
  } shdrs[1];
  struct ElfWriterSyms {
    size_t i, n;
    Elf64_Sym *p;
  } syms[3][1];
  struct {
    size_t i, j, n;
    struct ElfWriterRela {
      uint64_t offset;
      struct ElfWriterSymRef {
        enum ElfWriterSymOrder {
          kElfWriterSymSection,
          kElfWriterSymLocal,
          kElfWriterSymGlobal
        } slg;
        uint32_t sym;
      } symkey;
      uint32_t type;
      int64_t addend;
    } * p;
  } relas[1];
  struct Interner *strtab;
  struct Interner *shstrtab;
};

struct ElfWriter *elfwriter_open(const char *, int) nodiscard;
void elfwriter_cargoculting(struct ElfWriter *);
void elfwriter_close(struct ElfWriter *);
void elfwriter_align(struct ElfWriter *, size_t, size_t);
size_t elfwriter_startsection(struct ElfWriter *, const char *, int, int);
void *elfwriter_reserve(struct ElfWriter *, size_t);
void elfwriter_commit(struct ElfWriter *, size_t);
void elfwriter_finishsection(struct ElfWriter *);
void elfwriter_appendrela(struct ElfWriter *, uint64_t, struct ElfWriterSymRef,
                          uint32_t, int64_t);
struct ElfWriterSymRef elfwriter_linksym(struct ElfWriter *, const char *, int,
                                         int);
struct ElfWriterSymRef elfwriter_appendsym(struct ElfWriter *, const char *,
                                           int, int, size_t, size_t);
void elfwriter_yoink(struct ElfWriter *, const char *);
void elfwriter_setsection(struct ElfWriter *, struct ElfWriterSymRef, uint16_t);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_TOOL_BUILD_LIB_ELFWRITER_H_ */
