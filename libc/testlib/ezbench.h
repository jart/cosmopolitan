#ifndef COSMOPOLITAN_LIBC_TESTLIB_EZBENCH_H_
#define COSMOPOLITAN_LIBC_TESTLIB_EZBENCH_H_
#include "libc/nexgen32e/bench.h"
#include "libc/nexgen32e/x86feature.h"
#include "libc/testlib/bench.h"
#include "libc/testlib/testlib.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

#ifndef EZBENCH_COUNT
#define EZBENCH_COUNT 128
#endif

#define EZBENCH_TRIES 10

#define EZBENCH(INIT, EXPR) EZBENCH2(#EXPR, INIT, EXPR)

#define EZBENCH2(NAME, INIT, EXPR)                                        \
  do {                                                                    \
    int Core, Tries, Interrupts;                                          \
    int64_t Speculative, MemoryStrict;                                    \
    Tries = 0;                                                            \
    do {                                                                  \
      __testlib_yield();                                                  \
      Core = __testlib_getcore();                                         \
      Interrupts = __testlib_getinterrupts();                             \
      INIT;                                                               \
      EXPR;                                                               \
      Speculative = BENCHLOOP(__startbench, __endbench, EZBENCH_COUNT, ({ \
                                INIT;                                     \
                                __polluteregisters();                     \
                              }),                                         \
                              (EXPR));                                    \
    } while (++Tries < EZBENCH_TRIES &&                                   \
             (__testlib_getcore() != Core &&                              \
              __testlib_getinterrupts() > Interrupts));                   \
    if (Tries == EZBENCH_TRIES) __testlib_ezbenchwarn(" speculative");    \
    Tries = 0;                                                            \
    do {                                                                  \
      __testlib_yield();                                                  \
      Core = __testlib_getcore();                                         \
      Interrupts = __testlib_getinterrupts();                             \
      INIT;                                                               \
      EXPR;                                                               \
      MemoryStrict = BENCHLOOP(__startbench_m, __endbench_m, 32, ({       \
                                 INIT;                                    \
                                 thrashcodecache();                       \
                                 __polluteregisters();                    \
                               }),                                        \
                               (EXPR));                                   \
    } while (++Tries < EZBENCH_TRIES &&                                   \
             (__testlib_getcore() != Core &&                              \
              __testlib_getinterrupts() > Interrupts));                   \
    if (Tries == EZBENCH_TRIES) __testlib_ezbenchwarn(" memory strict");  \
    __testlib_ezbenchreport(                                              \
        NAME, MAX(0, Speculative - __testlib_ezbenchcontrol()),           \
        MAX(0, MemoryStrict - __testlib_ezbenchcontrol()));               \
  } while (0)

#define EZBENCH3(NAME, NUM, INIT, EXPR)                                  \
  do {                                                                   \
    int Core, Tries, Interrupts;                                         \
    int64_t Speculative, MemoryStrict;                                   \
    Tries = 0;                                                           \
    do {                                                                 \
      __testlib_yield();                                                 \
      Core = __testlib_getcore();                                        \
      Interrupts = __testlib_getinterrupts();                            \
      INIT;                                                              \
      EXPR;                                                              \
      Speculative = BENCHLOOP(__startbench, __endbench, NUM, ({          \
                                INIT;                                    \
                                __polluteregisters();                    \
                              }),                                        \
                              (EXPR));                                   \
    } while (++Tries < EZBENCH_TRIES &&                                  \
             (__testlib_getcore() != Core &&                             \
              __testlib_getinterrupts() > Interrupts));                  \
    if (Tries == EZBENCH_TRIES) __testlib_ezbenchwarn(" speculative");   \
    Tries = 0;                                                           \
    do {                                                                 \
      __testlib_yield();                                                 \
      Core = __testlib_getcore();                                        \
      Interrupts = __testlib_getinterrupts();                            \
      INIT;                                                              \
      EXPR;                                                              \
      MemoryStrict = BENCHLOOP(__startbench_m, __endbench_m, NUM, ({     \
                                 INIT;                                   \
                                 thrashcodecache();                      \
                                 __polluteregisters();                   \
                               }),                                       \
                               (EXPR));                                  \
    } while (++Tries < EZBENCH_TRIES &&                                  \
             (__testlib_getcore() != Core &&                             \
              __testlib_getinterrupts() > Interrupts));                  \
    if (Tries == EZBENCH_TRIES) __testlib_ezbenchwarn(" memory strict"); \
    __testlib_ezbenchreport(                                             \
        NAME, MAX(0, Speculative - __testlib_ezbenchcontrol()),          \
        MAX(0, MemoryStrict - __testlib_ezbenchcontrol()));              \
  } while (0)

#define EZBENCH_C(NAME, CONTROL, EXPR)                                    \
  do {                                                                    \
    int Core, Tries, Interrupts;                                          \
    int64_t Control, Speculative, MemoryStrict;                           \
    Tries = 0;                                                            \
    do {                                                                  \
      __testlib_yield();                                                  \
      Core = __testlib_getcore();                                         \
      Interrupts = __testlib_getinterrupts();                             \
      Control = BENCHLOOP(__startbench_m, __endbench_m, EZBENCH_COUNT, ({ \
                            thrashcodecache();                            \
                            __polluteregisters();                         \
                          }),                                             \
                          (CONTROL));                                     \
    } while (++Tries < EZBENCH_TRIES &&                                   \
             (__testlib_getcore() != Core &&                              \
              __testlib_getinterrupts() > Interrupts));                   \
    if (Tries == EZBENCH_TRIES) __testlib_ezbenchwarn(" control");        \
    Tries = 0;                                                            \
    do {                                                                  \
      __testlib_yield();                                                  \
      Core = __testlib_getcore();                                         \
      Interrupts = __testlib_getinterrupts();                             \
      EXPR;                                                               \
      Speculative = BENCHLOOP(__startbench, __endbench, EZBENCH_COUNT,    \
                              __polluteregisters(), (EXPR));              \
    } while (++Tries < EZBENCH_TRIES &&                                   \
             (__testlib_getcore() != Core &&                              \
              __testlib_getinterrupts() > Interrupts));                   \
    if (Tries == EZBENCH_TRIES) __testlib_ezbenchwarn(" speculative");    \
    Tries = 0;                                                            \
    do {                                                                  \
      __testlib_yield();                                                  \
      Core = __testlib_getcore();                                         \
      Interrupts = __testlib_getinterrupts();                             \
      EXPR;                                                               \
      MemoryStrict = BENCHLOOP(__startbench_m, __endbench_m, 8, ({        \
                                 thrashcodecache();                       \
                                 __polluteregisters();                    \
                               }),                                        \
                               (EXPR));                                   \
    } while (++Tries < EZBENCH_TRIES &&                                   \
             (__testlib_getcore() != Core &&                              \
              __testlib_getinterrupts() > Interrupts));                   \
    if (Tries == EZBENCH_TRIES) __testlib_ezbenchwarn(" memory strict");  \
    __testlib_ezbenchreport(NAME, MAX(0, Speculative - Control),          \
                            MAX(0, MemoryStrict - Control));              \
  } while (0)

#define EZBENCH_N(NAME, N, EXPR)                                         \
  do {                                                                   \
    int64_t Speculative, Toto;                                           \
    int Core, Tries, Interrupts;                                         \
    Tries = 0;                                                           \
    do {                                                                 \
      __testlib_yield();                                                 \
      Core = __testlib_getcore();                                        \
      Interrupts = __testlib_getinterrupts();                            \
      EXPR;                                                              \
      Speculative = BENCHLOOP(__startbench, __endbench, 32,              \
                              __polluteregisters(), (EXPR));             \
    } while (++Tries < EZBENCH_TRIES &&                                  \
             (__testlib_getcore() != Core &&                             \
              __testlib_getinterrupts() > Interrupts));                  \
    if (Tries == EZBENCH_TRIES) __testlib_ezbenchwarn("");               \
    __testlib_ezbenchreport_n(                                           \
        NAME, 'n', N, MAX(0, Speculative - __testlib_ezbenchcontrol())); \
  } while (0)

#define EZBENCH_K(NAME, K, EXPR)                                         \
  do {                                                                   \
    int Core;                                                            \
    int64_t Speculative;                                                 \
    do {                                                                 \
      __testlib_yield();                                                 \
      Core = __testlib_getcore();                                        \
      EXPR;                                                              \
      Speculative = BENCHLOOP(__startbench, __endbench, EZBENCH_COUNT,   \
                              donothing, (EXPR));                        \
    } while (Core != __testlib_getcore());                               \
    __testlib_ezbenchreport_n(                                           \
        NAME, 'k', K, MAX(0, Speculative - __testlib_ezbenchcontrol())); \
  } while (0)

void __polluteregisters(void);
void __testlib_yield(void);
int __testlib_getcore(void);
int64_t __testlib_getinterrupts(void);
int64_t __testlib_ezbenchcontrol(void);
void __testlib_ezbenchwarn(const char *);
void __testlib_ezbenchreport(const char *, uint64_t, uint64_t);
void __testlib_ezbenchreport_n(const char *, char, size_t, uint64_t);

#ifdef __STRICT_ANSI__
#undef EZBENCH2
#undef EZBENCH3
#undef EZBENCH_N
#undef EZBENCH_K
#define EZBENCH2(NAME, INIT, EXPR)      (void)0
#define EZBENCH3(NAME, NUM, INIT, EXPR) (void)0
#define EZBENCH_N(NAME, N, EXPR)        (void)0
#define EZBENCH_K(NAME, K, EXPR)        (void)0
#endif

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_TESTLIB_EZBENCH_H_ */
