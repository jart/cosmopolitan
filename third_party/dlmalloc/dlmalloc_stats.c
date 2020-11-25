#include "libc/mem/mem.h"
#include "libc/str/str.h"
#include "third_party/dlmalloc/dlmalloc.internal.h"

/**
 * Prints on stderr the amount of space obtained from the system (both
 * via sbrk and mmap), the maximum amount (which may be more than
 * current if malloc_trim and/or munmap got called), and the current
 * number of bytes allocated via malloc (or realloc, etc) but not yet
 * freed. Note that this is the number of bytes allocated, not the
 * number requested. It will be larger than the number requested because
 * of alignment and bookkeeping overhead. Because it includes alignment
 * wastage as being in use, this figure may be greater than zero even
 * when no user-level chunks are allocated.
 *
 * The reported current and maximum system memory can be inaccurate if a
 * program makes other calls to system memory allocation functions
 * (normally sbrk) outside of malloc.
 *
 * malloc_stats prints only the most commonly interesting statistics.
 * More information can be obtained by calling mallinfo.
 */
struct MallocStats dlmalloc_stats(mstate m) {
  struct MallocStats res;
  memset(&res, 0, sizeof(res));
  ensure_initialization();
  if (!PREACTION(m)) {
    check_malloc_state(m);
    if (is_initialized(m)) {
      msegmentptr s = &m->seg;
      res.maxfp = m->max_footprint;
      res.fp = m->footprint;
      res.used = res.fp - (m->topsize + TOP_FOOT_SIZE);
      while (s != 0) {
        mchunkptr q = align_as_chunk(s->base);
        while (segment_holds(s, q) && q != m->top &&
               q->head != FENCEPOST_HEAD) {
          if (!is_inuse(q)) res.used -= chunksize(q);
          q = next_chunk(q);
        }
        s = s->next;
      }
    }
    POSTACTION(m); /* drop lock */
  }
  return res;
}
