#ifndef COSMOPOLITAN_LIBC_CALLS_IOCTL_H_
#define COSMOPOLITAN_LIBC_CALLS_IOCTL_H_
#include "libc/sysv/consts/fio.h"
#include "libc/sysv/consts/sio.h"
#include "libc/sysv/consts/termios.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

/*───────────────────────────────────────────────────────────────────────────│─╗
│ cosmopolitan § system calls » ioctl                                      ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/

int ioctl(int, uint64_t, ...);

#if defined(__GNUC__) && !defined(__STRICT_ANSI__)
/*───────────────────────────────────────────────────────────────────────────│─╗
│ cosmopolitan § system calls » ioctl » undiamonding                       ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/

#define ioctl(FD, REQUEST, ...)                                             \
  __IOCTL_DISPATCH(__EQUIVALENT, ioctl_default(FD, REQUEST, ##__VA_ARGS__), \
                   FD, REQUEST, ##__VA_ARGS__)

#define __EQUIVALENT(X, Y) (__builtin_constant_p((X) == (Y)) && ((X) == (Y)))

#define __IOCTL_DISPATCH(CMP, DEFAULT, FD, REQUEST, ...) \
  ({                                                     \
    int ReZ;                                             \
    if (CMP(REQUEST, TIOCGWINSZ)) {                      \
      ReZ = ioctl_tiocgwinsz(FD, ##__VA_ARGS__);         \
    } else if (CMP(REQUEST, TIOCSWINSZ)) {               \
      ReZ = ioctl_tiocswinsz(FD, ##__VA_ARGS__);         \
    } else if (CMP(REQUEST, TCGETS)) {                   \
      ReZ = ioctl_tcgets(FD, ##__VA_ARGS__);             \
    } else if (CMP(REQUEST, TCSETS)) {                   \
      ReZ = ioctl_tcsets(FD, REQUEST, ##__VA_ARGS__);    \
    } else if (CMP(REQUEST, TCSETSW)) {                  \
      ReZ = ioctl_tcsets(FD, REQUEST, ##__VA_ARGS__);    \
    } else if (CMP(REQUEST, TCSETSF)) {                  \
      ReZ = ioctl_tcsets(FD, REQUEST, ##__VA_ARGS__);    \
    } else if (CMP(REQUEST, SIOCGIFCONF)) {              \
      ReZ = ioctl_siocgifconf(FD, ##__VA_ARGS__);        \
    } else if (CMP(REQUEST, SIOCGIFADDR)) {              \
      ReZ = ioctl_siocgifaddr(FD, ##__VA_ARGS__);        \
    } else if (CMP(REQUEST, SIOCGIFNETMASK)) {           \
      ReZ = ioctl_siocgifnetmask(FD, ##__VA_ARGS__);     \
    } else if (CMP(REQUEST, SIOCGIFBRDADDR)) {           \
      ReZ = ioctl_siocgifbrdaddr(FD, ##__VA_ARGS__);     \
    } else if (CMP(REQUEST, SIOCGIFDSTADDR)) {           \
      ReZ = ioctl_siocgifdstaddr(FD, ##__VA_ARGS__);     \
    } else if (CMP(REQUEST, SIOCGIFFLAGS)) {             \
      ReZ = ioctl_siocgifflags(FD, ##__VA_ARGS__);       \
    } else if (CMP(REQUEST, FIONBIO)) {                  \
      ReZ = ioctl_default(FD, REQUEST, ##__VA_ARGS__);   \
    } else if (CMP(REQUEST, FIOCLEX)) {                  \
      ReZ = ioctl_fioclex(FD, REQUEST);                  \
    } else if (CMP(REQUEST, FIONCLEX)) {                 \
      ReZ = ioctl_fioclex(FD, REQUEST);                  \
    } else {                                             \
      ReZ = DEFAULT;                                     \
    }                                                    \
    ReZ;                                                 \
  })

int ioctl_default(int, uint64_t, ...);
int ioctl_fioclex(int, int);
int ioctl_siocgifaddr(int, ...);
int ioctl_siocgifbrdaddr(int, ...);
int ioctl_siocgifconf(int, ...);
int ioctl_siocgifdstaddr(int, ...);
int ioctl_siocgifflags(int, ...);
int ioctl_siocgifnetmask(int, ...);
int ioctl_tcgets(int, ...);
int ioctl_tcsets(int, uint64_t, ...);
int ioctl_tiocgwinsz(int, ...);
int ioctl_tiocswinsz(int, ...);

#endif /* GNUC && !ANSI */
COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_CALLS_IOCTL_H_ */
