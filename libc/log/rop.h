#ifndef COSMOPOLITAN_LIBC_LOG_ROP_H_
#define COSMOPOLITAN_LIBC_LOG_ROP_H_
#include "libc/intrin/likely.h"

#define RETURN_ON_ERROR(expr)     \
  do {                            \
    if (UNLIKELY((expr) == -1)) { \
      goto OnError;               \
    }                             \
  } while (0)

#endif /* COSMOPOLITAN_LIBC_LOG_ROP_H_ */
