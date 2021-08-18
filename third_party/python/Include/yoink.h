#ifndef COSMOPOLITAN_THIRD_PARTY_PYTHON_INCLUDE_YOINK_H_
#define COSMOPOLITAN_THIRD_PARTY_PYTHON_INCLUDE_YOINK_H_
#include "libc/dce.h"

#if !IsTiny()
#define PYTHON_YOINK(s)          \
  __asm__(".section .yoink\n\t"  \
          "nopl\t\"" s "\"\n\t"  \
          "nopl\t\"" s "c\"\n\t" \
          ".previous")
#else
#define PYTHON_YOINK(s)          \
  __asm__(".section .yoink\n\t"  \
          "nopl\t\"" s "c\"\n\t" \
          ".previous")
#endif

#endif /* COSMOPOLITAN_THIRD_PARTY_PYTHON_INCLUDE_YOINK_H_ */
