#include "libc/mem/mem.h"
#include "third_party/dlmalloc/dlmalloc.h"

void *dlpvalloc(size_t bytes) {
  size_t pagesz;
  ensure_initialization();
  pagesz = mparams.page_size;
  return dlmemalign(pagesz,
                    (bytes + pagesz - SIZE_T_ONE) & ~(pagesz - SIZE_T_ONE));
}
