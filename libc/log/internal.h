#ifndef COSMOPOLITAN_LIBC_LOG_INTERNAL_H_
#define COSMOPOLITAN_LIBC_LOG_INTERNAL_H_
#include "libc/log/log.h"
#include "libc/calls/struct/sigaction.h"
#include "libc/calls/calls.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

#define RED (cancolor() ? "\x1b[30;101m" : "")
#define UNBOLD (cancolor() ? "\x1b[22m" : "")
#define RED2 (cancolor() ? "\x1b[91;1m" : "")
#define BLUE1 (cancolor() ? "\x1b[94;49m" : "")
#define BLUE2 (cancolor() ? "\x1b[34m" : "")
#define RESET (cancolor() ? "\x1b[0m" : "")
#define SUBTLE (cancolor() ? "\x1b[35m" : "")

enum NtExceptionHandlerActions;
struct NtExceptionPointers;

extern int kCrashSigs[8];
extern struct sigaction g_oldcrashacts[8];
extern const char kCrashSigNames[8][5] aligned(1);
extern const char kGodHatesFlags[12][2] aligned(1);
extern const char kGregNames[17][4] aligned(1);
extern const char kGregOrder[17] aligned(1);

void startfatal(const char *, int) hidden;
void startfatal_ndebug(void) hidden;
void oncrash(int, struct siginfo *, struct ucontext *) relegated;
enum NtExceptionHandlerActions wincrash$nt(
    const struct NtExceptionPointers *) relegated;

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_LOG_INTERNAL_H_ */
