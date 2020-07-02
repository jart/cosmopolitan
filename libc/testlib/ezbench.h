#ifndef COSMOPOLITAN_LIBC_TESTLIB_EZBENCH_H_
#define COSMOPOLITAN_LIBC_TESTLIB_EZBENCH_H_
#include "libc/macros.h"
#include "libc/testlib/bench.h"
#include "libc/testlib/testlib.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)

#define EZBENCH(INIT, EXPR) EZBENCH2(#EXPR, INIT, EXPR)
#define EZBENCH2(NAME, INIT, EXPR)                                        \
  do {                                                                    \
    uint64_t Control, Speculative, MemoryStrict;                          \
    Control = __testlib_ezbenchcontrol();                                 \
    INIT;                                                                 \
    EXPR;                                                                 \
    Speculative = BENCHLOOP(__startbench, __endbench, 128, INIT, (EXPR)); \
    MemoryStrict = BENCHLOOP(__startbench_m, __endbench_m, 8, ({          \
                               INIT;                                      \
                               thrashcodecache();                         \
                             }),                                          \
                             (EXPR));                                     \
    Control = MIN(Control, MIN(Speculative, MemoryStrict));               \
    __testlib_ezbenchreport(NAME, Speculative - Control,                  \
                            MemoryStrict - Control);                      \
  } while (0)

void __testlib_ezbenchreport(const char *, uint64_t, uint64_t);
uint64_t __testlib_ezbenchcontrol(void);

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_TESTLIB_EZBENCH_H_ */
