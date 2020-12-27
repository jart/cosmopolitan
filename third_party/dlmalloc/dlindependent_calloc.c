#include "libc/mem/mem.h"
#include "libc/str/str.h"
#include "third_party/dlmalloc/dlmalloc.internal.h"

/*
  Common support for independent_X routines, handling
    all of the combinations that can result.
  The opts arg has:
    bit 0 set if all elements are same size (using sizes[0])
    bit 1 set if elements should be zeroed
*/
static void **ialloc(mstate m, size_t n_elements, size_t *sizes, int opts,
                     void *chunks[]) {
  size_t element_size;   /* chunksize of each element, if all same */
  size_t contents_size;  /* total size of elements */
  size_t array_size;     /* request size of pointer array */
  void *mem;             /* malloced aggregate space */
  mchunkptr p;           /* corresponding chunk */
  size_t remainder_size; /* remaining bytes while splitting */
  void **marray;         /* either "chunks" or malloced ptr array */
  mchunkptr array_chunk; /* chunk for malloced ptr array */
  flag_t was_enabled;    /* to disable mmap */
  size_t size;
  size_t i;

  ensure_initialization();
  /* compute array length, if needed */
  if (chunks != 0) {
    if (n_elements == 0) return chunks; /* nothing to do */
    marray = chunks;
    array_size = 0;
  } else {
    /* if empty req, must still return chunk representing empty array */
    if (n_elements == 0) return (void **)dlmalloc(0);
    marray = 0;
    array_size = request2size(n_elements * (sizeof(void *)));
  }

  /* compute total element size */
  if (opts & 0x1) { /* all-same-size */
    element_size = request2size(*sizes);
    contents_size = n_elements * element_size;
  } else { /* add up all the sizes */
    element_size = 0;
    contents_size = 0;
    for (i = 0; i != n_elements; ++i) contents_size += request2size(sizes[i]);
  }

  size = contents_size + array_size;

  /*
     Allocate the aggregate chunk.  First disable direct-mmapping so
     malloc won't use it, since we would not be able to later
     free/realloc space internal to a segregated mmap region.
  */
  was_enabled = use_mmap(m);
  disable_mmap(m);
  mem = dlmalloc(size - CHUNK_OVERHEAD);
  if (was_enabled) enable_mmap(m);
  if (mem == 0) return 0;

  if (PREACTION(m)) return 0;
  p = mem2chunk(mem);
  remainder_size = chunksize(p);

  assert(!is_mmapped(p));

  if (opts & 0x2) { /* optionally clear the elements */
    memset((size_t *)mem, 0, remainder_size - SIZE_T_SIZE - array_size);
  }

  /* If not provided, allocate the pointer array as final part of chunk */
  if (marray == 0) {
    size_t array_chunk_size;
    array_chunk = chunk_plus_offset(p, contents_size);
    array_chunk_size = remainder_size - contents_size;
    marray = ADDRESS_BIRTH_ACTION((void **)(chunk2mem(array_chunk)));
    set_size_and_pinuse_of_inuse_chunk(m, array_chunk, array_chunk_size);
    remainder_size = contents_size;
  }

  /* split out elements */
  for (i = 0;; ++i) {
    marray[i] = ADDRESS_BIRTH_ACTION(chunk2mem(p));
    if (i != n_elements - 1) {
      if (element_size != 0)
        size = element_size;
      else
        size = request2size(sizes[i]);
      remainder_size -= size;
      set_size_and_pinuse_of_inuse_chunk(m, p, size);
      p = chunk_plus_offset(p, size);
    } else { /* the final element absorbs any overallocation slop */
      set_size_and_pinuse_of_inuse_chunk(m, p, remainder_size);
      break;
    }
  }

#if DEBUG + MODE_DBG + 0
  if (marray != chunks) {
    /* final element must have exactly exhausted chunk */
    if (element_size != 0) {
      assert(remainder_size == element_size);
    } else {
      assert(remainder_size == request2size(sizes[i]));
    }
    check_inuse_chunk(m, mem2chunk(marray));
  }
  for (i = 0; i != n_elements; ++i) check_inuse_chunk(m, mem2chunk(marray[i]));

#endif /* DEBUG */

  POSTACTION(m);
  return marray;
}

/**
 * independent_calloc(size_t n_elements, size_t element_size, void* chunks[]);
 *
 * independent_calloc is similar to calloc, but instead of returning a
 * single cleared space, it returns an array of pointers to n_elements
 * independent elements that can hold contents of size elem_size, each
 * of which starts out cleared, and can be independently freed,
 * realloc'ed etc. The elements are guaranteed to be adjacently
 * allocated (this is not guaranteed to occur with multiple callocs or
 * mallocs), which may also improve cache locality in some applications.
 *
 * The "chunks" argument is optional (i.e., may be null, which is
 * probably the most typical usage). If it is null, the returned array
 * is itself dynamically allocated and should also be freed when it is
 * no longer needed. Otherwise, the chunks array must be of at least
 * n_elements in length. It is filled in with the pointers to the
 * chunks.
 *
 * In either case, independent_calloc returns this pointer array, or
 * null if the allocation failed. * If n_elements is zero and "chunks"
 * is null, it returns a chunk representing an array with zero elements
 * (which should be freed if not wanted).
 *
 * Each element must be freed when it is no longer needed. This can be
 * done all at once using bulk_free.
 *
 * independent_calloc simplifies and speeds up implementations of many
 * kinds of pools. * It may also be useful when constructing large data
 * structures that initially have a fixed number of fixed-sized nodes,
 * but the number is not known at compile time, and some of the nodes
 * may later need to be freed. For example:
 *
 *     struct Node { int item; struct Node* next; };
 *     struct Node* build_list() {
 *       struct Node **pool;
 *       int n = read_number_of_nodes_needed();
 *       if (n <= 0) return 0;
 *       pool = (struct Node**)(independent_calloc(n, sizeof(struct Node), 0);
 *       if (pool == 0) __die();
 *       // organize into a linked list...
 *       struct Node* first = pool[0];
 *       for (i = 0; i < n-1; ++i)
 *       pool[i]->next = pool[i+1];
 *       free(pool); * // Can now free the array (or not, if it is needed later)
 *       return first;
 *     }
 */
void **dlindependent_calloc(size_t n_elements, size_t elem_size,
                            void *chunks[]) {
  size_t sz = elem_size; /* serves as 1-element array */
  return ialloc(g_dlmalloc, n_elements, &sz, 3, chunks);
}

/**
 * independent_comalloc(size_t n_elements, size_t sizes[], void* chunks[]);
 *
 * independent_comalloc allocates, all at once, a set of n_elements
 * chunks with sizes indicated in the "sizes" array. It returns an array
 * of pointers to these elements, each of which can be independently
 * freed, realloc'ed etc. The elements are guaranteed to be adjacently
 * allocated (this is not guaranteed to occur with multiple callocs or
 * mallocs), which may also improve cache locality in some applications.
 *
 * The "chunks" argument is optional (i.e., may be null). If it is null
 * the returned array is itself dynamically allocated and should also
 * be freed when it is no longer needed. Otherwise, the chunks array
 * must be of at least n_elements in length. It is filled in with the
 * pointers to the chunks.
 *
 * In either case, independent_comalloc returns this pointer array, or
 * null if the allocation failed.  If n_elements is zero and chunks is
 * null, it returns a chunk representing an array with zero elements
 * (which should be freed if not wanted).
 *
 * Each element must be freed when it is no longer needed. This can be
 * done all at once using bulk_free.
 *
 * independent_comallac differs from independent_calloc in that each
 * element may have a different size, and also that it does not
 * automatically clear elements.
 *
 * independent_comalloc can be used to speed up allocation in cases
 * where several structs or objects must always be allocated at the
 * same time.  For example:
 *
 *     struct Head { ... }
 *     struct Foot { ... }
 *     void send_message(char* msg) {
 *       int msglen = strlen(msg);
 *       size_t sizes[3] = { sizeof(struct Head), msglen, sizeof(struct Foot) };
 *       void* chunks[3];
 *       if (independent_comalloc(3, sizes, chunks) == 0) __die();
 *       struct Head* head = (struct Head*)(chunks[0]);
 *       char*        body = (char*)(chunks[1]);
 *       struct Foot* foot = (struct Foot*)(chunks[2]);
 *       // ...
 *     }
 *
 * In general though, independent_comalloc is worth using only for
 * larger values of n_elements. For small values, you probably won't
 * detect enough difference from series of malloc calls to bother.
 *
 * Overuse of independent_comalloc can increase overall memory usage,
 * since it cannot reuse existing noncontiguous small chunks that might
 * be available for some of the elements.
 */
void **dlindependent_comalloc(size_t n_elements, size_t sizes[],
                              void *chunks[]) {
  return ialloc(g_dlmalloc, n_elements, sizes, 0, chunks);
}
