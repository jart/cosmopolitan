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
#include "libc/calls/flocks.h"
#include "libc/calls/internal.h"
#include "libc/calls/state.internal.h"
#include "libc/calls/syscall-sysv.internal.h"
#include "libc/calls/syscall_support-nt.internal.h"
#include "libc/dce.h"
#include "libc/intrin/kprintf.h"
#include "libc/intrin/strace.h"
#include "libc/intrin/weaken.h"
#include "libc/nt/runtime.h"
#include "libc/runtime/zipos.internal.h"
#include "libc/sock/syscall_fd.internal.h"
#include "libc/sysv/errfuns.h"
#include "libc/sysv/pib.h"

static int close_impl(int fd) {

  // handle obvious case
  if (fd < 0)
    return ebadf();

  // give kprintf() the opportunity to dup() stderr
  if (fd == 2 && !__vforked && _weaken(kloghandle))
    _weaken(kloghandle)();

  // unix isn't required to track fds
  if (fd >= __get_pib()->fds.n) {
    if (IsWindows() || IsMetal())
      return ebadf();
    return sys_close(fd);
  }

  // if we're vforked on unix then we can't even modify f->kind
  if (__vforked && !(IsWindows() || IsMetal()))
    return sys_close(fd);

  // atomically close file descriptor table entry
  int rc = 0;
  struct Fd *f = __get_pib()->fds.p + fd;
  switch (atomic_exchange(&f->kind, kFdReserved)) {
    case kFdEmpty:
      if (IsWindows() || IsMetal()) {
        rc = ebadf();
      } else {
        rc = sys_close(fd);
      }
      break;
    case kFdReserved:
      return ebadf();
    case kFdZip:
      if (_weaken(__zipos_close))
        rc = _weaken(__zipos_close)(fd);
      break;
#if SupportsMetal()
    case kFdSerial:
      break;
#endif
#if SupportsWindows()
    case kFdDevRandom:
      break;
    case kFdFile:
      if (!__vforked)
        if (_weaken(__flocks_close))
          _weaken(__flocks_close)(f);
      if (!__vforked || f->was_created_during_vfork) {
        if (f->cursor)
          __cursor_unref(f->cursor);
        if (!CloseHandle(f->handle))
          rc = __winerr();
      }
      break;
    case kFdDevNull:
    case kFdConsole:
      if (!__vforked || f->was_created_during_vfork)
        if (!CloseHandle(f->handle))
          rc = __winerr();
      break;
    case kFdSocket:
      if (!__vforked || f->was_created_during_vfork)
        if (_weaken(sys_closesocket_nt))
          rc = _weaken(sys_closesocket_nt)(f);
      break;
#endif
    default:
      rc = ebadf();
      break;
  }

  // wipe fd and update lowest file descriptor number
  // we do this even if there was an underlying os error
  __releasefd(fd);

  return rc;
}

/**
 * Closes file descriptor.
 *
 * This function releases resources returned by functions such as:
 *
 * - openat()
 * - socket()
 * - accept()
 * - pipe()
 * - socketpair()
 * - landlock_create_ruleset()
 *
 * This function should never be reattempted if an error is returned;
 * however, that doesn't mean the error should be ignored. This goes
 * against the conventional wisdom of looping on `EINTR`.
 *
 * @return 0 on success, or -1 w/ errno
 * @raise EINTR if signal was delivered; do *not* retry
 * @raise EBADF if `fd` is negative or not open
 * @raise EIO if a low-level i/o error occurred
 * @asyncsignalsafe
 * @vforksafe
 */
int close(int fd) {
  int rc = close_impl(fd);
  STRACE("close(%d) → %d% m", fd, rc);
  return rc;
}
