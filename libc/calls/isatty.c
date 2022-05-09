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
#include "libc/calls/calls.h"
#include "libc/calls/internal.h"
#include "libc/calls/strace.internal.h"
#include "libc/calls/struct/winsize.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/sysv/consts/termios.h"

/**
 * Returns true if file descriptor is backed by a terminal device.
 */
bool32 isatty(int fd) {
  int e;
  bool32 res;
  struct winsize ws;
  e = errno;
  if (fd >= 0) {
    if (__isfdkind(fd, kFdZip)) {
      res = false;
    } else if (IsMetal()) {
      res = false;
    } else if (!IsWindows()) {
      res = sys_ioctl(fd, TIOCGWINSZ, &ws) != -1;
    } else {
      res = sys_isatty_nt(fd);
    }
  } else {
    res = false;
  }
  STRACE("isatty(%d) → %hhhd% m", fd, res);
  errno = e;
  return res;
}
