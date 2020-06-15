#ifndef COSMOPOLITAN_LIBC_SYSV_CONSTS_AI_H_
#define COSMOPOLITAN_LIBC_SYSV_CONSTS_AI_H_
#include "libc/runtime/symbolic.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

hidden extern const long AI_ADDRCONFIG;
hidden extern const long AI_ALL;
hidden extern const long AI_CANONNAME;
hidden extern const long AI_NUMERICHOST;
hidden extern const long AI_NUMERICSERV;
hidden extern const long AI_PASSIVE;
hidden extern const long AI_V4MAPPED;

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */

#define AI_ADDRCONFIG SYMBOLIC(AI_ADDRCONFIG)
#define AI_ALL SYMBOLIC(AI_ALL)
#define AI_CANONNAME LITERALLY(2)
#define AI_NUMERICHOST LITERALLY(4)
#define AI_NUMERICSERV SYMBOLIC(AI_NUMERICSERV)
#define AI_PASSIVE LITERALLY(1)
#define AI_V4MAPPED SYMBOLIC(AI_V4MAPPED)

#endif /* COSMOPOLITAN_LIBC_SYSV_CONSTS_AI_H_ */
