#ifndef COSMOPOLITAN_LIBC_LOG_INTERNAL_H_
#define COSMOPOLITAN_LIBC_LOG_INTERNAL_H_
#include "libc/calls/struct/siginfo.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

extern bool __nocolor;
extern bool _wantcrashreports;
extern bool g_isrunningundermake;

void __start_fatal(const char *, int);
void __restore_tty(void);
void RestoreDefaultCrashSignalHandlers(void);
void __oncrash_amd64(int, struct siginfo *, void *) relegated;
void __oncrash_arm64(int, struct siginfo *, void *) relegated;

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_LOG_INTERNAL_H_ */
