#ifndef COSMOPOLITAN_LIBC_NT_STRUCT_POLLFD_H_
#define COSMOPOLITAN_LIBC_NT_STRUCT_POLLFD_H_

struct sys_pollfd_nt {
  int64_t handle;
  int16_t events;
  int16_t revents;
};

#endif /* COSMOPOLITAN_LIBC_NT_STRUCT_POLLFD_H_ */
