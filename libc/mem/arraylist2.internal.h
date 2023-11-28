#ifndef COSMOPOLITAN_LIBC_ALG_ARRAYLIST2_H_
#define COSMOPOLITAN_LIBC_ALG_ARRAYLIST2_H_
#include "libc/mem/internal.h"
#include "libc/str/str.h"
COSMOPOLITAN_C_START_

/* TODO(jart): Fully develop these better macros. */

#define APPEND(LIST_P, LIST_I, LIST_N, ITEM) \
  CONCAT(LIST_P, LIST_I, LIST_N, ITEM, 1)

#ifndef CONCAT
#define CONCAT(LIST_P, LIST_I, LIST_N, ITEM, COUNT)                     \
  ({                                                                    \
    autotype(LIST_P) ListP = (LIST_P);                                  \
    autotype(LIST_I) ListI = (LIST_I);                                  \
    autotype(LIST_N) ListN = (LIST_N);                                  \
    typeof(&(*ListP)[0]) Item = (ITEM);                                 \
    size_t SizE = sizeof(*Item);                                        \
    size_t Count = (COUNT);                                             \
    ssize_t Entry = -1;                                                 \
    /* NOTE: We use `<` to guarantee one additional slot */             \
    /*       grow() will memset(0) extended memory, thus */             \
    /*       you get a nul-terminator for free sometimes */             \
    /*       the exception is if you list.i=0 and re-use */             \
    /*       so you need concat(...); list.p[list.i++]=0 */             \
    if (*ListI + Count < *ListN || __grow(ListP, ListN, SizE, Count)) { \
      memcpy(&(*ListP)[*ListI], Item, (SizE) * (Count));                \
      Entry = *ListI;                                                   \
      *ListI += Count; /* happens after copy in case signal */          \
    }                                                                   \
    Entry;                                                              \
  })
#endif /* CONCAT */

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_ALG_ARRAYLIST2_H_ */
