#ifndef COSMOPOLITAN_LIBC_SOCK_INTERNAL_H_
#define COSMOPOLITAN_LIBC_SOCK_INTERNAL_H_
#include "libc/calls/struct/iovec.h"
#include "libc/calls/struct/sigset.h"
#include "libc/nt/struct/overlapped.h"
#include "libc/nt/thunk/msabi.h"
#include "libc/nt/winsock.h"
#include "libc/sock/select.h"
#include "libc/sock/sock.h"
#include "libc/sock/struct/sockaddr.h"
COSMOPOLITAN_C_START_

#define kNtFdRead                   1
#define kNtFdWrite                  2
#define kNtFdOob                    4
#define kNtFdAccept                 8
#define kNtFdConnect                16
#define kNtFdClose                  32
#define kNtFdQos                    64
#define kNtFdGroupQos               128
#define kNtFdRoutingInterfaceChange 256
#define kNtFdAddressListChange      512

/* ------------------------------------------------------------------------------------*/

#define SOCKFD_OVERLAP_BUFSIZ 128

errno_t __dos2errno(uint32_t);

int32_t __sys_accept(int32_t, void *, uint32_t *, int) __wur;
int32_t __sys_accept4(int32_t, void *, uint32_t *, int) __wur;
int32_t __sys_bind(int32_t, const void *, uint32_t);
int32_t __sys_connect(int32_t, const void *, uint32_t);
int32_t __sys_getpeername(int32_t, void *, uint32_t *);
int32_t __sys_getsockname(int32_t, void *, uint32_t *);
int32_t __sys_socket(int32_t, int32_t, int32_t);
int32_t __sys_socketpair(int32_t, int32_t, int32_t, int32_t[2]);

int32_t sys_accept4(int32_t, struct sockaddr_storage *, int) __wur;
int32_t sys_bind(int32_t, const void *, uint32_t);
int32_t sys_connect(int32_t, const void *, uint32_t);
int32_t sys_getsockopt(int32_t, int32_t, int32_t, void *, uint32_t *);
int32_t sys_listen(int32_t, int32_t);
int32_t sys_getsockname(int32_t, void *, uint32_t *);
int32_t sys_getpeername(int32_t, void *, uint32_t *);
int32_t sys_shutdown(int32_t, int32_t);
int32_t sys_socket(int32_t, int32_t, int32_t);
int32_t sys_socketpair(int32_t, int32_t, int32_t, int32_t[2]);
ssize_t sys_recvfrom(int, void *, size_t, int, void *, uint32_t *);
ssize_t sys_sendto(int, const void *, size_t, int, const void *, uint32_t);
int32_t sys_select(int32_t, fd_set *, fd_set *, fd_set *, struct timeval *);
int sys_pselect(int, fd_set *, fd_set *, fd_set *, struct timespec *,
                const void *);
int sys_setsockopt(int, int, int, const void *, uint32_t);
int32_t sys_epoll_create(int32_t);
int32_t sys_epoll_ctl(int32_t, int32_t, int32_t, void *);
int32_t sys_epoll_wait(int32_t, void *, int32_t, int32_t);
int32_t sys_epoll_pwait(int32_t, void *, int32_t, int32_t, const sigset_t *,
                        size_t);

int sys_socket_nt(int, int, int);

/*
int sys_socketpair_nt_stream(int, int, int, int[2]) ;
int sys_socketpair_nt_dgram(int, int, int, int[2]) ;
*/
int sys_socketpair_nt(int, int, int, int[2]);
int sys_select_nt(int, fd_set *, fd_set *, fd_set *, struct timeval *,
                  const sigset_t *);

size_t __iovec2nt(struct NtIovec[hasatleast 16], const struct iovec *, size_t);

ssize_t __winsock_block(int64_t, uint32_t, bool, uint32_t, uint64_t,
                        int (*)(int64_t, struct NtOverlapped *, uint32_t *,
                                void *),
                        void *);

void WinSockInit(void);
int64_t __winsockerr(void);
int __fixupnewsockfd(int, int);
int64_t GetNtBaseSocket(int64_t);
int sys_close_epoll(int);

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_SOCK_INTERNAL_H_ */
