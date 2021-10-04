#ifndef COSMOPOLITAN_LIBC_SYMBOLS_H_
#define COSMOPOLITAN_LIBC_SYMBOLS_H_
#include "libc/assert.h"
#include "libc/elf/elf.h"
#include "libc/runtime/ezmap.internal.h"
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
void __hook(void *, struct SymbolTable *);

forceinline int GetSymbol(struct SymbolTable *t, intptr_t a) {
  unsigned l, m, r, n, k;
  if (t) {
    l = 0;
    r = n = t->count;
    k = a - t->addr_base;
    while (l < r) {
      m = (l + r) >> 1;
      if (t->symbols[m].y < k) {
        l = m + 1;
      } else {
        r = m;
      }
    }
    if (l < n && t->symbols[l].x <= k && k <= t->symbols[l].y) {
      return l;
    }
  }
  return -1;
}

forceinline char *GetSymbolName(struct SymbolTable *t, int s) {
  if (t && s != -1) {
    return t->name_base + t->names[s];
  } else {
    return 0;
  }
}

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_SYMBOLS_H_ */
