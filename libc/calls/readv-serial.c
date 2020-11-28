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
#include "ape/lib/pc.h"
#include "libc/calls/internal.h"
#include "libc/nexgen32e/uart.internal.h"

static bool IsDataAvailable(struct Fd *fd) {
  return inb(fd->handle + UART_LSR) & UART_TTYDA;
}

static int GetFirstIov(struct iovec *iov, int iovlen) {
  int i;
  for (i = 0; i < iovlen; ++i) {
    if (iov[i].iov_len) {
      return i;
    }
  }
  return -1;
}

ssize_t readv$serial(struct Fd *fd, const struct iovec *iov, int iovlen) {
  size_t i, j, got = 0;
  if ((i = GetFirstIov(iov, iovlen)) != -1) {
    while (!IsDataAvailable(fd)) asm("pause");
    i = 0;
    j = 0;
    do {
      ++got;
      ((char *)iov[i].iov_base)[j] = inb(fd->handle);
      if (++j == iov[i].iov_len) {
        j = 0;
        if (++i == iovlen) {
          break;
        }
      }
    } while (IsDataAvailable(fd));
  }
  return got;
}
