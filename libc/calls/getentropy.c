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
#include "libc/calls/syscall-sysv.internal.h"
#include "libc/calls/syscall_support-sysv.internal.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/intrin/kprintf.h"
#include "libc/intrin/strace.h"
#include "libc/nt/runtime.h"
#include "libc/runtime/syslib.internal.h"
#include "libc/stdio/rand.h"
#include "libc/str/str.h"
#include "libc/sysv/errfuns.h"

int sys_getentropy(void *, size_t) asm("sys_getrandom");

/**
 * Returns random bytes from operating system.
 *
 * This function shall fill a reasonably small buffer with strong random
 * data that's provided by the host operating system. If you don't cause
 * the two API usage usage errors below, then it's believed this can not
 * fail (and the process will die if that somehow happens).
 *
 * Since this requires often multiple system calls, users are encouraged
 * to consider arc4random_buf() instead, for all their randomness needs,
 * since it uses a strong crytpographic ChaCha userspace generator which
 * is seeded when appropriate by this getentropy() implementation.
 *
 * @return 0 on success, or -1 w/ errno
 * @raise EFAULT if `n` is nonzero and `p` isn't valid memory
 * @raise EIO if more than 256 bytes are requested
 * @raise EIO if some other rare error occurred
 * @see getrandom()
 */
int getentropy(void *p, size_t n) {
  int rc;
  if (!n) {
    rc = 0;
  } else if (n > 256) {
    rc = eio();
  } else if (kisdangerous(p)) {
    rc = efault();
  } else {
    if (IsXnuSilicon()) {
      rc = _sysret(__syslib->__getentropy(p, n));
    } else if (IsXnu() || IsOpenbsd()) {
      rc = sys_getentropy(p, n);
      if (rc > 0) {
        // TODO(jart): check kernel code to see if needed
        errno = rc;
        rc = -1;
      }
    } else if (IsWindows()) {
      ProcessPrng(p, n);  // never fails
      rc = 0;
    } else {
      BLOCK_CANCELATION;
      size_t i;
      ssize_t got;
      char buf[256];
      int e = errno;
      for (rc = i = 0; i < n; i += got) {
        got = __getrandom(buf + i, n - i, 0);
        if (got < 0) {
          if (errno == EINTR) {
            errno = e;
            continue;
          }
          rc = -1;
          break;
        } else if (!got) {
          rc = eio();
          break;
        }
      }
      if (!rc)
        memcpy(p, buf, n);
      ALLOW_CANCELATION;
    }
    rc = 0;
  }
  STRACE("getentropy(%p, %'zu) → %'ld% m", p, n, rc);
  return rc;
}
