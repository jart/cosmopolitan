/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:4;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=4 sts=4 sw=4 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Python 3                                                                     │
│ https://docs.python.org/3/license.html                                       │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "third_party/python/pyconfig.h"

#ifdef X87_DOUBLE_ROUNDING
/* On x86 platforms using an x87 FPU, this function is called from the
   Py_FORCE_DOUBLE macro (defined in pymath.h) to force a floating-point
   number out of an 80-bit x87 FPU register and into a 64-bit memory location,
   thus rounding from extended precision to double precision. */
double _Py_force_double(double x)
{
    volatile double y;
    y = x;
    return y;
}
#endif

#ifdef HAVE_GCC_ASM_FOR_X87
/* inline assembly for getting and setting the 387 FPU control word on
   gcc/x86 */
#ifdef _Py_MEMORY_SANITIZER
__attribute__((no_sanitize_memory))
#endif
unsigned short _Py_get_387controlword(void) {
    unsigned short cw;
    __asm__ __volatile__ ("fnstcw %0" : "=m" (cw));
    return cw;
}
void _Py_set_387controlword(unsigned short cw) {
    __asm__ __volatile__ ("fldcw %0" : : "m" (cw));
}
#endif
