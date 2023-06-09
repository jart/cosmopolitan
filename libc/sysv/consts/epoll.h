#ifndef COSMOPOLITAN_LIBC_SYSV_CONSTS_EPOLL_H_
#define COSMOPOLITAN_LIBC_SYSV_CONSTS_EPOLL_H_
#include "libc/sysv/consts/o.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

extern const int EPOLL_CLOEXEC;

extern const int EPOLL_CTL_ADD;
extern const int EPOLL_CTL_DEL;
extern const int EPOLL_CTL_MOD;

extern const uint32_t EPOLLIN;
extern const uint32_t EPOLLPRI;
extern const uint32_t EPOLLOUT;
extern const uint32_t EPOLLERR;
extern const uint32_t EPOLLHUP;
extern const uint32_t EPOLLRDNORM;
extern const uint32_t EPOLLRDBAND;
extern const uint32_t EPOLLWRNORM;
extern const uint32_t EPOLLWRBAND;
extern const uint32_t EPOLLMSG;
extern const uint32_t EPOLLRDHUP;
extern const uint32_t EPOLLEXCLUSIVE;
extern const uint32_t EPOLLWAKEUP;
extern const uint32_t EPOLLONESHOT;
extern const uint32_t EPOLLET;

#define EPOLL_CLOEXEC O_CLOEXEC

#define EPOLL_CTL_ADD 1
#define EPOLL_CTL_DEL 2
#define EPOLL_CTL_MOD 3

#define EPOLLIN        1
#define EPOLLPRI       2
#define EPOLLOUT       4
#define EPOLLERR       8
#define EPOLLHUP       0x10
#define EPOLLRDNORM    0x40
#define EPOLLRDBAND    0x80
#define EPOLLWRNORM    0x0100
#define EPOLLWRBAND    0x0200
#define EPOLLMSG       0x0400
#define EPOLLRDHUP     0x2000
#define EPOLLEXCLUSIVE 0x10000000
#define EPOLLWAKEUP    0x20000000
#define EPOLLONESHOT   0x40000000
#define EPOLLET        0x80000000

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_SYSV_CONSTS_EPOLL_H_ */
