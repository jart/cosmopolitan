#ifndef COSMOPOLITAN_LIBC_SOCK_SYSCALL_INTERNAL_H_
#define COSMOPOLITAN_LIBC_SOCK_SYSCALL_INTERNAL_H_
#include "libc/calls/struct/iovec.h"
#include "libc/intrin/fds.h"
#include "libc/nt/struct/overlapped.h"
#include "libc/sock/struct/sockaddr.h"
COSMOPOLITAN_C_START_

int sys_accept_nt(struct Fd *, struct sockaddr_storage *, int);
int sys_bind_nt(struct Fd *, const void *, uint32_t);
int sys_closesocket_nt(struct Fd *);
int sys_ioctlsocket_nt(struct Fd *);
int sys_connect_nt(struct Fd *, const void *, uint32_t);
int sys_getpeername_nt(struct Fd *, void *, uint32_t *);
int sys_getsockname_nt(struct Fd *, void *, uint32_t *);
int sys_getsockopt_nt(struct Fd *, int, int, void *, uint32_t *);
int sys_listen_nt(struct Fd *, int);
int sys_setsockopt_nt(struct Fd *, int, int, const void *, uint32_t);
int sys_shutdown_nt(struct Fd *, int);
ssize_t sys_recv_nt(int, const struct iovec *, size_t, uint32_t);
ssize_t sys_recvfrom_nt(int, const struct iovec *, size_t, uint32_t, void *,
                        uint32_t *);

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_SOCK_SYSCALL_INTERNAL_H_ */
