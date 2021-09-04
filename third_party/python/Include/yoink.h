#ifndef COSMOPOLITAN_THIRD_PARTY_PYTHON_INCLUDE_YOINK_H_
#define COSMOPOLITAN_THIRD_PARTY_PYTHON_INCLUDE_YOINK_H_
#include "libc/dce.h"

#define PYTHON_YOINK(s)            \
  __asm__(".section .yoink\n\t"    \
          "nopl\t\"py:" s "\"\n\t" \
          ".previous")

#endif /* COSMOPOLITAN_THIRD_PARTY_PYTHON_INCLUDE_YOINK_H_ */
