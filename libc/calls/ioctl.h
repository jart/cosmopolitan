#ifndef COSMOPOLITAN_LIBC_CALLS_IOCTL_H_
#define COSMOPOLITAN_LIBC_CALLS_IOCTL_H_
#include "libc/macros.internal.h"
#include "libc/sysv/consts/fio.h"
#include "libc/sysv/consts/sio.h"
#include "libc/sysv/consts/termios.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

/*───────────────────────────────────────────────────────────────────────────│─╗
│ cosmopolitan § system calls » ioctl                                      ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/

int ioctl(int, uint64_t, ...);

/*───────────────────────────────────────────────────────────────────────────│─╗
│ cosmopolitan § system calls » ioctl » undiamonding (size optimization)   ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/
#if defined(__GNUC__) && !defined(__STRICT_ANSI__)

#define ioctl(FD, REQUEST, ...)                                             \
  __IOCTL_DISPATCH(__EQUIVALENT, ioctl_default(FD, REQUEST, ##__VA_ARGS__), \
                   FD, REQUEST, ##__VA_ARGS__)

#define __EQUIVALENT(X, Y) (__builtin_constant_p((X) == (Y)) && ((X) == (Y)))

#define __IOCTL_DISPATCH(CMP, DEFAULT, FD, REQUEST, ...) \
  ({                                                     \
    int ReZ;                                             \
    if (CMP(REQUEST, TIOCGWINSZ)) {                      \
      ReZ = ioctl_tiocgwinsz(FD, __VA_ARGS__);           \
    } else if (CMP(REQUEST, TIOCSWINSZ)) {               \
      ReZ = ioctl_tiocswinsz(FD, __VA_ARGS__);           \
    } else if (CMP(REQUEST, TCGETS)) {                   \
      ReZ = ioctl_tcgets(FD, __VA_ARGS__);               \
    } else if (CMP(REQUEST, TCSETS)) {                   \
      ReZ = ioctl_tcsets(FD, REQUEST, __VA_ARGS__);      \
    } else if (CMP(REQUEST, TCSETSW)) {                  \
      ReZ = ioctl_tcsets(FD, REQUEST, __VA_ARGS__);      \
    } else if (CMP(REQUEST, TCSETSF)) {                  \
      ReZ = ioctl_tcsets(FD, REQUEST, __VA_ARGS__);      \
    } else if (CMP(REQUEST, SIOCGIFCONF)) {              \
      ReZ = ioctl_siocgifconf(FD, __VA_ARGS__);          \
    } else if (CMP(REQUEST, SIOCGIFADDR)) {              \
      ReZ = ioctl_siocgifaddr(FD, __VA_ARGS__);          \
    } else if (CMP(REQUEST, SIOCGIFNETMASK)) {           \
      ReZ = ioctl_siocgifnetmask(FD, __VA_ARGS__);       \
    } else if (CMP(REQUEST, SIOCGIFBRDADDR)) {           \
      ReZ = ioctl_siocgifbrdaddr(FD, __VA_ARGS__);       \
    } else if (CMP(REQUEST, SIOCGIFDSTADDR)) {           \
      ReZ = ioctl_siocgifdstaddr(FD, __VA_ARGS__);       \
    } else if (CMP(REQUEST, SIOCGIFFLAGS)) {             \
      ReZ = ioctl_siocgifflags(FD, __VA_ARGS__);         \
    } else if (CMP(REQUEST, FIOCLEX)) {                  \
      ReZ = ioctl_fioclex(FD);                           \
    } else if (CMP(REQUEST, FIONCLEX)) {                 \
      ReZ = ioctl_fionclex(FD);                          \
    } else {                                             \
      ReZ = DEFAULT;                                     \
    }                                                    \
    ReZ;                                                 \
  })

int ioctl_tcgets(int, void *);
int ioctl_tcgets_nt(int, void *);
int ioctl_tcsets(int, uint64_t, void *);
int ioctl_tcsets_nt(int, uint64_t, void *);
int ioctl_tiocgwinsz(int, void *);
int ioctl_tiocgwinsz_nt(int, void *);
int ioctl_tiocswinsz(int, void *);
int ioctl_tiocswinsz_nt(int, void *);
int ioctl_siocgifconf(int, void *);
int ioctl_siocgifaddr(int, void *);
int ioctl_siocgifdstaddr(int, void *);
int ioctl_siocgifnetmask(int, void *);
int ioctl_siocgifbrdaddr(int, void *);
int ioctl_siocgifflags(int, void *);
int ioctl_default(int, uint64_t, void *);
int ioctl_fioclex(int);
int ioctl_fionclex(int);

#endif /* GNUC && !ANSI */
COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_CALLS_IOCTL_H_ */
