#ifndef COSMOPOLITAN_LIBC_SYSV_CONSTS_PROT_H_
#define COSMOPOLITAN_LIBC_SYSV_CONSTS_PROT_H_
#include "libc/runtime/symbolic.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

extern const long PROT_NONE;
extern const long PROT_READ;
extern const long PROT_WRITE;
extern const long PROT_EXEC;
extern const long PROT_GROWSDOWN;
extern const long PROT_GROWSUP;

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */

#define PROT_NONE 0
#define PROT_READ 1
#define PROT_WRITE 2
#define PROT_EXEC 4
#define PROT_GROWSDOWN SYMBOLIC(PROT_GROWSDOWN)
#define PROT_GROWSUP SYMBOLIC(PROT_GROWSUP)

#endif /* COSMOPOLITAN_LIBC_SYSV_CONSTS_PROT_H_ */
