#ifndef COSMOPOLITAN_LIBC_RAND_RAND_H_
#define COSMOPOLITAN_LIBC_RAND_RAND_H_
#define RAND_MAX __INT_MAX__
COSMOPOLITAN_C_START_
/*───────────────────────────────────────────────────────────────────────────│─╗
│ cosmopolitan § random                                                    ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/

int rand(void) libcesque;
void srand(unsigned) libcesque;
char *strfry(char *) libcesque;
int getentropy(void *, size_t) libcesque;
ssize_t getrandom(void *, size_t, unsigned) libcesque;
char *initstate(unsigned, char *, size_t) libcesque;
char *setstate(char *) libcesque;
long random(void) libcesque;
void srandom(unsigned) libcesque;

#ifdef _COSMO_SOURCE
#define vigna   __vigna
#define vigna_r __vigna_r
#define rngset  __rngset
#define rdrand  __rdrand
#define rdseed  __rdseed
double poz(double) libcesque;
double pochisq(double, int) libcesque;
uint64_t lemur64(void) libcesque;
uint64_t _rand64(void) libcesque;
uint64_t vigna(void) libcesque;
uint64_t vigna_r(uint64_t[hasatleast 1]) libcesque;
void svigna(uint64_t) libcesque;
uint64_t rdrand(void) libcesque;
uint64_t rdseed(void) libcesque;
void _smt19937(uint64_t) libcesque;
void _Smt19937(uint64_t[], size_t) libcesque;
uint64_t _mt19937(void) libcesque;
double _real1(uint64_t) libcesque;
double _real2(uint64_t) libcesque;
double _real3(uint64_t) libcesque;
double MeasureEntropy(const char *, size_t) libcesque;
void *rngset(void *, size_t, uint64_t (*)(void), size_t) libcesque;
void rt_init(int) libcesque;
void rt_add(void *, int) libcesque;
void rt_end(double *, double *, double *, double *, double *) libcesque;
#endif /* _COSMO_SOURCE */

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_RAND_RAND_H_ */
