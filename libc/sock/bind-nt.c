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
#include "libc/nt/winsock.h"
#include "libc/sock/internal.h"
#include "libc/sock/yoink.inc"
#include "libc/sysv/errfuns.h"

/**
 * Assigns local address and port number to socket.
 *
 * @param fd is the file descriptor returned by socket()
 * @param addr is usually the binary-encoded ip:port on which to listen
 * @param addrsize is the byte-length of addr's true polymorphic form
 * @return socket file descriptor or -1 w/ errno
 * @error ENETDOWN, EPFNOSUPPORT, etc.
 * @asyncsignalsafe
 */
textwindows int bind$nt(struct Fd *fd, const void *addr, uint32_t addrsize) {
  assert(fd->kind == kFdSocket);
  if (__bind$nt(fd->handle, addr, addrsize) != -1) {
    return 0;
  } else {
    return __winsockerr();
  }
}
