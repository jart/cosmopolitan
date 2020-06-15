#include "third_party/dlmalloc/dlmalloc.h"
#include "libc/mem/mem.h"

void *dlmemalign(size_t alignment, size_t bytes) {
  if (alignment <= MALLOC_ALIGNMENT) {
    return dlmalloc(bytes);
  }
  return dlmemalign$impl(gm, alignment, bytes);
}
