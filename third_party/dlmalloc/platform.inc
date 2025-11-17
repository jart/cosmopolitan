#define MAX_SIZE_T (~(size_t)0)

/* ------------------- size_t and alignment properties -------------------- */

/* The byte and bit size of a size_t */
#define SIZE_T_SIZE         (sizeof(size_t))
#define SIZE_T_BITSIZE      (sizeof(size_t) << 3)

/* Some constants coerced to size_t */
/* Annoying but necessary to avoid errors on some platforms */
#define SIZE_T_ZERO         ((size_t)0)
#define SIZE_T_ONE          ((size_t)1)
#define SIZE_T_TWO          ((size_t)2)
#define SIZE_T_FOUR         ((size_t)4)
#define TWO_SIZE_T_SIZES    (SIZE_T_SIZE<<1)
#define FOUR_SIZE_T_SIZES   (SIZE_T_SIZE<<2)
#define SIX_SIZE_T_SIZES    (FOUR_SIZE_T_SIZES+TWO_SIZE_T_SIZES)
#define HALF_MAX_SIZE_T     (MAX_SIZE_T / 2U)

/* The bit mask value corresponding to MALLOC_ALIGNMENT */
#define CHUNK_ALIGN_MASK    (MALLOC_ALIGNMENT - SIZE_T_ONE)

/* True if address a has acceptable alignment */
#define is_aligned(A)       (((size_t)((A)) & (CHUNK_ALIGN_MASK)) == 0)

/* the number of bytes to offset an address to align it */
#define align_offset(A)\
 ((((size_t)(A) & CHUNK_ALIGN_MASK) == 0)? 0 :\
  ((MALLOC_ALIGNMENT - ((size_t)(A) & CHUNK_ALIGN_MASK)) & CHUNK_ALIGN_MASK))

/* -------------------------- MMAP preliminaries ------------------------- */

/* MORECORE and MMAP must return MFAIL on failure */
#define MFAIL                NULL
#define CMFAIL               ((char*)(MFAIL)) /* defined for convenience */

#define USE_MMAP_BIT (SIZE_T_ONE)

#if HAVE_MREMAP
static void *dlmalloc_mremap(void *old_addr, size_t old_size, size_t new_size, int flags) {
  int olderr = errno;
  void *res = cosmo_mremap(old_addr, old_size, new_size, flags);
  if (res == MAP_FAILED)
    errno = olderr;
  return res;
}
#define CALL_MREMAP(addr, osz, nsz, mv) dlmalloc_mremap(addr, osz, nsz, mv)
#else  /* HAVE_MREMAP */
#define CALL_MREMAP(addr, osz, nsz, mv) MAP_FAILED
#endif /* HAVE_MREMAP */

/* mstate bit set if continguous morecore disabled or failed */
#define USE_NONCONTIGUOUS_BIT (4U)

/* segment bit set in create_mspace_with_base */
#define EXTERN_BIT            (8U)
 
