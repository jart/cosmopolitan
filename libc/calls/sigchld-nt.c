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
#include "libc/calls/internal.h"
#include "libc/calls/sig.internal.h"
#include "libc/calls/state.internal.h"
#include "libc/calls/syscall_support-nt.internal.h"
#include "libc/cosmo.h"
#include "libc/nt/enum/wait.h"
#include "libc/nt/runtime.h"
#include "libc/nt/synchronization.h"
#include "libc/nt/thread.h"
#include "libc/sysv/consts/sa.h"
#include "libc/sysv/consts/sicode.h"
#include "libc/sysv/consts/sig.h"
#include "libc/thread/tls.h"
#include "libc/thread/tls2.internal.h"
#ifdef __x86_64__

intptr_t __sigchld_thread;
static atomic_uint __sigchld_once;
static struct CosmoTib __sigchld_tls;

static textwindows bool __sigchld_check(void) {
  bool should_signal = false;
  for (;;) {
    int pids[64];
    int64_t handles[64];
    uint32_t n = __sample_pids(pids, handles, true);
    if (!n) return should_signal;
    uint32_t i = WaitForMultipleObjects(n, handles, false, 0);
    if (i == kNtWaitFailed) return should_signal;
    if (i == kNtWaitTimeout) return should_signal;
    i &= ~kNtWaitAbandoned;
    if ((__sighandrvas[SIGCHLD] >= kSigactionMinRva) &&
        (__sighandflags[SIGCHLD] & SA_NOCLDWAIT)) {
      CloseHandle(handles[i]);
      __releasefd(pids[i]);
    } else {
      g_fds.p[pids[i]].zombie = true;
    }
    should_signal = true;
  }
}

static textwindows uint32_t __sigchld_worker(void *arg) {
  __set_tls_win32(&__sigchld_tls);
  for (;;) {
    if (__sigchld_check()) {
      __sig_notify(SIGCHLD, CLD_EXITED);
    }
    SleepEx(100, false);
  }
  return 0;
}

static textwindows void __sigchld_init(void) {
  __sigchld_thread = CreateThread(0, 65536, __sigchld_worker, 0, 0, 0);
}

void _init_sigchld(void) {
  cosmo_once(&__sigchld_once, __sigchld_init);
}

#endif /* __x86_64__ */
