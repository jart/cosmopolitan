#include "libc/bits/bits.h"
#include "libc/mem/mem.h"
#include "libc/sysv/errfuns.h"
#include "third_party/dlmalloc/dlmalloc.internal.h"

void* dlmemalign$impl(mstate m, size_t alignment, size_t bytes) {
  void* mem = 0;
  if (alignment < MIN_CHUNK_SIZE) { /* must be at least a minimum chunk size */
    alignment = MIN_CHUNK_SIZE;     /* is 32 bytes on NexGen32e */
  }
  if ((alignment & (alignment - SIZE_T_ONE)) != 0) { /* Ensure a power of 2 */
    alignment = roundup2pow(alignment);
  }
  if (bytes >= MAX_REQUEST - alignment) {
    if (m != 0) { /* Test isn't needed but avoids compiler warning */
      enomem();
    }
  } else {
    size_t nb = request2size(bytes);
    size_t req = nb + alignment + MIN_CHUNK_SIZE - CHUNK_OVERHEAD;
    mem = dlmalloc(req);
    if (mem != 0) {
      mchunkptr p = mem2chunk(mem);
      if (PREACTION(m)) return 0;
      if ((((size_t)(mem)) & (alignment - 1)) != 0) { /* misaligned */
        /*
          Find an aligned spot inside chunk.  Since we need to give
          back leading space in a chunk of at least MIN_CHUNK_SIZE, if
          the first calculation places us at a spot with less than
          MIN_CHUNK_SIZE leader, we can move to the next aligned spot.
          We've allocated enough total room so that this is always
          possible.
        */
        char* br = (char*)mem2chunk((size_t)(
            ((size_t)((char*)mem + alignment - SIZE_T_ONE)) & -alignment));
        char* pos =
            ((size_t)(br - (char*)(p)) >= MIN_CHUNK_SIZE) ? br : br + alignment;
        mchunkptr newp = (mchunkptr)pos;
        size_t leadsize = pos - (char*)(p);
        size_t newsize = chunksize(p) - leadsize;
        if (is_mmapped(p)) { /* For mmapped chunks, just adjust offset */
          newp->prev_foot = p->prev_foot + leadsize;
          newp->head = newsize;
        } else { /* Otherwise, give back leader, use the rest */
          set_inuse(m, newp, newsize);
          set_inuse(m, p, leadsize);
          dlmalloc_dispose_chunk(m, p, leadsize);
        }
        p = newp;
      }
      /* Give back spare room at the end */
      if (!is_mmapped(p)) {
        size_t size = chunksize(p);
        if (size > nb + MIN_CHUNK_SIZE) {
          size_t remainder_size = size - nb;
          mchunkptr remainder = chunk_plus_offset(p, nb);
          set_inuse(m, p, nb);
          set_inuse(m, remainder, remainder_size);
          dlmalloc_dispose_chunk(m, remainder, remainder_size);
        }
      }
      mem = chunk2mem(p);
      assert(chunksize(p) >= nb);
      assert(((size_t)mem & (alignment - 1)) == 0);
      check_inuse_chunk(m, p);
      POSTACTION(m);
    }
  }
  return ADDRESS_BIRTH_ACTION(mem);
}
