#ifndef COSMOPOLITAN_LIBC_LOG_INTERNAL_H_
#define COSMOPOLITAN_LIBC_LOG_INTERNAL_H_
#include "libc/calls/struct/siginfo.h"
#include "libc/calls/ucontext.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

extern _Hide bool __nocolor;
extern _Hide int kCrashSigs[8];
extern _Hide bool _wantcrashreports;
extern _Hide bool g_isrunningundermake;

void __start_fatal(const char *, int) _Hide;
void __oncrash(int, struct siginfo *, struct ucontext *) relegated;
void __restore_tty(void);
void RestoreDefaultCrashSignalHandlers(void);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_LOG_INTERNAL_H_ */
