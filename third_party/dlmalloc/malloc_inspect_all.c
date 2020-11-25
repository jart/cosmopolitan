#include "libc/mem/mem.h"
#include "third_party/dlmalloc/dlmalloc.internal.h"

static void internal_inspect_all(mstate m,
                                 void (*handler)(void* start, void* end,
                                                 size_t used_bytes,
                                                 void* callback_arg),
                                 void* arg) {
  if (is_initialized(m)) {
    mchunkptr top = m->top;
    msegmentptr s;
    for (s = &m->seg; s != 0; s = s->next) {
      mchunkptr q = align_as_chunk(s->base);
      while (segment_holds(s, q) && q->head != FENCEPOST_HEAD) {
        mchunkptr next = next_chunk(q);
        size_t sz = chunksize(q);
        size_t used;
        void* start;
        if (is_inuse(q)) {
          used = sz - CHUNK_OVERHEAD; /* must not be mmapped */
          start = chunk2mem(q);
        } else {
          used = 0;
          if (is_small(sz)) { /* offset by possible bookkeeping */
            start = (void*)((char*)q + sizeof(struct malloc_chunk));
          } else {
            start = (void*)((char*)q + sizeof(struct malloc_tree_chunk));
          }
        }
        if (start < (void*)next) /* skip if all space is bookkeeping */
          handler(start, next, used, arg);
        if (q == top) break;
        q = next;
      }
    }
  }
}

/**
 * Traverses the heap and calls the given handler for each managed
 * region, skipping all bytes that are (or may be) used for bookkeeping
 * purposes.  Traversal does not include include chunks that have been
 * directly memory mapped. Each reported region begins at the start
 * address, and continues up to but not including the end address.  The
 * first used_bytes of the region contain allocated data. If
 * used_bytes is zero, the region is unallocated. The handler is
 * invoked with the given callback argument. If locks are defined, they
 * are held during the entire traversal. It is a bad idea to invoke
 * other malloc functions from within the handler.
 *
 * For example, to count the number of in-use chunks with size greater
 * than 1000, you could write:
 *
 *     static int count = 0;
 *     void count_chunks(void* start, void* end, size_t used, void* arg) {
 *       if (used >= 1000) ++count;
 *     }
 *
 * then,
 *
 *     malloc_inspect_all(count_chunks, NULL);
 */
void malloc_inspect_all(void (*handler)(void* start, void* end,
                                        size_t used_bytes, void* callback_arg),
                        void* arg) {
  ensure_initialization();
  if (!PREACTION(g_dlmalloc)) {
    internal_inspect_all(g_dlmalloc, handler, arg);
    POSTACTION(g_dlmalloc);
  }
}
