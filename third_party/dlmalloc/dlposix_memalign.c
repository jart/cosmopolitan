#include "libc/errno.h"
#include "libc/mem/mem.h"
#include "third_party/dlmalloc/dlmalloc.h"

int dlposix_memalign(void** pp, size_t alignment, size_t bytes) {
  void* mem = 0;
  if (alignment == MALLOC_ALIGNMENT)
    mem = dlmalloc(bytes);
  else {
    size_t d = alignment / sizeof(void*);
    size_t r = alignment % sizeof(void*);
    if (r != 0 || d == 0 || (d & (d - SIZE_T_ONE)) != 0)
      return EINVAL;
    else if (bytes <= MAX_REQUEST - alignment) {
      if (alignment < MIN_CHUNK_SIZE) alignment = MIN_CHUNK_SIZE;
      mem = dlmemalign$impl(gm, alignment, bytes);
    }
  }
  if (mem == 0)
    return ENOMEM;
  else {
    *pp = mem;
    return 0;
  }
}
