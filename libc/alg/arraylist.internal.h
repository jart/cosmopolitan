#ifndef COSMOPOLITAN_LIBC_ALG_ARRAYLIST_H_
#define COSMOPOLITAN_LIBC_ALG_ARRAYLIST_H_
#include "libc/bits/bits.h"
#include "libc/mem/mem.h"
#include "libc/str/str.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
#if 0
/**
 * @fileoverview Cosmopolitan Array List.
 *
 * This is a generically-typed ArrayList<T> template which follows a
 * duck-typing philosophy like Python, exporting an interface paradigm
 * similar to Go, that's implicitly defined by way of macros like Lisp.
 *
 *     struct MyArrayList {
 *       size_t i;  // current item count
 *       size_t n;  // current item capacity
 *       T *p;      // pointer to array (initially NULL)
 *     };
 *
 * Any struct with those fields can be used. It's also very important
 * that other data structures, which reference items in an arraylist, do
 * so using indices rather than pointers, since realloc() can relocate.
 *     
 * @see libc/mem/grow.c
 */
#endif

#define append(ARRAYLIST, ITEM) concat((ARRAYLIST), (ITEM), 1)

#define concat(ARRAYLIST, ITEM, COUNT)                                      \
  ({                                                                        \
    autotype(ARRAYLIST) List = (ARRAYLIST);                                 \
    autotype(&List->p[0]) Item = (ITEM);                                    \
    size_t SizE = sizeof(*Item);                                            \
    size_t Count = (COUNT);                                                 \
    size_t Idx = List->i;                                                   \
    if (Idx + Count < List->n || __grow(&List->p, &List->n, SizE, Count)) { \
      memcpy(&List->p[Idx], Item, SizE *Count);                             \
      atomic_store(&List->i, Idx + Count);                                  \
    } else {                                                                \
      Idx = -1UL;                                                           \
    }                                                                       \
    (ssize_t)(Idx);                                                         \
  })

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_ALG_ARRAYLIST_H_ */
