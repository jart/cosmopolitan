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
#include "libc/nt/winsock.h"
#include "libc/sock/internal.h"
#include "libc/sock/yoink.inc"
#include "libc/sysv/consts/fio.h"
#include "libc/sysv/consts/sock.h"

#define CLOEXEC  0x00080000
#define NONBLOCK 0x00000800

textwindows int socket$nt(int family, int type, int protocol) {
  int fd;
  uint32_t yes;
  if ((fd = __getemptyfd()) == -1) return -1;
  if ((g_fds.p[fd].handle = WSASocket(family, type & ~(CLOEXEC | NONBLOCK),
                                      protocol, NULL, 0, (type & CLOEXEC))) !=
      -1) {
    if (type & NONBLOCK) {
      yes = 1;
      if (__ioctlsocket$nt(g_fds.p[fd].handle, FIONBIO, &yes) == -1) {
        __closesocket$nt(g_fds.p[fd].handle);
        return __winsockerr();
      }
    }
    g_fds.p[fd].kind = kFdSocket;
    g_fds.p[fd].flags = type & (CLOEXEC | NONBLOCK);
    return fd;
  } else {
    return __winsockerr();
  }
}
