#ifndef COSMOPOLITAN_LIBC_LOG_COUNTBRANCH_H_
#define COSMOPOLITAN_LIBC_LOG_COUNTBRANCH_H_
#include "libc/macros.internal.h"
#include "libc/stdbool.h"
COSMOPOLITAN_C_START_

#define COUNTBRANCH(x) COUNTBRANCH_(x, #x, STRINGIFY(__FILE__), __LINE__)
#define COUNTBRANCH_(x, xs, file, line) \
  COUNTBRANCH__(x, STRINGIFY(xs), STRINGIFY(xs), file, line)
#define COUNTBRANCH__(x, xs, xss, file, line)                   \
  ({                                                            \
    bool Cond;                                                  \
    struct countbranch *Info;                                   \
    asm(".section .rodata.str1.1,\"aMS\",@progbits,1\n\t"       \
        ".balign\t1\n"                                          \
        "31338:\t"                                              \
        ".asciz\t" xs "\n"                                      \
        "31339:\t"                                              \
        ".asciz\t" xss "\n"                                     \
        "31340:\t"                                              \
        ".asciz\t" file "\n\t"                                  \
        ".previous\n\t"                                         \
        ".section .yoink\n\t"                                   \
        "nopl\tcountbranch_data(%%rip)\n\t"                     \
        ".previous\n\t"                                         \
        ".section .sort.data.countbranch.2,\"a\",@progbits\n\t" \
        ".balign\t8\n31337:\t"                                  \
        ".quad\t0\n\t"                                          \
        ".quad\t0\n\t"                                          \
        ".quad\t31338b\n\t"                                     \
        ".quad\t31339b\n\t"                                     \
        ".quad\t31340b\n\t"                                     \
        ".quad\t" #line "\n\t"                                  \
        ".previous\n\t"                                         \
        "lea\t31337b(%%rip),%0"                                 \
        : "=r"(Info));                                          \
    Cond = (x);                                                 \
    ++Info->total;                                              \
    if (Cond) ++Info->taken;                                    \
    Cond;                                                       \
  })

struct countbranch {
  long total;
  long taken;
  const char *code;
  const char *xcode;
  const char *file;
  long line;
};

extern struct countbranch countbranch_data[];

void countbranch_report(void);

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_LOG_COUNTBRANCH_H_ */
