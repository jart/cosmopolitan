#ifndef COSMOPOLITAN_LIBC_RAND_RAND_H_
#define COSMOPOLITAN_LIBC_RAND_RAND_H_
#define RAND_MAX __INT_MAX__
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_
/*───────────────────────────────────────────────────────────────────────────│─╗
│ cosmopolitan § random                                                    ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/

int rand(void);
void srand(unsigned);
double poz(double);
double pochisq(double, int);
void rt_init(int);
void rt_add(void *, int);
void rt_end(double *, double *, double *, double *, double *);
char *strfry(char *);
int getentropy(void *, size_t);
ssize_t getrandom(void *, size_t, unsigned);
char *initstate(unsigned, char *, size_t);
char *setstate(char *);
long random(void);
void srandom(unsigned);

uint64_t lemur64(void);
uint64_t _rand64(void);
uint64_t vigna(void);
uint64_t vigna_r(uint64_t[hasatleast 1]);
void svigna(uint64_t);
uint64_t rdrand(void);
uint64_t rdseed(void);
void _smt19937(uint64_t);
void _Smt19937(uint64_t[], size_t);
uint64_t _mt19937(void);
double _real1(uint64_t);
double _real2(uint64_t);
double _real3(uint64_t);
double MeasureEntropy(const char *, size_t);
void *rngset(void *, size_t, uint64_t (*)(void), size_t);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_RAND_RAND_H_ */
