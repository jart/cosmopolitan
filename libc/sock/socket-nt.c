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
#include "libc/mem/mem.h"
#include "libc/nt/enum/fileflagandattributes.h"
#include "libc/nt/iphlpapi.h"
#include "libc/nt/winsock.h"
#include "libc/sock/internal.h"
#include "libc/sock/yoink.inc"
#include "libc/sysv/consts/fio.h"
#include "libc/sysv/consts/ipproto.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/so.h"
#include "libc/sysv/consts/sock.h"
#include "libc/sysv/consts/sol.h"

/*
 * ioctl(SIOCGIFCONFIG) for Windows need to access the following
 * functions through weak reference. This ensure those symbols are not
 * stripped during final link.
 */
STATIC_YOINK("GetAdaptersAddresses");
STATIC_YOINK("tprecode16to8");
STATIC_YOINK("_dupsockfd");

textwindows int sys_socket_nt(int family, int type, int protocol) {
  int64_t h;
  struct SockFd *sockfd;
  int fd, oflags, truetype;
  fd = __reservefd(-1);
  if (fd == -1) return -1;
  truetype = type & ~(SOCK_CLOEXEC | SOCK_NONBLOCK);
  if ((h = WSASocket(family, truetype, protocol, NULL, 0,
                     kNtWsaFlagOverlapped)) != -1) {
    oflags = 0;
    if (type & SOCK_CLOEXEC) oflags |= O_CLOEXEC;
    if (type & SOCK_NONBLOCK) oflags |= O_NONBLOCK;
    if (type & SOCK_NONBLOCK) {
      if (__sys_ioctlsocket_nt(h, FIONBIO, (uint32_t[1]){1}) == -1) {
        __sys_closesocket_nt(h);
        __releasefd(fd);
        return __winsockerr();
      }
    }
    sockfd = calloc(1, sizeof(struct SockFd));
    sockfd->family = family;
    sockfd->type = truetype;
    sockfd->protocol = protocol;
    __fds_lock();
    g_fds.p[fd].kind = kFdSocket;
    g_fds.p[fd].flags = oflags;
    g_fds.p[fd].mode = 0140666;
    g_fds.p[fd].handle = h;
    g_fds.p[fd].extra = (uintptr_t)sockfd;
    __fds_unlock();
    return fd;
  } else {
    __releasefd(fd);
    return __winsockerr();
  }
}
