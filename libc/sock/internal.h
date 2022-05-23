#ifndef COSMOPOLITAN_LIBC_SOCK_INTERNAL_H_
#define COSMOPOLITAN_LIBC_SOCK_INTERNAL_H_
#include "libc/nt/struct/overlapped.h"
#include "libc/nt/thunk/msabi.h"
#include "libc/nt/winsock.h"
#include "libc/sock/select.h"
#include "libc/sock/sock.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

#define FD_READ        (1 << FD_READ_BIT)
#define FD_READ_BIT    0
#define FD_WRITE       (1 << FD_WRITE_BIT)
#define FD_WRITE_BIT   1
#define FD_OOB         (1 << FD_OOB_BIT)
#define FD_OOB_BIT     2
#define FD_ACCEPT      (1 << FD_ACCEPT_BIT)
#define FD_ACCEPT_BIT  3
#define FD_CONNECT     (1 << FD_CONNECT_BIT)
#define FD_CONNECT_BIT 4
#define FD_CLOSE       (1 << FD_CLOSE_BIT)
#define FD_CLOSE_BIT   5

struct sockaddr_bsd {
  uint8_t sa_len;    /* « different type */
  uint8_t sa_family; /* « different type */
  char sa_data[14];
};

struct sockaddr_in_bsd {
  uint8_t sin_len;    /* « different type */
  uint8_t sin_family; /* « different type */
  uint16_t sin_port;
  struct in_addr sin_addr;
  uint8_t sin_zero[8];
};

struct msghdr_bsd {
  void *msg_name;
  uint32_t msg_namelen;
  struct iovec *msg_iov;
  uint32_t msg_iovlen; /* « different type */
  void *msg_control;
  uint64_t msg_controllen;
  uint32_t msg_flags; /* « different type */
};

struct sockaddr_un_bsd {
  uint8_t sun_len; /* sockaddr len including NUL on freebsd but excluding it on
                      openbsd/xnu */
  uint8_t sun_family;
  char sun_path[108];
};

/* ------------------------------------------------------------------------------------*/

#define SOCKFD_OVERLAP_BUFSIZ 128

struct SockFd {
  int family;
  int type;
  int protocol;
  bool32 (*__msabi ConnectEx)(int64_t s, const struct sockaddr *name,
                              int namelen, const void *opt_lpSendBuffer,
                              uint32_t dwSendDataLength,
                              uint32_t *out_lpdwBytesSent,
                              struct NtOverlapped *inout_lpOverlapped);
  bool32 (*__msabi AcceptEx)(
      int64_t sListenSocket, int64_t sAcceptSocket,
      void *out_lpOutputBuffer /*[recvlen+local+remoteaddrlen]*/,
      uint32_t dwReceiveDataLength, uint32_t dwLocalAddressLength,
      uint32_t dwRemoteAddressLength, uint32_t *out_lpdwBytesReceived,
      struct NtOverlapped *inout_lpOverlapped);
};

errno_t __dos2errno(uint32_t) hidden;

void _firewall(const void *, uint32_t) hidden;

int32_t __sys_accept(int32_t, void *, uint32_t *, int) dontdiscard hidden;
int32_t __sys_accept4(int32_t, void *, uint32_t *, int) dontdiscard hidden;
int32_t __sys_connect(int32_t, const void *, uint32_t) hidden;
int32_t __sys_socket(int32_t, int32_t, int32_t) hidden;
int32_t __sys_getsockname(int32_t, void *, uint32_t *) hidden;
int32_t __sys_getpeername(int32_t, void *, uint32_t *) hidden;
int32_t __sys_socketpair(int32_t, int32_t, int32_t, int32_t[2]) hidden;

int32_t sys_accept4(int32_t, void *, uint32_t *, int) dontdiscard hidden;
int32_t sys_accept(int32_t, void *, uint32_t *) hidden;
int32_t sys_bind(int32_t, const void *, uint32_t) hidden;
int32_t sys_connect(int32_t, const void *, uint32_t) hidden;
int32_t sys_getsockopt(int32_t, int32_t, int32_t, void *, uint32_t *) hidden;
int32_t sys_listen(int32_t, int32_t) hidden;
int32_t sys_getsockname(int32_t, void *, uint32_t *) hidden;
int32_t sys_getpeername(int32_t, void *, uint32_t *) hidden;
int32_t sys_poll(struct pollfd *, uint64_t, signed) hidden;
int32_t sys_shutdown(int32_t, int32_t) hidden;
int32_t sys_socket(int32_t, int32_t, int32_t) hidden;
int32_t sys_socketpair(int32_t, int32_t, int32_t, int32_t[2]) hidden;
int64_t sys_readv(int32_t, const struct iovec *, int32_t) hidden;
int64_t sys_writev(int32_t, const struct iovec *, int32_t) hidden;
ssize_t sys_recvfrom(int, void *, size_t, int, void *, uint32_t *) hidden;
ssize_t sys_sendto(int, const void *, size_t, int, const void *,
                   uint32_t) hidden;
ssize_t sys_sendmsg(int, const struct msghdr *, int) hidden;
ssize_t sys_recvmsg(int, struct msghdr *, int) hidden;
int32_t sys_select(int32_t, fd_set *, fd_set *, fd_set *,
                   struct timeval *) hidden;
int sys_setsockopt(int, int, int, const void *, uint32_t) hidden;
int32_t sys_epoll_create(int32_t) hidden;
int32_t sys_epoll_ctl(int32_t, int32_t, int32_t, void *) hidden;
int32_t sys_epoll_wait(int32_t, void *, int32_t, int32_t) hidden;
int sys_poll_metal(struct pollfd *, size_t, unsigned);

int sys_poll_nt(struct pollfd *, uint64_t, uint64_t *) hidden;
int sys_socket_nt(int, int, int) hidden;
/*
int sys_socketpair_nt_stream(int, int, int, int[2]) hidden;
int sys_socketpair_nt_dgram(int, int, int, int[2]) hidden;
*/
int sys_socketpair_nt(int, int, int, int[2]) hidden;
int sys_select_nt(int, fd_set *, fd_set *, fd_set *, struct timeval *) hidden;

bool __asan_is_valid_msghdr(const struct msghdr *);
ssize_t sys_send_nt(int, const struct iovec *, size_t, uint32_t) hidden;
size_t __iovec2nt(struct NtIovec[hasatleast 16], const struct iovec *,
                  size_t) hidden;
ssize_t sys_sendto_nt(int, const struct iovec *, size_t, uint32_t, void *,
                      uint32_t) hidden;

void WinSockInit(void) hidden;
int64_t __winsockerr(void) nocallback hidden;
int __fixupnewsockfd(int, int) hidden;
int __wsablock(int64_t, struct NtOverlapped *, uint32_t *, bool) hidden;
int64_t __winsockblock(int64_t, unsigned, int64_t) hidden;
struct SockFd *_dupsockfd(struct SockFd *) hidden;
int64_t GetNtBaseSocket(int64_t) hidden;
int sys_close_epoll(int) hidden;

/**
 * Converts sockaddr (Linux/Windows) → sockaddr_bsd (XNU/BSD).
 */
forceinline void sockaddr2bsd(void *saddr) {
  char *p;
  if (saddr) {
    p = saddr;
    p[1] = p[0];
    p[0] = sizeof(struct sockaddr_in_bsd);
  }
}

/**
 * Converts sockaddr_in_bsd (XNU/BSD) → sockaddr (Linux/Windows).
 */
forceinline void sockaddr2linux(void *saddr) {
  char *p;
  if (saddr) {
    p = saddr;
    p[0] = p[1];
    p[1] = 0;
  }
}

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_SOCK_INTERNAL_H_ */
