#include "libc/limits.h"
#include "libc/mem/mem.h"
#include "third_party/dlmalloc/dlmalloc.internal.h"

/**
 * Returns the number of bytes that the heap is allowed to obtain from
 * the system, returning the last value returned by
 * malloc_set_footprint_limit, or the maximum size_t value if never set.
 * The returned value reflects a permission. There is no guarantee that
 * this number of bytes can actually be obtained from the system.
 */
size_t malloc_footprint_limit(void) {
  size_t maf = g_dlmalloc->footprint_limit;
  return maf == 0 ? SIZE_MAX : maf;
}
