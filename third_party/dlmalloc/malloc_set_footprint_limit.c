#include "libc/limits.h"
#include "libc/mem/mem.h"
#include "third_party/dlmalloc/dlmalloc.internal.h"

/**
 * Sets the maximum number of bytes to obtain from the system, causing
 * failure returns from malloc and related functions upon attempts to
 * exceed this value. The argument value may be subject to page rounding
 * to an enforceable limit; this actual value is returned. Using an
 * argument of the maximum possible size_t effectively disables checks.
 * If the argument is less than or equal to the current
 * malloc_footprint, then all future allocations that require additional
 * system memory will fail. However, invocation cannot retroactively
 * deallocate existing used memory.
 */
size_t malloc_set_footprint_limit(size_t bytes) {
  size_t result;                                 /* invert sense of 0 */
  if (bytes == 0) result = granularity_align(1); /* Use minimal size */
  if (bytes == SIZE_MAX) {
    result = 0; /* disable */
  } else {
    result = granularity_align(bytes);
  }
  return g_dlmalloc->footprint_limit = result;
}
