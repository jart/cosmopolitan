#include "libc/mem/mem.h"
#include "third_party/dlmalloc/dlmalloc.internal.h"

/**
 * Frees and clears (sets to NULL) each non-null pointer in the given
 * array. This is likely to be faster than freeing them one-by-one. If
 * footers are used, pointers that have been allocated in different
 * mspaces are not freed or cleared, and the count of all such pointers
 * is returned. For large arrays of pointers with poor locality, it may
 * be worthwhile to sort this array before calling bulk_free.
 */
size_t bulk_free(void *array[], size_t nelem) {
  /*
   * Try to free all pointers in the given array. Note: this could be
   * made faster, by delaying consolidation, at the price of disabling
   * some user integrity checks, We still optimize some consolidations
   * by combining adjacent chunks before freeing, which will occur often
   * if allocated with ialloc or the array is sorted.
   */
  size_t unfreed = 0;
  if (!PREACTION(g_dlmalloc)) {
    void **a;
    void **fence = &(array[nelem]);
    for (a = array; a != fence; ++a) {
      void *mem = *a;
      if (mem != 0) {
        mchunkptr p = mem2chunk(ADDRESS_DEATH_ACTION(mem));
        size_t psize = chunksize(p);
#if FOOTERS
        if (get_mstate_for(p) != g_dlmalloc) {
          ++unfreed;
          continue;
        }
#endif
        check_inuse_chunk(g_dlmalloc, p);
        *a = 0;
        if (RTCHECK(ok_address(g_dlmalloc, p) && ok_inuse(p))) {
          void **b = a + 1; /* try to merge with next chunk */
          mchunkptr next = next_chunk(p);
          if (b != fence && *b == chunk2mem(next)) {
            size_t newsize = chunksize(next) + psize;
            set_inuse(g_dlmalloc, p, newsize);
            *b = chunk2mem(p);
          } else
            dlmalloc_dispose_chunk(g_dlmalloc, p, psize);
        } else {
          CORRUPTION_ERROR_ACTION(g_dlmalloc);
          break;
        }
      }
    }
    if (should_trim(g_dlmalloc, g_dlmalloc->topsize)) {
      dlmalloc_sys_trim(g_dlmalloc, 0);
    }
    POSTACTION(g_dlmalloc);
  }
  return unfreed;
}
