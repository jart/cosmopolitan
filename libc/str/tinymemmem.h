#ifndef COSMOPOLITAN_LIBC_STR_TINYSTRSTR_H_
#define COSMOPOLITAN_LIBC_STR_TINYSTRSTR_H_
#include "libc/str/str.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)

forceinline void *tinymemmemi(const void *haystk, size_t haystksize,
                              const void *needle, size_t needlesize) {
  const char *p = haystk;
  const char *pe = p + haystksize;
  while (p < pe) {
    size_t i = 0;
    ++p;
    for (;;) {
      ++i;
      if (i > needlesize) return (/*unconst*/ char *)(p - 1);
      if (p == pe) break;
      if (((const char *)needle)[i - 1] != (p - 1)[i - 1]) break;
    }
  }
  return (/*unconst*/ char *)(!haystksize && !needlesize ? haystk : NULL);
}

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_STR_TINYMEMMEM_H_ */
