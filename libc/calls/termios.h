#ifndef COSMOPOLITAN_LIBC_CALLS_TERMIOS_H_
#define COSMOPOLITAN_LIBC_CALLS_TERMIOS_H_
#include "libc/calls/ioctl.h"
#include "libc/calls/struct/termios.h"
#include "libc/calls/struct/winsize.h"
#include "libc/sysv/consts/termios.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

/*───────────────────────────────────────────────────────────────────────────│─╗
│ cosmopolitan § teletypewriter control                                    ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/

int tcgetattr(int, struct termios *);
int tcsetattr(int, int, const struct termios *);

int openpty(int *, int *, char *, const struct termios *,
            const struct winsize *) paramsnonnull((1, 2));
int forkpty(int *, char *, const struct termios *, const struct winsize *)
    paramsnonnull((1, 2)) dontdiscard;
char *ptsname(int);
errno_t ptsname_r(int, char *, size_t);

int grantpt(int);
int unlockpt(int);
int posix_openpt(int) dontdiscard;

int tcdrain(int);
int tcgetsid(int);
int tcflow(int, int);
int tcflush(int, int);
int tcsendbreak(int, int);
void cfmakeraw(struct termios *);
int cfsetospeed(struct termios *, unsigned);
int cfsetispeed(struct termios *, unsigned);
uint32_t cfgetospeed(const struct termios *);
uint32_t cfgetispeed(const struct termios *);
int tcsetwinsize(int, const struct winsize *);
int tcgetwinsize(int, struct winsize *);

/*───────────────────────────────────────────────────────────────────────────│─╗
│ cosmopolitan § teletypewriter » undiamonding                             ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/
#if defined(__GNUC__) && !defined(__STRICT_ANSI__)

#define tcsetattr(FD, OPT, TIO) tcsetattr_dispatch(FD, OPT, TIO)
forceinline int tcsetattr_dispatch(int fd, int opt, const struct termios *tio) {
  if (__EQUIVALENT(opt, TCSANOW)) return ioctl(fd, TCSETS, (void *)tio);
  if (__EQUIVALENT(opt, TCSADRAIN)) return ioctl(fd, TCSETSW, (void *)tio);
  if (__EQUIVALENT(opt, TCSAFLUSH)) return ioctl(fd, TCSETSF, (void *)tio);
  return (tcsetattr)(fd, opt, tio);
}

#define tcgetattr(FD, TIO) tcgetattr_dispatch(FD, TIO)
forceinline int tcgetattr_dispatch(int fd, const struct termios *tio) {
  return ioctl(fd, TCGETS, (void *)tio);
}

#endif /* GNUC && !ANSI */
COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_CALLS_TERMIOS_H_ */
