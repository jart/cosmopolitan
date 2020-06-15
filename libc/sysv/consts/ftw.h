#ifndef COSMOPOLITAN_LIBC_SYSV_CONSTS_FTW_H_
#define COSMOPOLITAN_LIBC_SYSV_CONSTS_FTW_H_
#include "libc/runtime/symbolic.h"

#define FTW_CHDIR SYMBOLIC(FTW_CHDIR)
#define FTW_D SYMBOLIC(FTW_D)
#define FTW_DEPTH SYMBOLIC(FTW_DEPTH)
#define FTW_DNR SYMBOLIC(FTW_DNR)
#define FTW_DP SYMBOLIC(FTW_DP)
#define FTW_F SYMBOLIC(FTW_F)
#define FTW_MOUNT SYMBOLIC(FTW_MOUNT)
#define FTW_NS SYMBOLIC(FTW_NS)
#define FTW_PHYS SYMBOLIC(FTW_PHYS)
#define FTW_SL SYMBOLIC(FTW_SL)
#define FTW_SLN SYMBOLIC(FTW_SLN)

#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

hidden extern const long FTW_CHDIR;
hidden extern const long FTW_D;
hidden extern const long FTW_DEPTH;
hidden extern const long FTW_DNR;
hidden extern const long FTW_DP;
hidden extern const long FTW_F;
hidden extern const long FTW_MOUNT;
hidden extern const long FTW_NS;
hidden extern const long FTW_PHYS;
hidden extern const long FTW_SL;
hidden extern const long FTW_SLN;

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_SYSV_CONSTS_FTW_H_ */
