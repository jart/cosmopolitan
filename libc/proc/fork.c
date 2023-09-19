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
#include "libc/assert.h"
#include "libc/calls/blockcancel.internal.h"
#include "libc/calls/blocksigs.internal.h"
#include "libc/calls/calls.h"
#include "libc/calls/struct/sigset.h"
#include "libc/calls/struct/sigset.internal.h"
#include "libc/calls/syscall-nt.internal.h"
#include "libc/calls/syscall-sysv.internal.h"
#include "libc/dce.h"
#include "libc/intrin/atomic.h"
#include "libc/intrin/dll.h"
#include "libc/intrin/strace.internal.h"
#include "libc/intrin/weaken.h"
#include "libc/nt/process.h"
#include "libc/nt/runtime.h"
#include "libc/nt/synchronization.h"
#include "libc/proc/proc.internal.h"
#include "libc/runtime/internal.h"
#include "libc/runtime/runtime.h"
#include "libc/sysv/consts/sig.h"
#include "libc/thread/posixthread.internal.h"
#include "libc/thread/tls.h"

int _fork(uint32_t dwCreationFlags) {
  struct Dll *e;
  struct CosmoTib *tib;
  int ax, dx, tid, parent;
  struct PosixThread *me, *other;
  (void)parent;
  BLOCK_SIGNALS;
  BLOCK_CANCELLATIONS;
  if (IsWindows()) __proc_lock();
  if (__threaded && _weaken(_pthread_onfork_prepare)) {
    _weaken(_pthread_onfork_prepare)();
  }
  if (!IsWindows()) {
    ax = sys_fork();
  } else {
    ax = sys_fork_nt(dwCreationFlags);
  }
  if (!ax) {
    if (!IsWindows()) {
      dx = sys_getpid().ax;
    } else {
      dx = GetCurrentProcessId();
    }
    parent = __pid;
    __pid = dx;
    tib = __get_tls();
    me = (struct PosixThread *)tib->tib_pthread;
    dll_remove(&_pthread_list, &me->list);
    for (e = dll_first(_pthread_list); e; e = dll_next(_pthread_list, e)) {
      other = POSIXTHREAD_CONTAINER(e);
      atomic_store_explicit(&other->status, kPosixThreadZombie,
                            memory_order_relaxed);
      other->tib->tib_syshand = 0;
    }
    dll_make_first(&_pthread_list, &me->list);
    tid = IsLinux() || IsXnuSilicon() ? dx : sys_gettid();
    atomic_store_explicit(&tib->tib_tid, tid, memory_order_relaxed);
    atomic_store_explicit(&me->ptid, tid, memory_order_relaxed);
    atomic_store_explicit(&me->cancelled, false, memory_order_relaxed);
    if (IsWindows()) npassert((me->semaphore = CreateSemaphore(0, 0, 1, 0)));
    if (__threaded && _weaken(_pthread_onfork_child)) {
      _weaken(_pthread_onfork_child)();
    }
    if (IsWindows()) __proc_wipe();
    STRACE("fork() → 0 (child of %d)", parent);
  } else {
    if (__threaded && _weaken(_pthread_onfork_parent)) {
      _weaken(_pthread_onfork_parent)();
    }
    if (IsWindows()) __proc_unlock();
    STRACE("fork() → %d% m", ax);
  }
  ALLOW_CANCELLATIONS;
  ALLOW_SIGNALS;
  return ax;
}

/**
 * Creates new process.
 *
 * @return 0 to child, child pid to parent, or -1 w/ errno
 * @raise EAGAIN if `RLIMIT_NPROC` was exceeded or system lacked resources
 * @raise ENOMEM if we require more vespene gas
 * @asyncsignalsafe
 * @threadsafe
 */
int fork(void) {
  return _fork(0);
}
