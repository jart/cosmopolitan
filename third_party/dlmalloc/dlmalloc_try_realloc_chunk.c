#include "third_party/dlmalloc/dlmalloc.internal.h"

/* Realloc using mmap */
static mchunkptr mmap_resize(mstate m, mchunkptr oldp, size_t nb, int flags) {
  size_t oldsize = chunksize(oldp);
  if (is_small(nb)) return 0; /* Can't shrink mmap regions below small size */
  /* Keep old chunk if big enough but not too big */
  if (oldsize >= nb + SIZE_T_SIZE &&
      (oldsize - nb) <= (g_mparams.granularity << 1)) {
    return oldp;
  } else {
    size_t offset = oldp->prev_foot;
    size_t oldmmsize = oldsize + offset + MMAP_FOOT_PAD;
    size_t newmmsize = mmap_align(nb + SIX_SIZE_T_SIZES + CHUNK_ALIGN_MASK);
    char *cp = mremap((char *)oldp - offset, oldmmsize, newmmsize, flags, 0);
    if (cp != CMFAIL) {
      mchunkptr newp = (mchunkptr)(cp + offset);
      size_t psize = newmmsize - offset - MMAP_FOOT_PAD;
      newp->head = psize;
      mark_inuse_foot(m, newp, psize);
      chunk_plus_offset(newp, psize)->head = FENCEPOST_HEAD;
      chunk_plus_offset(newp, psize + SIZE_T_SIZE)->head = 0;
      if (cp < m->least_addr) m->least_addr = cp;
      if ((m->footprint += newmmsize - oldmmsize) > m->max_footprint) {
        m->max_footprint = m->footprint;
      }
      check_mmapped_chunk(m, newp);
      return newp;
    }
  }
  return 0;
}

/* Try to realloc; only in-place unless can_move true */
mchunkptr dlmalloc_try_realloc_chunk(mstate m, mchunkptr p, size_t nb,
                                     int can_move) {
  mchunkptr newp = 0;
  size_t oldsize = chunksize(p);
  mchunkptr next = chunk_plus_offset(p, oldsize);
  if (RTCHECK(ok_address(m, p) && ok_inuse(p) && ok_next(p, next) &&
              ok_pinuse(next))) {
    if (is_mmapped(p)) {
      newp = mmap_resize(m, p, nb, can_move);
    } else if (oldsize >= nb) { /* already big enough */
      size_t rsize = oldsize - nb;
      if (rsize >= MIN_CHUNK_SIZE) { /* split off remainder */
        mchunkptr r = chunk_plus_offset(p, nb);
        set_inuse(m, p, nb);
        set_inuse(m, r, rsize);
        dlmalloc_dispose_chunk(m, r, rsize);
      }
      newp = p;
    } else if (next == m->top) { /* extend into top */
      if (oldsize + m->topsize > nb) {
        size_t newsize = oldsize + m->topsize;
        size_t newtopsize = newsize - nb;
        mchunkptr newtop = chunk_plus_offset(p, nb);
        set_inuse(m, p, nb);
        newtop->head = newtopsize | PINUSE_BIT;
        m->top = newtop;
        m->topsize = newtopsize;
        newp = p;
      }
    } else if (next == m->dv) { /* extend into dv */
      size_t dvs = m->dvsize;
      if (oldsize + dvs >= nb) {
        size_t dsize = oldsize + dvs - nb;
        if (dsize >= MIN_CHUNK_SIZE) {
          mchunkptr r = chunk_plus_offset(p, nb);
          mchunkptr n = chunk_plus_offset(r, dsize);
          set_inuse(m, p, nb);
          set_size_and_pinuse_of_free_chunk(r, dsize);
          clear_pinuse(n);
          m->dvsize = dsize;
          m->dv = r;
        } else { /* exhaust dv */
          size_t newsize = oldsize + dvs;
          set_inuse(m, p, newsize);
          m->dvsize = 0;
          m->dv = 0;
        }
        newp = p;
      }
    } else if (!cinuse(next)) { /* extend into next free chunk */
      size_t nextsize = chunksize(next);
      if (oldsize + nextsize >= nb) {
        size_t rsize = oldsize + nextsize - nb;
        unlink_chunk(m, next, nextsize);
        if (rsize < MIN_CHUNK_SIZE) {
          size_t newsize = oldsize + nextsize;
          set_inuse(m, p, newsize);
        } else {
          mchunkptr r = chunk_plus_offset(p, nb);
          set_inuse(m, p, nb);
          set_inuse(m, r, rsize);
          dlmalloc_dispose_chunk(m, r, rsize);
        }
        newp = p;
      }
    }
  } else {
    USAGE_ERROR_ACTION(m, chunk2mem(p));
  }
  return newp;
}
