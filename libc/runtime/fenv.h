#ifndef COSMOPOLITAN_LIBC_RUNTIME_FENV_H_
#define COSMOPOLITAN_LIBC_RUNTIME_FENV_H_

#ifdef __x86_64__
#define FE_INVALID    1
#define __FE_DENORM   2
#define FE_DIVBYZERO  4
#define FE_OVERFLOW   8
#define FE_UNDERFLOW  16
#define FE_INEXACT    32
#define FE_ALL_EXCEPT 63
#define FE_TONEAREST  0x0000
#define FE_DOWNWARD   0x0400
#define FE_UPWARD     0x0800
#define FE_TOWARDZERO 0x0c00
#define FE_DFL_ENV    ((const fenv_t *)-1)
typedef void *fenv_t;
typedef uint16_t fexcept_t;

#elif defined(__aarch64__)
#define FE_INVALID    1
#define FE_DIVBYZERO  2
#define FE_OVERFLOW   4
#define FE_UNDERFLOW  8
#define FE_INEXACT    16
#define FE_ALL_EXCEPT 31
#define FE_TONEAREST  0
#define FE_DOWNWARD   0x800000
#define FE_UPWARD     0x400000
#define FE_TOWARDZERO 0xc00000
#define FE_DFL_ENV    ((const fenv_t *)-1)
typedef void *fenv_t;
typedef uint32_t fexcept_t;

#elif defined(__powerpc64__)
#define FE_TONEAREST                  0
#define FE_TOWARDZERO                 1
#define FE_UPWARD                     2
#define FE_DOWNWARD                   3
#define FE_INEXACT                    0x02000000
#define FE_DIVBYZERO                  0x04000000
#define FE_UNDERFLOW                  0x08000000
#define FE_OVERFLOW                   0x10000000
#define FE_INVALID                    0x20000000
#define FE_ALL_EXCEPT                 0x3e000000
#define FE_INVALID_SNAN               0x01000000
#define FE_INVALID_ISI                0x00800000
#define FE_INVALID_IDI                0x00400000
#define FE_INVALID_ZDZ                0x00200000
#define FE_INVALID_IMZ                0x00100000
#define FE_INVALID_COMPARE            0x00080000
#define FE_INVALID_SOFTWARE           0x00000400
#define FE_INVALID_SQRT               0x00000200
#define FE_INVALID_INTEGER_CONVERSION 0x00000100
#define FE_ALL_INVALID                0x01f80700
#define FE_DFL_ENV                    ((const fenv_t *)-1)
typedef unsigned fexcept_t;
typedef double fenv_t;

#endif /* __x86_64__ */

#ifdef __FLT_EVAL_METHOD__
#define FLT_EVAL_METHOD __FLT_EVAL_METHOD__
#else
#define FLT_EVAL_METHOD 0
#endif

COSMOPOLITAN_C_START_

#define FLT_ROUNDS (__flt_rounds())

int feclearexcept(int);
int fegetenv(fenv_t *);
int fegetexceptflag(fexcept_t *, int);
int fegetround(void);
int feholdexcept(fenv_t *);
int feraiseexcept(int);
int fesetenv(const fenv_t *);
int fesetexceptflag(const fexcept_t *, int);
int fesetround(int);
int fetestexcept(int);
int feupdateenv(const fenv_t *);
int __flt_rounds(void);
int __fesetround(int);

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_RUNTIME_FENV_H_ */
