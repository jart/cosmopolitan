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

extern const long FTW_CHDIR;
extern const long FTW_D;
extern const long FTW_DEPTH;
extern const long FTW_DNR;
extern const long FTW_DP;
extern const long FTW_F;
extern const long FTW_MOUNT;
extern const long FTW_NS;
extern const long FTW_PHYS;
extern const long FTW_SL;
extern const long FTW_SLN;

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_SYSV_CONSTS_FTW_H_ */
