#ifndef COSMOPOLITAN_LIBC_SOCK_INTERNAL_H_
#define COSMOPOLITAN_LIBC_SOCK_INTERNAL_H_
#ifndef __STRICT_ANSI__
#include "libc/bits/bits.h"
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

struct Fd;
struct iovec;
struct timeval;

struct sockaddr$bsd {
  uint8_t sa_len;    /* « different type */
  uint8_t sa_family; /* « different type */
  char sa_data[14];
};

struct sockaddr_in$bsd {
  uint8_t sin_len;    /* « different type */
  uint8_t sin_family; /* « different type */
  uint16_t sin_port;
  struct in_addr sin_addr;
  uint8_t sin_zero[8];
};

struct msghdr$bsd {
  void *msg_name;
  uint32_t msg_namelen;
  struct iovec *msg_iov;
  uint32_t msg_iovlen; /* « different type */
  void *msg_control;
  uint64_t msg_controllen;
  uint32_t msg_flags; /* « different type */
};

errno_t MapDosErrorToErrno(uint32_t);

int32_t __accept$sysv(int32_t, void *, uint32_t *) nodiscard hidden;
int32_t __accept4$sysv(int32_t, void *, uint32_t *, int) nodiscard hidden;
int32_t __connect$sysv(int32_t, const void *, uint32_t) hidden;
int32_t __socket$sysv(int32_t, int32_t, int32_t) hidden;
int32_t __getsockname$sysv(int32_t, void *, uint32_t *) hidden;
int32_t __getpeername$sysv(int32_t, void *, uint32_t *) hidden;

int32_t setsockopt$sysv(int32_t, int32_t, int32_t, const void *,
                        uint32_t) hidden;
int32_t accept4$sysv(int32_t, void *, uint32_t *, int) nodiscard hidden;
int32_t accept$sysv(int32_t, void *, uint32_t *) hidden;
int32_t bind$sysv(int32_t, const void *, uint32_t) hidden;
int32_t connect$sysv(int32_t, const void *, uint32_t) hidden;
int32_t getsockopt$sysv(int32_t, int32_t, int32_t, void *, uint32_t *) hidden;
int32_t listen$sysv(int32_t, int32_t) hidden;
int32_t getsockname$sysv(int32_t, void *, uint32_t *) hidden;
int32_t getpeername$sysv(int32_t, void *, uint32_t *) hidden;
int32_t poll$sysv(struct pollfd *, uint64_t, signed) hidden;
int32_t shutdown$sysv(int32_t, int32_t) hidden;
int32_t socket$sysv(int32_t, int32_t, int32_t) hidden;
int64_t readv$sysv(int32_t, const struct iovec *, int32_t) hidden;
int64_t writev$sysv(int32_t, const struct iovec *, int32_t) hidden;
ssize_t recvfrom$sysv(int, void *, size_t, int, void *, uint32_t *) hidden;
ssize_t sendto$sysv(int, const void *, size_t, int, const void *,
                    uint32_t) hidden;
int32_t select$sysv(int32_t, fd_set *, fd_set *, fd_set *,
                    struct timeval *) hidden;
int32_t epoll_create$sysv(int32_t) hidden;
int32_t epoll_ctl$sysv(int32_t, int32_t, int32_t, void *) hidden;
int32_t epoll_wait$sysv(int32_t, void *, int32_t, int32_t) hidden;

int poll$nt(struct pollfd *, uint64_t, int32_t) hidden;
int getsockopt$nt(struct Fd *, int, int, void *, uint32_t *) hidden;
int getsockname$nt(struct Fd *, void *, uint32_t *) hidden;
int getpeername$nt(struct Fd *, void *, uint32_t *) hidden;
int listen$nt(struct Fd *, int) hidden;
int connect$nt(struct Fd *, const void *, uint32_t) hidden;
int bind$nt(struct Fd *, const void *, uint32_t);
int accept$nt(struct Fd *, void *, uint32_t *, int) hidden;
int closesocket$nt(int) hidden;
int socket$nt(int, int, int) hidden;
int select$nt(int, fd_set *, fd_set *, fd_set *, struct timeval *) hidden;

size_t iovec2nt(struct NtIovec[hasatleast 16], const struct iovec *,
                size_t) hidden;
ssize_t sendto$nt(struct Fd *, const struct iovec *, size_t, uint32_t, void *,
                  uint32_t) hidden;
ssize_t recvfrom$nt(struct Fd *, const struct iovec *, size_t, uint32_t, void *,
                    uint32_t *) hidden;

void winsockinit(void) hidden;
int64_t __winsockerr(void) nocallback hidden;
int fixupnewsockfd$sysv(int, int) hidden;
ssize_t WinSendRecv(int64_t, void *, size_t, uint32_t, struct sockaddr *,
                    uint32_t *, bool) hidden;
int64_t winsockblock(int64_t, unsigned, int64_t) hidden;

int close$epoll(int) hidden;

/**
 * Converts sockaddr (Linux/Windows) → sockaddr$bsd (XNU/BSD).
 */
forceinline void sockaddr2bsd(void *saddr) {
  uint8_t *p;
  uint16_t fam;
  if (saddr) {
    p = saddr;
    fam = read16le(p);
    p[0] = sizeof(struct sockaddr_in$bsd);
    p[1] = fam;
  }
}

/**
 * Converts sockaddr_in$bsd (XNU/BSD) → sockaddr (Linux/Windows).
 */
forceinline void sockaddr2linux(void *saddr) {
  uint8_t *p, fam;
  if (saddr) {
    p = saddr;
    fam = p[1];
    WRITE16LE(p, fam);
  }
}

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* !ANSI */
#endif /* COSMOPOLITAN_LIBC_SOCK_INTERNAL_H_ */
