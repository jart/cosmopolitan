#ifndef COSMOPOLITAN_LIBC_SOCK_STRUCT_MSGHDR_INTERNAL_H_
#define COSMOPOLITAN_LIBC_SOCK_STRUCT_MSGHDR_INTERNAL_H_
#include "libc/sock/struct/msghdr.h"
COSMOPOLITAN_C_START_

struct msghdr_bsd {
  void *msg_name;
  uint32_t msg_namelen;
  struct iovec *msg_iov;
  uint32_t msg_iovlen; /* « different type */
  void *msg_control;
  uint64_t msg_controllen;
  uint32_t msg_flags; /* « different type */
};

ssize_t sys_sendmsg(int, const struct msghdr *, int);
ssize_t sys_recvmsg(int, struct msghdr *, int);
bool __asan_is_valid_msghdr(const struct msghdr *);

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_SOCK_STRUCT_MSGHDR_INTERNAL_H_ */
