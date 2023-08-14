#ifndef COSMOPOLITAN_LIBC_CALLS_TERMIOS_H_
#define COSMOPOLITAN_LIBC_CALLS_TERMIOS_H_
#include "libc/calls/struct/termios.h"
#include "libc/calls/struct/winsize.h"
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
int tcgetpgrp(int);
int tcflow(int, int);
int tcflush(int, int);
int tcsetpgrp(int, int);
int tcsendbreak(int, int);
void cfmakeraw(struct termios *);
int cfsetspeed(struct termios *, uint32_t);
int cfsetospeed(struct termios *, uint32_t);
int cfsetispeed(struct termios *, uint32_t);
uint32_t cfgetospeed(const struct termios *);
uint32_t cfgetispeed(const struct termios *);
int tcsetwinsize(int, const struct winsize *);
int tcgetwinsize(int, struct winsize *);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_CALLS_TERMIOS_H_ */
