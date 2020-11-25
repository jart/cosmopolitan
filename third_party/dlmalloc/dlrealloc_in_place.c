#include "libc/mem/mem.h"
#include "libc/sysv/errfuns.h"
#include "third_party/dlmalloc/dlmalloc.internal.h"

void *dlrealloc_in_place(void *oldmem, size_t bytes) {
  void *mem = 0;
  if (oldmem != 0) {
    if (bytes >= MAX_REQUEST) {
      enomem();
    } else {
      size_t nb = request2size(bytes);
      mchunkptr oldp = mem2chunk(oldmem);
#if !FOOTERS
      mstate m = g_dlmalloc;
#else  /* FOOTERS */
      mstate m = get_mstate_for(oldp);
      if (!ok_magic(m)) {
        USAGE_ERROR_ACTION(m, oldmem);
        return 0;
      }
#endif /* FOOTERS */
      if (!PREACTION(m)) {
        mchunkptr newp = dlmalloc_try_realloc_chunk(m, oldp, nb, 0);
        POSTACTION(m);
        if (newp == oldp) {
          check_inuse_chunk(m, newp);
          mem = oldmem;
        }
      }
    }
  }
  return mem;
}
