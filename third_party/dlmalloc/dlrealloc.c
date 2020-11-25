#include "libc/str/str.h"
#include "libc/sysv/errfuns.h"
#include "third_party/dlmalloc/dlmalloc.internal.h"

void *dlrealloc(void *oldmem, size_t bytes) {
  void *mem = 0;
  if (oldmem == 0) {
    mem = dlmalloc(bytes);
  } else if (bytes >= MAX_REQUEST) {
    enomem();
  } else if (bytes == 0) {
    dlfree(oldmem);
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
      mchunkptr newp = dlmalloc_try_realloc_chunk(m, oldp, nb, 1);
      POSTACTION(m);
      if (newp != 0) {
        check_inuse_chunk(m, newp);
        mem = chunk2mem(newp);
      } else {
        mem = dlmalloc(bytes);
        if (mem != 0) {
          size_t oc = chunksize(oldp) - overhead_for(oldp);
          memcpy(mem, oldmem, (oc < bytes) ? oc : bytes);
          dlfree(oldmem);
        }
      }
    }
  }
  return mem;
}
