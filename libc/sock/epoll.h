#ifndef COSMOPOLITAN_LIBC_SOCK_WEPOLL_H_
#define COSMOPOLITAN_LIBC_SOCK_WEPOLL_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

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

int epoll_create(int);
int epoll_create1(int);
int epoll_ctl(int, int, int, struct epoll_event *);
int epoll_wait(int, struct epoll_event *, int, int);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_SOCK_WEPOLL_H_ */
