#ifndef COSMOPOLITAN_LIBC_FMT_STRTOL_H_
#define COSMOPOLITAN_LIBC_FMT_STRTOL_H_
#include "libc/errno.h"

#define CONSUME_SPACES(s, c) \
  if (endptr) *endptr = s;   \
  while (c == ' ' || c == '\t') c = *++s

#define GET_SIGN(s, c, d) \
  d = c == '-' ? -1 : 1;  \
  if (c == '-' || c == '+') c = *++s

#define GET_RADIX(s, c, r)                     \
  if (!r) {                                    \
    if (c == '0') {                            \
      t |= 1;                                  \
      c = *++s;                                \
      if (c == 'x' || c == 'X') {              \
        c = *++s;                              \
        r = 16;                                \
      } else if (c == 'b' || c == 'B') {       \
        c = *++s;                              \
        r = 2;                                 \
      } else {                                 \
        r = 8;                                 \
      }                                        \
    } else {                                   \
      r = 10;                                  \
    }                                          \
  } else if (!(2 <= r && r <= 36)) {           \
    errno = EINVAL;                            \
    return 0;                                  \
  } else if (c == '0') {                       \
    t |= 1;                                    \
    c = *++s;                                  \
    if ((r == 2 && (c == 'b' || c == 'B')) ||  \
        (r == 16 && (c == 'x' || c == 'X'))) { \
      c = *++s;                                \
    }                                          \
  }

#endif /* COSMOPOLITAN_LIBC_FMT_STRTOL_H_ */
