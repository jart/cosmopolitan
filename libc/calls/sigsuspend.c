/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
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
#include "libc/calls/internal.h"
#include "libc/calls/sig.internal.h"
#include "libc/calls/sigbits.h"
#include "libc/calls/strace.internal.h"
#include "libc/calls/struct/sigset.h"
#include "libc/dce.h"
#include "libc/intrin/asan.internal.h"
#include "libc/nt/synchronization.h"
#include "libc/sysv/errfuns.h"

/**
 * Blocks until SIG ∉ MASK is delivered to process.
 *
 * @param ignore is a bitset of signals to block temporarily
 * @return -1 w/ EINTR or possibly EFAULT
 * @asyncsignalsafe
 * @norestart
 */
int sigsuspend(const sigset_t *ignore) {
  int rc;
  char buf[41];
  sigset_t save, mask, *arg;
  STRACE("sigsuspend(%s) → [...]",
         __strace_sigset(buf, sizeof(buf), 0, ignore));
  if (IsAsan() && ignore && !__asan_is_valid(ignore, sizeof(*ignore))) {
    rc = efault();
  } else if (IsXnu() || IsOpenbsd()) {
    // openbsd and xnu only support 32 signals
    // they use a register calling convention for sigsuspend
    if (ignore) {
      arg = (sigset_t *)(uintptr_t)(*(uint32_t *)ignore);
    } else {
      arg = 0;
    }
    rc = sys_sigsuspend(arg, 8);
  } else if (IsLinux() || IsFreebsd() || IsNetbsd() || IsWindows()) {
    if (ignore) {
      arg = ignore;
    } else {
      sigemptyset(&mask);
      arg = &mask;
    }
    if (!IsWindows()) {
      rc = sys_sigsuspend(arg, 8);
    } else {
      save = __sig_mask(arg);
      do {
        if (_check_interrupts(false, g_fds.p)) {
          rc = eintr();
          break;
        }
        SleepEx(__SIG_POLLING_INTERVAL_MS, true);
      } while (1);
      __sig_mask(&save);
    }
  } else {
    // TODO(jart): sigsuspend metal support
    rc = enosys();
  }
  STRACE("[...] sigsuspend → %d% m", rc);
  return rc;
}
