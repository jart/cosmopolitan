#ifndef COSMOPOLITAN_LIBC_NEXGEN32E_BENCH_H_
#define COSMOPOLITAN_LIBC_NEXGEN32E_BENCH_H_
#include "libc/nexgen32e/rdtsc.h"

/**
 * @fileoverview NexGen32e Microbenchmarking.
 *
 * @see X86_HAVE(INVTSC)
 * @see libc/testlib/bench.h
 */

#ifdef __x86__
#define __startbench()                            \
  ({                                              \
    uint64_t Ticks;                               \
    asm volatile("lfence\n\t"                     \
                 "push\t%%rbx\n\t"                \
                 "cpuid\n\t"                      \
                 "pop\t%%rbx\n\t"                 \
                 "rdtsc\n\t"                      \
                 "shl\t%2,%%rdx\n\t"              \
                 "or\t%%rdx,%0"                   \
                 : "=a"(Ticks)                    \
                 : "0"(0), "J"(32)                \
                 : "rcx", "rdx", "memory", "cc"); \
    Ticks;                                        \
  })
#define __endbench()                                     \
  ({                                                     \
    uint64_t Ticks;                                      \
    asm volatile("rdtscp\n\t"                            \
                 "shl\t%1,%%rdx\n\t"                     \
                 "or\t%%rdx,%%rax\n\t"                   \
                 "mov\t%%rax,%0\n\t"                     \
                 "xor\t%%eax,%%eax\n\t"                  \
                 "push\t%%rbx\n\t"                       \
                 "cpuid\n\t"                             \
                 "pop\t%%rbx"                            \
                 : "=r"(Ticks)                           \
                 : "J"(32)                               \
                 : "rax", "rcx", "rdx", "memory", "cc"); \
    Ticks;                                               \
  })
#else
#define __startbench()                \
  ({                                  \
    uint64_t _ts;                     \
    asm volatile("isb" ::: "memory"); \
    _ts = rdtsc();                    \
    asm volatile("isb" ::: "memory"); \
    _ts;                              \
  })
#define __endbench()                  \
  ({                                  \
    uint64_t _ts;                     \
    asm volatile("isb" ::: "memory"); \
    _ts = rdtsc();                    \
    asm volatile("isb" ::: "memory"); \
    _ts;                              \
  })
#endif

#define __startbench_m() mfence_lfence_rdtsc_lfence()
#define __endbench_m()   __startbench_m()
#define __marker()       asm("nop")
#define __ordered()      asm volatile("" ::: "memory")
#define __fakeread(X)    asm volatile("" : /* no outputs */ : "g"(X))
#define __fakereadwrite(X)                 \
  ({                                       \
    autotype(X) Res = (X);                 \
    asm volatile("" : "=g"(Res) : "0"(X)); \
    Res;                                   \
  })

#endif /* COSMOPOLITAN_LIBC_NEXGEN32E_BENCH_H_ */
