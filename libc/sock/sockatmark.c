/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
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
#include "libc/intrin/strace.internal.h"
#include "libc/calls/syscall-sysv.internal.h"
#include "libc/dce.h"
#include "libc/nt/winsock.h"
#include "libc/sock/sock.h"
#include "libc/sysv/consts/sio.h"

static textwindows int sockatmark_nt(int64_t fd) {
  bool32 res;
  uint32_t bytes;
  if (WSAIoctl(fd, SIOCATMARK, 0, 0, &res, sizeof(res), &bytes, 0, 0) != -1) {
    return !res;
  } else {
    return -1;
  }
}

/**
 * Returns true if out of band data is available on socket for reading.
 *
 * @return 1 if OOB'd, 0 if not, or -1 w/ errno
 */
int sockatmark(int fd) {
  int rc;
  if (!IsWindows()) {
    if (sys_ioctl(fd, SIOCATMARK, &rc) == -1) {
      rc = -1;
    }
  } else {
    rc = sockatmark_nt(fd);
  }
  STRACE("sockatmark(%d) → %d% m", fd, rc);
  return rc;
}
