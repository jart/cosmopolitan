#include "libc/runtime/valist.h"

#define __GNUC_VA_LIST 1
#define __gnuc_va_list va_list

#define va_end(AP)
#define va_copy(DST, SRC) ((DST)[0] = (SRC)[0])
#define va_start(AP, LAST)                    \
  do {                                        \
    *(AP) = *(struct __va_list *)__va_area__; \
  } while (0)

#define va_arg(AP, TYPE)                               \
  (*(TYPE *)__va_arg(AP, sizeof(TYPE), _Alignof(TYPE), \
                     __builtin_reg_class(TYPE)))

typedef struct __va_list va_list[1];
