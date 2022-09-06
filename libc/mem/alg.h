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
void qsort(void *, size_t, size_t, int (*)(const void *, const void *))
    paramsnonnull();
void qsort_r(void *, size_t, size_t,
             int (*)(const void *, const void *, void *), void *)
    paramsnonnull((1, 4));
void smoothsort(void *, size_t, size_t, int (*)(const void *, const void *));
void smoothsort_r(void *, size_t, size_t,
                  int (*)(const void *, const void *, void *), void *);
int tarjan(int, const int (*)[2], int, int[], int[], int *)
    paramsnonnull((2, 4)) nocallback dontthrow;

#define __algalloc returnspointerwithnoaliases dontthrow nocallback dontdiscard

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

int cmpsb(const void *, const void *);
int cmpub(const void *, const void *);
int cmpsw(const void *, const void *);
int cmpuw(const void *, const void *);
int cmpsl(const void *, const void *);
int cmpul(const void *, const void *);
int cmpsq(const void *, const void *);
int cmpuq(const void *, const void *);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_ALG_ALG_H_ */
