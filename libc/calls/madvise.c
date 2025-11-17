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
#include "libc/calls/syscall-nt.internal.h"
#include "libc/calls/syscall-sysv.internal.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/runtime/runtime.h"
#include "libc/sysv/consts/madv.h"
#include "libc/sysv/errfuns.h"

/**
 * Advises kernel about memory intentions.
 *
 * @return 0 on success, or -1 w/ errno
 * @raise EINVAL if `advice` isn't valid
 * @raise EINVAL if `len` is negative
 * @raise EINVAL if `addr` isn't multiple of `sysconf(_SC_PAGESIZE)`
 * @raise ENOMEM if interval overlaps unmapped pages (on some OSes)
 */
int madvise(void *addr, size_t len, int advice) {

  switch (advice) {
    case MADV_NORMAL:
    case MADV_RANDOM:
    case MADV_WILLNEED:
    case MADV_SEQUENTIAL:
    case MADV_DONTNEED:
      break;
    default:
      return einval();
  }

  if (!len)
    return 0;

  if ((int64_t)len < 0)
    return einval();

  if ((uintptr_t)addr & (__pagesize - 1))
    return einval();

  int rc = 0;
  if (!IsWindows()) {
    rc = sys_madvise(addr, len, advice);
  } else {
    errno_t err = sys_posix_madvise_nt(addr, len, advice);
    if (err) {
      errno = err;
      rc = -1;
    }
  }

  return rc;
}
