#include "libc/mem/mem.h"
#include "third_party/dlmalloc/dlmalloc.internal.h"

size_t dlmalloc_usable_size(const void* mem) {
  if (mem != 0) {
    mchunkptr p = mem2chunk(mem);
    if (is_inuse(p)) return chunksize(p) - overhead_for(p);
  }
  return 0;
}
