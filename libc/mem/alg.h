#ifndef COSMOPOLITAN_LIBC_ALG_ALG_H_
#define COSMOPOLITAN_LIBC_ALG_ALG_H_
COSMOPOLITAN_C_START_

void *bsearch(const void *, const void *, size_t, size_t,
              int (*)(const void *, const void *)) paramsnonnull() nosideeffect;
void *bsearch_r(const void *, const void *, size_t, size_t,
                int (*)(const void *, const void *, void *), void *)
    paramsnonnull((1, 2, 5)) nosideeffect;
void djbsort(int32_t *, size_t) libcesque;
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

int radix_sort_int32(int32_t *, size_t) libcesque;
int radix_sort_int64(int64_t *, size_t) libcesque;

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_ALG_ALG_H_ */
