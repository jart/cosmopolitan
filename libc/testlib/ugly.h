#ifndef COSMOPOLITAN_LIBC_TESTLIB_UGLY_H_
#define COSMOPOLITAN_LIBC_TESTLIB_UGLY_H_
#include "libc/macros.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)

#if defined(__GNUC__) || defined(__llvm__)
#pragma GCC diagnostic ignored "-Wpointer-to-int-cast" /* j.f.c. */
#endif
#define _I(x) (TYPE_SIGNED(typeof(x)) ? (intmax_t)(x) : (uintmax_t)(x))

#define __TEST_ARRAY(S) \
  _Section(".piro.relo.sort.testcase.2." #S ",\"aw\",@init_array #")

#define __BENCH_ARRAY(S) \
  _Section(".piro.relo.sort.bench.2." #S ",\"aw\",@init_array #")

#define __TEST_PROTOTYPE(S, N, A, K)                                 \
  testonly void S##_##N(void);                                       \
  _Alignas(long) const void *const S##_##N##_ptr[] A(S) = {S##_##N}; \
  testonly K void S##_##N(void)

#define __TEST_SECTION(NAME, CONTENT) \
  ".section " NAME "\n" CONTENT "\n\t.previous\n"

#define __RELOSECTION(NAME, CONTENT) \
  __TEST_SECTION(".piro.relo.sort" NAME ",\"aw\",@progbits", CONTENT)

#define __ROSTR(STR) __TEST_SECTION(".rodata.str1.1,\"aSM\",@progbits,1", STR)

#define __FIXTURE(KIND, GROUP, ENTRY)                       \
  asm(__RELOSECTION("." KIND ".2." #GROUP #ENTRY,           \
                    "\t.quad\t1f\n"                         \
                    "\t.quad\t2f\n"                         \
                    "\t.quad\t" STRINGIFY(GROUP##_##ENTRY)) \
          __ROSTR("1:\t.asciz\t" STRINGIFY(#GROUP))         \
              __ROSTR("2:\t.asciz\t" STRINGIFY(#ENTRY)));   \
  testonly void GROUP##_##ENTRY(void)

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_TESTLIB_UGLY_H_ */
