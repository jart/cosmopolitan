#ifndef COSMOPOLITAN_LIBC_SYSV_CONSTS_PROT_H_
#define COSMOPOLITAN_LIBC_SYSV_CONSTS_PROT_H_
#include "libc/runtime/symbolic.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

hidden extern const long PROT_NONE;
hidden extern const long PROT_READ;
hidden extern const long PROT_WRITE;
hidden extern const long PROT_EXEC;
hidden extern const long PROT_GROWSDOWN;
hidden extern const long PROT_GROWSUP;

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */

#define PROT_NONE 0
#define PROT_READ 1
#define PROT_WRITE 2
#define PROT_EXEC 4
#define PROT_GROWSDOWN SYMBOLIC(PROT_GROWSDOWN)
#define PROT_GROWSUP SYMBOLIC(PROT_GROWSUP)

#endif /* COSMOPOLITAN_LIBC_SYSV_CONSTS_PROT_H_ */
