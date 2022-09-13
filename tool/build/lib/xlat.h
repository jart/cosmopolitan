#ifndef COSMOPOLITAN_TOOL_BUILD_LIB_XLAT_H_
#define COSMOPOLITAN_TOOL_BUILD_LIB_XLAT_H_
#include "libc/calls/struct/itimerval.h"
#include "libc/calls/struct/rusage.h"
#include "libc/calls/struct/sigset.h"
#include "libc/calls/struct/stat.h"
#include "libc/calls/struct/termios.h"
#include "libc/calls/struct/winsize.h"
#include "libc/sock/struct/sockaddr.h"
#include "tool/build/lib/bits.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

int UnXlatSicode(int, int);
int UnXlatSignal(int);
int XlatAccess(int);
int XlatAdvice(int);
int XlatAtf(int);
int XlatClock(int);
int XlatErrno(int);
int XlatFcntlArg(int);
int XlatFcntlCmd(int);
int XlatLock(int);
int XlatMapFlags(int);
int XlatMsyncFlags(int);
int XlatOpenFlags(int);
int XlatOpenMode(int);
int XlatRusage(int);
int XlatSig(int);
int XlatSignal(int);
int XlatSocketFamily(int);
int XlatSocketFlags(int);
int XlatSocketLevel(int);
int XlatSocketOptname(int, int);
int XlatSocketProtocol(int);
int XlatSocketType(int);
int XlatWait(int);

void XlatSockaddrToHost(struct sockaddr_in *, const struct sockaddr_in_bits *);
void XlatSockaddrToLinux(struct sockaddr_in_bits *, const struct sockaddr_in *);
void XlatStatToLinux(struct stat_bits *, const struct stat *);
void XlatRusageToLinux(struct rusage_bits *, const struct rusage *);
void XlatItimervalToLinux(struct itimerval_bits *, const struct itimerval *);
void XlatLinuxToItimerval(struct itimerval *, const struct itimerval_bits *);
void XlatLinuxToTermios(struct termios *, const struct termios_bits *);
void XlatTermiosToLinux(struct termios_bits *, const struct termios *);
void XlatWinsizeToLinux(struct winsize_bits *, const struct winsize *);
void XlatSigsetToLinux(uint8_t[8], const sigset_t *);
void XlatLinuxToSigset(sigset_t *, const uint8_t[8]);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_TOOL_BUILD_LIB_XLAT_H_ */
