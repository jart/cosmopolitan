#include "libc/mem/mem.h"
#include "third_party/dlmalloc/dlmalloc.internal.h"

void *dlmemalign(size_t alignment, size_t bytes) {
  if (alignment <= MALLOC_ALIGNMENT) return dlmalloc(bytes);
  return dlmemalign$impl(g_dlmalloc, alignment, bytes);
}
