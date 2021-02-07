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
#include "libc/assert.h"
#include "libc/calls/internal.h"
#include "libc/calls/struct/iovec.h"
#include "libc/limits.h"
#include "libc/nt/files.h"
#include "libc/nt/runtime.h"
#include "libc/nt/struct/overlapped.h"
#include "libc/sysv/errfuns.h"

static textwindows size_t SumIovecLen(const struct iovec *v, size_t n) {
  size_t i, sum;
  for (sum = i = 0; i < n; ++i) {
    sum += v[i].iov_len;
  }
  return sum;
}

textwindows ssize_t sys_write_nt(struct Fd *fd, const struct iovec *iov,
                                 size_t iovlen, ssize_t opt_offset) {
  size_t i, total;
  uint32_t size, wrote;
  struct NtOverlapped overlap;
  while (iovlen && !iov[0].iov_len) iov++, iovlen--;
  if (iovlen) {
    for (total = i = 0; i < iovlen; ++i) {
      if (!iov[i].iov_len) continue;
      size = clampio(iov[0].iov_len);
      if (WriteFile(fd->handle, iov[i].iov_base, size, &wrote,
                    offset2overlap(opt_offset, &overlap))) {
        total += wrote;
        if (opt_offset != -1) opt_offset += wrote;
        if (wrote < iov[i].iov_len) break;
      } else {
        return __winerr();
      }
    }
    if (!total) assert(!SumIovecLen(iov, iovlen));
    return total;
  } else {
    if (WriteFile(fd->handle, NULL, 0, &wrote,
                  offset2overlap(opt_offset, &overlap))) {
      return 0;
    } else {
      return __winerr();
    }
  }
}
