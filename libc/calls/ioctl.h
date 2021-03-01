#ifndef COSMOPOLITAN_LIBC_CALLS_IOCTL_H_
#define COSMOPOLITAN_LIBC_CALLS_IOCTL_H_
#include "libc/macros.internal.h"
#include "libc/sysv/consts/termios.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

/*───────────────────────────────────────────────────────────────────────────│─╗
│ cosmopolitan § system calls » ioctl                                      ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/

int ioctl(int, uint64_t, void *);

/*───────────────────────────────────────────────────────────────────────────│─╗
│ cosmopolitan § system calls » ioctl » undiamonding (size optimization)   ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/
#if defined(__GNUC__) && !defined(__STRICT_ANSI__)

#define ioctl(FD, REQUEST, MEMORY) ioctl_dispatch(FD, REQUEST, MEMORY)

#define __EQUIVALENT(X, Y) (__builtin_constant_p((X) == (Y)) && ((X) == (Y)))
#define __IOCTL_DISPATCH(CMP, FD, REQUEST, MEMORY)                       \
  do {                                                                   \
    if (CMP(request, TIOCGWINSZ)) return ioctl_tiocgwinsz(FD, MEMORY);   \
    if (CMP(request, TIOCSWINSZ)) return ioctl_tiocswinsz(FD, MEMORY);   \
    if (CMP(request, TCGETS)) return ioctl_tcgets(FD, MEMORY);           \
    if (CMP(request, TCSETS)) return ioctl_tcsets(FD, REQUEST, MEMORY);  \
    if (CMP(request, TCSETSW)) return ioctl_tcsets(FD, REQUEST, MEMORY); \
    if (CMP(request, TCSETSF)) return ioctl_tcsets(FD, REQUEST, MEMORY); \
  } while (0)

int ioctl_tcgets(int, void *);
int ioctl_tcgets_nt(int, void *);
int ioctl_tcsets(int, uint64_t, void *);
int ioctl_tcsets_nt(int, uint64_t, void *);
int ioctl_tiocgwinsz(int, void *);
int ioctl_tiocgwinsz_nt(int, void *);
int ioctl_tiocswinsz(int, void *);
int ioctl_tiocswinsz_nt(int, void *);
int ioctl_default(int, uint64_t, void *);

forceinline int ioctl_dispatch(int fd, uint64_t request, void *memory) {
  __IOCTL_DISPATCH(__EQUIVALENT, fd, request, memory);
  return ioctl_default(fd, request, memory);
}

#endif /* GNUC && !ANSI */
COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_CALLS_IOCTL_H_ */
