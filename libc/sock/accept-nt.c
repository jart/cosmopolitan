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
#include "libc/assert.h"
#include "libc/calls/internal.h"
#include "libc/nt/files.h"
#include "libc/nt/winsock.h"
#include "libc/sock/internal.h"
#include "libc/sysv/consts/fio.h"
#include "libc/sysv/consts/sock.h"
#include "libc/sysv/errfuns.h"

textwindows int accept$nt(struct Fd *fd, void *addr, uint32_t *addrsize,
                          int flags) {
  int client;
  uint32_t yes;
  assert(fd->kind == kFdSocket);
  if ((client = createfd()) == -1) return -1;
  if ((g_fds.p[client].handle = WSAAccept(fd->handle, addr, (int32_t *)addrsize,
                                          NULL, NULL)) != -1) {
    if (flags & SOCK_NONBLOCK) {
      yes = 1;
      if (__ioctlsocket$nt(g_fds.p[client].handle, FIONBIO, &yes) == -1) {
        __closesocket$nt(g_fds.p[client].handle);
        return winsockerr();
      }
    }
    g_fds.p[client].kind = kFdSocket;
    g_fds.p[client].flags = flags;
    return client;
  } else {
    return winsockerr();
  }
}
