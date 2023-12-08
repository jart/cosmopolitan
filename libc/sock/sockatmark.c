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
#include "libc/calls/internal.h"
#include "libc/calls/struct/fd.internal.h"
#include "libc/calls/syscall-sysv.internal.h"
#include "libc/dce.h"
#include "libc/intrin/strace.internal.h"
#include "libc/nt/winsock.h"
#include "libc/sock/internal.h"
#include "libc/sock/sock.h"
#include "libc/sysv/errfuns.h"

static textwindows int sockatmark_nt(int fd, unsigned long magnum) {
  bool32 res;
  int64_t hand;
  uint32_t bytes;
  if (fd >= g_fds.n) return ebadf();
  if (g_fds.p[fd].kind != kFdSocket) return einval();
  hand = g_fds.p[fd].handle;
  if (WSAIoctl(hand, magnum, 0, 0, &res, sizeof(res), &bytes, 0, 0) == -1) {
    return __winsockerr();
  }
  return !res;
}

/**
 * Returns true if out of band data is available on socket for reading.
 *
 * @return 1 if OOB'd, 0 if not, or -1 w/ errno
 * @raise EBADF if `fd` isn't an open file descriptor
 * @raise EINVAL if `fd` isn't an appropriate file descriptor
 */
int sockatmark(int fd) {
  int rc;
  unsigned long magnum;
  if (IsLinux()) {
    magnum = 0x8905;      // SIOCATMARK (Linux only)
  } else {                //
    magnum = 0x40047307;  // SIOCATMARK (BSD, Windows)
  }
  if (fd < g_fds.n && g_fds.p[fd].kind == kFdZip) {
    rc = enotsock();
  } else if (!IsWindows()) {
    if (sys_ioctl(fd, magnum, &rc) == -1) {
      rc = -1;
    }
  } else {
    rc = sockatmark_nt(fd, magnum);
  }
  STRACE("sockatmark(%d) → %d% m", fd, rc);
  return rc;
}
