/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
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
#include "libc/calls/internal.h"
#include "libc/calls/struct/fd.internal.h"
#include "libc/calls/syscall_support-sysv.internal.h"
#include "libc/calls/termios.h"
#include "libc/errno.h"
#include "libc/intrin/strace.internal.h"
#include "libc/sysv/errfuns.h"

static char g_ptsname[16];

/**
 * Gets name subordinate pseudoteletypewriter.
 *
 * @return static string path on success, or NULL w/ errno
 */
char *ptsname(int fd) {
  char *res;
  if (fd < g_fds.n && g_fds.p[fd].kind == kFdZip) {
    enotty();
    res = 0;
  } else if (!_ptsname(fd, g_ptsname, sizeof(g_ptsname))) {
    res = g_ptsname;
  } else {
    res = 0;
  }
  STRACE("ptsname(%d) → %#s% m", fd, res);
  return res;
}
