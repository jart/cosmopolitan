#include "libc/mem/mem.h"
#include "third_party/dlmalloc/dlmalloc.internal.h"

/**
 * Returns (by copy) a struct containing various summary statistics:
 *
 * - arena: current total non-mmapped bytes allocated from system
 *
 * - ordblks: the number of free chunks
 *
 * - smblks: always zero.
 *
 * - hblks: current number of mmapped regions
 *
 * - hblkhd: total bytes held in mmapped regions
 *
 * - usmblks: the maximum total allocated space. This will be greater
 *   than current total if trimming has occurred.
 *
 * - fsmblks: always zero
 *
 * - uordblks: current total allocated space (normal or mmapped)
 *
 * - fordblks: total free space
 *
 * - keepcost: the maximum number of bytes that could ideally be
 *   released back to system via malloc_trim. ("ideally" means that it
 *   ignores page restrictions etc.)
 *
 * Because these fields are ints, but internal bookkeeping may
 * be kept as longs, the reported values may wrap around zero and
 * thus be inaccurate.
 */
struct mallinfo mallinfo(void) {
  struct mallinfo nm = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  ensure_initialization();
  if (!PREACTION(g_dlmalloc)) {
    check_malloc_state(g_dlmalloc);
    if (is_initialized(g_dlmalloc)) {
      size_t nfree = SIZE_T_ONE; /* top always free */
      size_t mfree = g_dlmalloc->topsize + TOP_FOOT_SIZE;
      size_t sum = mfree;
      msegmentptr s = &g_dlmalloc->seg;
      while (s != 0) {
        mchunkptr q = align_as_chunk(s->base);
        while (segment_holds(s, q) && q != g_dlmalloc->top &&
               q->head != FENCEPOST_HEAD) {
          size_t sz = chunksize(q);
          sum += sz;
          if (!is_inuse(q)) {
            mfree += sz;
            ++nfree;
          }
          q = next_chunk(q);
        }
        s = s->next;
      }
      nm.arena = sum;
      nm.ordblks = nfree;
      nm.hblkhd = g_dlmalloc->footprint - sum;
      nm.usmblks = g_dlmalloc->max_footprint;
      nm.uordblks = g_dlmalloc->footprint - mfree;
      nm.fordblks = mfree;
      nm.keepcost = g_dlmalloc->topsize;
    }
    POSTACTION(g_dlmalloc);
  }
  return nm;
}
