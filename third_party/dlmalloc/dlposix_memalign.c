#include "libc/errno.h"
#include "libc/mem/mem.h"
#include "libc/sysv/errfuns.h"
#include "third_party/dlmalloc/dlmalloc.internal.h"

int dlposix_memalign(void** pp, size_t alignment, size_t bytes) {
  void* mem;
  size_t d, r;
  mem = NULL;
  if (alignment == MALLOC_ALIGNMENT) {
    mem = dlmalloc(bytes);
  } else {
    d = alignment / sizeof(void*);
    r = alignment % sizeof(void*);
    if (r != 0 || d == 0 || (d & (d - SIZE_T_ONE)) != 0) {
      return einval();
    } else if (bytes <= MAX_REQUEST - alignment) {
      if (alignment < MIN_CHUNK_SIZE) alignment = MIN_CHUNK_SIZE;
      mem = dlmemalign$impl(g_dlmalloc, alignment, bytes);
    }
  }
  if (mem == 0) {
    return enomem();
  } else {
    *pp = mem;
    return 0;
  }
}
