#ifndef COSMOPOLITAN_LIBC_SOCK_WEPOLL_H_
#define COSMOPOLITAN_LIBC_SOCK_WEPOLL_H_
COSMOPOLITAN_C_START_
#include "libc/calls/struct/sigset.h"

typedef union epoll_data {
  void *ptr;
  int fd;
  uint32_t u32;
  uint64_t u64;
} epoll_data_t;

struct thatispacked epoll_event {
  uint32_t events;
  epoll_data_t data;
};

int epoll_create(int) libcesque;
int epoll_create1(int) libcesque;
int epoll_ctl(int, int, int, struct epoll_event *) libcesque;
int epoll_wait(int, struct epoll_event *, int, int) libcesque;
int epoll_pwait(int, struct epoll_event *, int, int, const sigset_t *);

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_SOCK_WEPOLL_H_ */
