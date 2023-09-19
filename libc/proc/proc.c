/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
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
#include "libc/calls/calls.h"
#include "libc/calls/internal.h"
#include "libc/calls/sig.internal.h"
#include "libc/calls/state.internal.h"
#include "libc/calls/struct/siginfo.h"
#include "libc/cosmo.h"
#include "libc/intrin/dll.h"
#include "libc/intrin/leaky.internal.h"
#include "libc/intrin/weaken.h"
#include "libc/mem/mem.h"
#include "libc/nt/accounting.h"
#include "libc/nt/enum/processcreationflags.h"
#include "libc/nt/enum/status.h"
#include "libc/nt/enum/wait.h"
#include "libc/nt/runtime.h"
#include "libc/nt/synchronization.h"
#include "libc/nt/thread.h"
#include "libc/proc/proc.internal.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/sa.h"
#include "libc/sysv/consts/sicode.h"
#include "libc/sysv/consts/sig.h"
#include "libc/sysv/errfuns.h"
#include "libc/thread/tls.h"
#ifdef __x86_64__

struct Procs __proc;

static textwindows dontinstrument uint32_t __proc_worker(void *arg) {
  __bootstrap_tls(&__proc.tls, __builtin_frame_address(0));
  for (;;) {
    int64_t handles[64];
    int sic, dosignal = 0;
    struct Proc *pr, *objects[64];
    struct Dll *e, *e2, *samples = 0;
    uint32_t millis, status, i, n = 1;
    __proc_lock();
    handles[0] = __proc.onstart;
    for (e = dll_first(__proc.list); e && n < 64; ++n, e = e2) {
      pr = PROC_CONTAINER(e);
      e2 = dll_next(__proc.list, e);
      dll_remove(&__proc.list, e);
      dll_make_last(&samples, e);
      handles[n] = pr->handle;
      objects[n] = pr;
    }
    dll_make_last(&__proc.list, samples);
    __proc_unlock();
    millis = n == 64 ? __SIG_PROC_INTERVAL_MS : -1u;
    i = WaitForMultipleObjects(n, handles, false, millis);
    i &= ~kNtWaitAbandoned;
    if (!i || i == kNtWaitTimeout) continue;
    GetExitCodeProcess(handles[i], &status);
    if (status == kNtStillActive) continue;
    __proc_lock();
    pr = objects[i];
    if (status == 0xc9af3d51u) status = kNtStillActive;
    pr->wstatus = status;
    if ((__sighandrvas[SIGCHLD] == (uintptr_t)SIG_IGN ||
         (__sighandflags[SIGCHLD] & SA_NOCLDWAIT)) &&
        (!pr->waiters && !__proc.waiters)) {
      CloseHandle(pr->handle);
      dll_remove(&__proc.list, &pr->elem);
      dll_make_first(&__proc.free, &pr->elem);
    } else {
      pr->iszombie = 1;
      dll_remove(&__proc.list, &pr->elem);
      dll_make_first(&__proc.zombies, &pr->elem);
      if (pr->waiters) {
        nsync_cv_broadcast(&pr->onexit);
      } else if (__proc.waiters) {
        nsync_cv_signal(&__proc.onexit);
      } else {
        dosignal = 1;
        sic = WIFSIGNALED(status) ? CLD_KILLED : CLD_EXITED;
      }
    }
    __proc_unlock();
    if (dosignal) {
      __sig_generate(SIGCHLD, sic);
    }
  }
  return 0;
}

/**
 * Lazy initializes process tracker data structures and worker.
 */
static textwindows void __proc_setup(void) {
  __proc.onstart = CreateSemaphore(0, 0, 1, 0);
  __proc.thread = CreateThread(0, 65536, __proc_worker, 0,
                               kNtStackSizeParamIsAReservation, 0);
}

/**
 * Locks process tracker.
 */
textwindows void __proc_lock(void) {
  cosmo_once(&__proc.once, __proc_setup);
  nsync_mu_lock(&__proc.lock);
}

/**
 * Unlocks process tracker.
 */
textwindows void __proc_unlock(void) {
  nsync_mu_unlock(&__proc.lock);
}

/**
 * Resets process tracker from forked child.
 */
textwindows void __proc_wipe(void) {
  bzero(&__proc, sizeof(__proc));
}

/**
 * Allocates object for new process.
 *
 * The returned memory is not tracked by any list. It must be filled in
 * with system process information and then added back to the system by
 * calling __proc_add(). If process creation fails, then it needs to be
 * released using __proc_free().
 */
textwindows struct Proc *__proc_new(void) {
  struct Dll *e;
  struct Proc *proc = 0;
  int i, n = ARRAYLEN(__proc.pool);
  if (atomic_load_explicit(&__proc.allocated, memory_order_relaxed) < n &&
      (i = atomic_fetch_add(&__proc.allocated, 1)) < n) {
    proc = __proc.pool + i;
  } else {
    if ((e = dll_first(__proc.free))) {
      proc = PROC_CONTAINER(e);
      dll_remove(&__proc.free, &proc->elem);
    }
    if (!proc) {
      if (_weaken(malloc)) {
        proc = _weaken(malloc)(sizeof(struct Proc));
      } else {
        enomem();
        return 0;
      }
    }
  }
  if (proc) {
    bzero(proc, sizeof(*proc));
    dll_init(&proc->elem);
  }
  return proc;
}

IGNORE_LEAKS(__proc_new)

/**
 * Adds process to active list.
 *
 * The handle and pid must be filled in before calling this.
 */
textwindows void __proc_add(struct Proc *proc) {
  dll_make_first(&__proc.list, &proc->elem);
  ReleaseSemaphore(__proc.onstart, 1, 0);
}

/**
 * Frees process allocation.
 *
 * Process must not be currently tracked in the active or zombies list.
 */
textwindows void __proc_free(struct Proc *proc) {
  dll_make_first(&__proc.free, &proc->elem);
}

#endif /* __x86_64__ */
