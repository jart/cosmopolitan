#ifndef COSMOPOLITAN_LIBC_BITS_MORTON_H_
#define COSMOPOLITAN_LIBC_BITS_MORTON_H_
#include "libc/intrin/pdep.h"
#include "libc/intrin/pext.h"
#include "libc/nexgen32e/x86feature.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

unsigned long morton(unsigned long, unsigned long) libcesque;
axdx_t unmorton(unsigned long) libcesque;

#ifndef __STRICT_ANSI__
#define morton(Y, X)                                                          \
  (X86_NEED(BMI2) ? pdep(X, 0x5555555555555555) | pdep(Y, 0xAAAAAAAAAAAAAAAA) \
                  : morton(Y, X))
#define unmorton(I)                                                         \
  (X86_NEED(BMI2)                                                           \
       ? (axdx_t){pext(I, 0xAAAAAAAAAAAAAAAA), pext(I, 0x5555555555555555)} \
       : unmorton(I))
#endif

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_BITS_MORTON_H_ */
