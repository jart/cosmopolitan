#ifndef COSMOPOLITAN_LIBC_FMT_CONV_H_
#define COSMOPOLITAN_LIBC_FMT_CONV_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

int abs(int) libcesque pureconst;
long labs(long) libcesque pureconst;
long long llabs(long long) libcesque pureconst;
intmax_t imaxabs(intmax_t)
libcesque pureconst;
int atoi(const char *) paramsnonnull() libcesque;
long atol(const char *) paramsnonnull() libcesque;
long long atoll(const char *) paramsnonnull() libcesque;
unsigned long strtoul(const char *, char **, int) paramsnonnull((1));
long long strtoll(const char *, char **, int) paramsnonnull((1));
unsigned long long strtoull(const char *, char **, int) paramsnonnull((1));
intmax_t strtoimax(const char *, char **, int) paramsnonnull((1));
uintmax_t strtoumax(const char *, char **, int) paramsnonnull((1));
intmax_t wcstoimax(const wchar_t *, wchar_t **, int);
uintmax_t wcstoumax(const wchar_t *, wchar_t **, int);
long wcstol(const wchar_t *, wchar_t **, int);
unsigned long wcstoul(const wchar_t *, wchar_t **, int);
long strtol(const char *, char **, int) paramsnonnull((1)) libcesque;
long sizetol(const char *, long) paramsnonnull() libcesque;
char *sizefmt(char *, uint64_t, uint64_t);
long long wcstoll(const wchar_t *, wchar_t **, int);
unsigned long long wcstoull(const wchar_t *, wchar_t **, int);
int wcscoll(const wchar_t *, const wchar_t *);
size_t wcsxfrm(wchar_t *, const wchar_t *, size_t);

double atof(const char *);
float strtof(const char *, char **);
double strtod(const char *, char **);
long double strtold(const char *, char **);
float wcstof(const wchar_t *, wchar_t **);
double wcstod(const wchar_t *, wchar_t **);
long double wcstold(const wchar_t *, wchar_t **);

#ifdef _COSMO_SOURCE
char *stripext(char *);
char *stripexts(char *);
#endif /* _COSMO_SOURCE */

typedef struct {
  int quot;
  int rem;
} div_t;

typedef struct {
  long int quot;
  long int rem;
} ldiv_t;

typedef struct {
  long long int quot;
  long long int rem;
} lldiv_t;

typedef struct {
  intmax_t quot;
  intmax_t rem;
} imaxdiv_t;

div_t div(int, int) pureconst;
ldiv_t ldiv(long, long) pureconst;
lldiv_t lldiv(long long, long long) pureconst;
imaxdiv_t imaxdiv(intmax_t, intmax_t) pureconst;

#if __STDC_VERSION__ + 0 >= 199901L
#define div(num, den)   ((div_t){(num) / (den), (num) % (den)})
#define ldiv(num, den)  ((ldiv_t){(num) / (den), (num) % (den)})
#define lldiv(num, den) ((lldiv_t){(num) / (den), (num) % (den)})
#endif

#if (__GNUC__ * 100 + __GNUC_MINOR__ >= 406 || defined(__llvm__)) && \
    !defined(__STRICT_ANSI__) && defined(_COSMO_SOURCE)
int128_t i128abs(int128_t)
libcesque pureconst;
int128_t strtoi128(const char *, char **, int) paramsnonnull((1));
uint128_t strtou128(const char *, char **, int) paramsnonnull((1));
int128_t wcstoi128(const wchar_t *, wchar_t **, int);
uint128_t wcstou128(const wchar_t *, wchar_t **, int);
#endif /* gcc 4.6+ */

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_FMT_CONV_H_ */
