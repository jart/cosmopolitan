#ifndef COSMOPOLITAN_LIBC_ALG_ALG_H_
#define COSMOPOLITAN_LIBC_ALG_ALG_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

void *bsearch(const void *, const void *, size_t, size_t,
              int (*)(const void *, const void *))
    paramsnonnull() dontthrow nosideeffect;
void *bsearch_r(const void *, const void *, size_t, size_t,
                int (*)(const void *, const void *, void *), void *)
    paramsnonnull((1, 2, 5)) dontthrow nosideeffect;
void djbsort(int32_t *, size_t);
void qsort3(void *, size_t, size_t, int (*)(const void *, const void *))
    paramsnonnull();
void qsort(void *, size_t, size_t, int (*)(const void *, const void *))
    paramsnonnull();
void qsort_r(void *, size_t, size_t,
             int (*)(const void *, const void *, void *), void *)
    paramsnonnull((1, 4));
void smoothsort(void *, size_t, size_t, int (*)(const void *, const void *));
void smoothsort_r(void *, size_t, size_t,
                  int (*)(const void *, const void *, void *), void *);
int heapsort(void *, size_t, size_t, int (*)(const void *, const void *));
int heapsort_r(void *, size_t, size_t,
               int (*)(const void *, const void *, void *), void *);
int mergesort(void *, size_t, size_t, int (*)(const void *, const void *));
int mergesort_r(void *, size_t, size_t,
                int (*)(const void *, const void *, void *), void *);
int _tarjan(int, const int (*)[2], int, int[], int[], int *)
    paramsnonnull((2, 4)) nocallback dontthrow;

#define __algalloc returnspointerwithnoaliases dontthrow nocallback dontdiscard

char *_replacestr(const char *, const char *, const char *)
    paramsnonnull() __algalloc;

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_ALG_ALG_H_ */
