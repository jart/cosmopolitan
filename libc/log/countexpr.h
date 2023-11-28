#ifndef COSMOPOLITAN_LIBC_LOG_COUNTEXPR_H_
#define COSMOPOLITAN_LIBC_LOG_COUNTEXPR_H_
#include "libc/macros.internal.h"
#include "libc/nexgen32e/bench.h"
COSMOPOLITAN_C_START_

/**
 * Shows nanosecond timings histogram for expr at exit().
 */
#define COUNTEXPR(expr)                                                \
  COUNTEXPR_(expr, #expr, STRINGIFY(__FILE__), __LINE__, rdtsc, rdtsc, \
             "COUNTEXPR")

/**
 * Like COUNTEXPR() but can be used on function calls that return void.
 */
#define COUNTSTMT(stmt)                                                    \
  (void)COUNTEXPR_((stmt, 0), #stmt, STRINGIFY(__FILE__), __LINE__, rdtsc, \
                   rdtsc, "COUNTSTMT")

/**
 * Same as COUNTEXPR() but uses Intel's expensive measurement technique.
 */
#define BENCHEXPR(expr)                                                \
  COUNTEXPR_(expr, #expr, STRINGIFY(__FILE__), __LINE__, __startbench, \
             __endbench, "BENCHEXPR")

#define COUNTEXPR_(expr, code, file, line, start, stop, macro) \
  COUNTEXPR__(expr, STRINGIFY(code), file, line, start, stop, STRINGIFY(macro))

#define COUNTEXPR__(expr, code, file, line, start, stop, macro) \
  ({                                                            \
    struct countexpr *InfO;                                     \
    uint64_t t1_, t2_, TiCkS, NaNoS;                            \
    t1_ = start();                                              \
    asm volatile("" ::: "memory");                              \
    autotype(expr) ReS = (expr);                                \
    asm volatile("" ::: "memory");                              \
    t2_ = stop();                                               \
    TiCkS = t2_ >= t1_ ? t2_ - t1_ : ~t1_ + t2_ + 1;            \
    asm(".section .rodata.str1.1,\"aMS\",@progbits,1\n\t"       \
        ".balign\t1\n"                                          \
        "31340:\t.asciz\t" file "\n\t"                          \
        "31338:\t.asciz\t" code "\n"                            \
        "31332:\t.asciz\t" macro "\n"                           \
        ".previous\n\t"                                         \
        ".section .yoink\n\t"                                   \
        "nopl\tcountexpr_data(%%rip)\n\t"                       \
        ".previous\n\t"                                         \
        ".section .sort.data.countexpr.2,\"a\",@progbits\n\t"   \
        ".balign\t8\n31337:\t"                                  \
        ".quad\t" #line "\n\t"                                  \
        ".quad\t31340b\n\t"                                     \
        ".quad\t31338b\n\t"                                     \
        ".quad\t31332b\n\t"                                     \
        ".rept\t65\n\t"                                         \
        ".quad\t0\n\t"                                          \
        ".endr\n\t"                                             \
        ".previous\n\t"                                         \
        "lea\t31337b(%%rip),%0"                                 \
        : "=r"(InfO));                                          \
    /* approximation of round(x*.323018) which is usually */    \
    /* the ratio, between x86 rdtsc ticks and nanoseconds */    \
    NaNoS = (TiCkS * 338709) >> 20;                             \
    ++InfO->logos[NaNoS ? bsrl(NaNoS) + 1 : 0];                 \
    ReS;                                                        \
  })

struct countexpr {
  long line; /* zero for last entry */
  const char *file;
  const char *code;
  const char *macro;
  long logos[65];
};

extern struct countexpr countexpr_data[];

void countexpr_report(void);

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_LOG_COUNTEXPR_H_ */
