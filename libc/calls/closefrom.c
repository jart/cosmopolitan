/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2022 Justine Alexandra Roberts Tunney                              │
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
#include "libc/calls/blockcancel.internal.h"
#include "libc/calls/calls.h"
#include "libc/calls/struct/rlimit.h"
#include "libc/calls/syscall-sysv.internal.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/intrin/strace.h"
#include "libc/sysv/pib.h"

/**
 * Closes extra file descriptors.
 *
 * This system call is always successful.
 */
void closefrom(int minfd) {
  BLOCK_CANCELATION;

  // fix arg
  if (minfd < 0)
    minfd = 0;

  // try syscall
  int rc;
  int olderr = errno;
  if (IsFreebsd() || IsOpenbsd()) {
    rc = sys_closefrom(minfd);
  } else if (IsLinux()) {
    rc = sys_close_range(minfd, 0xffffffffu, 0);
  } else if (IsNetbsd()) {
    rc = __sys_fcntl(minfd, 10 /*F_CLOSEM*/, minfd);
  } else {
    rc = -1;
  }

  // try polyfill
  if (rc == -1) {
    long maxfd = 1024;
    if (IsWindows() || IsMetal()) {
      maxfd = __get_pib()->fds.n;
    } else {
      struct rlimit rl;
      if (!getrlimit(RLIMIT_NOFILE, &rl))
        if (rl.rlim_cur <= 0x7fffffffu)
          maxfd = rl.rlim_cur;
    }
    for (long fd = minfd; fd < maxfd; ++fd)
      close(fd);
    errno = olderr;
  }

  ALLOW_CANCELATION;
  STRACE("closefrom(%d)", minfd);
}
