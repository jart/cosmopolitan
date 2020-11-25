#ifndef COSMOPOLITAN_LIBC_MEM_MEM_H_
#define COSMOPOLITAN_LIBC_MEM_MEM_H_
#include "libc/fmt/pflink.h"

#define M_TRIM_THRESHOLD (-1)
#define M_GRANULARITY    (-2)
#define M_MMAP_THRESHOLD (-3)

#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_
/*───────────────────────────────────────────────────────────────────────────│─╗
│ cosmopolitan § dynamic memory                                            ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/

void free(void *) libcesque;
void *malloc(size_t) attributeallocsize((1)) mallocesque;
void *calloc(size_t, size_t) attributeallocsize((1, 2)) mallocesque;
void *memalign(size_t, size_t) attributeallocalign((1))
    attributeallocsize((2)) mallocesque;
void *realloc(void *, size_t) reallocesque;
void *realloc_in_place(void *, size_t);
void *reallocarray(void *, size_t, size_t) nodiscard;
void *valloc(size_t) attributeallocsize((1)) vallocesque;
void *pvalloc(size_t) attributeallocsize((1)) mallocesque;
char *strdup(const char *) paramsnonnull() mallocesque;
char *strndup(const char *, size_t) paramsnonnull()
    attributeallocsize((2)) mallocesque;
int posix_memalign(void **, size_t, size_t); /* wut */
bool __grow(void *, size_t *, size_t, size_t) paramsnonnull((1, 2)) libcesque;

int malloc_trim(size_t);
size_t bulk_free(void **, size_t);
size_t malloc_usable_size(const void *);
void **independent_calloc(size_t, size_t, void **);
void **independent_comalloc(size_t, size_t *, void **);

int asprintf(char **, const char *, ...) printfesque(2)
    paramsnonnull((1, 2)) libcesque;
int vasprintf(char **, const char *, va_list) paramsnonnull() libcesque;

wchar_t *wcsdup(const wchar_t *) strlenesque nodiscard;

struct mallinfo {
  size_t arena;    /* non-mmapped space allocated from system */
  size_t ordblks;  /* number of free chunks */
  size_t smblks;   /* always 0 */
  size_t hblks;    /* always 0 */
  size_t hblkhd;   /* space in mmapped regions */
  size_t usmblks;  /* maximum total allocated space */
  size_t fsmblks;  /* always 0 */
  size_t uordblks; /* total allocated space */
  size_t fordblks; /* total free space */
  size_t keepcost; /* releasable (via malloc_trim) space */
};
struct mallinfo mallinfo(void);

void malloc_stats(void);
bool32 mallopt(int, int);
size_t malloc_footprint(void);
size_t malloc_max_footprint(void);
size_t malloc_footprint_limit(void);
size_t malloc_set_footprint_limit(size_t);
void malloc_inspect_all(void (*handler)(void *, void *, size_t, void *),
                        void *);

/*───────────────────────────────────────────────────────────────────────────│─╗
│ cosmopolitan § dynamic memory » optimizations                            ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/

#if defined(__GNUC__) && !defined(__STRICT_ANSI__)
#define asprintf(SP, FMT, ...) (asprintf)(SP, PFLINK(FMT), ##__VA_ARGS__)
#define vasprintf(SP, FMT, VA) (vasprintf)(SP, PFLINK(FMT), VA)
#endif

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_MEM_MEM_H_ */
