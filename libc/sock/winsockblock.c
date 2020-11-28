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
#include "libc/errno.h"
#include "libc/nt/winsock.h"
#include "libc/sock/internal.h"
#include "libc/sock/sock.h"
#include "libc/str/str.h"

textwindows int64_t winsockblock(int64_t fh, unsigned eventbit, int64_t rc) {
  int64_t eh;
  struct NtWsaNetworkEvents ev;
  if (rc != -1) return rc;
  if (WSAGetLastError() != EWOULDBLOCK) return __winsockerr();
  eh = WSACreateEvent();
  memset(&ev, 0, sizeof(ev));
  if (WSAEventSelect(fh, eh, 1u << eventbit) != -1 &&
      WSAEnumNetworkEvents(fh, eh, &ev) != -1) {
    if (!ev.iErrorCode[eventbit]) {
      rc = 0;
    } else {
      errno = ev.iErrorCode[eventbit];
    }
  } else {
    __winsockerr();
  }
  WSACloseEvent(eh);
  return rc;
}
