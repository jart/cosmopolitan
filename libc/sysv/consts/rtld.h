#ifndef COSMOPOLITAN_LIBC_SYSV_CONSTS_RTLD_H_
#define COSMOPOLITAN_LIBC_SYSV_CONSTS_RTLD_H_
#include "libc/runtime/symbolic.h"

#define RTLD_DI_LINKMAP SYMBOLIC(RTLD_DI_LINKMAP)
#define RTLD_GLOBAL SYMBOLIC(RTLD_GLOBAL)
#define RTLD_LAZY SYMBOLIC(RTLD_LAZY)
#define RTLD_LOCAL SYMBOLIC(RTLD_LOCAL)
#define RTLD_NODELETE SYMBOLIC(RTLD_NODELETE)
#define RTLD_NOLOAD SYMBOLIC(RTLD_NOLOAD)
#define RTLD_NOW SYMBOLIC(RTLD_NOW)

#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

hidden extern const long RTLD_DI_LINKMAP;
hidden extern const long RTLD_GLOBAL;
hidden extern const long RTLD_LAZY;
hidden extern const long RTLD_LOCAL;
hidden extern const long RTLD_NODELETE;
hidden extern const long RTLD_NOLOAD;
hidden extern const long RTLD_NOW;

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_SYSV_CONSTS_RTLD_H_ */
