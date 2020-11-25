#ifndef COSMOPOLITAN_LIBC_CALLS_IOCTL_H_
#define COSMOPOLITAN_LIBC_CALLS_IOCTL_H_
#include "libc/macros.h"
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

#define ioctl(FD, REQUEST, MEMORY) ioctl$dispatch(FD, REQUEST, MEMORY)

#define __IOCTL_DISPATCH(CMP, FD, REQUEST, MEMORY)                       \
  do {                                                                   \
    if (CMP(request, TIOCGWINSZ)) return ioctl$tiocgwinsz(FD, MEMORY);   \
    if (CMP(request, TIOCSWINSZ)) return ioctl$tiocswinsz(FD, MEMORY);   \
    if (CMP(request, TCGETS)) return ioctl$tcgets(FD, MEMORY);           \
    if (CMP(request, TCSETS)) return ioctl$tcsets(FD, REQUEST, MEMORY);  \
    if (CMP(request, TCSETSW)) return ioctl$tcsets(FD, REQUEST, MEMORY); \
    if (CMP(request, TCSETSF)) return ioctl$tcsets(FD, REQUEST, MEMORY); \
  } while (0)

int ioctl$tcgets(int, void *);
int ioctl$tcgets$nt(int, void *);
int ioctl$tcsets(int, uint64_t, void *);
int ioctl$tcsets$nt(int, uint64_t, void *);
int ioctl$tiocgwinsz(int, void *);
int ioctl$tiocgwinsz$nt(int, void *);
int ioctl$tiocswinsz(int, void *);
int ioctl$tiocswinsz$nt(int, void *);
int ioctl$default(int, uint64_t, void *);

forceinline int ioctl$dispatch(int fd, uint64_t request, void *memory) {
  __IOCTL_DISPATCH(EQUIVALENT, fd, request, memory);
  return ioctl$default(fd, request, memory);
}

#endif /* GNUC && !ANSI */
COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_CALLS_IOCTL_H_ */
