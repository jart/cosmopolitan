#ifndef COSMOPOLITAN_LIBC_FMT_CONV_H_
#define COSMOPOLITAN_LIBC_FMT_CONV_H_
COSMOPOLITAN_C_START_

int abs(int) libcesque pureconst;
long labs(long) libcesque pureconst;
long long llabs(long long) libcesque pureconst;
libcesque intmax_t imaxabs(intmax_t) pureconst;
int atoi(const char *) paramsnonnull() libcesque;
long atol(const char *) paramsnonnull() libcesque;
long long atoll(const char *) paramsnonnull() libcesque;
unsigned long strtoul(const char *, char **, int) libcesque paramsnonnull((1));
long long strtoll(const char *, char **, int) libcesque paramsnonnull((1));
unsigned long long strtoull(const char *, char **, int) paramsnonnull((1));
intmax_t strtoimax(const char *, char **, int) libcesque paramsnonnull((1));
uintmax_t strtoumax(const char *, char **, int) libcesque paramsnonnull((1));
intmax_t wcstoimax(const wchar_t *, wchar_t **, int) libcesque;
uintmax_t wcstoumax(const wchar_t *, wchar_t **, int) libcesque;
long wcstol(const wchar_t *, wchar_t **, int) libcesque;
unsigned long wcstoul(const wchar_t *, wchar_t **, int) libcesque;
long strtol(const char *, char **, int) paramsnonnull((1)) libcesque;
long sizetol(const char *, long) paramsnonnull() libcesque;
char *sizefmt(char *, uint64_t, uint64_t) libcesque;
long long wcstoll(const wchar_t *, wchar_t **, int) libcesque;
unsigned long long wcstoull(const wchar_t *, wchar_t **, int) libcesque;
int wcscoll(const wchar_t *, const wchar_t *) libcesque;
size_t wcsxfrm(wchar_t *, const wchar_t *, size_t) libcesque;

double atof(const char *) libcesque;
float strtof(const char *, char **) libcesque;
double strtod(const char *, char **) libcesque;
long double strtold(const char *, char **) libcesque;
float wcstof(const wchar_t *, wchar_t **) libcesque;
double wcstod(const wchar_t *, wchar_t **) libcesque;
long double wcstold(const wchar_t *, wchar_t **) libcesque;

#ifdef _COSMO_SOURCE
char *stripext(char *) libcesque;
char *stripexts(char *) libcesque;
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

libcesque div_t div(int, int) pureconst;
libcesque ldiv_t ldiv(long, long) pureconst;
libcesque lldiv_t lldiv(long long, long long) pureconst;
libcesque imaxdiv_t imaxdiv(intmax_t, intmax_t) pureconst;

#if __STDC_VERSION__ + 0 >= 199901L
#define div(num, den)   ((div_t){(num) / (den), (num) % (den)})
#define ldiv(num, den)  ((ldiv_t){(num) / (den), (num) % (den)})
#define lldiv(num, den) ((lldiv_t){(num) / (den), (num) % (den)})
#endif

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_FMT_CONV_H_ */
