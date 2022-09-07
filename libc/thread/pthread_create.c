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
#include "libc/calls/calls.h"
#include "libc/errno.h"
#include "libc/intrin/asan.internal.h"
#include "libc/intrin/atomic.h"
#include "libc/intrin/pthread.h"
#include "libc/intrin/wait0.internal.h"
#include "libc/mem/mem.h"
#include "libc/nexgen32e/gettls.h"
#include "libc/runtime/runtime.h"
#include "libc/sysv/consts/clone.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/prot.h"
#include "libc/thread/posixthread.internal.h"
#include "libc/thread/spawn.h"
#include "libc/thread/thread.h"

void pthread_wait(struct PosixThread *pt) {
  _wait0(pt->spawn.ctid);
}

void pthread_free(struct PosixThread *pt) {
  free(pt->spawn.tls);
  if (pt->stacksize) {
    munmap(&pt->spawn.stk, pt->stacksize);
  }
  free(pt);
}

static int PosixThread(void *arg, int tid) {
  struct PosixThread *pt = arg;
  enum PosixThreadStatus status;
  if (!setjmp(pt->exiter)) {
    ((cthread_t)__get_tls())->pthread = pt;
    pt->rc = pt->start_routine(pt->arg);
  }
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

/**
 * Creates thread.
 *
 * Here's the OSI model of threads in Cosmopolitan:
 *
 *              ┌──────────────────┐
 *              │ pthread_create() │       - Standard
 *              └─────────┬────────┘         Abstraction
 *              ┌─────────┴────────┐
 *              │     _spawn()     │       - Cosmopolitan
 *              └─────────┬────────┘         Abstraction
 *              ┌─────────┴────────┐
 *              │     clone()      │       - Polyfill
 *              └─────────┬────────┘
 *            ┌────────┬──┴──┬─┬─────────┐ - Kernel
 *      ┌─────┴─────┐  │     │┌┴──────┐  │   Interfaces
 *      │ sys_clone │  │     ││ tfork │  │
 *      └───────────┘  │     │└───────┘ ┌┴─────────────┐
 *     ┌───────────────┴──┐ ┌┴────────┐ │ CreateThread │
 *     │ bsdthread_create │ │ thr_new │ └──────────────┘
 *     └──────────────────┘ └─────────┘
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
 */
int pthread_create(pthread_t *thread, const pthread_attr_t *attr,
                   void *(*start_routine)(void *), void *arg) {
  int rc, e = errno;
  struct PosixThread *pt;
  pthread_attr_t default_attr;
  pthread_zombies_decimate();

  // default attributes
  if (!attr) {
    pthread_attr_init(&default_attr);
    attr = &default_attr;
  }

  // create posix thread object
  if (!(pt = calloc(1, sizeof(struct PosixThread)))) {
    errno = e;
    return EAGAIN;
  }
  pt->start_routine = start_routine;
  pt->arg = arg;

  // create thread local storage memory
  if (!(pt->spawn.tls = _mktls(&pt->spawn.tib))) {
    free(pt);
    errno = e;
    return EAGAIN;
  }

  // child thread id is also a condition variable
  pt->spawn.ctid = (int *)(pt->spawn.tib + 0x38);

  // create stack
  if (attr && attr->stackaddr) {
    // caller is responsible for creating stacks
    pt->spawn.stk = attr->stackaddr;
  } else {
    // cosmo posix threads is managing the stack
    pt->spawn.stk = mmap(0, attr->stacksize, PROT_READ | PROT_WRITE,
                         MAP_STACK | MAP_ANONYMOUS, -1, 0);
    if (pt->spawn.stk != MAP_FAILED) {
      pt->stacksize = attr->stacksize;
    } else {
      rc = errno;
      pthread_free(pt);
      errno = e;
      if (rc == EINVAL || rc == EOVERFLOW) {
        return EINVAL;
      } else {
        return EAGAIN;
      }
    }
    // mmap(MAP_STACK) creates a 4096 guard by default
    if (attr->guardsize != PAGESIZE) {
      // user requested special guard size
      if (attr->guardsize) {
        rc = mprotect(pt->spawn.stk, attr->guardsize, PROT_NONE);
      } else {
        rc = mprotect(pt->spawn.stk, PAGESIZE, PROT_READ | PROT_WRITE);
      }
      if (rc) {
        notpossible;
      }
    }
    if (IsAsan()) {
      if (attr->guardsize) {
        __asan_poison(pt->spawn.stk, attr->guardsize, kAsanStackOverflow);
      }
      __asan_poison(
          pt->spawn.stk + attr->stacksize - 16 /* openbsd:stackbound */, 16,
          kAsanStackOverflow);
    }
  }

  // save the attributes for descriptive purposes
  pt->attr = *attr;

  // set initial status
  switch (attr->detachstate) {
    case PTHREAD_CREATE_JOINABLE:
      pt->status = kPosixThreadJoinable;
      break;
    case PTHREAD_CREATE_DETACHED:
      pt->status = kPosixThreadDetached;
      pthread_zombies_add(pt);
      break;
    default:
      pthread_free(pt);
      return EINVAL;
  }

  // launch PosixThread(pt) in new thread
  if (clone(PosixThread, pt->spawn.stk,
            attr->stacksize - 16 /* openbsd:stackbound */,
            CLONE_VM | CLONE_THREAD | CLONE_FS | CLONE_FILES | CLONE_SIGHAND |
                CLONE_SETTLS | CLONE_PARENT_SETTID | CLONE_CHILD_SETTID |
                CLONE_CHILD_CLEARTID,
            pt, &pt->spawn.ptid, pt->spawn.tib, pt->spawn.ctid) == -1) {
    rc = errno;
    pthread_free(pt);
    errno = e;
    if (rc == EINVAL) {
      return EINVAL;
    } else {
      return EAGAIN;
    }
  }

  if (thread) {
    *thread = pt;
  }
  return 0;
}
