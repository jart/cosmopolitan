#ifndef COSMOPOLITAN_LIBC_RUNTIME_FENV_H_
#define COSMOPOLITAN_LIBC_RUNTIME_FENV_H_

#define FE_TONEAREST  0x0000
#define FE_DOWNWARD   0x0400
#define FE_UPWARD     0x0800
#define FE_TOWARDZERO 0x0c00

#define FE_INVALID    1
#define __FE_DENORM   2
#define FE_DIVBYZERO  4
#define FE_OVERFLOW   8
#define FE_UNDERFLOW  16
#define FE_INEXACT    32
#define FE_ALL_EXCEPT 63

#ifdef __FLT_EVAL_METHOD__
#define FLT_EVAL_METHOD __FLT_EVAL_METHOD__
#else
#define FLT_EVAL_METHOD 0
#endif

#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

#define FLT_ROUNDS (__flt_rounds())
#define FE_DFL_ENV ((const fenv_t *)-1)

typedef void *fenv_t;
typedef uint16_t fexcept_t;

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

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_RUNTIME_FENV_H_ */
