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
#include "libc/sysv/consts/msync.h"
#include "libc/assert.h"
#include "libc/calls/calls.h"
#include "libc/calls/cp.internal.h"
#include "libc/calls/syscall-nt.internal.h"
#include "libc/calls/syscall-sysv.internal.h"
#include "libc/dce.h"
#include "libc/intrin/strace.internal.h"
#include "libc/macros.internal.h"
#include "libc/sysv/errfuns.h"

/**
 * Synchronize memory mapping changes to disk.
 *
 * Without this, there's no guarantee memory is written back to disk.
 * Particularly on RHEL5, OpenBSD, and Windows NT.
 *
 * @param addr needs to be 4096-byte page aligned
 * @param flags needs MS_ASYNC or MS_SYNC and can have MS_INVALIDATE
 * @return 0 on success or -1 w/ errno
 * @raise ECANCELED if thread was cancelled in masked mode
 * @raise EINTR if we needed to block and a signal was delivered instead
 * @raise EINVAL if `MS_SYNC` and `MS_ASYNC` were both specified
 * @raise EINVAL if unknown `flags` were passed
 * @cancelationpoint
 */
int msync(void *addr, size_t size, int flags) {
  int rc;

  if ((flags & ~(MS_SYNC | MS_ASYNC | MS_INVALIDATE)) ||
      (flags & (MS_SYNC | MS_ASYNC)) == (MS_SYNC | MS_ASYNC)) {
    rc = einval();
    goto Finished;
  }

  // According to POSIX, either MS_SYNC or MS_ASYNC must be specified
  // in flags, and indeed failure to include one of these flags will
  // cause msync() to fail on some systems. However, Linux permits a
  // call to msync() that specifies neither of these flags, with
  // semantics that are (currently) equivalent to specifying MS_ASYNC.
  // ──Quoth msync(2) of Linux Programmer's Manual
  int sysflags = flags;
  sysflags = flags;
  if (flags & MS_ASYNC) {
    sysflags = MS_ASYNC;
  } else if (flags & MS_SYNC) {
    sysflags = MS_SYNC;
  } else {
    sysflags = MS_ASYNC;
  }
  if (flags & MS_INVALIDATE) {
    sysflags |= MS_INVALIDATE;
  }

  // FreeBSD's manual says "The flags argument was both MS_ASYNC and
  // MS_INVALIDATE. Only one of these flags is allowed." which makes
  // following the POSIX recommendation somewhat difficult.
  if (IsFreebsd()) {
    if (sysflags == (MS_ASYNC | MS_INVALIDATE)) {
      sysflags = MS_INVALIDATE;
    }
  }

  // FreeBSD specifies MS_SYNC as 0 so we shift the Cosmo constants
  if (IsFreebsd()) {
    sysflags >>= 1;
  }

  BEGIN_CANCELATION_POINT;
  if (!IsWindows()) {
    rc = sys_msync(addr, size, sysflags);
  } else {
    rc = sys_msync_nt(addr, size, sysflags);
  }
  END_CANCELATION_POINT;

Finished:
  STRACE("msync(%p, %'zu, %#x) → %d% m", addr, size, flags, rc);
  return rc;
}
