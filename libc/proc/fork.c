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
#include "libc/atomic.h"
#include "libc/calls/calls.h"
#include "libc/calls/internal.h"
#include "libc/calls/sig.internal.h"
#include "libc/calls/state.internal.h"
#include "libc/calls/struct/metasigaltstack.h"
#include "libc/calls/struct/sigset.internal.h"
#include "libc/calls/struct/timespec.h"
#include "libc/calls/syscall-nt.internal.h"
#include "libc/calls/syscall-sysv.internal.h"
#include "libc/dce.h"
#include "libc/intrin/atomic.h"
#include "libc/intrin/cxaatexit.h"
#include "libc/intrin/dll.h"
#include "libc/intrin/maps.h"
#include "libc/intrin/stack.h"
#include "libc/intrin/strace.h"
#include "libc/intrin/weaken.h"
#include "libc/nt/files.h"
#include "libc/nt/process.h"
#include "libc/nt/runtime.h"
#include "libc/nt/thread.h"
#include "libc/nt/thunk/msabi.h"
#include "libc/proc/proc.h"
#include "libc/runtime/internal.h"
#include "libc/runtime/runtime.h"
#include "libc/runtime/syslib.internal.h"
#include "libc/stdio/internal.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/ss.h"
#include "libc/thread/itimer.h"
#include "libc/thread/posixthread.internal.h"
#include "libc/thread/thread.h"
#include "third_party/dlmalloc/dlmalloc.h"
#include "third_party/gdtoa/lock.h"
#include "third_party/tz/lock.h"

__msabi extern typeof(GetCurrentProcessId) *const __imp_GetCurrentProcessId;

extern atomic_int __sig_worker_state;
extern pthread_mutex_t __cxa_lock_obj;
extern pthread_mutex_t __pthread_lock_obj;

void __rand64_lock(void);
void __rand64_unlock(void);
void __rand64_wipe(void);

void __dlopen_lock(void);
void __dlopen_unlock(void);
void __dlopen_wipe(void);

// first and last and always
// it is the lord of all locks
// subordinate to no other lock
static pthread_mutex_t supreme_lock = PTHREAD_MUTEX_INITIALIZER;

static void fork_prepare_stdio(void) {
  struct Dll *e;
  // we acquire the following locks, in order
  //
  //   1. FILE objects created by the user
  //   2. stdin, stdout, and stderr
  //   3. __stdio.lock
  //
StartOver:
  __stdio_lock();
  for (e = dll_last(__stdio.files); e; e = dll_prev(__stdio.files, e)) {
    FILE *f = FILE_CONTAINER(e);
    if (f->forking)
      continue;
    f->forking = 1;
    __stdio_ref(f);
    __stdio_unlock();
    _pthread_mutex_lock(&f->lock);
    __stdio_unref(f);
    goto StartOver;
  }
}

static void fork_parent_stdio(void) {
  struct Dll *e;
  for (e = dll_first(__stdio.files); e; e = dll_next(__stdio.files, e)) {
    FILE_CONTAINER(e)->forking = 0;
    _pthread_mutex_unlock(&FILE_CONTAINER(e)->lock);
  }
  __stdio_unlock();
}

static void fork_child_stdio(void) {
  struct Dll *e;
  for (e = dll_first(__stdio.files); e; e = dll_next(__stdio.files, e)) {
    _pthread_mutex_wipe_np(&FILE_CONTAINER(e)->lock);
    FILE_CONTAINER(e)->forking = 0;
  }
  _pthread_mutex_wipe_np(&__stdio.lock);
}

static void fork_prepare(void) {
  _pthread_mutex_lock(&supreme_lock);
  if (_weaken(_pthread_onfork_prepare))
    _weaken(_pthread_onfork_prepare)();
  fork_prepare_stdio();
  if (_weaken(__localtime_lock))
    _weaken(__localtime_lock)();
  if (_weaken(__dlopen_lock))
    _weaken(__dlopen_lock)();
  if (IsWindows())
    __proc_lock();
  if (_weaken(cosmo_stack_lock))
    _weaken(cosmo_stack_lock)();
  __cxa_lock();
  if (_weaken(__gdtoa_lock)) {
    _weaken(__gdtoa_lock1)();
    _weaken(__gdtoa_lock)();
  }
  _pthread_lock();
  if (_weaken(dlmalloc_pre_fork))
    _weaken(dlmalloc_pre_fork)();
  __fds_lock();
  if (_weaken(__rand64_lock))
    _weaken(__rand64_lock)();
  __maps_lock();
  LOCKTRACE("READY TO LOCK AND ROLL");
}

static void fork_parent(void) {
  __maps_unlock();
  if (_weaken(__rand64_unlock))
    _weaken(__rand64_unlock)();
  __fds_unlock();
  if (_weaken(dlmalloc_post_fork_parent))
    _weaken(dlmalloc_post_fork_parent)();
  _pthread_unlock();
  if (_weaken(__gdtoa_unlock)) {
    _weaken(__gdtoa_unlock)();
    _weaken(__gdtoa_unlock1)();
  }
  __cxa_unlock();
  if (_weaken(cosmo_stack_unlock))
    _weaken(cosmo_stack_unlock)();
  if (IsWindows())
    __proc_unlock();
  if (_weaken(__dlopen_unlock))
    _weaken(__dlopen_unlock)();
  if (_weaken(__localtime_unlock))
    _weaken(__localtime_unlock)();
  fork_parent_stdio();
  if (_weaken(_pthread_onfork_parent))
    _weaken(_pthread_onfork_parent)();
  _pthread_mutex_unlock(&supreme_lock);
}

static void fork_child(int ppid_win32, int ppid_cosmo) {
  if (_weaken(__rand64_wipe))
    _weaken(__rand64_wipe)();
  _pthread_mutex_wipe_np(&__fds_lock_obj);
  dlmalloc_post_fork_child();
  if (_weaken(__gdtoa_wipe)) {
    _weaken(__gdtoa_wipe)();
    _weaken(__gdtoa_wipe1)();
  }
  fork_child_stdio();
  _pthread_mutex_wipe_np(&__pthread_lock_obj);
  _pthread_mutex_wipe_np(&__cxa_lock_obj);
  if (_weaken(cosmo_stack_wipe))
    _weaken(cosmo_stack_wipe)();
  if (_weaken(__dlopen_wipe))
    _weaken(__dlopen_wipe)();
  if (_weaken(__localtime_wipe))
    _weaken(__localtime_wipe)();
  if (IsWindows()) {
    // we don't bother locking the proc/itimer/sig locks above since
    // their state is reset in the forked child. nothing to protect.
    sys_read_nt_wipe_keystrokes();
    __proc_wipe_and_reset();
    __itimer_wipe_and_reset();
    atomic_init(&__sig_worker_state, 0);
    if (_weaken(__sig_init))
      _weaken(__sig_init)();
    if (_weaken(sys_getppid_nt_wipe))
      _weaken(sys_getppid_nt_wipe)(ppid_win32, ppid_cosmo);
  }
  if (_weaken(_pthread_onfork_child))
    _weaken(_pthread_onfork_child)();
  _pthread_mutex_wipe_np(&supreme_lock);
}

int _fork(uint32_t dwCreationFlags) {
  struct Dll *e;
  int ax, dx, tid, ppid_win32, ppid_cosmo;
  ppid_win32 = IsWindows() ? GetCurrentProcessId() : 0;
  ppid_cosmo = __pid;
  BLOCK_SIGNALS;
  fork_prepare();
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
      dx = __imp_GetCurrentProcessId();
    }
    __pid = dx;

    // get new thread id
    struct CosmoTib *tib = __get_tls();
    struct PosixThread *me = (struct PosixThread *)tib->tib_pthread;
    tid = IsLinux() || IsXnuSilicon() ? dx : sys_gettid();
    atomic_init(&tib->tib_ctid, tid);
    atomic_init(&tib->tib_ptid, tid);

    // tracing and kisdangerous need this lock wiped a little earlier
    atomic_init(&__maps.lock.word, 0);

    /*
     * it's now safe to call normal functions again
     */

    // this wipe must happen fast
    void nsync_waiter_wipe_(void);
    if (_weaken(nsync_waiter_wipe_))
      _weaken(nsync_waiter_wipe_)();

    // turn other threads into zombies
    // we can't free() them since we're monopolizing all locks
    // we assume the operating system already reclaimed system handles
    dll_remove(&_pthread_list, &me->list);
    struct Dll *old_threads = _pthread_list;
    _pthread_list = 0;
    dll_make_first(&_pthread_list, &me->list);
    atomic_init(&_pthread_count, 1);

    // get new system thread handle
    intptr_t syshand = 0;
    if (IsXnuSilicon()) {
      syshand = __syslib->__pthread_self();
    } else if (IsWindows()) {
      DuplicateHandle(GetCurrentProcess(), GetCurrentThread(),
                      GetCurrentProcess(), &syshand, 0, false,
                      kNtDuplicateSameAccess);
    }
    atomic_init(&tib->tib_syshand, syshand);

    // the child's pending signals is initially empty
    atomic_init(&tib->tib_sigpending, 0);

    // we can't be canceled if the canceler no longer exists
    atomic_init(&me->pt_canceled, false);

    // forget locks
    bzero(tib->tib_locks, sizeof(tib->tib_locks));

    // xnu fork() doesn't preserve sigaltstack()
    if (IsXnu() && me->tib->tib_sigstack_addr) {
      struct sigaltstack_bsd ss;
      ss.ss_sp = me->tib->tib_sigstack_addr;
      ss.ss_size = me->tib->tib_sigstack_size;
      ss.ss_flags = me->tib->tib_sigstack_flags;
      if (IsXnuSilicon()) {
        __syslib->__sigaltstack(&ss, 0);
      } else {
        sys_sigaltstack(&ss, 0);
      }
    }

    // run user fork callbacks
    fork_child(ppid_win32, ppid_cosmo);

    // free threads
    if (_weaken(_pthread_free)) {
      while ((e = dll_first(old_threads))) {
        struct PosixThread *pt = POSIXTHREAD_CONTAINER(e);
        atomic_init(&pt->tib->tib_syshand, 0);
        dll_remove(&old_threads, e);
        _weaken(_pthread_free)(pt);
      }
    }

    // reactivate ftrace
    /* if (ftrace_stackdigs) */
    /*   if (_weaken(ftrace_install)) */
    /*     _weaken(ftrace_install)(); */

    STRACE("fork() → 0 (child of %d)", ppid_cosmo);
  } else {
    // this is the parent process
    fork_parent();
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
