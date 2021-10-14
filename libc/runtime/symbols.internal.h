#ifndef COSMOPOLITAN_LIBC_SYMBOLS_H_
#define COSMOPOLITAN_LIBC_SYMBOLS_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

struct Symbol {
  unsigned x; /* start (relative to addr_base) */
  unsigned y; /* start + size - 1  (inclusive) */
};

struct SymbolTable {
  size_t count;            /* of `symbols` */
  size_t mapsize;          /* of this object */
  intptr_t addr_base;      /* IMAGE_BASE_VIRTUAL */
  intptr_t addr_end;       /* _end - 1 */
  unsigned *names;         /* relative to `name_base` */
  char *name_base;         /* double-nul terminated w/ empty first */
  struct Symbol symbols[]; /* sorted and non-overlapping intervals */
};

struct SymbolTable *GetSymbolTable(void);
const char *FindComBinary(void);
const char *FindDebugBinary(void);
struct SymbolTable *OpenSymbolTable(const char *);
int CloseSymbolTable(struct SymbolTable **);
int __hook(void *, struct SymbolTable *);
int __get_symbol(struct SymbolTable *, intptr_t);
char *__get_symbol_name(struct SymbolTable *, int);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_SYMBOLS_H_ */
