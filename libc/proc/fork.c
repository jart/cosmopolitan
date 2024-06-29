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
#include "libc/assert.h"
#include "libc/atomic.h"
#include "libc/calls/calls.h"
#include "libc/calls/state.internal.h"
#include "libc/calls/struct/sigset.h"
#include "libc/calls/struct/sigset.internal.h"
#include "libc/calls/syscall-nt.internal.h"
#include "libc/calls/syscall-sysv.internal.h"
#include "libc/dce.h"
#include "libc/intrin/atomic.h"
#include "libc/intrin/dll.h"
#include "libc/intrin/kprintf.h"
#include "libc/intrin/maps.h"
#include "libc/intrin/strace.internal.h"
#include "libc/intrin/weaken.h"
#include "libc/nt/files.h"
#include "libc/nt/process.h"
#include "libc/nt/runtime.h"
#include "libc/nt/synchronization.h"
#include "libc/nt/thread.h"
#include "libc/proc/proc.internal.h"
#include "libc/runtime/internal.h"
#include "libc/runtime/memtrack.internal.h"
#include "libc/runtime/runtime.h"
#include "libc/runtime/syslib.internal.h"
#include "libc/sysv/consts/sig.h"
#include "libc/thread/posixthread.internal.h"
#include "libc/thread/tls.h"

extern atomic_uint free_waiters_mu;

static void _onfork_prepare(void) {
  if (_weaken(_pthread_onfork_prepare))
    _weaken(_pthread_onfork_prepare)();
  _pthread_lock();
  __maps_lock();
  __fds_lock();
  while (atomic_exchange_explicit(&free_waiters_mu, 1, memory_order_acquire)) {
  }
}

static void _onfork_parent(void) {
  atomic_store_explicit(&free_waiters_mu, 0, memory_order_release);
  __fds_unlock();
  __maps_unlock();
  _pthread_unlock();
  if (_weaken(_pthread_onfork_parent))
    _weaken(_pthread_onfork_parent)();
}

static void _onfork_child(void) {
  pthread_mutexattr_t attr;
  pthread_mutexattr_init(&attr);
  pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
  pthread_mutex_init(&__fds_lock_obj, &attr);
  atomic_store_explicit(&free_waiters_mu, 0, memory_order_relaxed);
  pthread_mutexattr_destroy(&attr);
  _pthread_init();
  atomic_store_explicit(&__maps.lock, 0, memory_order_relaxed);
  atomic_store_explicit(&__get_tls()->tib_relock_maps, 0, memory_order_relaxed);
  if (_weaken(_pthread_onfork_child))
    _weaken(_pthread_onfork_child)();
}

int _fork(uint32_t dwCreationFlags) {
  struct Dll *e;
  int ax, dx, tid, parent;
  parent = __pid;
  BLOCK_SIGNALS;
  if (IsWindows())
    __proc_lock();
  if (__threaded)
    _onfork_prepare();
  if (!IsWindows()) {
    ax = sys_fork();
  } else {
    ax = sys_fork_nt(dwCreationFlags);
  }
  if (!ax) {

    // get new process id
    if (!IsWindows()) {
      dx = sys_getpid().ax;
    } else {
      dx = GetCurrentProcessId();
    }
    __pid = dx;

    // turn other threads into zombies
    // we can't free() them since we're monopolizing all locks
    // we assume the operating system already reclaimed system handles
    struct CosmoTib *tib = __get_tls();
    struct PosixThread *pt = (struct PosixThread *)tib->tib_pthread;
    dll_remove(&_pthread_list, &pt->list);
    for (e = dll_first(_pthread_list); e; e = dll_next(_pthread_list, e)) {
      atomic_store_explicit(&POSIXTHREAD_CONTAINER(e)->pt_status,
                            kPosixThreadZombie, memory_order_relaxed);
      atomic_store_explicit(&POSIXTHREAD_CONTAINER(e)->tib->tib_syshand, 0,
                            memory_order_relaxed);
    }
    dll_make_first(&_pthread_list, &pt->list);

    // get new main thread id
    tid = IsLinux() || IsXnuSilicon() ? dx : sys_gettid();
    atomic_store_explicit(&tib->tib_tid, tid, memory_order_relaxed);
    atomic_store_explicit(&pt->ptid, tid, memory_order_relaxed);

    // get new system thread handle
    intptr_t syshand = 0;
    if (IsXnuSilicon()) {
      syshand = __syslib->__pthread_self();
    } else if (IsWindows()) {
      DuplicateHandle(GetCurrentProcess(), GetCurrentThread(),
                      GetCurrentProcess(), &syshand, 0, false,
                      kNtDuplicateSameAccess);
    }
    atomic_store_explicit(&tib->tib_syshand, syshand, memory_order_relaxed);

    // we can't be canceled if the canceler no longer exists
    atomic_store_explicit(&pt->pt_canceled, false, memory_order_relaxed);

    // run user fork callbacks
    if (__threaded) {
      _onfork_child();
    }
    STRACE("fork() → 0 (child of %d)", parent);
  } else {
    // this is the parent process
    if (__threaded) {
      _onfork_parent();
    }
    if (IsWindows())
      __proc_unlock();
    STRACE("fork() → %d% m", ax);
  }
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
 */
int fork(void) {
  return _fork(0);
}
