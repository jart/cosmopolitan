#ifndef COSMOPOLITAN_LIBC_CALLS_STRUCT_CPUSET_H_
#define COSMOPOLITAN_LIBC_CALLS_STRUCT_CPUSET_H_

#define CPU_SETSIZE 1024

#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

typedef struct cpu_set_t {
  uint64_t __bits[16];
} cpu_set_t;

int sched_getcpu(void);
int sched_getaffinity(int, size_t, cpu_set_t *);
int sched_setaffinity(int, size_t, const cpu_set_t *);

#define CPU_SET(i, s)   ((s)->__bits[(i) / 64] |= 1ull << ((i) % 64))
#define CPU_CLR(i, s)   ((s)->__bits[(i) / 64] &= ~(1ull << ((i) % 64)))
#define CPU_ISSET(i, s) (!!((s)->__bits[(i) / 64] & (1ull << ((i) % 64))))
void CPU_ZERO(cpu_set_t *);
int CPU_COUNT(cpu_set_t *);
int CPU_EQUAL(cpu_set_t *, cpu_set_t *);
void CPU_AND(cpu_set_t *, cpu_set_t *, cpu_set_t *);
void CPU_OR(cpu_set_t *, cpu_set_t *, cpu_set_t *);
void CPU_XOR(cpu_set_t *, cpu_set_t *, cpu_set_t *);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_CALLS_STRUCT_CPUSET_H_ */
