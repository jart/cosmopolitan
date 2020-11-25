#ifndef COSMOPOLITAN_LIBC_RAND_RAND_H_
#define COSMOPOLITAN_LIBC_RAND_RAND_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_
/*───────────────────────────────────────────────────────────────────────────│─╗
│ cosmopolitan § random                                                    ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/

#define RAND_MAX __INT_MAX__              /* only applies to rand() */
void srand(uint64_t) nothrow nocallback;  /* seeds rand() only      */
int rand(void) nothrow nocallback;        /* ≥0 unseeded lcg prng   */
uint32_t rand32(void) nothrow nocallback; /* random as possible rng */
uint64_t rand64(void) nothrow nocallback; /* random as possible rng */
double poz(double);                       /* verify our claims      */
double pochisq(double, int);
void rt_init(int);
void rt_add(void *, int);
void rt_end(double *, double *, double *, double *, double *);
void *rngset(void *, size_t, uint64_t (*)(void), size_t) paramsnonnull();
char *strfry(char *);
int getentropy(void *, size_t);
ssize_t getrandom(void *, size_t, unsigned);
int devrand(void *, size_t);
int64_t winrandish(void);
uint64_t rdrand(void);
uint64_t rdseed(void);
float randf(void);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_RAND_RAND_H_ */
