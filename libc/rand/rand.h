#ifndef COSMOPOLITAN_LIBC_RAND_RAND_H_
#define COSMOPOLITAN_LIBC_RAND_RAND_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_
/*───────────────────────────────────────────────────────────────────────────│─╗
│ cosmopolitan § random                                                    ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/

#define RAND_MAX __INT_MAX__
void srand(uint64_t) nothrow nocallback;
int rand(void) nothrow nocallback;
uint64_t rand64(void) nothrow nocallback;
double poz(double);
double pochisq(double, int);
void rt_init(int);
void rt_add(void *, int);
void rt_end(double *, double *, double *, double *, double *);
void *rngset(void *, size_t, uint64_t (*)(void), size_t) paramsnonnull();
char *strfry(char *);
int getentropy(void *, size_t);
ssize_t getrandom(void *, size_t, unsigned);
uint64_t rdrand(void);
uint64_t rdseed(void);
float randf(void);
char *initstate(unsigned, char *, size_t);
char *setstate(char *);
long random(void);
void srandom(unsigned);
double MeasureEntropy(const char *, size_t);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_RAND_RAND_H_ */
