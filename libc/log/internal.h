#ifndef COSMOPOLITAN_LIBC_LOG_INTERNAL_H_
#define COSMOPOLITAN_LIBC_LOG_INTERNAL_H_
#include "libc/calls/struct/sigaction.h"
#include "libc/calls/struct/siginfo.h"
#include "libc/calls/struct/termios.h"
#include "libc/calls/ucontext.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

extern hidden int kCrashSigs[8];
extern hidden bool g_isrunningundermake;
extern hidden bool g_isterminalinarticulate;
extern hidden struct termios g_oldtermios;
extern hidden struct sigaction g_oldcrashacts[8];

void __start_fatal(const char *, int) hidden;
void __oncrash(int, struct siginfo *, struct ucontext *) relegated;
void __restore_tty(int);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_LOG_INTERNAL_H_ */
