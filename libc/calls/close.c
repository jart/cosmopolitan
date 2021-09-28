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
#include "libc/bits/weaken.h"
#include "libc/calls/calls.h"
#include "libc/calls/internal.h"
#include "libc/calls/sysdebug.internal.h"
#include "libc/macros.internal.h"
#include "libc/sock/internal.h"
#include "libc/sysv/errfuns.h"
#include "libc/zipos/zipos.internal.h"

/**
 * Closes file descriptor.
 *
 * This function may be used for file descriptors returned by socket,
 * accept, epoll_create, and zipos file descriptors too.
 *
 * @return 0 on success, or -1 w/ errno
 * @asyncsignalsafe
 * @vforksafe
 */
int close(int fd) {
  int rc;
  if (fd == -1) return 0;
  if (fd < 0) return einval();
  if (fd < g_fds.n && g_fds.p[fd].kind == kFdZip) {
    rc = weaken(__zipos_close)(fd);
  } else {
    if (!IsWindows() && !IsMetal()) {
      rc = sys_close(fd);
    } else if (IsMetal()) {
      rc = 0;
    } else {
      if (fd < g_fds.n && g_fds.p[fd].kind == kFdEpoll) {
        rc = weaken(sys_close_epoll_nt)(fd);
      } else if (fd < g_fds.n && g_fds.p[fd].kind == kFdSocket) {
        rc = weaken(sys_closesocket_nt)(g_fds.p + fd);
      } else if (fd < g_fds.n && (g_fds.p[fd].kind == kFdFile ||
                                  g_fds.p[fd].kind == kFdConsole ||
                                  g_fds.p[fd].kind == kFdProcess)) {
        rc = sys_close_nt(g_fds.p + fd);
      } else {
        rc = ebadf();
      }
    }
  }
  __releasefd(fd);
  SYSDEBUG("close(%d) -> %d", fd, rc);
  return rc;
}
