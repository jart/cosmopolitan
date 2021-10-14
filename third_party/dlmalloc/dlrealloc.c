#include "libc/bits/likely.h"
#include "libc/str/str.h"
#include "libc/sysv/errfuns.h"
#include "third_party/dlmalloc/dlmalloc.internal.h"

void *dlrealloc(void *oldmem, size_t bytes) {
  void *mem = 0;
  size_t oc, nb;
  struct MallocState *m;
  struct MallocChunk *oldp, *newp;
  if (oldmem) {
    if (LIKELY(bytes < MAX_REQUEST)) {
      if (bytes) {
        nb = request2size(bytes);
        oldp = mem2chunk(oldmem);
#if !FOOTERS
        m = g_dlmalloc;
#else /* FOOTERS */
        m = get_mstate_for(oldp);
        if (UNLIKELY(!ok_magic(m))) {
          USAGE_ERROR_ACTION(m, oldmem);
          return 0;
        }
#endif /* FOOTERS */
        if (!PREACTION(m)) {
          newp = dlmalloc_try_realloc_chunk(m, oldp, nb, 1);
          POSTACTION(m);
          if (newp) {
            check_inuse_chunk(m, newp);
            mem = chunk2mem(newp);
          } else if ((mem = dlmalloc(bytes))) {
            oc = chunksize(oldp) - overhead_for(oldp);
            memcpy(mem, oldmem, (oc < bytes) ? oc : bytes);
            dlfree(oldmem);
          }
        }
      } else {
        dlfree(oldmem);
      }
    } else {
      enomem();
    }
  } else {
    mem = dlmalloc(bytes);
  }
  return mem;
}
