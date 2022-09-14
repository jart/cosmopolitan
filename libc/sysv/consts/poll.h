#ifndef COSMOPOLITAN_LIBC_SYSV_CONSTS_POLL_H_
#define COSMOPOLITAN_LIBC_SYSV_CONSTS_POLL_H_
#include "libc/runtime/symbolic.h"

#define INFTIM     (-1)
#define POLLERR    SYMBOLIC(POLLERR)
#define POLLHUP    SYMBOLIC(POLLHUP)
#define POLLIN     SYMBOLIC(POLLIN)
#define POLLNVAL   SYMBOLIC(POLLNVAL)
#define POLLOUT    SYMBOLIC(POLLOUT)
#define POLLPRI    SYMBOLIC(POLLPRI)
#define POLLRDBAND SYMBOLIC(POLLRDBAND)
#define POLLRDHUP  SYMBOLIC(POLLRDHUP)
#define POLLRDNORM SYMBOLIC(POLLRDNORM)
#define POLLWRBAND SYMBOLIC(POLLWRBAND)
#define POLLWRNORM SYMBOLIC(POLLWRNORM)

#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

extern const int16_t POLLERR;
extern const int16_t POLLHUP;
extern const int16_t POLLIN;
extern const int16_t POLLNVAL;
extern const int16_t POLLOUT;
extern const int16_t POLLPRI;
extern const int16_t POLLRDBAND;
extern const int16_t POLLRDHUP;
extern const int16_t POLLRDNORM;
extern const int16_t POLLWRBAND;
extern const int16_t POLLWRNORM;

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_SYSV_CONSTS_POLL_H_ */
