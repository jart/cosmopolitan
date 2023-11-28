#ifndef COSMOPOLITAN_LIBC_ALG_ARRAYLIST_H_
#define COSMOPOLITAN_LIBC_ALG_ARRAYLIST_H_
#include "libc/mem/internal.h"
#include "libc/str/str.h"

/* TODO(jart): DELETE */

#define append(ARRAYLIST, ITEM) concat((ARRAYLIST), (ITEM), 1)

#ifndef concat
#define concat(ARRAYLIST, ITEM, COUNT)                                      \
  ({                                                                        \
    autotype(ARRAYLIST) List = (ARRAYLIST);                                 \
    autotype(&List->p[0]) Item = (ITEM);                                    \
    size_t SizE = sizeof(*Item);                                            \
    size_t Count = (COUNT);                                                 \
    size_t Idx = List->i;                                                   \
    if (Idx + Count < List->n || __grow(&List->p, &List->n, SizE, Count)) { \
      memcpy(&List->p[Idx], Item, SizE *Count);                             \
      List->i = Idx + Count;                                                \
    } else {                                                                \
      Idx = -1UL;                                                           \
    }                                                                       \
    (ssize_t)(Idx);                                                         \
  })
#endif /* concat */

#endif /* COSMOPOLITAN_LIBC_ALG_ARRAYLIST_H_ */
