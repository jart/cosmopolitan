#ifndef COSMOPOLITAN_LIBC_SOCK_SYSCALL_INTERNAL_H_
#define COSMOPOLITAN_LIBC_SOCK_SYSCALL_INTERNAL_H_
#include "libc/calls/struct/fd.internal.h"
#include "libc/calls/struct/iovec.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

int sys_accept_nt(struct Fd *, void *, uint32_t *, int) _Hide;
int sys_bind_nt(struct Fd *, const void *, uint32_t);
int sys_closesocket_nt(struct Fd *) _Hide;
int sys_connect_nt(struct Fd *, const void *, uint32_t) _Hide;
int sys_getpeername_nt(struct Fd *, void *, uint32_t *) _Hide;
int sys_getsockname_nt(struct Fd *, void *, uint32_t *) _Hide;
int sys_getsockopt_nt(struct Fd *, int, int, void *, uint32_t *) _Hide;
int sys_listen_nt(struct Fd *, int) _Hide;
int sys_setsockopt_nt(struct Fd *, int, int, const void *, uint32_t) _Hide;
int sys_shutdown_nt(struct Fd *, int) _Hide;
ssize_t sys_recv_nt(struct Fd *, const struct iovec *, size_t, uint32_t) _Hide;
ssize_t sys_recvfrom_nt(struct Fd *, const struct iovec *, size_t, uint32_t,
                        void *, uint32_t *) _Hide;

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_SOCK_SYSCALL_INTERNAL_H_ */
