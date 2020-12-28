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
