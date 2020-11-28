#ifndef COSMOPOLITAN_LIBC_SYSV_CONSTS_EPOLL_H_
#define COSMOPOLITAN_LIBC_SYSV_CONSTS_EPOLL_H_
#include "libc/runtime/symbolic.h"
#include "libc/sysv/consts/o.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

hidden extern const long EPOLL_CLOEXEC;

hidden extern const long EPOLL_CTL_ADD;
hidden extern const long EPOLL_CTL_DEL;
hidden extern const long EPOLL_CTL_MOD;

hidden extern const long EPOLLIN;
hidden extern const long EPOLLPRI;
hidden extern const long EPOLLOUT;
hidden extern const long EPOLLERR;
hidden extern const long EPOLLHUP;
hidden extern const long EPOLLRDNORM;
hidden extern const long EPOLLRDBAND;
hidden extern const long EPOLLWRNORM;
hidden extern const long EPOLLWRBAND;
hidden extern const long EPOLLMSG;
hidden extern const long EPOLLRDHUP;
hidden extern const long EPOLLEXCLUSIVE;
hidden extern const long EPOLLWAKEUP;
hidden extern const long EPOLLONESHOT;
hidden extern const long EPOLLET;

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
