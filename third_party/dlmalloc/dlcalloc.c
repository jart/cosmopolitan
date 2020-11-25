#include "libc/str/str.h"
#include "third_party/dlmalloc/dlmalloc.internal.h"

void *dlcalloc(size_t n_elements, size_t elem_size) {
  void *mem;
  size_t req;
  if (__builtin_mul_overflow(n_elements, elem_size, &req)) req = -1;
  mem = dlmalloc(req);
  if (mem != 0 && calloc_must_clear(mem2chunk(mem))) memset(mem, 0, req);
  return mem;
}
