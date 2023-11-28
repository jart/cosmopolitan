#ifndef COSMOPOLITAN_LIBC_SYMBOLS_H_
#define COSMOPOLITAN_LIBC_SYMBOLS_H_
COSMOPOLITAN_C_START_

#define SYMBOLS_MAGIC 0x544d5953 /* SYMT */
#define SYMBOLS_ABI   1

struct Symbol {
  unsigned x; /* start (relative to addr_base) */
  unsigned y; /* start + size - 1  (inclusive) */
};

struct SymbolTable {
  uint32_t magic;            /* 0xFEEDABEE little endian */
  uint32_t abi;              /* 1 */
  uint64_t count;            /* of `symbols` */
  uint64_t size;             /* file size */
  uint64_t mapsize;          /* of this object */
  int64_t addr_base;         /* IMAGE_BASE_VIRTUAL */
  int64_t addr_end;          /* _end - 1 */
  uint32_t *names;           /* relative to `name_base` */
  char *name_base;           /* double-nul terminated w/ empty first */
  uint32_t names_offset;     /* for file loading */
  uint32_t name_base_offset; /* for file loading */
  struct Symbol symbols[];   /* sorted and non-overlapping intervals */
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
#endif /* COSMOPOLITAN_LIBC_SYMBOLS_H_ */
