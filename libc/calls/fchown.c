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
#include "libc/calls/calls.h"
#include "libc/calls/syscall-sysv.internal.h"
#include "libc/intrin/strace.h"
#include "libc/sysv/consts/at.h"

/**
 * Changes owner and/or group of file, via open()'d descriptor.
 *
 * @param uid is user id, or -1u to not change
 * @param gid is group id, or -1u to not change
 * @return 0 on success, or -1 w/ errno
 * @see /etc/passwd for user ids
 * @see /etc/group for group ids
 * @raises ENOSYS on Windows
 */
int fchown(int fd, uint32_t uid, uint32_t gid) {
  int rc;
  rc = sys_fchown(fd, uid, gid);
  STRACE("fchown(%d, %d, %d) → %d% m", fd, uid, gid, rc);
  return rc;
}
