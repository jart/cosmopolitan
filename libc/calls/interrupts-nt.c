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
#include "libc/assert.h"
#include "libc/calls/calls.h"
#include "libc/calls/internal.h"
#include "libc/calls/sig.internal.h"
#include "libc/calls/state.internal.h"
#include "libc/calls/struct/fd.internal.h"
#include "libc/calls/struct/sigaction.h"
#include "libc/calls/struct/sigaction.internal.h"
#include "libc/calls/syscall_support-nt.internal.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/intrin/strace.internal.h"
#include "libc/intrin/weaken.h"
#include "libc/sysv/errfuns.h"
#include "libc/thread/thread.h"
#include "libc/thread/tls.h"

textwindows int _check_interrupts(int sigops) {
  int e, rc, flags = 0;
  e = errno;
  if (_weaken(pthread_testcancel_np) &&
      (rc = _weaken(pthread_testcancel_np)())) {
    errno = rc;
    return -1;
  }
  if (__tls_enabled) {
    flags = __get_tls()->tib_flags;
    __get_tls()->tib_flags |= TIB_FLAG_TIME_CRITICAL;
  }
  if (_weaken(_check_sigalrm)) {
    _weaken(_check_sigalrm)();
  }
  if (__tls_enabled) {
    __get_tls()->tib_flags = flags;
  }
  if (_weaken(_check_sigwinch)) {
    _weaken(_check_sigwinch)();
  }
  if (!__tls_enabled || !(__get_tls()->tib_flags & TIB_FLAG_TIME_CRITICAL)) {
    if (!(sigops & kSigOpNochld) && _weaken(_check_sigchld)) {
      _weaken(_check_sigchld)();
    }
  }
  if (_weaken(__sig_check) && _weaken(__sig_check)(sigops)) {
    return eintr();
  }
  errno = e;
  return 0;
}
