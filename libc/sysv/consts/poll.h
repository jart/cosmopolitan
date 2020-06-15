#ifndef COSMOPOLITAN_LIBC_SYSV_CONSTS_POLL_H_
#define COSMOPOLITAN_LIBC_SYSV_CONSTS_POLL_H_
#include "libc/runtime/symbolic.h"

#define POLLERR SYMBOLIC(POLLERR)
#define POLLHUP SYMBOLIC(POLLHUP)
#define POLLIN SYMBOLIC(POLLIN)
#define POLLNVAL SYMBOLIC(POLLNVAL)
#define POLLOUT SYMBOLIC(POLLOUT)
#define POLLPRI SYMBOLIC(POLLPRI)
#define POLLRDBAND SYMBOLIC(POLLRDBAND)
#define POLLRDHUP SYMBOLIC(POLLRDHUP)
#define POLLRDNORM SYMBOLIC(POLLRDNORM)
#define POLLWRBAND SYMBOLIC(POLLWRBAND)
#define POLLWRNORM SYMBOLIC(POLLWRNORM)

#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

hidden extern const long POLLERR;
hidden extern const long POLLHUP;
hidden extern const long POLLIN;
hidden extern const long POLLNVAL;
hidden extern const long POLLOUT;
hidden extern const long POLLPRI;
hidden extern const long POLLRDBAND;
hidden extern const long POLLRDHUP;
hidden extern const long POLLRDNORM;
hidden extern const long POLLWRBAND;
hidden extern const long POLLWRNORM;

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_SYSV_CONSTS_POLL_H_ */
