#ifndef COSMOPOLITAN_LIBC_INTRIN_ONCE_H_
#define COSMOPOLITAN_LIBC_INTRIN_ONCE_H_
#include "libc/intrin/spinlock.h"

#define _once(x)                       \
  ({                                   \
    typeof(x) oncerc;                  \
    static bool once;                  \
    static typeof(oncerc) onceresult;  \
    _Alignas(64) static char oncelock; \
    _spinlock(&oncelock);              \
    if (once) {                        \
      oncerc = onceresult;             \
    } else {                           \
      oncerc = onceresult = x;         \
    }                                  \
    _spunlock(&oncelock);              \
    oncerc;                            \
  })

#endif /* COSMOPOLITAN_LIBC_INTRIN_ONCE_H_ */
