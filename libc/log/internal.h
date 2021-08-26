#ifndef COSMOPOLITAN_LIBC_LOG_INTERNAL_H_
#define COSMOPOLITAN_LIBC_LOG_INTERNAL_H_
#include "libc/calls/struct/sigaction.h"
#include "libc/calls/struct/siginfo.h"
#include "libc/calls/struct/termios.h"
#include "libc/calls/ucontext.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

extern int kCrashSigs[8] hidden;
extern struct termios g_oldtermios hidden;
extern struct sigaction g_oldcrashacts[8] hidden;

void __start_fatal(const char *, int) hidden;
void __start_fatal_ndebug(void) hidden;
void __oncrash(int, struct siginfo *, struct ucontext *) relegated;
void __restore_tty(int);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_LOG_INTERNAL_H_ */
