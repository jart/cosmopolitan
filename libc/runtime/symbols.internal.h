#ifndef COSMOPOLITAN_LIBC_SYMBOLS_H_
#define COSMOPOLITAN_LIBC_SYMBOLS_H_
#include "libc/elf/elf.h"
#include "libc/runtime/ezmap.internal.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

struct Symbol {
  unsigned addr_rva;
  unsigned name_rva;
};

struct SymbolTable {
  union {
    struct MappedFile mf;
    struct {
      int64_t fd;
      struct Elf64_Ehdr *elf;
      size_t elfsize;
    };
  };
  size_t scratch;
  size_t count;
  intptr_t addr_base;
  intptr_t addr_end;
  const char *name_base;
  struct Symbol symbols[];
};

struct SymbolTable *GetSymbolTable(void);
const char *FindComBinary(void);
const char *FindDebugBinary(void);
struct SymbolTable *OpenSymbolTable(const char *) nodiscard;
int CloseSymbolTable(struct SymbolTable **);
void __hook(void (*)(void), struct SymbolTable *);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_SYMBOLS_H_ */
