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
#include "libc/calls/blockcancel.internal.h"
#include "libc/calls/struct/sigset.internal.h"
#include "libc/calls/syscall_support-sysv.internal.h"
#include "libc/dce.h"
#include "libc/intrin/asan.internal.h"
#include "libc/intrin/strace.internal.h"
#include "libc/stdio/rand.h"
#include "libc/sysv/errfuns.h"

int sys_getentropy(void *, size_t) asm("sys_getrandom");

/**
 * Returns random seeding bytes, the XNU/OpenBSD way.
 *
 * @return 0 on success, or -1 w/ errno
 * @raise EFAULT if the `n` bytes at `p` aren't valid memory
 * @raise EIO is returned if more than 256 bytes are requested
 * @see getrandom()
 */
int getentropy(void *p, size_t n) {
  int rc;
  if (n > 256) {
    rc = eio();
  } else if ((!p && n) || (IsAsan() && !__asan_is_valid(p, n))) {
    rc = efault();
  } else if (IsXnu() || IsOpenbsd()) {
    if (sys_getentropy(p, n)) notpossible;
    rc = 0;
  } else {
    BLOCK_SIGNALS;
    BLOCK_CANCELATION;
    if (__getrandom(p, n, 0) != n) notpossible;
    ALLOW_CANCELATION;
    ALLOW_SIGNALS;
    rc = 0;
  }
  STRACE("getentropy(%p, %'zu) → %'ld% m", p, n, rc);
  return rc;
}
