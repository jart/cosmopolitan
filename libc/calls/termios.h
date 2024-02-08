#ifndef COSMOPOLITAN_LIBC_CALLS_TERMIOS_H_
#define COSMOPOLITAN_LIBC_CALLS_TERMIOS_H_
#include "libc/calls/struct/termios.h"
#include "libc/calls/struct/winsize.h"
COSMOPOLITAN_C_START_

/*───────────────────────────────────────────────────────────────────────────│─╗
│ cosmopolitan § teletypewriter control                                    ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/

int tcgetattr(int, struct termios *) libcesque;
int tcsetattr(int, int, const struct termios *) libcesque;

int openpty(int *, int *, char *, const struct termios *,
            const struct winsize *) libcesque paramsnonnull((1, 2));
int forkpty(int *, char *, const struct termios *,
            const struct winsize *) libcesque paramsnonnull((1, 2)) __wur;
char *ptsname(int) libcesque;
errno_t ptsname_r(int, char *, size_t) libcesque;

int grantpt(int) libcesque;
int unlockpt(int) libcesque;
int posix_openpt(int) libcesque __wur;

int tcdrain(int) libcesque;
int tcgetsid(int) libcesque;
int tcflow(int, int) libcesque;
int tcflush(int, int) libcesque;
int tcsetsid(int, int) libcesque;
int tcsendbreak(int, int) libcesque;
void cfmakeraw(struct termios *) libcesque;
int cfsetspeed(struct termios *, uint32_t) libcesque;
int cfsetospeed(struct termios *, uint32_t) libcesque;
int cfsetispeed(struct termios *, uint32_t) libcesque;
uint32_t cfgetospeed(const struct termios *) libcesque;
uint32_t cfgetispeed(const struct termios *) libcesque;
int tcsetwinsize(int, const struct winsize *) libcesque;
int tcgetwinsize(int, struct winsize *) libcesque;

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_CALLS_TERMIOS_H_ */
