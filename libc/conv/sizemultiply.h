#ifndef COSMOPOLITAN_LIBC_CONV_SIZEMULTIPLY_H_
#define COSMOPOLITAN_LIBC_CONV_SIZEMULTIPLY_H_
#include "libc/limits.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)

/**
 * Multiplies memory sizes.
 *
 * @param count may be 0 to for realloc() → free() behavior
 * @param opt_out set to count*itemsize or SIZE_MAX on overflow
 * @return true on success or false on overflow
 */
forceinline bool sizemultiply(size_t *opt_out, size_t count, size_t itemsize) {
  size_t res = 0;
  bool overflowed = false;
  if (count != 0) {
    res = count * itemsize;
    if (((count | itemsize) & ~0xfffful) && (res / count != itemsize)) {
      overflowed = true;
      res = SIZE_MAX;
    }
  }
  if (opt_out) *opt_out = res;
  return !overflowed;
}

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_CONV_SIZEMULTIPLY_H_ */
