#ifndef COSMOPOLITAN_LIBC_LOG_INTERNAL_H_
#define COSMOPOLITAN_LIBC_LOG_INTERNAL_H_
#include "libc/calls/struct/siginfo.h"
COSMOPOLITAN_C_START_

extern bool __nocolor;
extern bool32 g_isrunningundermake;

void __start_fatal(const char *, int);
void __restore_tty(void);
void __oncrash(int, struct siginfo *, void *);

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_LOG_INTERNAL_H_ */
