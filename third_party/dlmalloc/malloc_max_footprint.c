#include "libc/mem/mem.h"
#include "third_party/dlmalloc/dlmalloc.internal.h"

/**
 * Returns the maximum number of bytes obtained from the system. This
 * value will be greater than current footprint if deallocated space has
 * been reclaimed by the system. The peak number of bytes allocated by
 * malloc, realloc etc., is less than this value. Unlike mallinfo, this
 * function returns only a precomputed result, so can be called
 * frequently to monitor memory consumption. Even if locks are otherwise
 * defined, this function does not use them, so results might not be up
 * to date.
 */
size_t malloc_max_footprint(void) {
  return g_dlmalloc->max_footprint;
}
