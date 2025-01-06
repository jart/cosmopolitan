/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2023 Justine Alexandra Roberts Tunney                              │
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
#include "libc/calls/sig.internal.h"
#include "libc/calls/struct/sigset.internal.h"
#include "libc/dce.h"
#include "libc/intrin/atomic.h"
#include "libc/intrin/weaken.h"
#include "libc/sysv/consts/sig.h"
#include "libc/thread/tls.h"

// since there's so many c library interfaces and system call wrappers
// that always need to block signals we avoid the distraction of their
// ftrace and strace output being muddied with sigprocmask lines. it's
// usually better that sigprocmask only strace the user is calling it.
// plus, since we have a very specific use case, this code goes faster

sigset_t __sig_block(void) {
  if (IsWindows() || IsMetal()) {
    if (__tls_enabled)
      return atomic_exchange_explicit(&__get_tls()->tib_sigmask, -1,
                                      memory_order_acquire);
    else
      return 0;
  } else {
    sigset_t res, neu = -1;
    sys_sigprocmask(SIG_SETMASK, &neu, &res);
    return res;
  }
}

void __sig_unblock(sigset_t m) {
  if (IsWindows() || IsMetal()) {
    if (__tls_enabled) {
      atomic_store_explicit(&__get_tls()->tib_sigmask, m, memory_order_release);
      if (_weaken(__sig_check))
        _weaken(__sig_check)();
    }
  } else {
    sys_sigprocmask(SIG_SETMASK, &m, 0);
  }
}
