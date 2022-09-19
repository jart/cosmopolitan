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
#include "libc/calls/sched-sysv.internal.h"
#include "libc/calls/struct/sigaltstack.h"
#include "libc/calls/syscall-sysv.internal.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/intrin/asan.internal.h"
#include "libc/intrin/atomic.h"
#include "libc/intrin/bits.h"
#include "libc/intrin/kprintf.h"
#include "libc/intrin/weaken.h"
#include "libc/log/internal.h"
#include "libc/macros.internal.h"
#include "libc/mem/mem.h"
#include "libc/nexgen32e/gc.internal.h"
#include "libc/runtime/clone.internal.h"
#include "libc/runtime/runtime.h"
#include "libc/runtime/stack.h"
#include "libc/sysv/consts/clone.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/prot.h"
#include "libc/sysv/consts/ss.h"
#include "libc/sysv/errfuns.h"
#include "libc/thread/posixthread.internal.h"
#include "libc/thread/spawn.h"
#include "libc/thread/thread.h"
#include "libc/thread/tls.h"
#include "libc/thread/wait0.internal.h"

STATIC_YOINK("nsync_mu_lock");
STATIC_YOINK("nsync_mu_unlock");

#define MAP_ANON_OPENBSD  0x1000
#define MAP_STACK_OPENBSD 0x4000

void _pthread_wait(struct PosixThread *pt) {
  _wait0(&pt->tib->tib_tid);
}

void _pthread_free(struct PosixThread *pt) {
  free(pt->tls);
  if (pt->ownstack &&        //
      pt->attr.stackaddr &&  //
      pt->attr.stackaddr != MAP_FAILED) {
    if (munmap(pt->attr.stackaddr, pt->attr.stacksize)) {
      notpossible;
    }
  }
  if (pt->altstack) {
    free(pt->altstack);
  }
  free(pt);
}

static int PosixThread(void *arg, int tid) {
  struct PosixThread *pt = arg;
  enum PosixThreadStatus status;
  struct sigaltstack ss;
  if (pt->altstack) {
    ss.ss_flags = 0;
    ss.ss_size = SIGSTKSZ;
    ss.ss_sp = pt->altstack;
    if (sigaltstack(&ss, 0)) {
      notpossible;
    }
  }
  if (pt->attr.inheritsched == PTHREAD_EXPLICIT_SCHED) {
    _pthread_reschedule(pt);
  }
  if (!setjmp(pt->exiter)) {
    __get_tls()->tib_pthread = (pthread_t)pt;
    pt->rc = pt->start_routine(pt->arg);
  }
  if (_weaken(_pthread_key_destruct)) {
    _weaken(_pthread_key_destruct)(0);
  }
  _pthread_ungarbage();
  if (atomic_load_explicit(&pt->status, memory_order_acquire) ==
      kPosixThreadDetached) {
    atomic_store_explicit(&pt->status, kPosixThreadZombie,
                          memory_order_release);
  } else {
    atomic_store_explicit(&pt->status, kPosixThreadTerminated,
                          memory_order_release);
  }
  return 0;
}

static int FixupCustomStackOnOpenbsd(pthread_attr_t *attr) {
  // OpenBSD: Only permits RSP to occupy memory that's been explicitly
  // defined as stack memory. We need to squeeze the provided interval
  // in order to successfully call mmap(), which will return EINVAL if
  // these calculations should overflow.
  size_t n;
  int e, rc;
  uintptr_t x, y;
  n = attr->stacksize;
  x = (uintptr_t)attr->stackaddr;
  y = ROUNDUP(x, PAGESIZE);
  n -= y - x;
  n = ROUNDDOWN(n, PAGESIZE);
  e = errno;
  if (__sys_mmap((void *)y, n, PROT_READ | PROT_WRITE,
                 MAP_PRIVATE | MAP_FIXED | MAP_ANON_OPENBSD | MAP_STACK_OPENBSD,
                 -1, 0, 0) == (void *)y) {
    attr->stackaddr = (void *)y;
    attr->stacksize = n;
    return 0;
  } else {
    rc = errno;
    errno = e;
    if (rc == EOVERFLOW) {
      rc = EINVAL;
    }
    return rc;
  }
}

/**
 * Creates thread, e.g.
 *
 *     void *worker(void *arg) {
 *       fputs(arg, stdout);
 *       return "there\n";
 *     }
 *
 *     int main() {
 *       void *result;
 *       pthread_t id;
 *       pthread_create(&id, 0, worker, "hi ");
 *       pthread_join(id, &result);
 *       fputs(result, stdout);
 *     }
 *
 * Here's the OSI model of threads in Cosmopolitan:
 *
 *              ┌──────────────────┐
 *              │ pthread_create() │       - Standard
 *              └─────────┬────────┘         Abstraction
 *              ┌─────────┴────────┐
 *              │     clone()      │       - Polyfill
 *              └─────────┬────────┘
 *            ┌────────┬──┴┬─┬─┬─────────┐ - Kernel
 *      ┌─────┴─────┐  │   │ │┌┴──────┐  │   Interfaces
 *      │ sys_clone │  │   │ ││ tfork │ ┌┴─────────────┐
 *      └───────────┘  │   │ │└───────┘ │ CreateThread │
 *     ┌───────────────┴──┐│┌┴────────┐ └──────────────┘
 *     │ bsdthread_create │││ thr_new │
 *     └──────────────────┘│└─────────┘
 *                 ┌───────┴──────┐
 *                 │ _lwp_create  │
 *                 └──────────────┘
 *
 * @param thread if non-null is used to output the thread id
 *     upon successful completion
 * @param attr points to launch configuration, or may be null
 *     to use sensible defaults; it must be initialized using
 *     pthread_attr_init()
 * @param start_routine is your thread's callback function
 * @param arg is an arbitrary value passed to `start_routine`
 * @return 0 on success, or errno on error
 * @raise EAGAIN if resources to create thread weren't available
 * @raise EINVAL if `attr` was supplied and had unnaceptable data
 * @raise EPERM if scheduling policy was requested and user account
 *     isn't authorized to use it
 * @threadsafe
 */
errno_t pthread_create(pthread_t *thread, const pthread_attr_t *attr,
                       void *(*start_routine)(void *), void *arg) {
  int rc, e = errno;
  struct PosixThread *pt;
  __require_tls();
  _pthread_zombies_decimate();

  // create posix thread object
  if (!(pt = calloc(1, sizeof(struct PosixThread)))) {
    errno = e;
    return EAGAIN;
  }
  pt->start_routine = start_routine;
  pt->arg = arg;

  // create thread local storage memory
  if (!(pt->tls = _mktls(&pt->tib))) {
    free(pt);
    errno = e;
    return EAGAIN;
  }

  // setup attributes
  if (attr) {
    pt->attr = *attr;
    attr = 0;
  } else {
    pthread_attr_init(&pt->attr);
  }

  // setup stack
  if (pt->attr.stackaddr) {
    // caller supplied their own stack
    // assume they know what they're doing as much as possible
    if (IsOpenbsd()) {
      if ((rc = FixupCustomStackOnOpenbsd(&pt->attr))) {
        _pthread_free(pt);
        return rc;
      }
    }
  } else {
    // cosmo is managing the stack
    // 1. in mono repo optimize for tiniest stack possible
    // 2. in public world optimize to *work* regardless of memory
    pt->ownstack = true;
    pt->attr.stacksize = MAX(pt->attr.stacksize, GetStackSize());
    pt->attr.stacksize = _roundup2pow(pt->attr.stacksize);
    pt->attr.guardsize = ROUNDUP(pt->attr.guardsize, PAGESIZE);
    if (pt->attr.guardsize + PAGESIZE >= pt->attr.stacksize) {
      _pthread_free(pt);
      return EINVAL;
    }
    if (pt->attr.guardsize == PAGESIZE) {
      // user is wisely using smaller stacks with default guard size
      pt->attr.stackaddr = mmap(0, pt->attr.stacksize, PROT_READ | PROT_WRITE,
                                MAP_STACK | MAP_ANONYMOUS, -1, 0);
    } else {
      // user is tuning things, performance may suffer
      pt->attr.stackaddr = mmap(0, pt->attr.stacksize, PROT_READ | PROT_WRITE,
                                MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
      if (pt->attr.stackaddr != MAP_FAILED) {
        if (IsOpenbsd() &&
            __sys_mmap(
                pt->attr.stackaddr, pt->attr.stacksize, PROT_READ | PROT_WRITE,
                MAP_PRIVATE | MAP_FIXED | MAP_ANON_OPENBSD | MAP_STACK_OPENBSD,
                -1, 0, 0) != pt->attr.stackaddr) {
          notpossible;
        }
        if (pt->attr.guardsize && !IsWindows() &&
            mprotect(pt->attr.stackaddr, pt->attr.guardsize, PROT_NONE)) {
          notpossible;
        }
      }
    }
    if (pt->attr.stackaddr == MAP_FAILED) {
      rc = errno;
      _pthread_free(pt);
      errno = e;
      if (rc == EINVAL || rc == EOVERFLOW) {
        return EINVAL;
      } else {
        return EAGAIN;
      }
    }
    if (IsAsan() && pt->attr.guardsize) {
      __asan_poison(pt->attr.stackaddr, pt->attr.guardsize, kAsanStackOverflow);
    }
  }

  // setup signal handler stack
  if (_wantcrashreports && !IsWindows()) {
    pt->altstack = malloc(SIGSTKSZ);
  }

  // set initial status
  switch (pt->attr.detachstate) {
    case PTHREAD_CREATE_JOINABLE:
      pt->status = kPosixThreadJoinable;
      break;
    case PTHREAD_CREATE_DETACHED:
      pt->status = kPosixThreadDetached;
      _pthread_zombies_add(pt);
      break;
    default:
      _pthread_free(pt);
      return EINVAL;
  }

  // launch PosixThread(pt) in new thread
  if (clone(PosixThread, pt->attr.stackaddr,
            pt->attr.stacksize - (IsOpenbsd() ? 16 : 0),
            CLONE_VM | CLONE_THREAD | CLONE_FS | CLONE_FILES | CLONE_SIGHAND |
                CLONE_SETTLS | CLONE_PARENT_SETTID | CLONE_CHILD_SETTID |
                CLONE_CHILD_CLEARTID,
            pt, &pt->tid, pt->tib, &pt->tib->tib_tid) == -1) {
    rc = errno;
    _pthread_free(pt);
    errno = e;
    return rc;
  }

  if (thread) {
    *thread = (pthread_t)pt;
  }
  return 0;
}
