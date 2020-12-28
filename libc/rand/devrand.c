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
#include "libc/bits/bits.h"
#include "libc/dce.h"
#include "libc/rand/rand.h"
#include "libc/str/str.h"
#include "libc/calls/internal.h"
#include "libc/calls/calls.h"
#include "libc/sysv/errfuns.h"
#include "libc/sysv/consts/at.h"
#include "libc/sysv/consts/nr.h"
#include "libc/sysv/consts/o.h"

/**
 * Reads random bytes from system pseudo random number api.
 * @return 0 on success or -1 w/ errno
 */
int devrand(void *buf, size_t size) {
  int fd;
  size_t got;
  ssize_t rc;
  unsigned char *p, *pe;
  fd = -1;
  if (IsWindows()) return enosys();
  if ((fd = openat$sysv(AT_FDCWD, "/dev/urandom", O_RDONLY, 0)) == -1) {
    return -1;
  }
  p = buf;
  pe = p + size;
  while (p < pe) {
    if ((rc = read$sysv(fd, p, pe - p)) == -1) break;
    if (!(got = (size_t)rc)) break;
    p += got;
  }
  close$sysv(fd);
  return p == pe ? 0 : -1;
}
