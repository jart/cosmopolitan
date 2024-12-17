#ifndef COSMOPOLITAN_LIBC_MEM_LEAKS_H_
#define COSMOPOLITAN_LIBC_MEM_LEAKS_H_
#include "libc/intrin/weaken.h"
COSMOPOLITAN_C_START_

void CheckForMemoryLeaks(void) libcesque;
void AssertNoLocksAreHeld(void) libcesque;

/**
 * Declares that allocation needn't be freed.
 *
 * This function does nothing if CheckForMemoryLeaks() hasn't been
 * linked into the binary.
 */
forceinline void *may_leak(void *__p) {
  void __may_leak(void *) libcesque;
  if (_weaken(__may_leak))
    _weaken(__may_leak)(__p);
  return __p;
}

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_MEM_LEAKS_H_ */
