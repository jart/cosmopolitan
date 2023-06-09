#ifndef COSMOPOLITAN_LIBC_SYSV_CONSTS_POLL_H_
#define COSMOPOLITAN_LIBC_SYSV_CONSTS_POLL_H_
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

#define INFTIM     (-1)
#define POLLERR    POLLERR
#define POLLHUP    POLLHUP
#define POLLIN     POLLIN
#define POLLNVAL   POLLNVAL
#define POLLOUT    POLLOUT
#define POLLPRI    POLLPRI
#define POLLRDBAND POLLRDBAND
#define POLLRDHUP  POLLRDHUP
#define POLLRDNORM POLLRDNORM
#define POLLWRBAND POLLWRBAND
#define POLLWRNORM POLLWRNORM

#define __tmpcosmo_POLLERR    1388165514
#define __tmpcosmo_POLLHUP    -1081758058
#define __tmpcosmo_POLLIN     -1297303806
#define __tmpcosmo_POLLNVAL   907399263
#define __tmpcosmo_POLLOUT    -1712263905
#define __tmpcosmo_POLLPRI    1106108134
#define __tmpcosmo_POLLRDBAND -140530500
#define __tmpcosmo_POLLRDHUP  -1946253190
#define __tmpcosmo_POLLRDNORM -1950521858
#define __tmpcosmo_POLLWRBAND 2089556804
#define __tmpcosmo_POLLWRNORM -128148805

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_SYSV_CONSTS_POLL_H_ */
