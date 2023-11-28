#ifndef COSMOPOLITAN_LIBC_SYSV_CONSTS_POLL_H_
#define COSMOPOLITAN_LIBC_SYSV_CONSTS_POLL_H_
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


COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_SYSV_CONSTS_POLL_H_ */
