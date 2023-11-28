#ifndef COSMOPOLITAN_LIBC_SOCK_STRUCT_MSGHDR_H_
#define COSMOPOLITAN_LIBC_SOCK_STRUCT_MSGHDR_H_
#include "libc/calls/struct/iovec.h"
COSMOPOLITAN_C_START_

struct msghdr {            /* Linux+NT ABI */
  void *msg_name;          /* optional address */
  uint32_t msg_namelen;    /* size of msg_name */
  struct iovec *msg_iov;   /* scatter/gather array */
  uint64_t msg_iovlen;     /* # elements in msg_iov */
  void *msg_control;       /* ancillary data c. cmsghdr */
  uint64_t msg_controllen; /* ancillary data buffer len */
  uint32_t msg_flags;      /* MSG_XXX */
};

ssize_t recvmsg(int, struct msghdr *, int);
ssize_t sendmsg(int, const struct msghdr *, int);

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_SOCK_STRUCT_MSGHDR_H_ */
