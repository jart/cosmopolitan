#ifndef COSMOPOLITAN_LIBC_BITS_SAFEMACROS_H_
#define COSMOPOLITAN_LIBC_BITS_SAFEMACROS_H_
#include "libc/macros.internal.h"
#include "libc/runtime/runtime.h"
COSMOPOLITAN_C_START_

#define min(x, y)              \
  ({                           \
    autotype(x) MinX = (x);    \
    autotype(y) MinY = (y);    \
    MinX < MinY ? MinX : MinY; \
  })

#define max(x, y)              \
  ({                           \
    autotype(x) MaxX = (x);    \
    autotype(y) MaxY = (y);    \
    MaxX > MaxY ? MaxX : MaxY; \
  })

#define roundup(x, k)            \
  ({                             \
    autotype(x) RoundupX = (x);  \
    autotype(k) RoundupK = (k);  \
    ROUNDUP(RoundupX, RoundupK); \
  })

#define rounddown(x, k)                \
  ({                                   \
    autotype(x) RounddownX = (x);      \
    autotype(k) RounddownK = (k);      \
    ROUNDDOWN(RounddownX, RounddownK); \
  })

#define isempty(s)              \
  ({                            \
    autotype(s) IsEmptyS = (s); \
    !IsEmptyS || !(*IsEmptyS);  \
  })

#define nulltoempty(s)                \
  ({                                  \
    autotype(s) NullToEmptyS = (s);   \
    NullToEmptyS ? NullToEmptyS : ""; \
  })

#define firstnonnull(a, b)                         \
  ({                                               \
    autotype(a) FirstNonNullA = (a);               \
    autotype(a) FirstNonNullB = (b);               \
    if (!FirstNonNullA && !FirstNonNullB) abort(); \
    FirstNonNullA ? FirstNonNullA : FirstNonNullB; \
  })

#define emptytonull(s)                                      \
  ({                                                        \
    autotype(s) EmptyToNullS = (s);                         \
    EmptyToNullS && !(*EmptyToNullS) ? NULL : EmptyToNullS; \
  })

#define unsignedsubtract(a, b)                                 \
  ({                                                           \
    uint64_t UnsubA = (a);                                     \
    uint64_t UnsubB = (b);                                     \
    UnsubA >= UnsubB ? UnsubA - UnsubB : ~UnsubB + UnsubA + 1; \
  })

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_BITS_SAFEMACROS_H_ */
