#ifndef COSMOPOLITAN_LIBC_SYSV_CONSTS_PT_H_
#define COSMOPOLITAN_LIBC_SYSV_CONSTS_PT_H_
#include "libc/runtime/symbolic.h"

#define PT_ATTACH SYMBOLIC(PT_ATTACH)
#define PT_CONTINUE SYMBOLIC(PT_CONTINUE)
#define PT_DETACH SYMBOLIC(PT_DETACH)
#define PT_GETEVENTMSG SYMBOLIC(PT_GETEVENTMSG)
#define PT_GETFPREGS SYMBOLIC(PT_GETFPREGS)
#define PT_GETFPXREGS SYMBOLIC(PT_GETFPXREGS)
#define PT_GETREGS SYMBOLIC(PT_GETREGS)
#define PT_GETSIGINFO SYMBOLIC(PT_GETSIGINFO)
#define PT_KILL SYMBOLIC(PT_KILL)
#define PT_READ_D SYMBOLIC(PT_READ_D)
#define PT_READ_I SYMBOLIC(PT_READ_I)
#define PT_READ_U SYMBOLIC(PT_READ_U)
#define PT_SETFPREGS SYMBOLIC(PT_SETFPREGS)
#define PT_SETFPXREGS SYMBOLIC(PT_SETFPXREGS)
#define PT_SETOPTIONS SYMBOLIC(PT_SETOPTIONS)
#define PT_SETREGS SYMBOLIC(PT_SETREGS)
#define PT_SETSIGINFO SYMBOLIC(PT_SETSIGINFO)
#define PT_STEP SYMBOLIC(PT_STEP)
#define PT_SYSCALL SYMBOLIC(PT_SYSCALL)
#define PT_TRACE_ME SYMBOLIC(PT_TRACE_ME)
#define PT_WRITE_D SYMBOLIC(PT_WRITE_D)
#define PT_WRITE_I SYMBOLIC(PT_WRITE_I)
#define PT_WRITE_U SYMBOLIC(PT_WRITE_U)

#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

hidden extern const long PT_ATTACH;
hidden extern const long PT_CONTINUE;
hidden extern const long PT_DETACH;
hidden extern const long PT_GETEVENTMSG;
hidden extern const long PT_GETFPREGS;
hidden extern const long PT_GETFPXREGS;
hidden extern const long PT_GETREGS;
hidden extern const long PT_GETSIGINFO;
hidden extern const long PT_KILL;
hidden extern const long PT_READ_D;
hidden extern const long PT_READ_I;
hidden extern const long PT_READ_U;
hidden extern const long PT_SETFPREGS;
hidden extern const long PT_SETFPXREGS;
hidden extern const long PT_SETOPTIONS;
hidden extern const long PT_SETREGS;
hidden extern const long PT_SETSIGINFO;
hidden extern const long PT_STEP;
hidden extern const long PT_SYSCALL;
hidden extern const long PT_TRACE_ME;
hidden extern const long PT_WRITE_D;
hidden extern const long PT_WRITE_I;
hidden extern const long PT_WRITE_U;

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_SYSV_CONSTS_PT_H_ */
