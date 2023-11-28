#ifndef COSMOPOLITAN_TOOL_BUILD_LIB_ELFWRITER_H_
#define COSMOPOLITAN_TOOL_BUILD_LIB_ELFWRITER_H_
#include "libc/calls/struct/timespec.h"
#include "libc/elf/struct/ehdr.h"
#include "libc/elf/struct/rela.h"
#include "libc/elf/struct/shdr.h"
#include "libc/elf/struct/sym.h"
#include "tool/build/lib/interner.h"
COSMOPOLITAN_C_START_

struct ElfWriterSyms {
  size_t i, n;
  Elf64_Sym *p;
};

enum ElfWriterSymOrder {
  kElfWriterSymSection,
  kElfWriterSymLocal,
  kElfWriterSymGlobal
};

struct ElfWriterSymRef {
  int slg;
  uint32_t sym;
};

struct ElfWriterRela {
  uint64_t offset;
  struct ElfWriterSymRef symkey;
  uint32_t type;
  int64_t addend;
};

struct ElfWriter {
  char *path;
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
  struct ElfWriterSyms syms[3][1];
  struct {
    size_t i, j, n;
    struct ElfWriterRela *p;
  } relas[1];
  struct Interner *strtab;
  struct Interner *shstrtab;
};

struct ElfWriter *elfwriter_open(const char *, int, int) __wur;
void elfwriter_cargoculting(struct ElfWriter *);
void elfwriter_close(struct ElfWriter *);
void elfwriter_align(struct ElfWriter *, size_t, size_t);
size_t elfwriter_startsection(struct ElfWriter *, const char *, int, int);
void *elfwriter_reserve(struct ElfWriter *, size_t);
void elfwriter_commit(struct ElfWriter *, size_t);
void elfwriter_finishsection(struct ElfWriter *);
void elfwriter_appendrela(struct ElfWriter *, uint64_t, struct ElfWriterSymRef,
                          uint32_t, int64_t);
uint32_t elfwriter_relatype_pc32(const struct ElfWriter *);
uint32_t elfwriter_relatype_abs32(const struct ElfWriter *);
struct ElfWriterSymRef elfwriter_linksym(struct ElfWriter *, const char *, int,
                                         int);
struct ElfWriterSymRef elfwriter_appendsym(struct ElfWriter *, const char *,
                                           int, int, size_t, size_t);
void elfwriter_yoink(struct ElfWriter *, const char *, int);
void elfwriter_setsection(struct ElfWriter *, struct ElfWriterSymRef, uint16_t);
void elfwriter_zip(struct ElfWriter *, const char *, const char *, size_t,
                   const void *, size_t, uint32_t, struct timespec,
                   struct timespec, struct timespec, bool);

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_TOOL_BUILD_LIB_ELFWRITER_H_ */
