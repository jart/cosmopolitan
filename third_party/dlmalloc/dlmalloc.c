#include "third_party/dlmalloc/dlmalloc.h"
#include "libc/assert.h"
#include "libc/atomic.h"
#include "libc/calls/calls.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/intrin/atomic.h"
#include "libc/intrin/bsr.h"
#include "libc/intrin/likely.h"
#include "libc/intrin/weaken.h"
#include "libc/macros.internal.h"
#include "libc/mem/mem.h"
#include "libc/nexgen32e/rdtsc.h"
#include "libc/runtime/internal.h"
#include "libc/runtime/runtime.h"
#include "libc/runtime/sysconf.h"
#include "libc/stdckdint.h"
#include "libc/stdio/rand.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/prot.h"
#include "libc/thread/thread.h"
#include "libc/thread/tls.h"
#include "third_party/dlmalloc/vespene.internal.h"
#include "third_party/nsync/mu.h"

#define FOOTERS 0
#define MSPACES 0

#define HAVE_MMAP 1
#define HAVE_MREMAP 0
#define HAVE_MORECORE 0
#define USE_LOCKS 2
#define USE_SPIN_LOCKS 0
#define MORECORE_CONTIGUOUS 0
#define MALLOC_INSPECT_ALL 1
#define ABORT_ON_ASSERT_FAILURE 0
#define LOCK_AT_FORK 1
#define NO_MALLOC_STATS 1

#if IsTiny()
#define INSECURE 1
#define PROCEED_ON_ERROR 1
#endif

#if IsModeDbg()
#define DEBUG 1
#endif

#undef assert
#if IsTiny()
#define assert(x) if(!(x)) __builtin_unreachable()
#else
#define assert(x) if(!(x)) ABORT
#endif

#include "third_party/dlmalloc/platform.inc"
#include "third_party/dlmalloc/locks.inc"
#include "third_party/dlmalloc/chunks.inc"
#include "third_party/dlmalloc/headfoot.inc"
#include "third_party/dlmalloc/global.inc"
#include "third_party/dlmalloc/system.inc"
#include "third_party/dlmalloc/hooks.inc"
#include "third_party/dlmalloc/debugging.inc"
#include "third_party/dlmalloc/indexing.inc"
#include "third_party/dlmalloc/binmaps.inc"
#include "third_party/dlmalloc/runtimechecks.inc"
#include "third_party/dlmalloc/init.inc"
#include "third_party/dlmalloc/debuglib.inc"
#include "third_party/dlmalloc/statistics.inc"
#include "third_party/dlmalloc/smallbins.inc"
#include "third_party/dlmalloc/directmap.inc"
#include "third_party/dlmalloc/trees.inc"
#include "third_party/dlmalloc/management.inc"

/* -------------------------- System allocation -------------------------- */

/* Get memory from system using MORECORE or MMAP */
static void* sys_alloc(mstate m, size_t nb) {
  char* tbase = CMFAIL;
  size_t tsize = 0;
  flag_t mmap_flag = 0;
  size_t asize; /* allocation size */

  ensure_initialization();

  /* Directly map large chunks, but only if already initialized */
  if (use_mmap(m) && nb >= mparams.mmap_threshold && m->topsize != 0) {
    void* mem = mmap_alloc(m, nb);
    if (mem != 0)
      return mem;
  }

  asize = granularity_align(nb + SYS_ALLOC_PADDING);
  if (asize <= nb)
    return 0; /* wraparound */
  if (m->footprint_limit != 0) {
    size_t fp = m->footprint + asize;
    if (fp <= m->footprint || fp > m->footprint_limit)
      return 0;
  }

  /*
    Try getting memory in any of three ways (in most-preferred to
    least-preferred order):
    1. A call to MORECORE that can normally contiguously extend memory.
       (disabled if not MORECORE_CONTIGUOUS or not HAVE_MORECORE or
       or main space is mmapped or a previous contiguous call failed)
    2. A call to MMAP new space (disabled if not HAVE_MMAP).
       Note that under the default settings, if MORECORE is unable to
       fulfill a request, and HAVE_MMAP is true, then mmap is
       used as a noncontiguous system allocator. This is a useful backup
       strategy for systems with holes in address spaces -- in this case
       sbrk cannot contiguously expand the heap, but mmap may be able to
       find space.
    3. A call to MORECORE that cannot usually contiguously extend memory.
       (disabled if not HAVE_MORECORE)

   In all cases, we need to request enough bytes from system to ensure
   we can malloc nb bytes upon success, so pad with enough space for
   top_foot, plus alignment-pad to make sure we don't lose bytes if
   not on boundary, and round this up to a granularity unit.
  */

  if (MORECORE_CONTIGUOUS && !use_noncontiguous(m)) {
    char* br = CMFAIL;
    size_t ssize = asize; /* sbrk call size */
    msegmentptr ss = (m->top == 0)? 0 : segment_holding(m, (char*)m->top);
    ACQUIRE_MALLOC_GLOBAL_LOCK();

    if (ss == 0) {  /* First time through or recovery */
      char* base = (char*)CALL_MORECORE(0);
      if (base != CMFAIL) {
        size_t fp;
        /* Adjust to end on a page boundary */
        if (!is_page_aligned(base))
          ssize += (page_align((size_t)base) - (size_t)base);
        fp = m->footprint + ssize; /* recheck limits */
        if (ssize > nb && ssize < HALF_MAX_SIZE_T &&
            (m->footprint_limit == 0 ||
             (fp > m->footprint && fp <= m->footprint_limit)) &&
            (br = (char*)(CALL_MORECORE(ssize))) == base) {
          tbase = base;
          tsize = ssize;
        }
      }
    }
    else {
      /* Subtract out existing available top space from MORECORE request. */
      ssize = granularity_align(nb - m->topsize + SYS_ALLOC_PADDING);
      /* Use mem here only if it did continuously extend old space */
      if (ssize < HALF_MAX_SIZE_T &&
          (br = (char*)(CALL_MORECORE(ssize))) == ss->base+ss->size) {
        tbase = br;
        tsize = ssize;
      }
    }

    if (tbase == CMFAIL) {    /* Cope with partial failure */
      if (br != CMFAIL) {    /* Try to use/extend the space we did get */
        if (ssize < HALF_MAX_SIZE_T &&
            ssize < nb + SYS_ALLOC_PADDING) {
          size_t esize = granularity_align(nb + SYS_ALLOC_PADDING - ssize);
          if (esize < HALF_MAX_SIZE_T) {
            char* end = (char*)CALL_MORECORE(esize);
            if (end != CMFAIL)
              ssize += esize;
            else {            /* Can't use; try to release */
              (void) CALL_MORECORE(-ssize);
              br = CMFAIL;
            }
          }
        }
      }
      if (br != CMFAIL) {    /* Use the space we did get */
        tbase = br;
        tsize = ssize;
      }
      else
        disable_contiguous(m); /* Don't try contiguous path in the future */
    }

    RELEASE_MALLOC_GLOBAL_LOCK();
  }

  if (HAVE_MMAP && tbase == CMFAIL) {  /* Try MMAP */
    char* mp = (char*)(dlmalloc_requires_more_vespene_gas(asize));
    if (mp != CMFAIL) {
      tbase = mp;
      tsize = asize;
      mmap_flag = USE_MMAP_BIT;
    }
  }

  if (HAVE_MORECORE && tbase == CMFAIL) { /* Try noncontiguous MORECORE */
    if (asize < HALF_MAX_SIZE_T) {
      char* br = CMFAIL;
      char* end = CMFAIL;
      ACQUIRE_MALLOC_GLOBAL_LOCK();
      br = (char*)(CALL_MORECORE(asize));
      end = (char*)(CALL_MORECORE(0));
      RELEASE_MALLOC_GLOBAL_LOCK();
      if (br != CMFAIL && end != CMFAIL && br < end) {
        size_t ssize = end - br;
        if (ssize > nb + TOP_FOOT_SIZE) {
          tbase = br;
          tsize = ssize;
        }
      }
    }
  }

  if (tbase != CMFAIL) {

    if ((m->footprint += tsize) > m->max_footprint)
      m->max_footprint = m->footprint;

    if (!is_initialized(m)) { /* first-time initialization */
      if (m->least_addr == 0 || tbase < m->least_addr)
        m->least_addr = tbase;
      m->seg.base = tbase;
      m->seg.size = tsize;
      m->seg.sflags = mmap_flag;
      m->magic = mparams.magic;
      m->release_checks = MAX_RELEASE_CHECK_RATE;
      init_bins(m);
#if !ONLY_MSPACES
      if (is_global(m))
        init_top(m, (mchunkptr)tbase, tsize - TOP_FOOT_SIZE);
      else
#endif
      {
        /* Offset top by embedded malloc_state */
        mchunkptr mn = next_chunk(mem2chunk(m));
        init_top(m, mn, (size_t)((tbase + tsize) - (char*)mn) -TOP_FOOT_SIZE);
      }
    }

    else {
      /* Try to merge with an existing segment */
      msegmentptr sp = &m->seg;
      /* Only consider most recent segment if traversal suppressed */
      while (sp != 0 && tbase != sp->base + sp->size)
        sp = (NO_SEGMENT_TRAVERSAL) ? 0 : sp->next;
      if (sp != 0 &&
          !is_extern_segment(sp) &&
          (sp->sflags & USE_MMAP_BIT) == mmap_flag &&
          segment_holds(sp, m->top)) { /* append */
        sp->size += tsize;
        init_top(m, m->top, m->topsize + tsize);
      }
      else {
        if (tbase < m->least_addr)
          m->least_addr = tbase;
        sp = &m->seg;
        while (sp != 0 && sp->base != tbase + tsize)
          sp = (NO_SEGMENT_TRAVERSAL) ? 0 : sp->next;
        if (sp != 0 &&
            !is_extern_segment(sp) &&
            (sp->sflags & USE_MMAP_BIT) == mmap_flag) {
          char* oldbase = sp->base;
          sp->base = tbase;
          sp->size += tsize;
          return prepend_alloc(m, tbase, oldbase, nb);
        }
        else
          add_segment(m, tbase, tsize, mmap_flag);
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

  MALLOC_FAILURE_ACTION;
  return 0;
}

/* -----------------------  system deallocation -------------------------- */

/* Unmap and unlink any mmapped segments that don't contain used chunks */
static size_t release_unused_segments(mstate m) {
  size_t released = 0;
  int nsegs = 0;
  msegmentptr pred = &m->seg;
  msegmentptr sp = pred->next;
  while (sp != 0) {
    char* base = sp->base;
    size_t size = sp->size;
    msegmentptr next = sp->next;
    ++nsegs;
    if (is_mmapped_segment(sp) && !is_extern_segment(sp)) {
      mchunkptr p = align_as_chunk(base);
      size_t psize = chunksize(p);
      /* Can unmap if first chunk holds entire segment and not pinned */
      if (!is_inuse(p) && (char*)p + psize >= base + size - TOP_FOOT_SIZE) {
        tchunkptr tp = (tchunkptr)p;
        assert(segment_holds(sp, (char*)sp));
        if (p == m->dv) {
          m->dv = 0;
          m->dvsize = 0;
        }
        else {
          unlink_large_chunk(m, tp);
        }
        if (CALL_MUNMAP(base, size) == 0) {
          released += size;
          m->footprint -= size;
          /* unlink obsoleted record */
          sp = pred;
          sp->next = next;
        }
        else { /* back out if cannot unmap */
          insert_large_chunk(m, tp, psize);
        }
      }
    }
    if (NO_SEGMENT_TRAVERSAL) /* scan only first segment */
      break;
    pred = sp;
    sp = next;
  }
  /* Reset check counter */
  m->release_checks = (((size_t) nsegs > (size_t) MAX_RELEASE_CHECK_RATE)?
                       (size_t) nsegs : (size_t) MAX_RELEASE_CHECK_RATE);
  return released;
}

static int sys_trim(mstate m, size_t pad) {
  size_t released = 0;
  ensure_initialization();
  if (pad < MAX_REQUEST && is_initialized(m)) {
    pad += TOP_FOOT_SIZE; /* ensure enough room for segment overhead */

    if (m->topsize > pad) {
      /* Shrink top space in granularity-size units, keeping at least one */
      size_t unit = mparams.granularity;
      size_t extra = ((m->topsize - pad + (unit - SIZE_T_ONE)) / unit -
                      SIZE_T_ONE) * unit;
      msegmentptr sp = segment_holding(m, (char*)m->top);

      if (!is_extern_segment(sp)) {
        if (is_mmapped_segment(sp)) {
          if (HAVE_MMAP &&
              sp->size >= extra &&
              !has_segment_link(m, sp)) { /* can't shrink if pinned */
            size_t newsize = sp->size - extra;
            (void)newsize; /* placate people compiling -Wunused-variable */
            /* Prefer mremap, fall back to munmap */
            if (CALL_MREMAP(sp->base, sp->size, newsize, 0) != MFAIL ||
                (!extra || !CALL_MUNMAP(sp->base + newsize, extra))) {
              released = extra;
            }
          }
        }
        else if (HAVE_MORECORE) {
          if (extra >= HALF_MAX_SIZE_T) /* Avoid wrapping negative */
            extra = (HALF_MAX_SIZE_T) + SIZE_T_ONE - unit;
          ACQUIRE_MALLOC_GLOBAL_LOCK();
          {
            /* Make sure end of memory is where we last set it. */
            char* old_br = (char*)(CALL_MORECORE(0));
            if (old_br == sp->base + sp->size) {
              char* rel_br = (char*)(CALL_MORECORE(-extra));
              char* new_br = (char*)(CALL_MORECORE(0));
              if (rel_br != CMFAIL && new_br < old_br)
                released = old_br - new_br;
            }
          }
          RELEASE_MALLOC_GLOBAL_LOCK();
        }
      }

      if (released != 0) {
        sp->size -= released;
        m->footprint -= released;
        init_top(m, m->top, m->topsize - released);
        check_top_chunk(m, m->top);
      }
    }

    /* Unmap any unused mmapped segments */
    if (HAVE_MMAP)
      released += release_unused_segments(m);

    /* On failure, disable autotrim to avoid repeated failed future calls */
    if (released == 0 && m->topsize > m->trim_check)
      m->trim_check = MAX_SIZE_T;
  }

  return (released != 0)? 1 : 0;
}

/* Consolidate and bin a chunk. Differs from exported versions
   of free mainly in that the chunk need not be marked as inuse.
*/
static void dispose_chunk(mstate m, mchunkptr p, size_t psize) {
  mchunkptr next = chunk_plus_offset(p, psize);
  if (!pinuse(p)) {
    mchunkptr prev;
    size_t prevsize = p->prev_foot;
    if (is_mmapped(p)) {
      psize += prevsize + MMAP_FOOT_PAD;
      if (CALL_MUNMAP((char*)p - prevsize, psize) == 0)
        m->footprint -= psize;
      return;
    }
    prev = chunk_minus_offset(p, prevsize);
    psize += prevsize;
    p = prev;
    if (RTCHECK(ok_address(m, prev))) { /* consolidate backward */
      if (p != m->dv) {
        unlink_chunk(m, p, prevsize);
      }
      else if ((next->head & INUSE_BITS) == INUSE_BITS) {
        m->dvsize = psize;
        set_free_with_pinuse(p, psize, next);
        return;
      }
    }
    else {
      CORRUPTION_ERROR_ACTION(m);
      return;
    }
  }
  if (RTCHECK(ok_address(m, next))) {
    if (!cinuse(next)) {  /* consolidate forward */
      if (next == m->top) {
        size_t tsize = m->topsize += psize;
        m->top = p;
        p->head = tsize | PINUSE_BIT;
        if (p == m->dv) {
          m->dv = 0;
          m->dvsize = 0;
        }
        return;
      }
      else if (next == m->dv) {
        size_t dsize = m->dvsize += psize;
        m->dv = p;
        set_size_and_pinuse_of_free_chunk(p, dsize);
        return;
      }
      else {
        size_t nsize = chunksize(next);
        psize += nsize;
        unlink_chunk(m, next, nsize);
        set_size_and_pinuse_of_free_chunk(p, psize);
        if (p == m->dv) {
          m->dvsize = psize;
          return;
        }
      }
    }
    else {
      set_free_with_pinuse(p, psize, next);
    }
    insert_chunk(m, p, psize);
  }
  else {
    CORRUPTION_ERROR_ACTION(m);
  }
}

/* ---------------------------- malloc --------------------------- */

/* allocate a large request from the best fitting chunk in a treebin */
static void* tmalloc_large(mstate m, size_t nb) {
  tchunkptr v = 0;
  size_t rsize = -nb; /* Unsigned negation */
  tchunkptr t;
  bindex_t idx;
  compute_tree_index(nb, idx);
  if ((t = *treebin_at(m, idx)) != 0) {
    /* Traverse tree for this bin looking for node with size == nb */
    size_t sizebits = nb << leftshift_for_tree_index(idx);
    tchunkptr rst = 0;  /* The deepest untaken right subtree */
    for (;;) {
      tchunkptr rt;
      size_t trem = chunksize(t) - nb;
      if (trem < rsize) {
        v = t;
        if ((rsize = trem) == 0)
          break;
      }
      rt = t->child[1];
      t = t->child[(sizebits >> (SIZE_T_BITSIZE-SIZE_T_ONE)) & 1];
      if (rt != 0 && rt != t)
        rst = rt;
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

/* allocate a small request from the best fitting chunk in a treebin */
static void* tmalloc_small(mstate m, size_t nb) {
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

#if !ONLY_MSPACES

void* dlmalloc(size_t bytes) {
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

  if (!PREACTION(gm)) {
    void* mem;
    size_t nb;
    if (bytes <= MAX_SMALL_REQUEST) {
      bindex_t idx;
      binmap_t smallbits;
      nb = (bytes < MIN_REQUEST)? MIN_CHUNK_SIZE : pad_request(bytes);
      idx = small_index(nb);
      smallbits = gm->smallmap >> idx;

      if ((smallbits & 0x3U) != 0) { /* Remainderless fit to a smallbin. */
        mchunkptr b, p;
        idx += ~smallbits & 1;       /* Uses next bin if idx empty */
        b = smallbin_at(gm, idx);
        p = b->fd;
        assert(chunksize(p) == small_index2size(idx));
        unlink_first_small_chunk(gm, b, p, idx);
        set_inuse_and_pinuse(gm, p, small_index2size(idx));
        mem = chunk2mem(p);
        check_malloced_chunk(gm, mem, nb);
        goto postaction;
      }

      else if (nb > gm->dvsize) {
        if (smallbits != 0) { /* Use chunk in next nonempty smallbin */
          mchunkptr b, p, r;
          size_t rsize;
          bindex_t i;
          binmap_t leftbits = (smallbits << idx) & left_bits(idx2bit(idx));
          binmap_t leastbit = least_bit(leftbits);
          compute_bit2idx(leastbit, i);
          b = smallbin_at(gm, i);
          p = b->fd;
          assert(chunksize(p) == small_index2size(i));
          unlink_first_small_chunk(gm, b, p, i);
          rsize = small_index2size(i) - nb;
          /* Fit here cannot be remainderless if 4byte sizes */
          if (SIZE_T_SIZE != 4 && rsize < MIN_CHUNK_SIZE)
            set_inuse_and_pinuse(gm, p, small_index2size(i));
          else {
            set_size_and_pinuse_of_inuse_chunk(gm, p, nb);
            r = chunk_plus_offset(p, nb);
            set_size_and_pinuse_of_free_chunk(r, rsize);
            replace_dv(gm, r, rsize);
          }
          mem = chunk2mem(p);
          check_malloced_chunk(gm, mem, nb);
          goto postaction;
        }

        else if (gm->treemap != 0 && (mem = tmalloc_small(gm, nb)) != 0) {
          check_malloced_chunk(gm, mem, nb);
          goto postaction;
        }
      }
    }
    else if (bytes >= MAX_REQUEST)
      nb = MAX_SIZE_T; /* Too big to allocate. Force failure (in sys alloc) */
    else {
      nb = pad_request(bytes);
      if (gm->treemap != 0 && (mem = tmalloc_large(gm, nb)) != 0) {
        check_malloced_chunk(gm, mem, nb);
        goto postaction;
      }
    }

    if (nb <= gm->dvsize) {
      size_t rsize = gm->dvsize - nb;
      mchunkptr p = gm->dv;
      if (rsize >= MIN_CHUNK_SIZE) { /* split dv */
        mchunkptr r = gm->dv = chunk_plus_offset(p, nb);
        gm->dvsize = rsize;
        set_size_and_pinuse_of_free_chunk(r, rsize);
        set_size_and_pinuse_of_inuse_chunk(gm, p, nb);
      }
      else { /* exhaust dv */
        size_t dvs = gm->dvsize;
        gm->dvsize = 0;
        gm->dv = 0;
        set_inuse_and_pinuse(gm, p, dvs);
      }
      mem = chunk2mem(p);
      check_malloced_chunk(gm, mem, nb);
      goto postaction;
    }

    else if (nb < gm->topsize) { /* Split top */
      size_t rsize = gm->topsize -= nb;
      mchunkptr p = gm->top;
      mchunkptr r = gm->top = chunk_plus_offset(p, nb);
      r->head = rsize | PINUSE_BIT;
      set_size_and_pinuse_of_inuse_chunk(gm, p, nb);
      mem = chunk2mem(p);
      check_top_chunk(gm, gm->top);
      check_malloced_chunk(gm, mem, nb);
      goto postaction;
    }

    mem = sys_alloc(gm, nb);
    POSTACTION(gm);
    if (mem == MAP_FAILED && _weaken(__oom_hook)) {
      _weaken(__oom_hook)(bytes);
    }
    return mem;

  postaction:
    POSTACTION(gm);
    return mem;
  }

  return 0;
}

/* ---------------------------- free --------------------------- */

void dlfree(void* mem) {
  /*
     Consolidate freed chunks with preceeding or succeeding bordering
     free chunks, if they exist, and then place in a bin.  Intermixed
     with special cases for top, dv, mmapped chunks, and usage errors.
  */

  if (mem != 0) {
    mchunkptr p  = mem2chunk(mem);
#if FOOTERS
    mstate fm = get_mstate_for(p);
    if (!ok_magic(fm)) {
      USAGE_ERROR_ACTION(fm, p);
      return;
    }
#else /* FOOTERS */
#define fm gm
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
            if (CALL_MUNMAP((char*)p - prevsize, psize) == 0)
              fm->footprint -= psize;
            goto postaction;
          }
          else {
            mchunkptr prev = chunk_minus_offset(p, prevsize);
            psize += prevsize;
            p = prev;
            if (RTCHECK(ok_address(fm, prev))) { /* consolidate backward */
              if (p != fm->dv) {
                unlink_chunk(fm, p, prevsize);
              }
              else if ((next->head & INUSE_BITS) == INUSE_BITS) {
                fm->dvsize = psize;
                set_free_with_pinuse(p, psize, next);
                goto postaction;
              }
            }
            else
              goto erroraction;
          }
        }

        if (RTCHECK(ok_next(p, next) && ok_pinuse(next))) {
          if (!cinuse(next)) {  /* consolidate forward */
            if (next == fm->top) {
              size_t tsize = fm->topsize += psize;
              fm->top = p;
              p->head = tsize | PINUSE_BIT;
              if (p == fm->dv) {
                fm->dv = 0;
                fm->dvsize = 0;
              }
              if (should_trim(fm, tsize))
                sys_trim(fm, 0);
              goto postaction;
            }
            else if (next == fm->dv) {
              size_t dsize = fm->dvsize += psize;
              fm->dv = p;
              set_size_and_pinuse_of_free_chunk(p, dsize);
              goto postaction;
            }
            else {
              size_t nsize = chunksize(next);
              psize += nsize;
              unlink_chunk(fm, next, nsize);
              set_size_and_pinuse_of_free_chunk(p, psize);
              if (p == fm->dv) {
                fm->dvsize = psize;
                goto postaction;
              }
            }
          }
          else
            set_free_with_pinuse(p, psize, next);

          if (is_small(psize)) {
            insert_small_chunk(fm, p, psize);
            check_free_chunk(fm, p);
          }
          else {
            tchunkptr tp = (tchunkptr)p;
            insert_large_chunk(fm, tp, psize);
            check_free_chunk(fm, p);
            if (--fm->release_checks == 0)
              release_unused_segments(fm);
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

void* dlcalloc(size_t n_elements, size_t elem_size) {
  void* mem;
  size_t req = 0;
  if (ckd_mul(&req, n_elements, elem_size)) req = -1;
  mem = dlmalloc(req);
  if (mem != 0 && calloc_must_clear(mem2chunk(mem)))
    bzero(mem, req);
  return mem;
}

#endif /* !ONLY_MSPACES */

/* ------------ Internal support for realloc, memalign, etc -------------- */

/* Try to realloc; only in-place unless can_move true */
static mchunkptr try_realloc_chunk(mstate m, mchunkptr p, size_t nb,
                                   int can_move) {
  mchunkptr newp = 0;
  size_t oldsize = chunksize(p);
  mchunkptr next = chunk_plus_offset(p, oldsize);
  if (RTCHECK(ok_address(m, p) && ok_inuse(p) &&
              ok_next(p, next) && ok_pinuse(next))) {
    if (is_mmapped(p)) {
      newp = mmap_resize(m, p, nb, can_move);
    }
    else if (oldsize >= nb) {             /* already big enough */
      size_t rsize = oldsize - nb;
      if (rsize >= MIN_CHUNK_SIZE) {      /* split off remainder */
        mchunkptr r = chunk_plus_offset(p, nb);
        set_inuse(m, p, nb);
        set_inuse(m, r, rsize);
        dispose_chunk(m, r, rsize);
      }
      newp = p;
    }
    else if (next == m->top) {  /* extend into top */
      if (oldsize + m->topsize > nb) {
        size_t newsize = oldsize + m->topsize;
        size_t newtopsize = newsize - nb;
        mchunkptr newtop = chunk_plus_offset(p, nb);
        set_inuse(m, p, nb);
        newtop->head = newtopsize |PINUSE_BIT;
        m->top = newtop;
        m->topsize = newtopsize;
        newp = p;
      }
    }
    else if (next == m->dv) { /* extend into dv */
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
        }
        else { /* exhaust dv */
          size_t newsize = oldsize + dvs;
          set_inuse(m, p, newsize);
          m->dvsize = 0;
          m->dv = 0;
        }
        newp = p;
      }
    }
    else if (!cinuse(next)) { /* extend into next free chunk */
      size_t nextsize = chunksize(next);
      if (oldsize + nextsize >= nb) {
        size_t rsize = oldsize + nextsize - nb;
        unlink_chunk(m, next, nextsize);
        if (rsize < MIN_CHUNK_SIZE) {
          size_t newsize = oldsize + nextsize;
          set_inuse(m, p, newsize);
        }
        else {
          mchunkptr r = chunk_plus_offset(p, nb);
          set_inuse(m, p, nb);
          set_inuse(m, r, rsize);
          dispose_chunk(m, r, rsize);
        }
        newp = p;
      }
    }
  }
  else {
    USAGE_ERROR_ACTION(m, chunk2mem(p));
  }
  return newp;
}

static void* internal_memalign(mstate m, size_t alignment, size_t bytes) {
  void* mem = 0;
  if (alignment <  MIN_CHUNK_SIZE) /* must be at least a minimum chunk size */
    alignment = MIN_CHUNK_SIZE;
  /* alignment is 32+ bytes rounded up to nearest two power */
  alignment = 2ul << _bsrl(MAX(MIN_CHUNK_SIZE, alignment) - 1);
  if (bytes >= MAX_REQUEST - alignment) {
    if (m != 0)  { /* Test isn't needed but avoids compiler warning */
      MALLOC_FAILURE_ACTION;
    }
  }
  else {
    size_t nb = request2size(bytes);
    size_t req = nb + alignment + MIN_CHUNK_SIZE - CHUNK_OVERHEAD;
    mem = internal_malloc(m, req);
    if (mem != 0) {
      mchunkptr p = mem2chunk(mem);
      if (PREACTION(m))
        return 0;
      if ((((size_t)(mem)) & (alignment - 1)) != 0) { /* misaligned */
        /*
          Find an aligned spot inside chunk.  Since we need to give
          back leading space in a chunk of at least MIN_CHUNK_SIZE, if
          the first calculation places us at a spot with less than
          MIN_CHUNK_SIZE leader, we can move to the next aligned spot.
          We've allocated enough total room so that this is always
          possible.
        */
        char* br = (char*)mem2chunk((size_t)(((size_t)((char*)mem + alignment -
                                                       SIZE_T_ONE)) &
                                             -alignment));
        char* pos = ((size_t)(br - (char*)(p)) >= MIN_CHUNK_SIZE)?
          br : br+alignment;
        mchunkptr newp = (mchunkptr)pos;
        size_t leadsize = pos - (char*)(p);
        size_t newsize = chunksize(p) - leadsize;

        if (is_mmapped(p)) { /* For mmapped chunks, just adjust offset */
          newp->prev_foot = p->prev_foot + leadsize;
          newp->head = newsize;
        }
        else { /* Otherwise, give back leader, use the rest */
          set_inuse(m, newp, newsize);
          set_inuse(m, p, leadsize);
          dispose_chunk(m, p, leadsize);
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
          dispose_chunk(m, remainder, remainder_size);
        }
      }

      mem = chunk2mem(p);
      assert (chunksize(p) >= nb);
      assert(((size_t)mem & (alignment - 1)) == 0);
      check_inuse_chunk(m, p);
      POSTACTION(m);
    }
  }
  return mem;
}

/*
  Common support for independent_X routines, handling
    all of the combinations that can result.
  The opts arg has:
    bit 0 set if all elements are same size (using sizes[0])
    bit 1 set if elements should be zeroed
*/
static void** ialloc(mstate m,
                     size_t n_elements,
                     size_t* sizes,
                     int opts,
                     void* chunks[]) {

  size_t    element_size;   /* chunksize of each element, if all same */
  size_t    contents_size;  /* total size of elements */
  size_t    array_size;     /* request size of pointer array */
  void*     mem;            /* malloced aggregate space */
  mchunkptr p;              /* corresponding chunk */
  size_t    remainder_size; /* remaining bytes while splitting */
  void**    marray;         /* either "chunks" or malloced ptr array */
  mchunkptr array_chunk;    /* chunk for malloced ptr array */
  flag_t    was_enabled;    /* to disable mmap */
  size_t    size;
  size_t    i;

  ensure_initialization();
  /* compute array length, if needed */
  if (chunks != 0) {
    if (n_elements == 0)
      return chunks; /* nothing to do */
    marray = chunks;
    array_size = 0;
  }
  else {
    /* if empty req, must still return chunk representing empty array */
    if (n_elements == 0)
      return (void**)internal_malloc(m, 0);
    marray = 0;
    array_size = request2size(n_elements * (sizeof(void*)));
  }

  /* compute total element size */
  if (opts & 0x1) { /* all-same-size */
    element_size = request2size(*sizes);
    contents_size = n_elements * element_size;
  }
  else { /* add up all the sizes */
    element_size = 0;
    contents_size = 0;
    for (i = 0; i != n_elements; ++i)
      contents_size += request2size(sizes[i]);
  }

  size = contents_size + array_size;

  /*
     Allocate the aggregate chunk.  First disable direct-mmapping so
     malloc won't use it, since we would not be able to later
     free/realloc space internal to a segregated mmap region.
  */
  was_enabled = use_mmap(m);
  disable_mmap(m);
  mem = internal_malloc(m, size - CHUNK_OVERHEAD);
  if (was_enabled)
    enable_mmap(m);
  if (mem == 0)
    return 0;

  if (PREACTION(m)) return 0;
  p = mem2chunk(mem);
  remainder_size = chunksize(p);

  assert(!is_mmapped(p));

  if (opts & 0x2) {       /* optionally clear the elements */
    bzero((size_t*)mem, remainder_size - SIZE_T_SIZE - array_size);
  }

  /* If not provided, allocate the pointer array as final part of chunk */
  if (marray == 0) {
    size_t  array_chunk_size;
    array_chunk = chunk_plus_offset(p, contents_size);
    array_chunk_size = remainder_size - contents_size;
    marray = (void**) (chunk2mem(array_chunk));
    set_size_and_pinuse_of_inuse_chunk(m, array_chunk, array_chunk_size);
    remainder_size = contents_size;
  }

  /* split out elements */
  for (i = 0; ; ++i) {
    marray[i] = chunk2mem(p);
    if (i != n_elements-1) {
      if (element_size != 0)
        size = element_size;
      else
        size = request2size(sizes[i]);
      remainder_size -= size;
      set_size_and_pinuse_of_inuse_chunk(m, p, size);
      p = chunk_plus_offset(p, size);
    }
    else { /* the final element absorbs any overallocation slop */
      set_size_and_pinuse_of_inuse_chunk(m, p, remainder_size);
      break;
    }
  }

#if DEBUG
  if (marray != chunks) {
    /* final element must have exactly exhausted chunk */
    if (element_size != 0) {
      assert(remainder_size == element_size);
    }
    else {
      assert(remainder_size == request2size(sizes[i]));
    }
    check_inuse_chunk(m, mem2chunk(marray));
  }
  for (i = 0; i != n_elements; ++i)
    check_inuse_chunk(m, mem2chunk(marray[i]));

#endif /* DEBUG */

  POSTACTION(m);
  return marray;
}

/* Try to free all pointers in the given array.
   Note: this could be made faster, by delaying consolidation,
   at the price of disabling some user integrity checks, We
   still optimize some consolidations by combining adjacent
   chunks before freeing, which will occur often if allocated
   with ialloc or the array is sorted.
*/
static size_t internal_bulk_free(mstate m, void* array[], size_t nelem) {
  size_t unfreed = 0;
  if (!PREACTION(m)) {
    void** a;
    void** fence = &(array[nelem]);
    for (a = array; a != fence; ++a) {
      void* mem = *a;
      if (mem != 0) {
        mchunkptr p = mem2chunk(mem);
        size_t psize = chunksize(p);
#if FOOTERS
        if (get_mstate_for(p) != m) {
          ++unfreed;
          continue;
        }
#endif
        check_inuse_chunk(m, p);
        *a = 0;
        if (RTCHECK(ok_address(m, p) && ok_inuse(p))) {
          void ** b = a + 1; /* try to merge with next chunk */
          mchunkptr next = next_chunk(p);
          if (b != fence && *b == chunk2mem(next)) {
            size_t newsize = chunksize(next) + psize;
            set_inuse(m, p, newsize);
            *b = chunk2mem(p);
          }
          else
            dispose_chunk(m, p, psize);
        }
        else {
          CORRUPTION_ERROR_ACTION(m);
          break;
        }
      }
    }
    if (should_trim(m, m->topsize))
      sys_trim(m, 0);
    POSTACTION(m);
  }
  return unfreed;
}

/* Traversal */
#if MALLOC_INSPECT_ALL
static void internal_inspect_all(mstate m,
                                 void(*handler)(void *start,
                                                void *end,
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
        }
        else {
          used = 0;
          if (is_small(sz)) {     /* offset by possible bookkeeping */
            start = (void*)((char*)q + sizeof(struct malloc_chunk));
          }
          else {
            start = (void*)((char*)q + sizeof(struct malloc_tree_chunk));
          }
        }
        if (start < (void*)next)  /* skip if all space is bookkeeping */
          handler(start, next, used, arg);
        if (q == top)
          break;
        q = next;
      }
    }
  }
}
#endif /* MALLOC_INSPECT_ALL */

/* ------------------ Exported realloc, memalign, etc -------------------- */

#if !ONLY_MSPACES

void* dlrealloc(void* oldmem, size_t bytes) {
  void* mem = 0;
  if (oldmem == 0) {
    mem = dlmalloc(bytes);
  }
  else if (UNLIKELY(bytes >= MAX_REQUEST)) {
    MALLOC_FAILURE_ACTION;
  }
#ifdef REALLOC_ZERO_BYTES_FREES
  else if (bytes == 0) {
    dlfree(oldmem);
  }
#endif /* REALLOC_ZERO_BYTES_FREES */
  else {
    size_t nb = request2size(bytes);
    mchunkptr oldp = mem2chunk(oldmem);
#if ! FOOTERS
    mstate m = gm;
#else /* FOOTERS */
    mstate m = get_mstate_for(oldp);
    if (!ok_magic(m)) {
      USAGE_ERROR_ACTION(m, oldmem);
      return 0;
    }
#endif /* FOOTERS */
    if (!PREACTION(m)) {
      mchunkptr newp = try_realloc_chunk(m, oldp, nb, 1);
      POSTACTION(m);
      if (newp != 0) {
        check_inuse_chunk(m, newp);
        mem = chunk2mem(newp);
      }
      else {
        mem = internal_malloc(m, bytes);
        if (mem != 0) {
          size_t oc = chunksize(oldp) - overhead_for(oldp);
          memcpy(mem, oldmem, (oc < bytes)? oc : bytes);
          internal_free(m, oldmem);
        }
      }
    }
  }
  return mem;
}

void* dlrealloc_in_place(void* oldmem, size_t bytes) {
  void* mem = 0;
  if (oldmem != 0) {
    if (bytes >= MAX_REQUEST) {
      MALLOC_FAILURE_ACTION;
    }
    else {
      size_t nb = request2size(bytes);
      mchunkptr oldp = mem2chunk(oldmem);
#if ! FOOTERS
      mstate m = gm;
#else /* FOOTERS */
      mstate m = get_mstate_for(oldp);
      if (!ok_magic(m)) {
        USAGE_ERROR_ACTION(m, oldmem);
        return 0;
      }
#endif /* FOOTERS */
      if (!PREACTION(m)) {
        mchunkptr newp = try_realloc_chunk(m, oldp, nb, 0);
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

void* dlmemalign(size_t alignment, size_t bytes) {
  if (alignment <= MALLOC_ALIGNMENT) {
    return dlmalloc(bytes);
  }
  return internal_memalign(gm, alignment, bytes);
}

#if USE_LOCKS
void dlmalloc_atfork(void) {
  bzero(&gm->mutex, sizeof(gm->mutex));
  bzero(&malloc_global_mutex, sizeof(malloc_global_mutex));
}
#endif

void** dlindependent_calloc(size_t n_elements, size_t elem_size,
                            void* chunks[]) {
  size_t sz = elem_size; /* serves as 1-element array */
  return ialloc(gm, n_elements, &sz, 3, chunks);
}

void** dlindependent_comalloc(size_t n_elements, size_t sizes[],
                              void* chunks[]) {
  return ialloc(gm, n_elements, sizes, 0, chunks);
}

size_t dlbulk_free(void* array[], size_t nelem) {
  return internal_bulk_free(gm, array, nelem);
}

#if MALLOC_INSPECT_ALL
void dlmalloc_inspect_all(void(*handler)(void *start,
                                         void *end,
                                         size_t used_bytes,
                                         void* callback_arg),
                          void* arg) {
  ensure_initialization();
  if (!PREACTION(gm)) {
    internal_inspect_all(gm, handler, arg);
    POSTACTION(gm);
  }
}
#endif /* MALLOC_INSPECT_ALL */

int dlmalloc_trim(size_t pad) {
  int result = 0;
  ensure_initialization();
  if (!PREACTION(gm)) {
    result = sys_trim(gm, pad);
    POSTACTION(gm);
  }
  return result;
}

size_t dlmalloc_footprint(void) {
  return gm->footprint;
}

size_t dlmalloc_max_footprint(void) {
  return gm->max_footprint;
}

size_t dlmalloc_footprint_limit(void) {
  size_t maf = gm->footprint_limit;
  return maf == 0 ? MAX_SIZE_T : maf;
}

size_t dlmalloc_set_footprint_limit(size_t bytes) {
  size_t result;  /* invert sense of 0 */
  if (bytes == 0)
    result = granularity_align(1); /* Use minimal size */
  if (bytes == MAX_SIZE_T)
    result = 0;                    /* disable */
  else
    result = granularity_align(bytes);
  return gm->footprint_limit = result;
}

#if !NO_MALLINFO
struct mallinfo dlmallinfo(void) {
  return internal_mallinfo(gm);
}
#endif /* NO_MALLINFO */

#if !NO_MALLOC_STATS
void dlmalloc_stats() {
  internal_malloc_stats(gm);
}
#endif /* NO_MALLOC_STATS */

int dlmallopt(int param_number, int value) {
  return change_mparam(param_number, value);
}

size_t dlmalloc_usable_size(void* mem) {
  mchunkptr p;
  size_t bytes;
  if (mem) {
    p = mem2chunk(mem);
    if (is_inuse(p)) {
      bytes = chunksize(p) - overhead_for(p);
    } else {
      bytes = 0;
    }
  } else {
    bytes = 0;
  }
  return bytes;
}

#endif /* !ONLY_MSPACES */

/* ----------------------------- user mspaces ---------------------------- */

#if MSPACES
#include "third_party/dlmalloc/mspaces.inc"
#endif /* MSPACES */
