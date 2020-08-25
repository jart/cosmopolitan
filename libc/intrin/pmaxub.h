#ifndef COSMOPOLITAN_LIBC_INTRIN_PMAXUB_H_
#define COSMOPOLITAN_LIBC_INTRIN_PMAXUB_H_
#include "libc/intrin/macros.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

void pmaxub(unsigned char[16], const unsigned char[16],
            const unsigned char[16]);

#define pmaxub(A, B, C) \
  INTRIN_SSEVEX_X_X_X_(pmaxub, SSE2, "pmaxub", INTRIN_COMMUTATIVE, A, B, C)

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_INTRIN_PMAXUB_H_ */
