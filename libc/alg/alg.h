#ifndef COSMOPOLITAN_LIBC_ALG_ALG_H_
#define COSMOPOLITAN_LIBC_ALG_ALG_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_
/*───────────────────────────────────────────────────────────────────────────│─╗
│ cosmopolitan § algorithms                                                ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/

void *bsearch(const void *, const void *, size_t, size_t,
              int cmp(const void *, const void *))
    paramsnonnull() nothrow nosideeffect;
void *bsearch_r(const void *, const void *, size_t, size_t,
                int cmp(const void *, const void *, void *), void *)
    paramsnonnull((1, 2, 5)) nothrow nosideeffect;
void djbsort(size_t n, int32_t[n]);
void qsort(void *items, size_t itemcount, size_t itemsize,
           int cmp(const void *, const void *)) paramsnonnull();
void qsort_r(void *items, size_t itemcount, size_t itemsize,
             int cmp(const void *, const void *, void *), void *arg)
    paramsnonnull((1, 4));
int tarjan(uint32_t vertex_count, const uint32_t (*edges)[2],
           uint32_t edge_count, uint32_t out_sorted[],
           uint32_t out_opt_components[], uint32_t *out_opt_componentcount)
    paramsnonnull((2, 4)) nocallback nothrow;
void heapsortcar(int32_t (*A)[2], unsigned n)
    paramsnonnull() nocallback nothrow;

void *memmem(const void *, size_t, const void *, size_t)
    paramsnonnull() nothrow nocallback nosideeffect;
void *memmem16(const void *, size_t, const void *, size_t)
    paramsnonnull() nothrow nocallback nosideeffect;
void *wmemmem(const void *, size_t, const void *, size_t)
    paramsnonnull() nothrow nocallback nosideeffect;

#define __algalloc returnspointerwithnoaliases nothrow nocallback nodiscard

char *replacestr(const char *, const char *, const char *)
    paramsnonnull() __algalloc;
char16_t *replacestr16(const char16_t *, const char16_t *, const char16_t *)
    paramsnonnull() __algalloc;
wchar_t *replacewcs(const wchar_t *, const wchar_t *, const wchar_t *)
    paramsnonnull() __algalloc;

char *concatstr(const char *, ...) nullterminated() paramsnonnull() __algalloc;
char16_t *concatstr16(const char16_t *, ...) nullterminated()
    paramsnonnull() __algalloc;
wchar_t *concatwcs(const wchar_t *, ...) nullterminated()
    paramsnonnull() __algalloc;

/*───────────────────────────────────────────────────────────────────────────│─╗
│ cosmopolitan § algorithms » containers                                   ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/

struct critbit0 {
  void *root;
  size_t count;
};

bool critbit0_contains(struct critbit0 *t, const char *u) nothrow nosideeffect
    paramsnonnull();
bool critbit0_insert(struct critbit0 *t, const char *u) paramsnonnull();
bool critbit0_delete(struct critbit0 *t, const char *u) nothrow paramsnonnull();
void critbit0_clear(struct critbit0 *t) nothrow paramsnonnull();
char *critbit0_get(struct critbit0 *t, const char *u);
intptr_t critbit0_allprefixed(struct critbit0 *t, const char *prefix,
                              intptr_t (*callback)(const char *elem, void *arg),
                              void *arg) paramsnonnull((1, 2, 3)) nothrow;
bool critbit0_emplace(struct critbit0 *t, char *u, size_t ulen) paramsnonnull();

/*───────────────────────────────────────────────────────────────────────────│─╗
│ cosmopolitan § algorithms » comparators                                  ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/

int cmpsb(/*const signed char[1]*/ const void *, const void *)
    paramsnonnull() nothrow nocallback nosideeffect;
int cmpub(/*const unsigned char[1]*/ const void *, const void *)
    paramsnonnull() nothrow nocallback nosideeffect;
int cmpsw(/*const signed short[1]*/ const void *, const void *)
    paramsnonnull() nothrow nocallback nosideeffect;
int cmpuw(/*const unsigned short[1]*/ const void *, const void *)
    paramsnonnull() nothrow nocallback nosideeffect;
int cmpsl(/*const signed int[1]*/ const void *, const void *)
    paramsnonnull() nothrow nocallback nosideeffect;
int cmpul(/*const unsigned int[1]*/ const void *, const void *)
    paramsnonnull() nothrow nocallback nosideeffect;
int cmpsq(/*const signed long[1]*/ const void *, const void *)
    paramsnonnull() nothrow nocallback nosideeffect;
int cmpuq(/*const unsigned long[1]*/ const void *, const void *)
    paramsnonnull() nothrow nocallback nosideeffect;

/*───────────────────────────────────────────────────────────────────────────│─╗
│ cosmopolitan § algorithms » generics                                     ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/

#if __STDC_VERSION__ + 0 >= 201112

#define memmem(haystack, haystacklen, needle, needlelen) \
  _Generic(*(haystack), wchar_t                          \
           : wmemmem, char16_t                           \
           : memmem16, default                           \
           : memmem)(haystack, haystacklen, needle, needlelen)

#define replacestr(s, needle, replacement) \
  _Generic(*(s), wchar_t                   \
           : replacewcs, char16_t          \
           : replacestr16, default         \
           : replacestr)(s, needle, replacement)

#define concatstr(s, ...)         \
  _Generic(*(s), wchar_t          \
           : concatwcs, char16_t  \
           : concatstr16, default \
           : concatstr)(s, __VA_ARGS__)

#endif /* C11 */

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_ALG_ALG_H_ */
