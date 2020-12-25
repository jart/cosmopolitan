#ifndef COSMOPOLITAN_LIBC_MEM_DLMALLOC_H_
#define COSMOPOLITAN_LIBC_MEM_DLMALLOC_H_
#ifndef __STRICT_ANSI__
#include "libc/assert.h"
#include "libc/bits/bits.h"
#include "libc/calls/calls.h"
#include "libc/dce.h"
#include "libc/runtime/runtime.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_
#if 0
/**
 * @fileoverview Internal header for Doug Lea's malloc.
*/
#endif

#define DLMALLOC_VERSION 20806

#ifndef FOOTERS
#define FOOTERS !NoDebug()
#endif

#define HAVE_MMAP              1
#define HAVE_MREMAP            0 /* IsLinux() */
#define MMAP_CLEARS            1
#define MALLOC_ALIGNMENT       __BIGGEST_ALIGNMENT__
#define NO_SEGMENT_TRAVERSAL   1
#define MAX_RELEASE_CHECK_RATE 128
#define MALLOC_ABORT           abort()
#define DEFAULT_GRANULARITY    (64UL * 1024UL)
#define DEFAULT_TRIM_THRESHOLD (10UL * 1024UL * 1024UL)
#define DEFAULT_MMAP_THRESHOLD (256UL * 1024UL)
#define USE_LOCKS              0
#define USE_SPIN_LOCKS         0
#define LOCK_AT_FORK           0
#define NSMALLBINS             (32u)
#define NTREEBINS              (32u)
#define SMALLBIN_SHIFT         (3u)
#define SMALLBIN_WIDTH         (SIZE_T_ONE << SMALLBIN_SHIFT)
#define TREEBIN_SHIFT          (8u)
#define MIN_LARGE_SIZE         (SIZE_T_ONE << TREEBIN_SHIFT)
#define MAX_SMALL_SIZE         (MIN_LARGE_SIZE - SIZE_T_ONE)
#define MAX_SMALL_REQUEST      (MAX_SMALL_SIZE - CHUNK_ALIGN_MASK - CHUNK_OVERHEAD)
#define M_TRIM_THRESHOLD       (-1)
#define M_GRANULARITY          (-2)
#define M_MMAP_THRESHOLD       (-3)

/* ─────────────────── size_t and alignment properties ──────────────────── */

#define MALLINFO_FIELD_TYPE size_t

/* The byte and bit size of a size_t */
#define SIZE_T_SIZE    (sizeof(size_t))
#define SIZE_T_BITSIZE (sizeof(size_t) << 3)

/* Some constants coerced to size_t */
/* Annoying but necessary to avoid errors on some platforms */
#define SIZE_T_ZERO       0UL
#define SIZE_T_ONE        1UL
#define SIZE_T_TWO        2UL
#define SIZE_T_FOUR       4UL
#define TWO_SIZE_T_SIZES  (SIZE_T_SIZE << 1)
#define FOUR_SIZE_T_SIZES (SIZE_T_SIZE << 2)
#define SIX_SIZE_T_SIZES  (FOUR_SIZE_T_SIZES + TWO_SIZE_T_SIZES)
#define HALF_SIZE_MAX     (__SIZE_MAX__ / 2U)

/* The bit mask value corresponding to MALLOC_ALIGNMENT */
#define CHUNK_ALIGN_MASK (MALLOC_ALIGNMENT - SIZE_T_ONE)

/* True if address a has acceptable alignment */
#define is_aligned(A) (((size_t)((A)) & (CHUNK_ALIGN_MASK)) == 0)

/* the number of bytes to offset an address to align it */
#define align_offset(A)                                         \
  ((((size_t)(A)&CHUNK_ALIGN_MASK) == 0)                        \
       ? 0                                                      \
       : ((MALLOC_ALIGNMENT - ((size_t)(A)&CHUNK_ALIGN_MASK)) & \
          CHUNK_ALIGN_MASK))

/* ────────────────────────── MMAP preliminaries ───────────────────────── */

#define MFAIL                           MAP_FAILED
#define CMFAIL                          ((char *)MAP_FAILED)
#define MMAP_DEFAULT(s)                 dlmalloc_requires_more_vespene_gas(s)
#define MUNMAP_DEFAULT(a, s)            munmap(a, s)
#define MMAP_PROT                       (PROT_READ | PROT_WRITE)
#define DIRECT_MMAP_DEFAULT(s)          MMAP_DEFAULT(s)
#define USE_MMAP_BIT                    (SIZE_T_ONE)
#define CALL_MMAP(s)                    MMAP_DEFAULT(s)
#define CALL_MUNMAP(a, s)               MUNMAP_DEFAULT((a), (s))
#define CALL_DIRECT_MMAP(s)             DIRECT_MMAP_DEFAULT(s)
#define CALL_MREMAP(addr, osz, nsz, mv) MREMAP((addr), (osz), (nsz), (mv))
#define USE_NONCONTIGUOUS_BIT           (4U)
#define EXTERN_BIT                      (8U)

/* ─────────────────────────── Lock preliminaries ──────────────────────── */

/*
  When locks are defined, there is one global lock, plus
  one per-mspace lock.

  The global lock_ensures that mparams.magic and other unique
  mparams values are initialized only once. It also protects
  sequences of calls to MORECORE.  In many cases sys_alloc requires
  two calls, that should not be interleaved with calls by other
  threads.  This does not protect against direct calls to MORECORE
  by other threads not using this lock, so there is still code to
  cope the best we can on interference.

  Per-mspace locks surround calls to malloc, free, etc.
  By default, locks are simple non-reentrant mutexes.

  Because lock-protected regions generally have bounded times, it is
  OK to use the supplied simple spinlocks. Spinlocks are likely to
  improve performance for lightly contended applications, but worsen
  performance under heavy contention.

  If USE_LOCKS is > 1, the definitions of lock routines here are
  bypassed, in which case you will need to define the type MLOCK_T,
  and at least INITIAL_LOCK, DESTROY_LOCK, ACQUIRE_LOCK, RELEASE_LOCK
  and TRY_LOCK.  You must also declare a
    static MLOCK_T malloc_global_mutex = { initialization values };.
*/

#define USE_LOCK_BIT    (0U)
#define INITIAL_LOCK(l) (0)
#define DESTROY_LOCK(l) (0)
#define ACQUIRE_MALLOC_GLOBAL_LOCK()
#define RELEASE_MALLOC_GLOBAL_LOCK()

/* ───────────────────────  Chunk representations ──────────────────────── */

/*
  (The following includes lightly edited explanations by Colin Plumb.)

  The malloc_chunk declaration below is misleading (but accurate and
  necessary).  It declares a "view" into memory allowing access to
  necessary fields at known offsets from a given base.

  Chunks of memory are maintained using a `boundary tag' method as
  originally described by Knuth.  (See the paper by Paul Wilson
  ftp://ftp.cs.utexas.edu/pub/garbage/allocsrv.ps for a survey of such
  techniques.)  Sizes of free chunks are stored both in the front of
  each chunk and at the end.  This makes consolidating fragmented
  chunks into bigger chunks fast.  The head fields also hold bits
  representing whether chunks are free or in use.

  Here are some pictures to make it clearer.  They are "exploded" to
  show that the state of a chunk can be thought of as extending from
  the high 31 bits of the head field of its header through the
  prev_foot and PINUSE_BIT bit of the following chunk header.

  A chunk that's in use looks like:

    chunk→ ┌───────────────────────────────────────────────────────────────┐
           │ Size of previous chunk (if P = 0)                             │
           └─────────────────────────────────────────────────────────────┬─┤
         ┌─────────────────────────────────────────────────────────────┐ │P│
         │ Size of this chunk                                         1│ └─┘
    mem→ ├───────────────────────────────────────────────────────────────┐
         │                                                               │
         ├─                                                             ─┤
         │                                                               │
         ├─                                                             ─┤
         │                                                               :
         ├─      size - sizeof(size_t) available payload bytes          ─┤
         :                                                               │
  chunk→ ├─                                                             ─┤
         │                                                               │
         └───────────────────────────────────────────────────────────────┤
       ┌─────────────────────────────────────────────────────────────┐ |1│
       │ Size of next chunk (may or may not be in use)               │ ├─┘
  mem→ └───────────────────────────────────────────────────────────────┘

    And if it's free, it looks like this:

    chunk→ ┌─                                                             ─┐
           │ User payload (must be in use, or we would have merged!)       │
           └───────────────────────────────────────────────────────────────┤
         ┌─────────────────────────────────────────────────────────────┐ │P│
         │ Size of this chunk                                         0│ │─┘
    mem→ ├───────────────────────────────────────────────────────────────┤
         │ Next pointer                                                  │
         ├───────────────────────────────────────────────────────────────┤
         │ Prev pointer                                                  │
         ├───────────────────────────────────────────────────────────────┤
         │                                                               :
         ├─      size - sizeof(struct chunk) unused bytes               ─┤
         :                                                               │
  chunk→ ├───────────────────────────────────────────────────────────────┤
         │ Size of this chunk                                            │
         └───────────────────────────────────────────────────────────────┤
       ┌───────────────────────────────────────────────────────────────│0│
       │ Size of next chunk (must be in use, or we would have merged)| │─┘
  mem→ ├───────────────────────────────────────────────────────────────┤
       │                                                               :
       ├─ User payload                                                ─┤
       :                                                               │
       └───────────────────────────────────────────────────────────────┤
                                                                     │0│
                                                                     └─┘
  Note that since we always merge adjacent free chunks, the chunks
  adjacent to a free chunk must be in use.

  Given a pointer to a chunk (which can be derived trivially from the
  payload pointer) we can, in O(1) time, find out whether the adjacent
  chunks are free, and if so, unlink them from the lists that they
  are on and merge them with the current chunk.

  Chunks always begin on even word boundaries, so the mem portion
  (which is returned to the user) is also on an even word boundary, and
  thus at least double-word aligned.

  The P (PINUSE_BIT) bit, stored in the unused low-order bit of the
  chunk size (which is always a multiple of two words), is an in-use
  bit for the *previous* chunk.  If that bit is *clear*, then the
  word before the current chunk size contains the previous chunk
  size, and can be used to find the front of the previous chunk.
  The very first chunk allocated always has this bit set, preventing
  access to non-existent (or non-owned) memory. If pinuse is set for
  any given chunk, then you CANNOT determine the size of the
  previous chunk, and might even get a memory addressing fault when
  trying to do so.

  The C (CINUSE_BIT) bit, stored in the unused second-lowest bit of
  the chunk size redundantly records whether the current chunk is
  inuse (unless the chunk is mmapped). This redundancy enables usage
  checks within free and realloc, and reduces indirection when freeing
  and consolidating chunks.

  Each freshly allocated chunk must have both cinuse and pinuse set.
  That is, each allocated chunk borders either a previously allocated
  and still in-use chunk, or the base of its memory arena. This is
  ensured by making all allocations from the `lowest' part of any
  found chunk.  Further, no free chunk physically borders another one,
  so each free chunk is known to be preceded and followed by either
  inuse chunks or the ends of memory.

  Note that the `foot' of the current chunk is actually represented
  as the prev_foot of the NEXT chunk. This makes it easier to
  deal with alignments etc but can be very confusing when trying
  to extend or adapt this code.

  The exceptions to all this are

     1. The special chunk `top' is the top-most available chunk (i.e.,
        the one bordering the end of available memory). It is treated
        specially.  Top is never included in any bin, is used only if
        no other chunk is available, and is released back to the
        system if it is very large (see M_TRIM_THRESHOLD).  In effect,
        the top chunk is treated as larger (and thus less well
        fitting) than any other available chunk.  The top chunk
        doesn't update its trailing size field since there is no next
        contiguous chunk that would have to index off it. However,
        space is still allocated for it (TOP_FOOT_SIZE) to enable
        separation or merging when space is extended.

     3. Chunks allocated via mmap, have both cinuse and pinuse bits
        cleared in their head fields.  Because they are allocated
        one-by-one, each must carry its own prev_foot field, which is
        also used to hold the offset this chunk has within its mmapped
        region, which is needed to preserve alignment. Each mmapped
        chunk is trailed by the first two fields of a fake next-chunk
        for sake of usage checks.

*/

struct malloc_chunk {
  size_t prev_foot;        /* Size of previous chunk (if free).  */
  size_t head;             /* Size and inuse bits. */
  struct malloc_chunk *fd; /* double links -- used only if free. */
  struct malloc_chunk *bk;
};

typedef struct malloc_chunk mchunk;
typedef struct malloc_chunk *mchunkptr;
typedef struct malloc_chunk *sbinptr; /* The type of bins of chunks */
typedef unsigned int bindex_t;        /* Described below */
typedef unsigned int binmap_t;        /* Described below */
typedef unsigned int flag_t;          /* The type of various bit flag sets */

/* ─────────────────── Chunks sizes and alignments ─────────────────────── */

#define MCHUNK_SIZE (sizeof(mchunk))

#define CHUNK_OVERHEAD (TWO_SIZE_T_SIZES)

/* MMapped chunks need a second word of overhead ... */
#define MMAP_CHUNK_OVERHEAD (TWO_SIZE_T_SIZES)
/* ... and additional padding for fake next-chunk at foot */
#define MMAP_FOOT_PAD (FOUR_SIZE_T_SIZES)

/* The smallest size we can malloc is an aligned minimal chunk */
#define MIN_CHUNK_SIZE ((MCHUNK_SIZE + CHUNK_ALIGN_MASK) & ~CHUNK_ALIGN_MASK)

/* conversion from malloc headers to user pointers, and back */
#define chunk2mem(p)   ((void *)((char *)(p) + TWO_SIZE_T_SIZES))
#define mem2chunk(mem) ((mchunkptr)((char *)(mem)-TWO_SIZE_T_SIZES))
/* chunk associated with aligned address A */
#define align_as_chunk(A) (mchunkptr)((A) + align_offset(chunk2mem(A)))

/* Bounds on request (not chunk) sizes. */
#define MAX_REQUEST ((-MIN_CHUNK_SIZE) << 2)
#define MIN_REQUEST (MIN_CHUNK_SIZE - CHUNK_OVERHEAD - SIZE_T_ONE)

/* pad request bytes into a usable size */
#define pad_request(req) \
  (((req) + CHUNK_OVERHEAD + CHUNK_ALIGN_MASK) & ~CHUNK_ALIGN_MASK)

/* pad request, checking for minimum (but not maximum) */
#define request2size(req) \
  (((req) < MIN_REQUEST) ? MIN_CHUNK_SIZE : pad_request(req))

/* ────────────────── Operations on head and foot fields ───────────────── */

/*
  The head field of a chunk is or'ed with PINUSE_BIT when previous
  adjacent chunk in use, and or'ed with CINUSE_BIT if this chunk is in
  use, unless mmapped, in which case both bits are cleared.

  FLAG4_BIT is not used by this malloc, but might be useful in extensions.
*/

#define PINUSE_BIT (SIZE_T_ONE)
#define CINUSE_BIT (SIZE_T_TWO)
#define FLAG4_BIT  (SIZE_T_FOUR)
#define INUSE_BITS (PINUSE_BIT | CINUSE_BIT)
#define FLAG_BITS  (PINUSE_BIT | CINUSE_BIT | FLAG4_BIT)

/* Head value for fenceposts */
#define FENCEPOST_HEAD (INUSE_BITS | SIZE_T_SIZE)

/* extraction of fields from head words */
#define cinuse(p)     ((p)->head & CINUSE_BIT)
#define pinuse(p)     ((p)->head & PINUSE_BIT)
#define flag4inuse(p) ((p)->head & FLAG4_BIT)
#define is_inuse(p)   (((p)->head & INUSE_BITS) != PINUSE_BIT)
#define is_mmapped(p) (((p)->head & INUSE_BITS) == 0)

#define chunksize(p) ((p)->head & ~(FLAG_BITS))

#define clear_pinuse(p) ((p)->head &= ~PINUSE_BIT)
#define set_flag4(p)    ((p)->head |= FLAG4_BIT)
#define clear_flag4(p)  ((p)->head &= ~FLAG4_BIT)

/* Treat space at ptr +/- offset as a chunk */
#define chunk_plus_offset(p, s)  ((mchunkptr)(((char *)(p)) + (s)))
#define chunk_minus_offset(p, s) ((mchunkptr)(((char *)(p)) - (s)))

/* Ptr to next or previous physical malloc_chunk. */
#define next_chunk(p) ((mchunkptr)(((char *)(p)) + ((p)->head & ~FLAG_BITS)))
#define prev_chunk(p) ((mchunkptr)(((char *)(p)) - ((p)->prev_foot)))

/* extract next chunk's pinuse bit */
#define next_pinuse(p) ((next_chunk(p)->head) & PINUSE_BIT)

/* Get/set size at footer */
#define get_foot(p, s) (((mchunkptr)((char *)(p) + (s)))->prev_foot)
#define set_foot(p, s) (((mchunkptr)((char *)(p) + (s)))->prev_foot = (s))

/* Set size, pinuse bit, and foot */
#define set_size_and_pinuse_of_free_chunk(p, s) \
  ((p)->head = (s | PINUSE_BIT), set_foot(p, s))

/* Set size, pinuse bit, foot, and clear next pinuse */
#define set_free_with_pinuse(p, s, n) \
  (clear_pinuse(n), set_size_and_pinuse_of_free_chunk(p, s))

/* Get the internal overhead associated with chunk p */
#define overhead_for(p) (is_mmapped(p) ? MMAP_CHUNK_OVERHEAD : CHUNK_OVERHEAD)

/* Return true if malloced space is not necessarily cleared */

#define calloc_must_clear(p) (!is_mmapped(p))

/* ────────────────────── Overlaid data structures ─────────────────────── */

/*
  When chunks are not in use, they are treated as nodes of either
  lists or trees.

  "Small"  chunks are stored in circular doubly-linked lists, and look
  like this:

     chunk→ ┌───────────────────────────────────────────────────────────────┐
            │             Size of previous chunk                            │
            ├───────────────────────────────────────────────────────────────┤
    `head:' │             Size of chunk, in bytes                         |P│
       mem→ ├───────────────────────────────────────────────────────────────┤
            │             Forward pointer to next chunk in list             │
            ├───────────────────────────────────────────────────────────────┤
            │             Back pointer to previous chunk in list            │
            ├───────────────────────────────────────────────────────────────┤
            .             Unused space (may be 0 bytes long)                .
            .                                                               .
            │                                                               │
 nextchunk→ ├───────────────────────────────────────────────────────────────┤
    `foot:' │             Size of chunk, in bytes                           │
            └───────────────────────────────────────────────────────────────┘

  Larger chunks are kept in a form of bitwise digital trees (aka
  tries) keyed on chunksizes.  Because malloc_tree_chunks are only for
  free chunks greater than 256 bytes, their size doesn't impose any
  constraints on user chunk sizes.  Each node looks like:

     chunk→ ┌───────────────────────────────────────────────────────────────┐
            │             Size of previous chunk                            │
            ├─────────────────────────────────────────────────────────────┬─┤
    `head:' │             Size of chunk, in bytes                         │P│
       mem→ ├─────────────────────────────────────────────────────────────┴─┤
            │             Forward pointer to next chunk of same size        │
            ├───────────────────────────────────────────────────────────────┤
            │             Back pointer to previous chunk of same size       │
            ├───────────────────────────────────────────────────────────────┤
            │             Pointer to left child (child[0])                  │
            ├───────────────────────────────────────────────────────────────┤
            │             Pointer to right child (child[1])                 │
            ├───────────────────────────────────────────────────────────────┤
            │             Pointer to parent                                 │
            ├───────────────────────────────────────────────────────────────┤
            │             bin index of this chunk                           │
            ├───────────────────────────────────────────────────────────────┤
            │             Unused space                                      .
            .                                                               │
 nextchunk→ ├───────────────────────────────────────────────────────────────┤
    `foot:' │             Size of chunk, in bytes                           │
            └───────────────────────────────────────────────────────────────┘

  Each tree holding treenodes is a tree of unique chunk sizes.  Chunks
  of the same size are arranged in a circularly-linked list, with only
  the oldest chunk (the next to be used, in our FIFO ordering)
  actually in the tree.  (Tree members are distinguished by a non-null
  parent pointer.)  If a chunk with the same size an an existing node
  is inserted, it is linked off the existing node using pointers that
  work in the same way as fd/bk pointers of small chunks.

  Each tree contains a power of 2 sized range of chunk sizes (the
  smallest is 0x100 <= x < 0x180), which is is divided in half at each
  tree level, with the chunks in the smaller half of the range (0x100
  <= x < 0x140 for the top nose) in the left subtree and the larger
  half (0x140 <= x < 0x180) in the right subtree.  This is, of course,
  done by inspecting individual bits.

  Using these rules, each node's left subtree contains all smaller
  sizes than its right subtree.  However, the node at the root of each
  subtree has no particular ordering relationship to either.  (The
  dividing line between the subtree sizes is based on trie relation.)
  If we remove the last chunk of a given size from the interior of the
  tree, we need to replace it with a leaf node.  The tree ordering
  rules permit a node to be replaced by any leaf below it.

  The smallest chunk in a tree (a common operation in a best-fit
  allocator) can be found by walking a path to the leftmost leaf in
  the tree.  Unlike a usual binary tree, where we follow left child
  pointers until we reach a null, here we follow the right child
  pointer any time the left one is null, until we reach a leaf with
  both child pointers null. The smallest chunk in the tree will be
  somewhere along that path.

  The worst case number of steps to add, find, or remove a node is
  bounded by the number of bits differentiating chunks within
  bins. Under current bin calculations, this ranges from 6 up to 21
  (for 32 bit sizes) or up to 53 (for 64 bit sizes). The typical case
  is of course much better.
*/

struct malloc_tree_chunk {
  /* The first four fields must be compatible with malloc_chunk */
  size_t prev_foot;
  size_t head;
  struct malloc_tree_chunk *fd;
  struct malloc_tree_chunk *bk;

  struct malloc_tree_chunk *child[2];
  struct malloc_tree_chunk *parent;
  bindex_t index;
};

typedef struct malloc_tree_chunk tchunk;
typedef struct malloc_tree_chunk *tchunkptr;
typedef struct malloc_tree_chunk *tbinptr; /* The type of bins of trees */

/* A little helper macro for trees */
#define leftmost_child(t) ((t)->child[0] != 0 ? (t)->child[0] : (t)->child[1])

/* ───────────────────────────── Segments ──────────────────────────────── */

/*
  Each malloc space may include non-contiguous segments, held in a
  list headed by an embedded malloc_segment record representing the
  top-most space. Segments also include flags holding properties of
  the space. Large chunks that are directly allocated by mmap are not
  included in this list. They are instead independently created and
  destroyed without otherwise keeping track of them.

  Segment management mainly comes into play for spaces allocated by
  MMAP.  Any call to MMAP might or might not return memory that is
  adjacent to an existing segment.  MORECORE normally contiguously
  extends the current space, so this space is almost always adjacent,
  which is simpler and faster to deal with. (This is why MORECORE is
  used preferentially to MMAP when both are available -- see
  sys_alloc.)  When allocating using MMAP, we don't use any of the
  hinting mechanisms (inconsistently) supported in various
  implementations of unix mmap, or distinguish reserving from
  committing memory. Instead, we just ask for space, and exploit
  contiguity when we get it.  It is probably possible to do
  better than this on some systems, but no general scheme seems
  to be significantly better.

  Management entails a simpler variant of the consolidation scheme
  used for chunks to reduce fragmentation -- new adjacent memory is
  normally prepended or appended to an existing segment. However,
  there are limitations compared to chunk consolidation that mostly
  reflect the fact that segment processing is relatively infrequent
  (occurring only when getting memory from system) and that we
  don't expect to have huge numbers of segments:

  * Segments are not indexed, so traversal requires linear scans.  (It
    would be possible to index these, but is not worth the extra
    overhead and complexity for most programs on most platforms.)
  * New segments are only appended to old ones when holding top-most
    memory; if they cannot be prepended to others, they are held in
    different segments.

  Except for the top-most segment of an mstate, each segment record
  is kept at the tail of its segment. Segments are added by pushing
  segment records onto the list headed by &mstate.seg for the
  containing mstate.

  Segment flags control allocation/merge/deallocation policies:
  * If EXTERN_BIT set, then we did not allocate this segment,
    and so should not try to deallocate or merge with others.
    (This currently holds only for the initial segment passed
    into create_mspace_with_base.)
  * If USE_MMAP_BIT set, the segment may be merged with
    other surrounding mmapped segments and trimmed/de-allocated
    using munmap.
  * If neither bit is set, then the segment was obtained using
    MORECORE so can be merged with surrounding MORECORE'd segments
    and deallocated/trimmed using MORECORE with negative arguments.
*/

struct malloc_segment {
  char *base;                  /* base address */
  size_t size;                 /* allocated size */
  struct malloc_segment *next; /* ptr to next segment */
  flag_t sflags;               /* mmap and extern flag */
};

#define is_mmapped_segment(S) ((S)->sflags & USE_MMAP_BIT)
#define is_extern_segment(S)  ((S)->sflags & EXTERN_BIT)

typedef struct malloc_segment msegment;
typedef struct malloc_segment *msegmentptr;

/* ──────────────────────────── MallocState ───────────────────────────── */

/*
   A MallocState holds all of the bookkeeping for a space.
   The main fields are:

  Top
    The topmost chunk of the currently active segment. Its size is
    cached in topsize.  The actual size of topmost space is
    topsize+TOP_FOOT_SIZE, which includes space reserved for adding
    fenceposts and segment records if necessary when getting more
    space from the system.  The size at which to autotrim top is
    cached from mparams in trim_check, except that it is disabled if
    an autotrim fails.

  Designated victim (dv)
    This is the preferred chunk for servicing small requests that
    don't have exact fits.  It is normally the chunk split off most
    recently to service another small request.  Its size is cached in
    dvsize. The link fields of this chunk are not maintained since it
    is not kept in a bin.

  SmallBins
    An array of bin headers for free chunks.  These bins hold chunks
    with sizes less than MIN_LARGE_SIZE bytes. Each bin contains
    chunks of all the same size, spaced 8 bytes apart.  To simplify
    use in double-linked lists, each bin header acts as a malloc_chunk
    pointing to the real first node, if it exists (else pointing to
    itself).  This avoids special-casing for headers.  But to avoid
    waste, we allocate only the fd/bk pointers of bins, and then use
    repositioning tricks to treat these as the fields of a chunk.

  TreeBins
    Treebins are pointers to the roots of trees holding a range of
    sizes. There are 2 equally spaced treebins for each power of two
    from TREE_SHIFT to TREE_SHIFT+16. The last bin holds anything
    larger.

  Bin maps
    There is one bit map for small bins ("smallmap") and one for
    treebins ("treemap).  Each bin sets its bit when non-empty, and
    clears the bit when empty.  Bit operations are then used to avoid
    bin-by-bin searching -- nearly all "search" is done without ever
    looking at bins that won't be selected.  The bit maps
    conservatively use 32 bits per map word, even if on 64bit system.
    For a good description of some of the bit-based techniques used
    here, see Henry S. Warren Jr's book "Hacker's Delight" (and
    supplement at http://hackersdelight.org/). Many of these are
    intended to reduce the branchiness of paths through malloc etc, as
    well as to reduce the number of memory locations read or written.

  Segments
    A list of segments headed by an embedded malloc_segment record
    representing the initial space.

  Address check support
    The least_addr field is the least address ever obtained from
    MORECORE or MMAP. Attempted frees and reallocs of any address less
    than this are trapped (unless INSECURE is defined).

  Magic tag
    A cross-check field that should always hold same value as mparams.magic.

  Max allowed footprint
    The maximum allowed bytes to allocate from system (zero means no limit)

  Flags
    Bits recording whether to use MMAP, locks, or contiguous MORECORE

  Statistics
    Each space keeps track of current and maximum system memory
    obtained via MORECORE or MMAP.

  Trim support
    Fields holding the amount of unused topmost memory that should trigger
    trimming, and a counter to force periodic scanning to release unused
    non-topmost segments.

  Locking
    If USE_LOCKS is defined, the "mutex" lock is acquired and released
    around every public call using this mspace.

  Extension support
    A void* pointer and a size_t field that can be used to help implement
    extensions to this malloc.
*/

struct MallocState {
  binmap_t smallmap;
  binmap_t treemap;
  size_t dvsize;
  size_t topsize;
  char *least_addr;
  mchunkptr dv;
  mchunkptr top;
  size_t trim_check;
  size_t release_checks;
  size_t magic;
  mchunkptr smallbins[(NSMALLBINS + 1) * 2];
  tbinptr treebins[NTREEBINS];
  size_t footprint;
  size_t max_footprint;
  size_t footprint_limit; /* zero means no limit */
  flag_t mflags;
  msegment seg;
  void *extp; /* Unused but available for extensions */
  size_t exts;
};

struct MallocStats {
  size_t maxfp;
  size_t fp;
  size_t used;
};

typedef struct MallocState *mstate;

extern struct MallocState g_dlmalloc[1];

/* ───────────────────────────────  Hooks ──────────────────────────────── */

#ifdef MTRACE /* TODO(jart): Add --mtrace flag for this */
void *AddressBirthAction(void *);
void *AddressDeathAction(void *);
#define ADDRESS_BIRTH_ACTION(A) AddressBirthAction(A)
#define ADDRESS_DEATH_ACTION(A) AddressDeathAction(A)
#else
#define ADDRESS_BIRTH_ACTION(A) (A)
#define ADDRESS_DEATH_ACTION(A) (A)
#endif

/*
  PREACTION should be defined to return 0 on success, and nonzero on
  failure. If you are not using locking, you can redefine these to do
  anything you like.
*/

#define PREACTION(M) (0)
#define POSTACTION(M)

/*
  CORRUPTION_ERROR_ACTION is triggered upon detected bad addresses.
  USAGE_ERROR_ACTION is triggered on detected bad frees and
  reallocs. The argument p is an address that might have triggered the
  fault. It is ignored by the two predefined actions, but might be
  useful in custom actions that try to help diagnose errors.
*/

#define CORRUPTION_ERROR_ACTION(m) MALLOC_ABORT
#define USAGE_ERROR_ACTION(m, p)   MALLOC_ABORT

/*  True if segment S holds address A */
#define segment_holds(S, A) \
  ((char *)(A) >= S->base && (char *)(A) < S->base + S->size)

/*
  TOP_FOOT_SIZE is padding at the end of a segment, including space
  that may be needed to place segment records and fenceposts when new
  noncontiguous segments are added.
*/
#define TOP_FOOT_SIZE                                                        \
  (align_offset(chunk2mem(0)) + pad_request(sizeof(struct malloc_segment)) + \
   MIN_CHUNK_SIZE)

/* ───────────── Global MallocState and MallocParams ─────────────────── */

/*
  MallocParams holds global properties, including those that can be
  dynamically set using mallopt. There is a single instance, mparams,
  initialized in init_mparams. Note that the non-zeroness of "magic"
  also serves as an initialization flag.
*/

struct MallocParams {
  size_t magic;
  size_t page_size;
  size_t granularity;
  size_t mmap_threshold;
  size_t trim_threshold;
  flag_t default_mflags;
};

extern struct MallocParams g_mparams;

#define ensure_initialization()     \
  /* we use a constructor [jart] */ \
  assert(g_mparams.magic != 0)
/* (void)(g_mparams.magic != 0 || init_mparams()) */

#define is_initialized(M) ((M)->top != 0)
#define is_page_aligned(S) \
  (((size_t)(S) & (g_mparams.page_size - SIZE_T_ONE)) == 0)
#define is_granularity_aligned(S) \
  (((size_t)(S) & (g_mparams.granularity - SIZE_T_ONE)) == 0)

/* ────────────────────────── system alloc setup ───────────────────────── */

/* Operations on mflags */

#define use_lock(M)    ((M)->mflags & USE_LOCK_BIT)
#define enable_lock(M) ((M)->mflags |= USE_LOCK_BIT)
#if USE_LOCKS
#define disable_lock(M) ((M)->mflags &= ~USE_LOCK_BIT)
#else
#define disable_lock(M)
#endif

#define use_mmap(M)    ((M)->mflags & USE_MMAP_BIT)
#define enable_mmap(M) ((M)->mflags |= USE_MMAP_BIT)
#if HAVE_MMAP
#define disable_mmap(M) ((M)->mflags &= ~USE_MMAP_BIT)
#else
#define disable_mmap(M)
#endif

#define use_noncontiguous(M)  ((M)->mflags & USE_NONCONTIGUOUS_BIT)
#define disable_contiguous(M) ((M)->mflags |= USE_NONCONTIGUOUS_BIT)

#define set_lock(M, L) \
  ((M)->mflags =       \
       (L) ? ((M)->mflags | USE_LOCK_BIT) : ((M)->mflags & ~USE_LOCK_BIT))

/* page-align a size */
#define page_align(S)                           \
  (((S) + (g_mparams.page_size - SIZE_T_ONE)) & \
   ~(g_mparams.page_size - SIZE_T_ONE))

/* granularity-align a size */
#define granularity_align(S)                      \
  (((S) + (g_mparams.granularity - SIZE_T_ONE)) & \
   ~(g_mparams.granularity - SIZE_T_ONE))

#define mmap_align(s) granularity_align((size_t)(s))

/* ──────────────────────── Operations on bin maps ─────────────────────── */

#define idx2bit(i)               ((binmap_t)(1) << (i))
#define mark_smallmap(M, i)      ((M)->smallmap |= idx2bit(i))
#define clear_smallmap(M, i)     ((M)->smallmap &= ~idx2bit(i))
#define smallmap_is_marked(M, i) ((M)->smallmap & idx2bit(i))
#define mark_treemap(M, i)       ((M)->treemap |= idx2bit(i))
#define clear_treemap(M, i)      ((M)->treemap &= ~idx2bit(i))
#define treemap_is_marked(M, i)  ((M)->treemap & idx2bit(i))
#define least_bit(x)             ((x) & -(x))
#define left_bits(x)             ((x << 1) | -(x << 1))
#define same_or_left_bits(x)     ((x) | -(x))
#define compute_bit2idx(X, I) \
  {                           \
    unsigned int J;           \
    J = __builtin_ctz(X);     \
    I = (bindex_t)J;          \
  }

/* ──────────────────────────── Indexing Bins ──────────────────────────── */

#define is_small(s)         (((s) >> SMALLBIN_SHIFT) < NSMALLBINS)
#define small_index(s)      (bindex_t)((s) >> SMALLBIN_SHIFT)
#define small_index2size(i) ((i) << SMALLBIN_SHIFT)
#define MIN_SMALL_INDEX     (small_index(MIN_CHUNK_SIZE))

/* addressing by index. See above about smallbin repositioning */
#define smallbin_at(M, i) ((sbinptr)((char *)&((M)->smallbins[(i) << 1])))
#define treebin_at(M, i)  (&((M)->treebins[i]))

/* assign tree index for size S to variable I.  */
#define compute_tree_index(S, I)                                               \
  {                                                                            \
    unsigned int X = S >> TREEBIN_SHIFT;                                       \
    if (X == 0)                                                                \
      I = 0;                                                                   \
    else if (X > 0xFFFF)                                                       \
      I = NTREEBINS - 1;                                                       \
    else {                                                                     \
      unsigned int K =                                                         \
          (unsigned)sizeof(X) * __CHAR_BIT__ - 1 - (unsigned)__builtin_clz(X); \
      I = (bindex_t)((K << 1) + ((S >> (K + (TREEBIN_SHIFT - 1)) & 1)));       \
    }                                                                          \
  }

/* Bit representing maximum resolved size in a treebin at i */
#define bit_for_tree_index(i) \
  (i == NTREEBINS - 1) ? (SIZE_T_BITSIZE - 1) : (((i) >> 1) + TREEBIN_SHIFT - 2)

/* Shift placing maximum resolved bit in a treebin at i as sign bit */
#define leftshift_for_tree_index(i) \
  ((i == NTREEBINS - 1)             \
       ? 0                          \
       : ((SIZE_T_BITSIZE - SIZE_T_ONE) - (((i) >> 1) + TREEBIN_SHIFT - 2)))

/* The size of the smallest chunk held in bin with index i */
#define minsize_for_tree_index(i)                 \
  ((SIZE_T_ONE << (((i) >> 1) + TREEBIN_SHIFT)) | \
   (((size_t)((i)&SIZE_T_ONE)) << (((i) >> 1) + TREEBIN_SHIFT - 1)))

/* ─────────────────────── Runtime Check Support ───────────────────────── */

/*
  For security, the main invariant is that malloc/free/etc never
  writes to a static address other than MallocState, unless static
  MallocState itself has been corrupted, which cannot occur via
  malloc (because of these checks). In essence this means that we
  believe all pointers, sizes, maps etc held in MallocState, but
  check all of those linked or offsetted from other embedded data
  structures.  These checks are interspersed with main code in a way
  that tends to minimize their run-time cost.

  When FOOTERS is defined, in addition to range checking, we also
  verify footer fields of inuse chunks, which can be used guarantee
  that the mstate controlling malloc/free is intact.  This is a
  streamlined version of the approach described by William Robertson
  et al in "Run-time Detection of Heap-based Overflows" LISA'03
  http://www.usenix.org/events/lisa03/tech/robertson.html The footer
  of an inuse chunk holds the xor of its mstate and a random seed,
  that is checked upon calls to free() and realloc().  This is
  (probabalistically) unguessable from outside the program, but can be
  computed by any code successfully malloc'ing any chunk, so does not
  itself provide protection against code that has already broken
  security through some other means.  Unlike Robertson et al, we
  always dynamically check addresses of all offset chunks (previous,
  next, etc). This turns out to be cheaper than relying on hashes.
*/

#if !IsTrustworthy()
/* Check if address a is at least as high as any from MORECORE or MMAP */
#define ok_address(M, a) ((char *)(a) >= (M)->least_addr)
/* Check if address of next chunk n is higher than base chunk p */
#define ok_next(p, n) ((char *)(p) < (char *)(n))
/* Check if p has inuse status */
#define ok_inuse(p) is_inuse(p)
/* Check if p has its pinuse bit on */
#define ok_pinuse(p) pinuse(p)

#else /* !IsTrustworthy() */
#define ok_address(M, a) (1)
#define ok_next(b, n)    (1)
#define ok_inuse(p)      (1)
#define ok_pinuse(p)     (1)
#endif /* !IsTrustworthy() */

#if (FOOTERS && !IsTrustworthy())
/* Check if (alleged) mstate m has expected magic field */
#define ok_magic(M) \
  ((uintptr_t)(M) <= 0x00007ffffffffffful && (M)->magic == g_mparams.magic)
#else /* (FOOTERS && !IsTrustworthy()) */
#define ok_magic(M) (1)
#endif /* (FOOTERS && !IsTrustworthy()) */

/* In gcc, use __builtin_expect to minimize impact of checks */
#if !IsTrustworthy()
#if defined(__GNUC__) && __GNUC__ >= 3
#define RTCHECK(e) __builtin_expect(e, 1)
#else /* GNUC */
#define RTCHECK(e) (e)
#endif /* GNUC */
#else  /* !IsTrustworthy() */
#define RTCHECK(e) (1)
#endif /* !IsTrustworthy() */

/* macros to set up inuse chunks with or without footers */

#if !FOOTERS

#define mark_inuse_foot(M, p, s)

/* Macros for setting head/foot of non-mmapped chunks */

/* Set cinuse bit and pinuse bit of next chunk */
#define set_inuse(M, p, s)                                  \
  ((p)->head = (((p)->head & PINUSE_BIT) | s | CINUSE_BIT), \
   ((mchunkptr)(((char *)(p)) + (s)))->head |= PINUSE_BIT)

/* Set cinuse and pinuse of this chunk and pinuse of next chunk */
#define set_inuse_and_pinuse(M, p, s)         \
  ((p)->head = (s | PINUSE_BIT | CINUSE_BIT), \
   ((mchunkptr)(((char *)(p)) + (s)))->head |= PINUSE_BIT)

/* Set size, cinuse and pinuse bit of this chunk */
#define set_size_and_pinuse_of_inuse_chunk(M, p, s) \
  ((p)->head = (s | PINUSE_BIT | CINUSE_BIT))

#else /* FOOTERS */

/* Set foot of inuse chunk to be xor of mstate and seed */
#define mark_inuse_foot(M, p, s)                 \
  (((mchunkptr)((char *)(p) + (s)))->prev_foot = \
       ((size_t)(M) ^ g_mparams.magic))

#define get_mstate_for(p)                                            \
  ((mstate)(((mchunkptr)((char *)(p) + (chunksize(p))))->prev_foot ^ \
            g_mparams.magic))

#define set_inuse(M, p, s)                                   \
  ((p)->head = (((p)->head & PINUSE_BIT) | s | CINUSE_BIT),  \
   (((mchunkptr)(((char *)(p)) + (s)))->head |= PINUSE_BIT), \
   mark_inuse_foot(M, p, s))

#define set_inuse_and_pinuse(M, p, s)                        \
  ((p)->head = (s | PINUSE_BIT | CINUSE_BIT),                \
   (((mchunkptr)(((char *)(p)) + (s)))->head |= PINUSE_BIT), \
   mark_inuse_foot(M, p, s))

#define set_size_and_pinuse_of_inuse_chunk(M, p, s) \
  ((p)->head = (s | PINUSE_BIT | CINUSE_BIT), mark_inuse_foot(M, p, s))

#endif /* !FOOTERS */

/* Return segment holding given address */
forceinline msegmentptr segment_holding(mstate m, char *addr) {
  msegmentptr sp = &m->seg;
  for (;;) {
    if (addr >= sp->base && addr < sp->base + sp->size) return sp;
    if ((sp = sp->next) == 0) return 0;
  }
}

/* ─────────────────────── Operations on smallbins ─────────────────────── */

/*
  Various forms of linking and unlinking are defined as macros.  Even
  the ones for trees, which are very long but have very short typical
  paths.  This is ugly but reduces reliance on inlining support of
  compilers.
*/

/* Link a free chunk into a smallbin  */
#define insert_small_chunk(M, P, S)         \
  {                                         \
    bindex_t I = small_index(S);            \
    mchunkptr B = smallbin_at(M, I);        \
    mchunkptr F = B;                        \
    assert(S >= MIN_CHUNK_SIZE);            \
    if (!smallmap_is_marked(M, I))          \
      mark_smallmap(M, I);                  \
    else if (RTCHECK(ok_address(M, B->fd))) \
      F = B->fd;                            \
    else {                                  \
      CORRUPTION_ERROR_ACTION(M);           \
    }                                       \
    B->fd = P;                              \
    F->bk = P;                              \
    P->fd = F;                              \
    P->bk = B;                              \
  }

/* Unlink a chunk from a smallbin  */
#define unlink_small_chunk(M, P, S)                                            \
  {                                                                            \
    mchunkptr F = P->fd;                                                       \
    mchunkptr B = P->bk;                                                       \
    bindex_t I = small_index(S);                                               \
    assert(P != B);                                                            \
    assert(P != F);                                                            \
    assert(chunksize(P) == small_index2size(I));                               \
    if (RTCHECK(F == smallbin_at(M, I) || (ok_address(M, F) && F->bk == P))) { \
      if (B == F) {                                                            \
        clear_smallmap(M, I);                                                  \
      } else if (RTCHECK(B == smallbin_at(M, I) ||                             \
                         (ok_address(M, B) && B->fd == P))) {                  \
        F->bk = B;                                                             \
        B->fd = F;                                                             \
      } else {                                                                 \
        CORRUPTION_ERROR_ACTION(M);                                            \
      }                                                                        \
    } else {                                                                   \
      CORRUPTION_ERROR_ACTION(M);                                              \
    }                                                                          \
  }

/* Unlink the first chunk from a smallbin */
#define unlink_first_small_chunk(M, B, P, I)              \
  {                                                       \
    mchunkptr F = P->fd;                                  \
    assert(P != B);                                       \
    assert(P != F);                                       \
    assert(chunksize(P) == small_index2size(I));          \
    if (B == F) {                                         \
      clear_smallmap(M, I);                               \
    } else if (RTCHECK(ok_address(M, F) && F->bk == P)) { \
      F->bk = B;                                          \
      B->fd = F;                                          \
    } else {                                              \
      CORRUPTION_ERROR_ACTION(M);                         \
    }                                                     \
  }

/* Replace dv node, binning the old one */
/* Used only when dvsize known to be small */
#define replace_dv(M, P, S)           \
  {                                   \
    size_t DVS = M->dvsize;           \
    assert(is_small(DVS));            \
    if (DVS != 0) {                   \
      mchunkptr DV = M->dv;           \
      insert_small_chunk(M, DV, DVS); \
    }                                 \
    M->dvsize = S;                    \
    M->dv = P;                        \
  }

/* ───────────────────────── Operations on trees ───────────────────────── */

/* Insert chunk into tree */
#define insert_large_chunk(M, X, S)                                  \
  {                                                                  \
    tbinptr *H;                                                      \
    bindex_t I;                                                      \
    compute_tree_index(S, I);                                        \
    H = treebin_at(M, I);                                            \
    X->index = I;                                                    \
    X->child[0] = X->child[1] = 0;                                   \
    if (!treemap_is_marked(M, I)) {                                  \
      mark_treemap(M, I);                                            \
      *H = X;                                                        \
      X->parent = (tchunkptr)H;                                      \
      X->fd = X->bk = X;                                             \
    } else {                                                         \
      tchunkptr T = *H;                                              \
      size_t K = S << leftshift_for_tree_index(I);                   \
      for (;;) {                                                     \
        if (chunksize(T) != S) {                                     \
          tchunkptr *C =                                             \
              &(T->child[(K >> (SIZE_T_BITSIZE - SIZE_T_ONE)) & 1]); \
          K <<= 1;                                                   \
          if (*C != 0)                                               \
            T = *C;                                                  \
          else if (RTCHECK(ok_address(M, C))) {                      \
            *C = X;                                                  \
            X->parent = T;                                           \
            X->fd = X->bk = X;                                       \
            break;                                                   \
          } else {                                                   \
            CORRUPTION_ERROR_ACTION(M);                              \
            break;                                                   \
          }                                                          \
        } else {                                                     \
          tchunkptr F = T->fd;                                       \
          if (RTCHECK(ok_address(M, T) && ok_address(M, F))) {       \
            T->fd = F->bk = X;                                       \
            X->fd = F;                                               \
            X->bk = T;                                               \
            X->parent = 0;                                           \
            break;                                                   \
          } else {                                                   \
            CORRUPTION_ERROR_ACTION(M);                              \
            break;                                                   \
          }                                                          \
        }                                                            \
      }                                                              \
    }                                                                \
  }

/*
  Unlink steps:

  1. If x is a chained node, unlink it from its same-sized fd/bk links
     and choose its bk node as its replacement.
  2. If x was the last node of its size, but not a leaf node, it must
     be replaced with a leaf node (not merely one with an open left or
     right), to make sure that lefts and rights of descendents
     correspond properly to bit masks.  We use the rightmost descendent
     of x.  We could use any other leaf, but this is easy to locate and
     tends to counteract removal of leftmosts elsewhere, and so keeps
     paths shorter than minimally guaranteed.  This doesn't loop much
     because on average a node in a tree is near the bottom.
  3. If x is the base of a chain (i.e., has parent links) relink
     x's parent and children to x's replacement (or null if none).
*/

#define unlink_large_chunk(M, X)                                   \
  {                                                                \
    tchunkptr XP = X->parent;                                      \
    tchunkptr R;                                                   \
    if (X->bk != X) {                                              \
      tchunkptr F = X->fd;                                         \
      R = X->bk;                                                   \
      if (RTCHECK(ok_address(M, F) && F->bk == X && R->fd == X)) { \
        F->bk = R;                                                 \
        R->fd = F;                                                 \
      } else {                                                     \
        CORRUPTION_ERROR_ACTION(M);                                \
      }                                                            \
    } else {                                                       \
      tchunkptr *RP;                                               \
      if (((R = *(RP = &(X->child[1]))) != 0) ||                   \
          ((R = *(RP = &(X->child[0]))) != 0)) {                   \
        tchunkptr *CP;                                             \
        while ((*(CP = &(R->child[1])) != 0) ||                    \
               (*(CP = &(R->child[0])) != 0)) {                    \
          R = *(RP = CP);                                          \
        }                                                          \
        if (RTCHECK(ok_address(M, RP)))                            \
          *RP = 0;                                                 \
        else {                                                     \
          CORRUPTION_ERROR_ACTION(M);                              \
        }                                                          \
      }                                                            \
    }                                                              \
    if (XP != 0) {                                                 \
      tbinptr *H = treebin_at(M, X->index);                        \
      if (X == *H) {                                               \
        if ((*H = R) == 0) clear_treemap(M, X->index);             \
      } else if (RTCHECK(ok_address(M, XP))) {                     \
        if (XP->child[0] == X)                                     \
          XP->child[0] = R;                                        \
        else                                                       \
          XP->child[1] = R;                                        \
      } else                                                       \
        CORRUPTION_ERROR_ACTION(M);                                \
      if (R != 0) {                                                \
        if (RTCHECK(ok_address(M, R))) {                           \
          tchunkptr C0, C1;                                        \
          R->parent = XP;                                          \
          if ((C0 = X->child[0]) != 0) {                           \
            if (RTCHECK(ok_address(M, C0))) {                      \
              R->child[0] = C0;                                    \
              C0->parent = R;                                      \
            } else                                                 \
              CORRUPTION_ERROR_ACTION(M);                          \
          }                                                        \
          if ((C1 = X->child[1]) != 0) {                           \
            if (RTCHECK(ok_address(M, C1))) {                      \
              R->child[1] = C1;                                    \
              C1->parent = R;                                      \
            } else                                                 \
              CORRUPTION_ERROR_ACTION(M);                          \
          }                                                        \
        } else                                                     \
          CORRUPTION_ERROR_ACTION(M);                              \
      }                                                            \
    }                                                              \
  }

/* Relays to large vs small bin operations */
#define insert_chunk(M, P, S)                         \
  if (is_small(S)) insert_small_chunk(M, P, S) else { \
      tchunkptr TP = (tchunkptr)(P);                  \
      insert_large_chunk(M, TP, S);                   \
    }
#define unlink_chunk(M, P, S)                         \
  if (is_small(S)) unlink_small_chunk(M, P, S) else { \
      tchunkptr TP = (tchunkptr)(P);                  \
      unlink_large_chunk(M, TP);                      \
    }

#ifndef MORECORE_CANNOT_TRIM
#define should_trim(M, s) ((s) > (M)->trim_check)
#else /* MORECORE_CANNOT_TRIM */
#define should_trim(M, s) (0)
#endif /* MORECORE_CANNOT_TRIM */

/*
  TOP_FOOT_SIZE is padding at the end of a segment, including space
  that may be needed to place segment records and fenceposts when new
  noncontiguous segments are added.
*/
#define TOP_FOOT_SIZE                                                        \
  (align_offset(chunk2mem(0)) + pad_request(sizeof(struct malloc_segment)) + \
   MIN_CHUNK_SIZE)

/* ────────────────────────── Debugging setup ──────────────────────────── */

#if !(DEBUG + MODE_DBG + 0)
#define check_free_chunk(M, P)
#define check_inuse_chunk(M, P)
#define check_malloced_chunk(M, P, N)
#define check_mmapped_chunk(M, P)
#define check_malloc_state(M)
#define check_top_chunk(M, P)
#else /* DEBUG */
#define check_free_chunk(M, P)        do_check_free_chunk(M, P)
#define check_inuse_chunk(M, P)       do_check_inuse_chunk(M, P)
#define check_top_chunk(M, P)         do_check_top_chunk(M, P)
#define check_malloced_chunk(M, P, N) do_check_malloced_chunk(M, P, N)
#define check_mmapped_chunk(M, P)     do_check_mmapped_chunk(M, P)
#define check_malloc_state(M)         do_check_malloc_state(M)
#endif /* DEBUG */

void do_check_free_chunk(mstate m, mchunkptr p) hidden;
void do_check_inuse_chunk(mstate m, mchunkptr p) hidden;
void do_check_top_chunk(mstate m, mchunkptr p) hidden;
void do_check_malloced_chunk(mstate m, void *mem, size_t s) hidden;
void do_check_mmapped_chunk(mstate m, mchunkptr p) hidden;
void do_check_malloc_state(mstate m) hidden;

/* ─────────────────────────── prototypes ──────────────────────────────── */

void *dlmalloc(size_t) hidden;
void *dlcalloc(size_t, size_t) hidden;
void dlfree(void *) nothrow nocallback hidden;
void *dlmemalign$impl(mstate, size_t, size_t) hidden;
void *dlrealloc(void *, size_t) hidden;
void *dlrealloc_in_place(void *, size_t) hidden;
void *dlvalloc(size_t) hidden;
void *dlpvalloc(size_t) hidden;
void *dlmemalign(size_t, size_t) hidden;
int dlmalloc_trim(size_t) hidden;
size_t dlmalloc_usable_size(const void *) hidden;
int dlposix_memalign(void **, size_t, size_t) hidden;
void **dlindependent_calloc(size_t, size_t, void *[]) hidden;
void **dlindependent_comalloc(size_t, size_t[], void *[]) hidden;
struct MallocStats dlmalloc_stats(mstate) hidden;
int dlmalloc_sys_trim(mstate, size_t) hidden;
void dlmalloc_dispose_chunk(mstate, mchunkptr, size_t) hidden;
mchunkptr dlmalloc_try_realloc_chunk(mstate, mchunkptr, size_t, int) hidden;

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* !ANSI */
#endif /* COSMOPOLITAN_LIBC_MEM_DLMALLOC_H_ */
