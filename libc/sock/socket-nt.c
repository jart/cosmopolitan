/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2020 Justine Alexandra Roberts Tunney                              │
│                                                                              │
│ This program is free software; you can redistribute it and/or modify         │
│ it under the terms of the GNU General Public License as published by         │
│ the Free Software Foundation; version 2 of the License.                      │
│                                                                              │
│ This program is distributed in the hope that it will be useful, but          │
│ WITHOUT ANY WARRANTY; without even the implied warranty of                   │
│ MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU             │
│ General Public License for more details.                                     │
│                                                                              │
│ You should have received a copy of the GNU General Public License            │
│ along with this program; if not, write to the Free Software                  │
│ Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA                │
│ 02110-1301 USA                                                               │
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
