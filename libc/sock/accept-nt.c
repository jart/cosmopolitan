/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2020 Justine Alexandra Roberts Tunney                              │
│                                                                              │
│ Permission to use, copy, modify, and/or distribute this software for         │
│ any purpose with or without fee is hereby granted, provided that the         │
│ above copyright notice and this permission notice appear in all copies.      │
│                                                                              │
│ THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL                │
│ WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED                │
│ WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE             │
│ AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL         │
│ DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR        │
│ PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER               │
│ TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR             │
│ PERFORMANCE OF THIS SOFTWARE.                                                │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "libc/calls/internal.h"
#include "libc/calls/state.internal.h"
#include "libc/macros.internal.h"
#include "libc/mem/mem.h"
#include "libc/nt/winsock.h"
#include "libc/sock/internal.h"
#include "libc/sock/struct/sockaddr.h"
#include "libc/sock/struct/sockaddr.internal.h"
#include "libc/sock/struct/sockaddr6.h"
#include "libc/sock/syscall_fd.internal.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/af.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/sock.h"

union AcceptExAddr {
  union sockaddr_storage_linux addr;
  char buf[sizeof(struct sockaddr_storage) + 16];
};

struct AcceptExBuffer {
  union AcceptExAddr local;
  union AcceptExAddr remote;
};

static void CopyLinuxSockAddr(const union sockaddr_storage_linux *addr,
                              void *out_addr, uint32_t *inout_addrsize) {
  uint32_t insize, outsize;
  if (out_addr && inout_addrsize) {
    if (addr->sa.sa_family == AF_INET) {
      outsize = sizeof(struct sockaddr_in);
    } else if (addr->sa.sa_family == AF_INET6) {
      outsize = sizeof(struct sockaddr_in6);
    } else if (addr->sa.sa_family == AF_UNIX) {
      outsize = sizeof(addr->sun.sun_family) +
                strnlen(addr->sun.sun_path, sizeof(addr->sun.sun_path)) + 1;
    } else {
      outsize = sizeof(union sockaddr_storage_linux);
    }
    insize = *inout_addrsize;
    if (insize) bzero(out_addr, insize);
    outsize = MIN(insize, outsize);
    if (outsize) memcpy(out_addr, addr, outsize);
    *inout_addrsize = outsize;
  }
}

textwindows int sys_accept_nt(struct Fd *fd, void *out_addr,
                              uint32_t *inout_addrsize, int accept4_flags) {
  int64_t handle;
  int rc, client, oflags;
  uint32_t bytes_received;
  uint32_t completion_flags;
  struct AcceptExBuffer buffer;
  struct SockFd *sockfd, *sockfd2;

  // deliver interrupt instead if any are pending
  if (_check_interrupts(true, g_fds.p)) {
    return -1;
  }

  // creates resources for child socket
  // inherit the listener configuration
  sockfd = (struct SockFd *)fd->extra;
  if (!(sockfd2 = malloc(sizeof(struct SockFd)))) {
    return -1;
  }
  memcpy(sockfd2, sockfd, sizeof(*sockfd));
  if ((handle = WSASocket(sockfd2->family, sockfd2->type, sockfd2->protocol,
                          NULL, 0, kNtWsaFlagOverlapped)) == -1) {
    free(sockfd2);
    return __winsockerr();
  }

  // accept network connection
  struct NtOverlapped overlapped = {.hEvent = WSACreateEvent()};
  if (!AcceptEx(fd->handle, handle, &buffer, 0, sizeof(buffer.local),
                sizeof(buffer.remote), &bytes_received, &overlapped)) {
    sockfd = (struct SockFd *)fd->extra;
    if (__wsablock(fd, &overlapped, &completion_flags, true,
                   sockfd->rcvtimeo) == -1) {
      WSACloseEvent(overlapped.hEvent);
      __sys_closesocket_nt(handle);
      free(sockfd2);
      return -1;
    }
  }
  WSACloseEvent(overlapped.hEvent);

  // create file descriptor for new socket
  // don't inherit the file open mode bits
  oflags = 0;
  if (accept4_flags & SOCK_CLOEXEC) oflags |= O_CLOEXEC;
  if (accept4_flags & SOCK_NONBLOCK) oflags |= O_NONBLOCK;
  __fds_lock();
  client = __reservefd_unlocked(-1);
  g_fds.p[client].kind = kFdSocket;
  g_fds.p[client].flags = oflags;
  g_fds.p[client].mode = 0140666;
  g_fds.p[client].handle = handle;
  g_fds.p[client].extra = (uintptr_t)sockfd2;
  __fds_unlock();

  // handoff information to caller;
  CopyLinuxSockAddr(&buffer.remote.addr, out_addr, inout_addrsize);
  return client;
}
