/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
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
#include "libc/nt/winsock.h"
#include "libc/sock/internal.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/sock.h"
#ifdef __x86_64__
#include "libc/sock/yoink.inc"

// ioctl(SIOCGIFCONFIG) for Windows need to access the following
// functions through weak reference. This ensure those symbols are not
// stripped during final link.
__static_yoink("GetAdaptersAddresses");
__static_yoink("tprecode16to8");

textwindows int sys_socket_nt(int family, int type, int protocol) {
  int64_t h;
  int fd, oflags, truetype;
  fd = __reservefd(-1);
  if (fd == -1) return -1;
  truetype = type & ~(SOCK_CLOEXEC | SOCK_NONBLOCK);
  if ((h = WSASocket(family, truetype, protocol, NULL, 0,
                     kNtWsaFlagOverlapped)) != -1) {
    oflags = O_RDWR;
    if (type & SOCK_CLOEXEC) oflags |= O_CLOEXEC;
    if (type & SOCK_NONBLOCK) oflags |= O_NONBLOCK;
    g_fds.p[fd].family = family;
    g_fds.p[fd].type = truetype;
    g_fds.p[fd].protocol = protocol;
    g_fds.p[fd].kind = kFdSocket;
    g_fds.p[fd].flags = oflags;
    g_fds.p[fd].mode = 0140666;
    g_fds.p[fd].handle = h;
    return fd;
  } else {
    __releasefd(fd);
    return __winsockerr();
  }
}

#endif /* __x86_64__ */
