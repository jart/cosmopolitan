#include "libc/bits/initializer.internal.h"
#include "libc/bits/safemacros.h"
#include "libc/calls/internal.h"
#include "libc/calls/struct/sysinfo.h"
#include "libc/dce.h"
#include "libc/fmt/conv.h"
#include "libc/limits.h"
#include "libc/macros.h"
#include "libc/mem/mem.h"
#include "libc/nt/systeminfo.h"
#include "libc/runtime/runtime.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/fileno.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/prot.h"
#include "libc/sysv/errfuns.h"
#include "third_party/dlmalloc/dlmalloc.internal.h"

STATIC_YOINK("_init_dlmalloc");

#define OOM_WARNING  "warning: running out of physical memory\n"
#define is_global(M) ((M) == g_dlmalloc)

hidden struct MallocState g_dlmalloc[1];
hidden struct MallocParams g_mparams;

/**
 * Acquires more system memory for dlmalloc.
 *
 * Each time dlmalloc needs 64kb, we ask for a 2mb page directory. The
 * empty space could help with buffer overflow detection; mremap() has
 * plenty of room to grow; and debuggability is greatly enhanced. This
 * should have less page table overhead than in security blanket mode.
 * Note that contiguous allocations are what Doug Lea recommends.
 */
static void *dlmalloc_requires_more_vespene_gas(size_t size) {
  if (0) {
    size_t need = mallinfo().arena + size;
    if (need > 8 * 1024 * 1024) {
      struct sysinfo info;
      if (sysinfo(&info) != -1) {
        if (info.freeram < (info.totalram >> 1) &&
            need > info.totalram * info.mem_unit / 2) {
          write(STDERR_FILENO, OOM_WARNING, strlen(OOM_WARNING));
          return NULL;
        }
      }
    }
  }
  return mapanon(size);
}

/* ─────────────────────────── mspace management ─────────────────────────── */

/* Initialize top chunk and its size */
static void dlmalloc_init_top(mstate m, mchunkptr p, size_t psize) {
  /* Ensure alignment */
  size_t offset = align_offset(chunk2mem(p));
  p = (mchunkptr)((char *)p + offset);
  psize -= offset;
  m->top = p;
  m->topsize = psize;
  p->head = psize | PINUSE_BIT;
  /* set size of fake trailing chunk holding overhead space only once */
  chunk_plus_offset(p, psize)->head = TOP_FOOT_SIZE;
  m->trim_check = g_mparams.trim_threshold; /* reset on each update */
}

/* Initialize bins for a new mstate that is otherwise zeroed out */
static void init_bins(mstate m) {
  /* Establish circular links for smallbins */
  bindex_t i;
  for (i = 0; i < NSMALLBINS; ++i) {
    sbinptr bin = smallbin_at(m, i);
    bin->fd = bin->bk = bin;
  }
}

/* Allocate chunk and prepend remainder with chunk in successor base. */
static void *dlmalloc_prepend_alloc(mstate m, char *newbase, char *oldbase,
                                    size_t nb) {
  mchunkptr p = align_as_chunk(newbase);
  mchunkptr oldfirst = align_as_chunk(oldbase);
  size_t psize = (char *)oldfirst - (char *)p;
  mchunkptr q = chunk_plus_offset(p, nb);
  size_t qsize = psize - nb;
  set_size_and_pinuse_of_inuse_chunk(m, p, nb);
  assert((char *)oldfirst > (char *)q);
  assert(pinuse(oldfirst));
  assert(qsize >= MIN_CHUNK_SIZE);
  /* consolidate remainder with first chunk of old base */
  if (oldfirst == m->top) {
    size_t tsize = m->topsize += qsize;
    m->top = q;
    q->head = tsize | PINUSE_BIT;
    check_top_chunk(m, q);
  } else if (oldfirst == m->dv) {
    size_t dsize = m->dvsize += qsize;
    m->dv = q;
    set_size_and_pinuse_of_free_chunk(q, dsize);
  } else {
    if (!is_inuse(oldfirst)) {
      size_t nsize = chunksize(oldfirst);
      unlink_chunk(m, oldfirst, nsize);
      oldfirst = chunk_plus_offset(oldfirst, nsize);
      qsize += nsize;
    }
    set_free_with_pinuse(q, qsize, oldfirst);
    insert_chunk(m, q, qsize);
    check_free_chunk(m, q);
  }
  check_malloced_chunk(m, chunk2mem(p), nb);
  return chunk2mem(p);
}

/* Add a segment to hold a new noncontiguous region */
static void dlmalloc_add_segment(mstate m, char *tbase, size_t tsize,
                                 flag_t mmapped) {
  /* Determine locations and sizes of segment, fenceposts, old top */
  char *old_top = (char *)m->top;
  msegmentptr oldsp = segment_holding(m, old_top);
  char *old_end = oldsp->base + oldsp->size;
  size_t ssize = pad_request(sizeof(struct malloc_segment));
  char *rawsp = old_end - (ssize + FOUR_SIZE_T_SIZES + CHUNK_ALIGN_MASK);
  size_t offset = align_offset(chunk2mem(rawsp));
  char *asp = rawsp + offset;
  char *csp = (asp < (old_top + MIN_CHUNK_SIZE)) ? old_top : asp;
  mchunkptr sp = (mchunkptr)csp;
  msegmentptr ss = (msegmentptr)(chunk2mem(sp));
  mchunkptr tnext = chunk_plus_offset(sp, ssize);
  mchunkptr p = tnext;
  int nfences = 0;
  /* reset top to new space */
  dlmalloc_init_top(m, (mchunkptr)tbase, tsize - TOP_FOOT_SIZE);
  /* Set up segment record */
  assert(is_aligned(ss));
  set_size_and_pinuse_of_inuse_chunk(m, sp, ssize);
  *ss = m->seg; /* Push current record */
  m->seg.base = tbase;
  m->seg.size = tsize;
  m->seg.sflags = mmapped;
  m->seg.next = ss;
  /* Insert trailing fenceposts */
  for (;;) {
    mchunkptr nextp = chunk_plus_offset(p, SIZE_T_SIZE);
    p->head = FENCEPOST_HEAD;
    ++nfences;
    if ((char *)(&(nextp->head)) < old_end)
      p = nextp;
    else
      break;
  }
  assert(nfences >= 2);
  /* Insert the rest of old top into a bin as an ordinary free chunk */
  if (csp != old_top) {
    mchunkptr q = (mchunkptr)old_top;
    size_t psize = csp - old_top;
    mchunkptr tn = chunk_plus_offset(q, psize);
    set_free_with_pinuse(q, psize, tn);
    insert_chunk(m, q, psize);
  }
  check_top_chunk(m, m->top);
}

/* ─────────────────────────── system integration ─────────────────────────── */

/* Return true if segment contains a segment link */
static int has_segment_link(mstate m, msegmentptr ss) {
  msegmentptr sp = &m->seg;
  for (;;) {
    if ((char *)sp >= ss->base && (char *)sp < ss->base + ss->size) return 1;
    if ((sp = sp->next) == 0) return 0;
  }
}

/*
  Directly mmapped chunks are set up with an offset to the start of
  the mmapped region stored in the prev_foot field of the chunk. This
  allows reconstruction of the required argument to MUNMAP when freed,
  and also allows adjustment of the returned chunk to meet alignment
  requirements (especially in memalign).
*/

/* For sys_alloc, enough padding to ensure can malloc request on success */
#define SYS_ALLOC_PADDING (TOP_FOOT_SIZE + MALLOC_ALIGNMENT)

/* Malloc using mmap */
static void *mmap_alloc(mstate m, size_t nb) {
  size_t mmsize = mmap_align(nb + SIX_SIZE_T_SIZES + CHUNK_ALIGN_MASK);
  if (m->footprint_limit != 0) {
    size_t fp = m->footprint + mmsize;
    if (fp <= m->footprint || fp > m->footprint_limit) return 0;
  }
  if (mmsize > nb) { /* Check for wrap around 0 */
    char *mm = (char *)(dlmalloc_requires_more_vespene_gas(mmsize));
    if (mm != CMFAIL) {
      size_t offset = align_offset(chunk2mem(mm));
      size_t psize = mmsize - offset - MMAP_FOOT_PAD;
      mchunkptr p = (mchunkptr)(mm + offset);
      p->prev_foot = offset;
      p->head = psize;
      mark_inuse_foot(m, p, psize);
      chunk_plus_offset(p, psize)->head = FENCEPOST_HEAD;
      chunk_plus_offset(p, psize + SIZE_T_SIZE)->head = 0;
      if (m->least_addr == 0 || mm < m->least_addr) m->least_addr = mm;
      if ((m->footprint += mmsize) > m->max_footprint)
        m->max_footprint = m->footprint;
      assert(is_aligned(chunk2mem(p)));
      check_mmapped_chunk(m, p);
      return chunk2mem(p);
    }
  }
  return 0;
}

/**
 * Gets memory from system.
 */
static void *dlmalloc_sys_alloc(mstate m, size_t nb) {
  char *tbase = CMFAIL;
  size_t tsize = 0;
  flag_t mmap_flag = 0;
  size_t asize; /* allocation size */

  ensure_initialization();

  /* Directly map large chunks, but only if already initialized */
  if (use_mmap(m) && nb >= g_mparams.mmap_threshold && m->topsize != 0) {
    void *mem = mmap_alloc(m, nb);
    if (mem != 0) return mem;
  }

  asize = granularity_align(nb + SYS_ALLOC_PADDING);
  if (asize <= nb) return 0; /* wraparound */
  if (m->footprint_limit != 0) {
    size_t fp = m->footprint + asize;
    if (fp <= m->footprint || fp > m->footprint_limit) return 0;
  }

  if (HAVE_MMAP && tbase == CMFAIL) { /* Try MMAP */
    char *mp = (char *)(dlmalloc_requires_more_vespene_gas(asize));
    if (mp != CMFAIL) {
      tbase = mp;
      tsize = asize;
      mmap_flag = USE_MMAP_BIT;
    }
  }

  if (tbase != CMFAIL) {
    if ((m->footprint += tsize) > m->max_footprint)
      m->max_footprint = m->footprint;

    if (!is_initialized(m)) { /* first-time initialization */
      if (m->least_addr == 0 || tbase < m->least_addr) m->least_addr = tbase;
      m->seg.base = tbase;
      m->seg.size = tsize;
      m->seg.sflags = mmap_flag;
      m->magic = g_mparams.magic;
      m->release_checks = MAX_RELEASE_CHECK_RATE;
      init_bins(m);
      if (is_global(m)) {
        dlmalloc_init_top(m, (mchunkptr)tbase, tsize - TOP_FOOT_SIZE);
      } else {
        /* Offset top by embedded MallocState */
        mchunkptr mn = next_chunk(mem2chunk(m));
        dlmalloc_init_top(
            m, mn, (size_t)((tbase + tsize) - (char *)mn) - TOP_FOOT_SIZE);
      }
    }

    else {
      /* Try to merge with an existing segment */
      msegmentptr sp = &m->seg;
      /* Only consider most recent segment if traversal suppressed */
      while (sp != 0 && tbase != sp->base + sp->size)
        sp = (NO_SEGMENT_TRAVERSAL) ? 0 : sp->next;
      if (sp != 0 && !is_extern_segment(sp) &&
          (sp->sflags & USE_MMAP_BIT) == mmap_flag &&
          segment_holds(sp, m->top)) { /* append */
        sp->size += tsize;
        dlmalloc_init_top(m, m->top, m->topsize + tsize);
      } else {
        if (tbase < m->least_addr) m->least_addr = tbase;
        sp = &m->seg;
        while (sp != 0 && sp->base != tbase + tsize)
          sp = (NO_SEGMENT_TRAVERSAL) ? 0 : sp->next;
        if (sp != 0 && !is_extern_segment(sp) &&
            (sp->sflags & USE_MMAP_BIT) == mmap_flag) {
          char *oldbase = sp->base;
          sp->base = tbase;
          sp->size += tsize;
          return dlmalloc_prepend_alloc(m, tbase, oldbase, nb);
        } else
          dlmalloc_add_segment(m, tbase, tsize, mmap_flag);
      }
    }

    if (nb < m->topsize) { /* Allocate from new or extended top space */
      size_t rsize = m->topsize -= nb;
      mchunkptr p = m->top;
      mchunkptr r = m->top = chunk_plus_offset(p, nb);
      r->head = rsize | PINUSE_BIT;
      set_size_and_pinuse_of_inuse_chunk(m, p, nb);
      check_top_chunk(m, m->top);
      check_malloced_chunk(m, chunk2mem(p), nb);
      return chunk2mem(p);
    }
  }

  enomem();
  return 0;
}

/* Unmap and unlink any mmapped segments that don't contain used chunks */
static size_t dlmalloc_release_unused_segments(mstate m) {
  size_t released = 0;
  int nsegs = 0;
  msegmentptr pred = &m->seg;
  msegmentptr sp = pred->next;
  while (sp != 0) {
    char *base = sp->base;
    size_t size = sp->size;
    msegmentptr next = sp->next;
    ++nsegs;
    if (is_mmapped_segment(sp) && !is_extern_segment(sp)) {
      mchunkptr p = align_as_chunk(base);
      size_t psize = chunksize(p);
      /* Can unmap if first chunk holds entire segment and not pinned */
      if (!is_inuse(p) && (char *)p + psize >= base + size - TOP_FOOT_SIZE) {
        tchunkptr tp = (tchunkptr)p;
        assert(segment_holds(sp, (char *)sp));
        if (p == m->dv) {
          m->dv = 0;
          m->dvsize = 0;
        } else {
          unlink_large_chunk(m, tp);
        }
        if (munmap(base, size) == 0) {
          released += size;
          m->footprint -= size;
          /* unlink obsoleted record */
          sp = pred;
          sp->next = next;
        } else { /* back out if cannot unmap */
          insert_large_chunk(m, tp, psize);
        }
      }
    }
    if (NO_SEGMENT_TRAVERSAL) { /* scan only first segment */
      break;
    }
    pred = sp;
    sp = next;
  }
  /* Reset check counter */
  m->release_checks = (((size_t)nsegs > (size_t)MAX_RELEASE_CHECK_RATE)
                           ? (size_t)nsegs
                           : (size_t)MAX_RELEASE_CHECK_RATE);
  return released;
}

int dlmalloc_sys_trim(mstate m, size_t pad) {
  size_t released = 0;
  ensure_initialization();
  if (pad < MAX_REQUEST && is_initialized(m)) {
    pad += TOP_FOOT_SIZE; /* ensure enough room for segment overhead */
    if (m->topsize > pad) {
      /* Shrink top space in granularity-size units, keeping at least one */
      size_t unit = g_mparams.granularity;
      size_t extra =
          ((m->topsize - pad + (unit - SIZE_T_ONE)) / unit - SIZE_T_ONE) * unit;
      msegmentptr sp = segment_holding(m, (char *)m->top);
      if (!is_extern_segment(sp)) {
        if (is_mmapped_segment(sp)) {
          if (HAVE_MMAP && sp->size >= extra &&
              !has_segment_link(m, sp)) { /* can't shrink if pinned */
            size_t newsize = sp->size - extra;
            (void)newsize; /* placate people compiling -Wunused-variable */
            /* Prefer mremap, fall back to munmap */
            if ((mremap(sp->base, sp->size, newsize, 0, 0) != MAP_FAILED) ||
                (munmap(sp->base + newsize, extra) == 0)) {
              released = extra;
            }
          }
        }
      }
      if (released != 0) {
        sp->size -= released;
        m->footprint -= released;
        dlmalloc_init_top(m, m->top, m->topsize - released);
        check_top_chunk(m, m->top);
      }
    }
    /* Unmap any unused mmapped segments */
    if (HAVE_MMAP) released += dlmalloc_release_unused_segments(m);
    /* On failure, disable autotrim to avoid repeated failed future calls */
    if (released == 0 && m->topsize > m->trim_check) m->trim_check = SIZE_MAX;
  }
  return (released != 0) ? 1 : 0;
}

/* ──────────────────────────── setting mparams ────────────────────────── */

#if LOCK_AT_FORK
static void pre_fork(void) {
  ACQUIRE_LOCK(&(g_dlmalloc)->mutex);
}
static void post_fork_parent(void) {
  RELEASE_LOCK(&(g_dlmalloc)->mutex);
}
static void post_fork_child(void) {
  INITIAL_LOCK(&(g_dlmalloc)->mutex);
}
#endif /* LOCK_AT_FORK */

/* ───────────────────────────── statistics ────────────────────────────── */

/* Consolidate and bin a chunk. Differs from exported versions
   of free mainly in that the chunk need not be marked as inuse.
*/
void dlmalloc_dispose_chunk(mstate m, mchunkptr p, size_t psize) {
  mchunkptr next = chunk_plus_offset(p, psize);
  if (!pinuse(p)) {
    mchunkptr prev;
    size_t prevsize = p->prev_foot;
    if (is_mmapped(p)) {
      psize += prevsize + MMAP_FOOT_PAD;
      if (munmap((char *)p - prevsize, psize) == 0) m->footprint -= psize;
      return;
    }
    prev = chunk_minus_offset(p, prevsize);
    psize += prevsize;
    p = prev;
    if (RTCHECK(ok_address(m, prev))) { /* consolidate backward */
      if (p != m->dv) {
        unlink_chunk(m, p, prevsize);
      } else if ((next->head & INUSE_BITS) == INUSE_BITS) {
        m->dvsize = psize;
        set_free_with_pinuse(p, psize, next);
        return;
      }
    } else {
      CORRUPTION_ERROR_ACTION(m);
      return;
    }
  }
  if (RTCHECK(ok_address(m, next))) {
    if (!cinuse(next)) { /* consolidate forward */
      if (next == m->top) {
        size_t tsize = m->topsize += psize;
        m->top = p;
        p->head = tsize | PINUSE_BIT;
        if (p == m->dv) {
          m->dv = 0;
          m->dvsize = 0;
        }
        return;
      } else if (next == m->dv) {
        size_t dsize = m->dvsize += psize;
        m->dv = p;
        set_size_and_pinuse_of_free_chunk(p, dsize);
        return;
      } else {
        size_t nsize = chunksize(next);
        psize += nsize;
        unlink_chunk(m, next, nsize);
        set_size_and_pinuse_of_free_chunk(p, psize);
        if (p == m->dv) {
          m->dvsize = psize;
          return;
        }
      }
    } else {
      set_free_with_pinuse(p, psize, next);
    }
    insert_chunk(m, p, psize);
  } else {
    CORRUPTION_ERROR_ACTION(m);
  }
}

/* ──────────────────────────── malloc ─────────────────────────── */

/* allocate a small request from the best fitting chunk in a treebin */
static void *tmalloc_small(mstate m, size_t nb) {
  tchunkptr t, v;
  size_t rsize;
  bindex_t i;
  binmap_t leastbit = least_bit(m->treemap);
  compute_bit2idx(leastbit, i);
  v = t = *treebin_at(m, i);
  rsize = chunksize(t) - nb;
  while ((t = leftmost_child(t)) != 0) {
    size_t trem = chunksize(t) - nb;
    if (trem < rsize) {
      rsize = trem;
      v = t;
    }
  }
  if (RTCHECK(ok_address(m, v))) {
    mchunkptr r = chunk_plus_offset(v, nb);
    assert(chunksize(v) == rsize + nb);
    if (RTCHECK(ok_next(v, r))) {
      unlink_large_chunk(m, v);
      if (rsize < MIN_CHUNK_SIZE)
        set_inuse_and_pinuse(m, v, (rsize + nb));
      else {
        set_size_and_pinuse_of_inuse_chunk(m, v, nb);
        set_size_and_pinuse_of_free_chunk(r, rsize);
        replace_dv(m, r, rsize);
      }
      return chunk2mem(v);
    }
  }
  CORRUPTION_ERROR_ACTION(m);
  return 0;
}

/* allocate a large request from the best fitting chunk in a treebin */
static void *tmalloc_large(mstate m, size_t nb) {
  tchunkptr v = 0;
  size_t rsize = -nb; /* Unsigned negation */
  tchunkptr t;
  bindex_t idx;
  compute_tree_index(nb, idx);
  if ((t = *treebin_at(m, idx)) != 0) {
    /* Traverse tree for this bin looking for node with size == nb */
    size_t sizebits = nb << leftshift_for_tree_index(idx);
    tchunkptr rst = 0; /* The deepest untaken right subtree */
    for (;;) {
      tchunkptr rt;
      size_t trem = chunksize(t) - nb;
      if (trem < rsize) {
        v = t;
        if ((rsize = trem) == 0) break;
      }
      rt = t->child[1];
      t = t->child[(sizebits >> (SIZE_T_BITSIZE - SIZE_T_ONE)) & 1];
      if (rt != 0 && rt != t) rst = rt;
      if (t == 0) {
        t = rst; /* set t to least subtree holding sizes > nb */
        break;
      }
      sizebits <<= 1;
    }
  }
  if (t == 0 && v == 0) { /* set t to root of next non-empty treebin */
    binmap_t leftbits = left_bits(idx2bit(idx)) & m->treemap;
    if (leftbits != 0) {
      bindex_t i;
      binmap_t leastbit = least_bit(leftbits);
      compute_bit2idx(leastbit, i);
      t = *treebin_at(m, i);
    }
  }
  while (t != 0) { /* find smallest of tree or subtree */
    size_t trem = chunksize(t) - nb;
    if (trem < rsize) {
      rsize = trem;
      v = t;
    }
    t = leftmost_child(t);
  }
  /*  If dv is a better fit, return 0 so malloc will use it */
  if (v != 0 && rsize < (size_t)(m->dvsize - nb)) {
    if (RTCHECK(ok_address(m, v))) { /* split */
      mchunkptr r = chunk_plus_offset(v, nb);
      assert(chunksize(v) == rsize + nb);
      if (RTCHECK(ok_next(v, r))) {
        unlink_large_chunk(m, v);
        if (rsize < MIN_CHUNK_SIZE)
          set_inuse_and_pinuse(m, v, (rsize + nb));
        else {
          set_size_and_pinuse_of_inuse_chunk(m, v, nb);
          set_size_and_pinuse_of_free_chunk(r, rsize);
          insert_chunk(m, r, rsize);
        }
        return chunk2mem(v);
      }
    }
    CORRUPTION_ERROR_ACTION(m);
  }
  return 0;
}

void *dlmalloc(size_t bytes) {
  /*
     Basic algorithm:
     If a small request (< 256 bytes minus per-chunk overhead):
       1. If one exists, use a remainderless chunk in associated smallbin.
          (Remainderless means that there are too few excess bytes to
          represent as a chunk.)
       2. If it is big enough, use the dv chunk, which is normally the
          chunk adjacent to the one used for the most recent small request.
       3. If one exists, split the smallest available chunk in a bin,
          saving remainder in dv.
       4. If it is big enough, use the top chunk.
       5. If available, get memory from system and use it
     Otherwise, for a large request:
       1. Find the smallest available binned chunk that fits, and use it
          if it is better fitting than dv chunk, splitting if necessary.
       2. If better fitting than any binned chunk, use the dv chunk.
       3. If it is big enough, use the top chunk.
       4. If request size >= mmap threshold, try to directly mmap this chunk.
       5. If available, get memory from system and use it

     The ugly goto's here ensure that postaction occurs along all paths.
  */

#if USE_LOCKS
  ensure_initialization(); /* initialize in sys_alloc if not using locks */
#endif

  if (!PREACTION(g_dlmalloc)) {
    void *mem;
    size_t nb;
    if (bytes <= MAX_SMALL_REQUEST) {
      bindex_t idx;
      binmap_t smallbits;
      nb = (bytes < MIN_REQUEST) ? MIN_CHUNK_SIZE : pad_request(bytes);
      idx = small_index(nb);
      smallbits = g_dlmalloc->smallmap >> idx;

      if ((smallbits & 0x3U) != 0) { /* Remainderless fit to a smallbin. */
        mchunkptr b, p;
        idx += ~smallbits & 1; /* Uses next bin if idx empty */
        b = smallbin_at(g_dlmalloc, idx);
        p = b->fd;
        assert(chunksize(p) == small_index2size(idx));
        unlink_first_small_chunk(g_dlmalloc, b, p, idx);
        set_inuse_and_pinuse(g_dlmalloc, p, small_index2size(idx));
        mem = chunk2mem(p);
        check_malloced_chunk(g_dlmalloc, mem, nb);
        goto postaction;
      }

      else if (nb > g_dlmalloc->dvsize) {
        if (smallbits != 0) { /* Use chunk in next nonempty smallbin */
          mchunkptr b, p, r;
          size_t rsize;
          bindex_t i;
          binmap_t leftbits = (smallbits << idx) & left_bits(idx2bit(idx));
          binmap_t leastbit = least_bit(leftbits);
          compute_bit2idx(leastbit, i);
          b = smallbin_at(g_dlmalloc, i);
          p = b->fd;
          assert(chunksize(p) == small_index2size(i));
          unlink_first_small_chunk(g_dlmalloc, b, p, i);
          rsize = small_index2size(i) - nb;
          /* Fit here cannot be remainderless if 4byte sizes */
          if (SIZE_T_SIZE != 4 && rsize < MIN_CHUNK_SIZE)
            set_inuse_and_pinuse(g_dlmalloc, p, small_index2size(i));
          else {
            set_size_and_pinuse_of_inuse_chunk(g_dlmalloc, p, nb);
            r = chunk_plus_offset(p, nb);
            set_size_and_pinuse_of_free_chunk(r, rsize);
            replace_dv(g_dlmalloc, r, rsize);
          }
          mem = chunk2mem(p);
          check_malloced_chunk(g_dlmalloc, mem, nb);
          goto postaction;
        }

        else if (g_dlmalloc->treemap != 0 &&
                 (mem = tmalloc_small(g_dlmalloc, nb)) != 0) {
          check_malloced_chunk(g_dlmalloc, mem, nb);
          goto postaction;
        }
      }
    } else if (bytes >= MAX_REQUEST) {
      nb = SIZE_MAX; /* Too big to allocate. Force failure (in sys alloc) */
    } else {
      nb = pad_request(bytes);
      if (g_dlmalloc->treemap != 0 &&
          (mem = tmalloc_large(g_dlmalloc, nb)) != 0) {
        check_malloced_chunk(g_dlmalloc, mem, nb);
        goto postaction;
      }
    }

    if (nb <= g_dlmalloc->dvsize) {
      size_t rsize = g_dlmalloc->dvsize - nb;
      mchunkptr p = g_dlmalloc->dv;
      if (rsize >= MIN_CHUNK_SIZE) { /* split dv */
        mchunkptr r = g_dlmalloc->dv = chunk_plus_offset(p, nb);
        g_dlmalloc->dvsize = rsize;
        set_size_and_pinuse_of_free_chunk(r, rsize);
        set_size_and_pinuse_of_inuse_chunk(g_dlmalloc, p, nb);
      } else { /* exhaust dv */
        size_t dvs = g_dlmalloc->dvsize;
        g_dlmalloc->dvsize = 0;
        g_dlmalloc->dv = 0;
        set_inuse_and_pinuse(g_dlmalloc, p, dvs);
      }
      mem = chunk2mem(p);
      check_malloced_chunk(g_dlmalloc, mem, nb);
      goto postaction;
    }

    else if (nb < g_dlmalloc->topsize) { /* Split top */
      size_t rsize = g_dlmalloc->topsize -= nb;
      mchunkptr p = g_dlmalloc->top;
      mchunkptr r = g_dlmalloc->top = chunk_plus_offset(p, nb);
      r->head = rsize | PINUSE_BIT;
      set_size_and_pinuse_of_inuse_chunk(g_dlmalloc, p, nb);
      mem = chunk2mem(p);
      check_top_chunk(g_dlmalloc, g_dlmalloc->top);
      check_malloced_chunk(g_dlmalloc, mem, nb);
      goto postaction;
    }

    mem = dlmalloc_sys_alloc(g_dlmalloc, nb);

  postaction:
    POSTACTION(g_dlmalloc);
    return ADDRESS_BIRTH_ACTION(mem);
  }

  return 0;
}

void dlfree(void *mem) {
  /*
     Consolidate freed chunks with preceeding or succeeding bordering
     free chunks, if they exist, and then place in a bin.  Intermixed
     with special cases for top, dv, mmapped chunks, and usage errors.
  */
  if (mem != 0) {
    mem = ADDRESS_DEATH_ACTION(mem);
    mchunkptr p = mem2chunk(mem);

#if FOOTERS
    mstate fm = get_mstate_for(p);
    if (!ok_magic(fm)) { /* HELLO
                          * TRY #1: rm -rf o && make -j8 -O MODE=dbg
                          * TRY #2: gdb: p/x (long*)(p+(*(long*)(p-8)&~(1|2|3)))
                          *         gdb: watch *0xDEADBEEF
                          */
      USAGE_ERROR_ACTION(fm, p);
      return;
    }
#else /* FOOTERS */
#define fm g_dlmalloc
#endif /* FOOTERS */

    if (!PREACTION(fm)) {
      check_inuse_chunk(fm, p);
      if (RTCHECK(ok_address(fm, p) && ok_inuse(p))) {
        size_t psize = chunksize(p);
        mchunkptr next = chunk_plus_offset(p, psize);
        if (!pinuse(p)) {
          size_t prevsize = p->prev_foot;
          if (is_mmapped(p)) {
            psize += prevsize + MMAP_FOOT_PAD;
            if (munmap((char *)p - prevsize, psize) == 0)
              fm->footprint -= psize;
            goto postaction;
          } else {
            mchunkptr prev = chunk_minus_offset(p, prevsize);
            psize += prevsize;
            p = prev;
            if (RTCHECK(ok_address(fm, prev))) { /* consolidate backward */
              if (p != fm->dv) {
                unlink_chunk(fm, p, prevsize);
              } else if ((next->head & INUSE_BITS) == INUSE_BITS) {
                fm->dvsize = psize;
                set_free_with_pinuse(p, psize, next);
                goto postaction;
              }
            } else
              goto erroraction;
          }
        }

        if (RTCHECK(ok_next(p, next) && ok_pinuse(next))) {
          if (!cinuse(next)) { /* consolidate forward */
            if (next == fm->top) {
              size_t tsize = fm->topsize += psize;
              fm->top = p;
              p->head = tsize | PINUSE_BIT;
              if (p == fm->dv) {
                fm->dv = 0;
                fm->dvsize = 0;
              }
              if (should_trim(fm, tsize)) dlmalloc_sys_trim(fm, 0);
              goto postaction;
            } else if (next == fm->dv) {
              size_t dsize = fm->dvsize += psize;
              fm->dv = p;
              set_size_and_pinuse_of_free_chunk(p, dsize);
              goto postaction;
            } else {
              size_t nsize = chunksize(next);
              psize += nsize;
              unlink_chunk(fm, next, nsize);
              set_size_and_pinuse_of_free_chunk(p, psize);
              if (p == fm->dv) {
                fm->dvsize = psize;
                goto postaction;
              }
            }
          } else {
            set_free_with_pinuse(p, psize, next);
          }

          if (is_small(psize)) {
            insert_small_chunk(fm, p, psize);
            check_free_chunk(fm, p);
          } else {
            tchunkptr tp = (tchunkptr)p;
            insert_large_chunk(fm, tp, psize);
            check_free_chunk(fm, p);
            if (--fm->release_checks == 0) dlmalloc_release_unused_segments(fm);
          }
          goto postaction;
        }
      }
    erroraction:
      USAGE_ERROR_ACTION(fm, p);
    postaction:
      POSTACTION(fm);
    }
  }
#if !FOOTERS
#undef fm
#endif /* FOOTERS */
}

textstartup void dlmalloc_init(void) {
#ifdef NEED_GLOBAL_LOCK_INIT
  if (malloc_global_mutex_status <= 0) init_malloc_global_mutex();
#endif
  ACQUIRE_MALLOC_GLOBAL_LOCK();
  if (g_mparams.magic == 0) {
    size_t magic;
    size_t psize = PAGESIZE;
    size_t gsize = MAX(g_ntsysteminfo.dwAllocationGranularity, 64 * 1024);
    /* Sanity-check configuration:
       size_t must be unsigned and as wide as pointer type.
       ints must be at least 4 bytes.
       alignment must be at least 8.
       Alignment, min chunk size, and page size must all be powers of 2.
    */
    if ((sizeof(size_t) != sizeof(char *)) || (SIZE_MAX < MIN_CHUNK_SIZE) ||
        (sizeof(int) < 4) || (MALLOC_ALIGNMENT < (size_t)8U) ||
        ((MALLOC_ALIGNMENT & (MALLOC_ALIGNMENT - SIZE_T_ONE)) != 0) ||
        ((MCHUNK_SIZE & (MCHUNK_SIZE - SIZE_T_ONE)) != 0) ||
        ((gsize & (gsize - SIZE_T_ONE)) != 0) ||
        ((psize & (psize - SIZE_T_ONE)) != 0))
      MALLOC_ABORT;
    g_mparams.granularity = gsize;
    g_mparams.page_size = psize;
    g_mparams.mmap_threshold = DEFAULT_MMAP_THRESHOLD;
    g_mparams.trim_threshold = DEFAULT_TRIM_THRESHOLD;
    g_mparams.default_mflags =
        USE_LOCK_BIT | USE_MMAP_BIT | USE_NONCONTIGUOUS_BIT;
    /* Set up lock for main malloc area */
    g_dlmalloc->mflags = g_mparams.default_mflags;
    (void)INITIAL_LOCK(&g_dlmalloc->mutex);
#if LOCK_AT_FORK
    pthread_atfork(&pre_fork, &post_fork_parent, &post_fork_child);
#endif
    magic = kStartTsc;
    magic |= (size_t)8U;  /* ensure nonzero */
    magic &= ~(size_t)7U; /* improve chances of fault for bad values */
    /* Until memory modes commonly available, use volatile-write */
    (*(volatile size_t *)(&(g_mparams.magic))) = magic;
  }
  RELEASE_MALLOC_GLOBAL_LOCK();
}
