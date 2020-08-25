#ifndef COSMOPOLITAN_LIBC_CALLS_TERMIOS_H_
#define COSMOPOLITAN_LIBC_CALLS_TERMIOS_H_
#include "libc/calls/ioctl.h"
#include "libc/calls/struct/termios.h"
#include "libc/calls/struct/winsize.h"
#include "libc/macros.h"
#include "libc/sysv/consts/termios.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

/*───────────────────────────────────────────────────────────────────────────│─╗
│ cosmopolitan § teletypewriter control                                    ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/

int tcgetattr(int, struct termios *);
int tcsetattr(int, int, const struct termios *);
int tcsetpgrp(int, int32_t);
int32_t tcgetpgrp(int);

int openpty(int *, int *, char *, const struct termios *,
            const struct winsize *) paramsnonnull((1, 2)) nodiscard;
int forkpty(int *, char *, const struct termios *, const struct winsize *)
    paramsnonnull((1, 2)) nodiscard;
errno_t ptsname_r(int, char *, size_t);

int grantpt(int);
int unlockpt(int);
int posix_openpt(int) nodiscard;

/*───────────────────────────────────────────────────────────────────────────│─╗
│ cosmopolitan § teletypewriter » undiamonding                             ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/
#if defined(__GNUC__) && !defined(__STRICT_ANSI__)

#define tcsetattr(FD, OPT, TIO) tcsetattr$dispatch(FD, OPT, TIO)
forceinline int tcsetattr$dispatch(int fd, int opt, const struct termios *tio) {
  if (EQUIVALENT(opt, TCSANOW)) return ioctl(fd, TCSETS, (void *)tio);
  if (EQUIVALENT(opt, TCSADRAIN)) return ioctl(fd, TCSETSW, (void *)tio);
  if (EQUIVALENT(opt, TCSAFLUSH)) return ioctl(fd, TCSETSF, (void *)tio);
  return (tcsetattr)(fd, opt, tio);
}

#define tcgetattr(FD, TIO) tcgetattr$dispatch(FD, TIO)
forceinline int tcgetattr$dispatch(int fd, const struct termios *tio) {
  return ioctl(fd, TCGETS, (void *)tio);
}

#endif /* GNUC && !ANSI */
COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_CALLS_TERMIOS_H_ */
