#ifndef COSMOPOLITAN_LIBC_FMT_STRTOL_H_
#define COSMOPOLITAN_LIBC_FMT_STRTOL_H_
#include "libc/errno.h"
#include "libc/str/str.h"

#define CONSUME_SPACES(t, s, c)   \
  if (endptr) *endptr = (t *)(s); \
  while (c == ' ' || c == '\t') c = *++s

#define GET_SIGN(s, c, d) \
  d = c == '-' ? -1 : 1;  \
  if (c == '-' || c == '+') c = *++s

#define GET_RADIX(s, c, r)                                                 \
  if (!r) {                                                                \
    if (c == '0') {                                                        \
      t |= 1;                                                              \
      c = *++s;                                                            \
      if ((c == 'x' || c == 'X') && isxdigit(s[1])) {                      \
        c = *++s;                                                          \
        r = 16;                                                            \
      } else if ((c == 'b' || c == 'B') && (s[1] == '0' || s[1] == '1')) { \
        c = *++s;                                                          \
        r = 2;                                                             \
      } else {                                                             \
        r = 8;                                                             \
      }                                                                    \
    } else {                                                               \
      r = 10;                                                              \
    }                                                                      \
  } else if (!(2 <= r && r <= 36)) {                                       \
    errno = EINVAL;                                                        \
    return 0;                                                              \
  } else if (c == '0') {                                                   \
    t |= 1;                                                                \
    c = *++s;                                                              \
    if ((r == 2 &&                                                         \
         ((c == 'b' || c == 'B') && (s[1] == '0' || s[1] == '1'))) ||      \
        (r == 16 && ((c == 'x' || c == 'X') && isxdigit(s[1])))) {         \
      c = *++s;                                                            \
    }                                                                      \
  }

int __vcscanf(int (*)(void *), int (*)(int, void *), void *, const char *,
              va_list);
int __fmt(void *, void *, const char *, va_list);

#endif /* COSMOPOLITAN_LIBC_FMT_STRTOL_H_ */
