#ifndef COSMOPOLITAN_LIBC_CALLS_HEFTY_MKVARARGV_H_
#define COSMOPOLITAN_LIBC_CALLS_HEFTY_MKVARARGV_H_
#include "libc/alg/arraylist2.internal.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

/**
 * Turns variadic program arguments into array.
 *
 * This is a support function for execl(), execlp(), spawnl(), etc.
 *
 * @note type signatures are fubar for these functions
 */
forceinline void *mkvarargv(const char *arg1, va_list va) {
  size_t i, n;
  const char **p, *arg;
  i = 0;
  n = 0;
  p = NULL;
  arg = arg1;
  do {
    if (APPEND(&p, &i, &n, &arg) == -1) {
      free(p);
      return NULL;
    }
  } while ((arg = va_arg(va, const char *)));
  return p;
}

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_CALLS_HEFTY_MKVARARGV_H_ */
