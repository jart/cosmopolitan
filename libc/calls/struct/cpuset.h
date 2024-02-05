#ifndef COSMOPOLITAN_LIBC_CALLS_STRUCT_CPUSET_H_
#define COSMOPOLITAN_LIBC_CALLS_STRUCT_CPUSET_H_

#define CPU_SETSIZE 1024

COSMOPOLITAN_C_START_

typedef struct cpu_set_t {
  uint64_t __bits[16];
} cpu_set_t;

int sched_getcpu(void) libcesque;
int sched_getaffinity(int, size_t, cpu_set_t *) libcesque;
int sched_setaffinity(int, size_t, const cpu_set_t *) libcesque;

#define CPU_SET(i, s)   ((s)->__bits[(i) / 64] |= 1ull << ((i) % 64))
#define CPU_CLR(i, s)   ((s)->__bits[(i) / 64] &= ~(1ull << ((i) % 64)))
#define CPU_ISSET(i, s) (!!((s)->__bits[(i) / 64] & (1ull << ((i) % 64))))

void CPU_ZERO(cpu_set_t *) libcesque;
#define CPU_ZERO(x) CPU_ZERO(x)

int CPU_COUNT(cpu_set_t *) libcesque;
#define CPU_COUNT(x) CPU_COUNT(x)

int CPU_EQUAL(cpu_set_t *, cpu_set_t *) libcesque;
#define CPU_EQUAL(x, y) CPU_EQUAL(x, y)

void CPU_AND(cpu_set_t *, cpu_set_t *, cpu_set_t *) libcesque;
#define CPU_AND(x, y, z) CPU_AND(x, y, z)

void CPU_OR(cpu_set_t *, cpu_set_t *, cpu_set_t *) libcesque;
#define CPU_OR(x, y, z) CPU_OR(x, y, z)

void CPU_XOR(cpu_set_t *, cpu_set_t *, cpu_set_t *) libcesque;
#define CPU_XOR(x, y, z) CPU_XOR(x, y, z)

int CPU_COUNT_S(size_t, const cpu_set_t *) libcesque;
#define CPU_COUNT_S(x, y) CPU_COUNT_S(x, y)

#define CPU_ALLOC_SIZE(n) \
  ((((n) + (8 * sizeof(long) - 1)) & -(8 * sizeof(long))) / sizeof(long))

#define CPU_ALLOC(n)                  ((cpu_set_t *)calloc(1, CPU_ALLOC_SIZE(n)))
#define CPU_FREE(set)                 free(set)
#define CPU_ZERO_S(size, set)         memset(set, 0, size)
#define CPU_EQUAL_S(size, set1, set2) (!memcmp(set1, set2, size))
#define _CPU_S(i, size, set, op)                                              \
  ((i) / 8U >= (size) ? 0                                                     \
                      : (((unsigned long *)(set))[(i) / 8 / sizeof(long)] op( \
                            1UL << ((i) % (8 * sizeof(long))))))
#define CPU_SET_S(i, size, set)   _CPU_S(i, size, set, |=)
#define CPU_CLR_S(i, size, set)   _CPU_S(i, size, set, &= ~)
#define CPU_ISSET_S(i, size, set) _CPU_S(i, size, set, &)

typedef cpu_set_t cpuset_t; /* for freebsd compatibility */

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_CALLS_STRUCT_CPUSET_H_ */
