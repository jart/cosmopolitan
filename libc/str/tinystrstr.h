#ifndef COSMOPOLITAN_LIBC_STR_TINYSTRSTR_H_
#define COSMOPOLITAN_LIBC_STR_TINYSTRSTR_H_
#include "libc/str/str.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)

#define tinystrstr(HAYSTACK, NEEDLE)          \
  ({                                          \
    autotype(HAYSTACK) Haystack = (HAYSTACK); \
    typeof(Haystack) Needle = (NEEDLE);       \
    for (;;) {                                \
      size_t i = 0;                           \
      for (;;) {                              \
        if (!Needle[i]) goto Found;           \
        if (!Haystack[i]) break;              \
        if (Needle[i] != Haystack[i]) break;  \
        ++i;                                  \
      }                                       \
      if (!*Haystack++) break;                \
    }                                         \
    Haystack = NULL;                          \
  Found:                                      \
    Haystack;                                 \
  })

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_STR_TINYSTRSTR_H_ */
