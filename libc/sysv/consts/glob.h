#ifndef COSMOPOLITAN_LIBC_SYSV_CONSTS_GLOB_H_
#define COSMOPOLITAN_LIBC_SYSV_CONSTS_GLOB_H_
#include "libc/runtime/symbolic.h"

#define GLOB_ABORTED SYMBOLIC(GLOB_ABORTED)
#define GLOB_APPEND SYMBOLIC(GLOB_APPEND)
#define GLOB_DOOFFS SYMBOLIC(GLOB_DOOFFS)
#define GLOB_ERR SYMBOLIC(GLOB_ERR)
#define GLOB_MARK SYMBOLIC(GLOB_MARK)
#define GLOB_NOCHECK SYMBOLIC(GLOB_NOCHECK)
#define GLOB_NOESCAPE SYMBOLIC(GLOB_NOESCAPE)
#define GLOB_NOMATCH SYMBOLIC(GLOB_NOMATCH)
#define GLOB_NOSORT SYMBOLIC(GLOB_NOSORT)
#define GLOB_NOSPACE SYMBOLIC(GLOB_NOSPACE)
#define GLOB_NOSYS SYMBOLIC(GLOB_NOSYS)

#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

hidden extern const long GLOB_ABORTED;
hidden extern const long GLOB_APPEND;
hidden extern const long GLOB_DOOFFS;
hidden extern const long GLOB_ERR;
hidden extern const long GLOB_MARK;
hidden extern const long GLOB_NOCHECK;
hidden extern const long GLOB_NOESCAPE;
hidden extern const long GLOB_NOMATCH;
hidden extern const long GLOB_NOSORT;
hidden extern const long GLOB_NOSPACE;
hidden extern const long GLOB_NOSYS;

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_SYSV_CONSTS_GLOB_H_ */
