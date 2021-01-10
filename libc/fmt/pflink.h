#ifndef COSMOPOLITAN_LIBC_FMT_PFLINK_H_
#define COSMOPOLITAN_LIBC_FMT_PFLINK_H_
#include "libc/dce.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
#ifndef __STRICT_ANSI__

/**
 * @fileoverview builtin+preprocessor+linker tricks for printf/scanf.
 *
 * Your printf() function only requires that you pay for what you use.
 * These macros ensure that its code size starts at under 4kb, growing
 * to about 40kb for a fully-loaded implementation. This works best when
 * format strings are constexprs that only contain directives.
 */

#define PFLINK(FMT)                                                \
  ({                                                               \
    if (___PFLINK(FMT, strpbrk, "bxdinupo")) STATIC_YOINK("ntoa"); \
    if (___PFLINK(FMT, strpbrk, "fFgGaA")) STATIC_YOINK("ftoa");   \
    if (___PFLINK(FMT, strpbrk, "cmrqs")) {                        \
      STATIC_YOINK("stoa");                                        \
      if (___PFLINK(FMT, strchr, '#')) STATIC_YOINK("kCp437");     \
      if (___PFLINK(FMT, strstr, "%m")) STATIC_YOINK("strerror");  \
      if (!IsTiny() && (___PFLINK(FMT, strstr, "%*") ||            \
                        ___PFLINK(FMT, strpbrk, "0123456789"))) {  \
        STATIC_YOINK("strnwidth");                                 \
        STATIC_YOINK("strnwidth16");                               \
        STATIC_YOINK("wcsnwidth");                                 \
      }                                                            \
    }                                                              \
    FMT;                                                           \
  })

#define SFLINK(FMT)                    \
  ({                                   \
    if (___PFLINK(FMT, strchr, 'm')) { \
      STATIC_YOINK("malloc");          \
      STATIC_YOINK("calloc");          \
      STATIC_YOINK("free_s");          \
      STATIC_YOINK("__grow");          \
    }                                  \
    FMT;                               \
  })

#if __GNUC__ + 0 < 4 || defined(__llvm__)
#define ___PFLINK(FMT, FN, C) 1
#else
#define ___PFLINK(FMT, FN, C) \
  !__builtin_constant_p(FMT) || ((FMT) && __builtin_##FN(FMT, C) != NULL)
#endif

#if defined(__GNUC__) && __GNUC__ < 6
/*
 * Compilers don't understand the features we've added to the format
 * string DSL, such as c string escaping, therefore we can't use it.
 * Ideally compilers should grant us more flexibility to define DSLs
 *
 * The recommended approach to turning this back on is `CFLAGS=-std=c11`
 * which puts the compiler in __STRICT_ANSI__ mode, which Cosmopolitan
 * respects by disabling all the esoteric tuning in headers like this.
 */
#pragma GCC diagnostic ignored "-Wformat-security"
#endif /* __GNUC__ + 0 < 6 */

#else
#define PFLINK(FMT) FMT
#define SFLINK(FMT) FMT
asm(".section .yoink\n\t"
    "nop\tntoa(%rip)\n\t"
    "nop\tftoa(%rip)\n\t"
    "nop\tkCp437(%rip)\n\t"
    "nop\tstrerror(%rip)\n\t"
    "nop\tstrnwidth(%rip)\n\t"
    "nop\tstrnwidth16(%rip)\n\t"
    "nop\twcsnwidth(%rip)\n\t"
    "nop\tmalloc(%rip)\n\t"
    "nop\tcalloc(%rip)\n\t"
    "nop\tfree_s(%rip)\n\t"
    "nop\t__grow(%rip)\n\t"
    ".previous");
#endif /* __STRICT_ANSI__ */
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_FMT_PFLINK_H_ */
