#include "libc/mem/mem.h"
#include "third_party/dlmalloc/dlmalloc.internal.h"

/**
 * Returns the number of bytes obtained from the system. The total
 * number of bytes allocated by malloc, realloc etc., is less than this
 * value. Unlike mallinfo, this function returns only a precomputed
 * result, so can be called frequently to monitor memory consumption.
 * Even if locks are otherwise defined, this function does not use them,
 * so results might not be up to date.
 */
size_t malloc_footprint(void) {
  return g_dlmalloc->footprint;
}
