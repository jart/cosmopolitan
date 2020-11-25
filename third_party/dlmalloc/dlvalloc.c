#include "libc/mem/mem.h"
#include "third_party/dlmalloc/dlmalloc.internal.h"

void *dlvalloc(size_t bytes) {
  size_t pagesz;
  ensure_initialization();
  pagesz = g_mparams.page_size;
  return dlmemalign(pagesz, bytes);
}
