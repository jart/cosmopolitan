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
#include "libc/calls/sig.internal.h"
#include "libc/calls/struct/fd.internal.h"
#include "libc/calls/struct/iovec.h"
#include "libc/calls/struct/sigset.h"
#include "libc/calls/struct/sigset.internal.h"
#include "libc/calls/syscall-nt.internal.h"
#include "libc/calls/syscall_support-nt.internal.h"
#include "libc/errno.h"
#include "libc/intrin/atomic.h"
#include "libc/intrin/nomultics.internal.h"
#include "libc/intrin/weaken.h"
#include "libc/nt/console.h"
#include "libc/nt/enum/consolemodeflags.h"
#include "libc/nt/errors.h"
#include "libc/nt/runtime.h"
#include "libc/sysv/consts/sicode.h"
#include "libc/sysv/consts/sig.h"
#include "libc/sysv/errfuns.h"
#include "libc/thread/tls.h"
#ifdef __x86_64__

static inline void RaiseSignal(int sig) {
  if (_weaken(__sig_raise)) {
    _weaken(__sig_raise)(sig, SI_KERNEL);
  } else {
    TerminateThisProcess(sig);
  }
}

static textwindows ssize_t sys_write_nt_impl(int fd, void *data, size_t size,
                                             ssize_t offset,
                                             uint64_t waitmask) {
  struct Fd *f = g_fds.p + fd;
  bool isconsole = f->kind == kFdConsole;

  // determine win32 handle for writing
  int64_t handle = f->handle;
  if (isconsole && _weaken(GetConsoleOutputHandle)) {
    handle = _weaken(GetConsoleOutputHandle)();
  }

  // intercept ansi tty configuration sequences
  if (isconsole && _weaken(GetConsoleOutputHandle)) {
    _weaken(InterceptTerminalCommands)(data, size);
  }

  // perform heavy lifting
  ssize_t rc;
  rc = sys_readwrite_nt(fd, data, size, offset, handle, waitmask,
                        (void *)WriteFile);
  if (rc != -2) return rc;

  // mops up win32 errors
  switch (GetLastError()) {
    // case kNtErrorInvalidHandle:
    //   return ebadf(); /* handled by consts.sh */
    // case kNtErrorNotEnoughQuota:
    //   return edquot(); /* handled by consts.sh */
    case kNtErrorBrokenPipe:  // broken pipe
    case kNtErrorNoData:      // closing named pipe
      RaiseSignal(SIGPIPE);
      return epipe();
    case kNtErrorAccessDenied:  // write doesn't return EACCESS
      return ebadf();
    default:
      return __winerr();
  }
}

static textwindows ssize_t sys_write_nt2(int fd, const struct iovec *iov,
                                         size_t iovlen, ssize_t opt_offset,
                                         uint64_t waitmask) {
  ssize_t rc;
  size_t i, total;
  if (opt_offset < -1) return einval();
  while (iovlen && !iov[0].iov_len) iov++, iovlen--;
  if (iovlen) {
    for (total = i = 0; i < iovlen; ++i) {
      if (!iov[i].iov_len) continue;
      rc = sys_write_nt_impl(fd, iov[i].iov_base, iov[i].iov_len, opt_offset,
                             waitmask);
      if (rc == -1) {
        if (total && errno != ECANCELED) {
          return total;
        } else {
          return -1;
        }
      }
      total += rc;
      if (opt_offset != -1) opt_offset += rc;
      if (rc < iov[i].iov_len) break;
    }
    return total;
  } else {
    return sys_write_nt_impl(fd, NULL, 0, opt_offset, waitmask);
  }
}

textwindows ssize_t sys_write_nt(int fd, const struct iovec *iov, size_t iovlen,
                                 ssize_t opt_offset) {
  ssize_t rc;
  sigset_t m = __sig_block();
  rc = sys_write_nt2(fd, iov, iovlen, opt_offset, m);
  __sig_unblock(m);
  return rc;
}

#endif /* __x86_64__ */
