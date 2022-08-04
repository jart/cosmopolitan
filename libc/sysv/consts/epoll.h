#ifndef COSMOPOLITAN_LIBC_SYSV_CONSTS_EPOLL_H_
#define COSMOPOLITAN_LIBC_SYSV_CONSTS_EPOLL_H_
#include "libc/runtime/symbolic.h"
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

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */

#define EPOLL_CLOEXEC O_CLOEXEC

#define EPOLL_CTL_ADD LITERALLY(1)
#define EPOLL_CTL_DEL LITERALLY(2)
#define EPOLL_CTL_MOD LITERALLY(3)

#define EPOLLIN        LITERALLY(1)
#define EPOLLPRI       LITERALLY(2)
#define EPOLLOUT       LITERALLY(4)
#define EPOLLERR       LITERALLY(8)
#define EPOLLHUP       LITERALLY(0x10)
#define EPOLLRDNORM    LITERALLY(0x40)
#define EPOLLRDBAND    LITERALLY(0x80)
#define EPOLLWRNORM    LITERALLY(0x0100)
#define EPOLLWRBAND    LITERALLY(0x0200)
#define EPOLLMSG       LITERALLY(0x0400)
#define EPOLLRDHUP     LITERALLY(0x2000)
#define EPOLLEXCLUSIVE LITERALLY(0x10000000)
#define EPOLLWAKEUP    LITERALLY(0x20000000)
#define EPOLLONESHOT   LITERALLY(0x40000000)
#define EPOLLET        LITERALLY(0x80000000)

#endif /* COSMOPOLITAN_LIBC_SYSV_CONSTS_EPOLL_H_ */
