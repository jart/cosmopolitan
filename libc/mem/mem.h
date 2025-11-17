#ifndef COSMOPOLITAN_LIBC_MEM_MEM_H_
#define COSMOPOLITAN_LIBC_MEM_MEM_H_

#define M_TRIM_THRESHOLD (-1)
#define M_GRANULARITY    (-2)
#define M_MMAP_THRESHOLD (-3)
#define M_RSEQ_MAX       (-4)

COSMOPOLITAN_C_START_
/*───────────────────────────────────────────────────────────────────────────│─╗
│ cosmopolitan § dynamic memory                                            ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/

void free(void *) libcesque;
void *malloc(size_t) attributeallocsize((1)) mallocesque;
void *calloc(size_t, size_t) attributeallocsize((1, 2)) mallocesque;
void *memalign(size_t, size_t) attributeallocalign((1))
    attributeallocsize((2)) returnspointerwithnoaliases libcesque __wur;
void *realloc(void *, size_t) reallocesque;
void *realloc_in_place(void *, size_t) reallocesque;
void *reallocarray(void *, size_t, size_t) __wur;
void *valloc(size_t) attributeallocsize((1)) vallocesque;
void *pvalloc(size_t) vallocesque;
char *strdup(const char *) paramsnonnull() mallocesque;
char *strndup(const char *, size_t) paramsnonnull() mallocesque;
void *aligned_alloc(size_t, size_t) attributeallocalign((1))
    attributeallocsize((2)) returnspointerwithnoaliases libcesque __wur;
int posix_memalign(void **, size_t, size_t) libcesque;
int mallopt(int, int) libcesque;
int malloc_trim(size_t) libcesque;
size_t malloc_usable_size(void *) libcesque;
wchar_t *wcsdup(const wchar_t *) strlenesque __wur;

struct mallinfo {
  size_t arena;    /* non-mmaped space allocated (bytes) */
  size_t ordblks;  /* number of free ordinary chunks */
  size_t smblks;   /* number of free rseq chunks */
  size_t hblks;    /* number of mapped regions */
  size_t hblkhd;   /* space in mmapped regions (bytes) */
  size_t usmblks;  /* peak space allocated from system (bytes) */
  size_t fsmblks;  /* space in freed rseq chunks (bytes) */
  size_t uordblks; /* total allocated space (bytes) */
  size_t fordblks; /* total free space (bytes) */
  size_t keepcost; /* releasable (via malloc_trim) space (bytes) */
};

struct mallinfo mallinfo(void) libcesque;

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_MEM_MEM_H_ */
