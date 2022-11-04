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
#include "libc/calls/sig.internal.h"
#include "libc/calls/state.internal.h"
#include "libc/mem/mem.h"
#include "libc/nt/winsock.h"
#include "libc/sock/internal.h"
#include "libc/sock/syscall_fd.internal.h"
#include "libc/sysv/consts/fio.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/poll.h"
#include "libc/sysv/consts/sock.h"
#include "libc/sysv/errfuns.h"

textwindows int sys_accept_nt(struct Fd *fd, void *addr, uint32_t *addrsize,
                              int flags) {
  int64_t h;
  int rc, client, oflags;
  struct SockFd *sockfd, *sockfd2;
  sockfd = (struct SockFd *)fd->extra;
  if (_check_interrupts(true, g_fds.p)) return -1;
  for (;;) {
    if (!WSAPoll(&(struct sys_pollfd_nt){fd->handle, POLLIN}, 1,
                 __SIG_POLLING_INTERVAL_MS)) {
      if (_check_interrupts(true, g_fds.p)) {
        return eintr();
      }
      continue;
    }
    if ((h = WSAAccept(fd->handle, addr, (int32_t *)addrsize, 0, 0)) != -1) {
      oflags = 0;
      if (flags & SOCK_CLOEXEC) oflags |= O_CLOEXEC;
      if (flags & SOCK_NONBLOCK) oflags |= O_NONBLOCK;
      if ((!(flags & SOCK_NONBLOCK) ||
           __sys_ioctlsocket_nt(h, FIONBIO, (uint32_t[]){1}) != -1) &&
          (sockfd2 = calloc(1, sizeof(struct SockFd)))) {
        __fds_lock();
        if ((client = __reservefd_unlocked(-1)) != -1) {
          sockfd2->family = sockfd->family;
          sockfd2->type = sockfd->type;
          sockfd2->protocol = sockfd->protocol;
          g_fds.p[client].kind = kFdSocket;
          g_fds.p[client].flags = oflags;
          g_fds.p[client].mode = 0140666;
          g_fds.p[client].handle = h;
          g_fds.p[client].extra = (uintptr_t)sockfd2;
          __fds_unlock();
          return client;
        }
        __fds_unlock();
        free(sockfd2);
      }
      __sys_closesocket_nt(h);
    }
    return __winsockerr();
  }
}
