#ifndef COSMOPOLITAN_LIBC_BITS_MORTON_H_
#define COSMOPOLITAN_LIBC_BITS_MORTON_H_
#include "libc/intrin/pdep.h"
#include "libc/intrin/pext.h"
#include "libc/nexgen32e/x86feature.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

unsigned long morton(unsigned long, unsigned long) libcesque pureconst;
axdx_t unmorton(unsigned long) libcesque pureconst;

#ifndef __STRICT_ANSI__
#define morton(Y, X)                                                   \
  (X86_NEED(BMI2)                                                      \
       ? pdep(X, 0x5555555555555555ul) | pdep(Y, 0xAAAAAAAAAAAAAAAAul) \
       : morton(Y, X))
#define unmorton(I)                                         \
  (X86_NEED(BMI2) ? (axdx_t){pext(I, 0xAAAAAAAAAAAAAAAAul), \
                             pext(I, 0x5555555555555555ul)} \
                  : unmorton(I))
#endif

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_BITS_MORTON_H_ */
