/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
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
#include "libc/calls/struct/sigset.h"
#include "libc/calls/struct/sigset.internal.h"
#include "libc/calls/syscall-sysv.internal.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/fmt/itoa.h"
#include "libc/intrin/asan.internal.h"
#include "libc/intrin/bsr.h"
#include "libc/intrin/describeflags.internal.h"
#include "libc/intrin/dll.h"
#include "libc/intrin/strace.internal.h"
#include "libc/intrin/weaken.h"
#include "libc/log/internal.h"
#include "libc/macros.internal.h"
#include "libc/mem/alloca.h"
#include "libc/mem/mem.h"
#include "libc/nexgen32e/crc32.h"
#include "libc/nt/enum/memflags.h"
#include "libc/nt/enum/pageflags.h"
#include "libc/nt/memory.h"
#include "libc/nt/runtime.h"
#include "libc/nt/synchronization.h"
#include "libc/runtime/runtime.h"
#include "libc/runtime/stack.h"
#include "libc/runtime/syslib.internal.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/auxv.h"
#include "libc/sysv/consts/clone.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/prot.h"
#include "libc/sysv/consts/sig.h"
#include "libc/sysv/consts/ss.h"
#include "libc/thread/posixthread.internal.h"
#include "libc/thread/thread.h"
#include "libc/thread/tls.h"

__static_yoink("nsync_mu_lock");
__static_yoink("nsync_mu_unlock");
__static_yoink("nsync_mu_trylock");
__static_yoink("nsync_mu_rlock");
__static_yoink("nsync_mu_runlock");
__static_yoink("_pthread_atfork");

#define MAP_ANON_OPENBSD  0x1000
#define MAP_STACK_OPENBSD 0x4000

void _pthread_free(struct PosixThread *pt, bool isfork) {
  unassert(dll_is_alone(&pt->list) && &pt->list != _pthread_list);
  if (pt->pt_flags & PT_STATIC) return;
  if (pt->pt_flags & PT_OWNSTACK) {
    unassert(!munmap(pt->pt_attr.__stackaddr, pt->pt_attr.__stacksize));
  }
  if (!isfork) {
    if (IsWindows()) {
      if (pt->tib->tib_syshand) {
        unassert(CloseHandle(pt->tib->tib_syshand));
      }
    } else if (IsXnuSilicon()) {
      if (pt->tib->tib_syshand) {
        __syslib->__pthread_join(pt->tib->tib_syshand, 0);
      }
    }
  }
  free(pt->pt_tls);
  free(pt);
}

void _pthread_decimate(void) {
  struct Dll *e;
  struct PosixThread *pt;
  enum PosixThreadStatus status;
StartOver:
  _pthread_lock();
  for (e = dll_last(_pthread_list); e; e = dll_prev(_pthread_list, e)) {
    pt = POSIXTHREAD_CONTAINER(e);
    status = atomic_load_explicit(&pt->pt_status, memory_order_acquire);
    if (status != kPosixThreadZombie) break;
    if (!atomic_load_explicit(&pt->tib->tib_tid, memory_order_acquire)) {
      dll_remove(&_pthread_list, e);
      _pthread_unlock();
      _pthread_unref(pt);
      goto StartOver;
    }
  }
  _pthread_unlock();
}

static int PosixThread(void *arg, int tid) {
  void *rc;
  struct PosixThread *pt = arg;
  if (pt->pt_attr.__inheritsched == PTHREAD_EXPLICIT_SCHED) {
    unassert(_weaken(_pthread_reschedule));
    _weaken(_pthread_reschedule)(pt);  // yoinked by attribute builder
  }
  // set long jump handler so pthread_exit can bring control back here
  if (!setjmp(pt->pt_exiter)) {
    if (IsWindows()) {
      atomic_store_explicit(&__get_tls()->tib_sigmask, pt->pt_attr.__sigmask,
                            memory_order_release);
    } else {
      sys_sigprocmask(SIG_SETMASK, &pt->pt_attr.__sigmask, 0);
    }
    rc = pt->pt_start(pt->pt_arg);
    // ensure pthread_cleanup_pop(), and pthread_exit() popped cleanup
    unassert(!pt->pt_cleanup);
    // calling pthread_exit() will either jump back here, or call exit
    pthread_exit(rc);
  }
  // avoid signal handler being triggered after we trash our own stack
  __sig_block();
  // return to clone polyfill which clears tid, wakes futex, and exits
  return 0;
}

static int FixupCustomStackOnOpenbsd(pthread_attr_t *attr) {
  // OpenBSD: Only permits RSP to occupy memory that's been explicitly
  // defined as stack memory. We need to squeeze the provided interval
  // in order to successfully call mmap(), which will return EINVAL if
  // these calculations should overflow.
  size_t n;
  uintptr_t x, y;
  int e, rc, pagesz;
  pagesz = getauxval(AT_PAGESZ);
  n = attr->__stacksize;
  x = (uintptr_t)attr->__stackaddr;
  y = ROUNDUP(x, pagesz);
  n -= y - x;
  n = ROUNDDOWN(n, pagesz);
  e = errno;
  if (__sys_mmap((void *)y, n, PROT_READ | PROT_WRITE,
                 MAP_PRIVATE | MAP_FIXED | MAP_ANON_OPENBSD | MAP_STACK_OPENBSD,
                 -1, 0, 0) == (void *)y) {
    attr->__stackaddr = (void *)y;
    attr->__stacksize = n;
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

static errno_t pthread_create_impl(pthread_t *thread,
                                   const pthread_attr_t *attr,
                                   void *(*start_routine)(void *), void *arg,
                                   sigset_t oldsigs) {
  int rc, e = errno;
  struct PosixThread *pt;

  // create posix thread object
  if (!(pt = calloc(1, sizeof(struct PosixThread)))) {
    errno = e;
    return EAGAIN;
  }
  dll_init(&pt->list);
  pt->pt_start = start_routine;
  pt->pt_arg = arg;

  // create thread local storage memory
  if (!(pt->pt_tls = _mktls(&pt->tib))) {
    free(pt);
    errno = e;
    return EAGAIN;
  }

  // setup attributes
  if (attr) {
    pt->pt_attr = *attr;
    attr = 0;
  } else {
    pthread_attr_init(&pt->pt_attr);
  }

  // setup stack
  if (pt->pt_attr.__stackaddr) {
    // caller supplied their own stack
    // assume they know what they're doing as much as possible
    if (IsOpenbsd()) {
      if ((rc = FixupCustomStackOnOpenbsd(&pt->pt_attr))) {
        _pthread_free(pt, false);
        return rc;
      }
    }
  } else {
    // cosmo is managing the stack
    // 1. in mono repo optimize for tiniest stack possible
    // 2. in public world optimize to *work* regardless of memory
    int granularity = FRAMESIZE;
    int pagesize = getauxval(AT_PAGESZ);
    pt->pt_attr.__guardsize = ROUNDUP(pt->pt_attr.__guardsize, pagesize);
    pt->pt_attr.__stacksize = ROUNDUP(pt->pt_attr.__stacksize, granularity);
    if (pt->pt_attr.__guardsize + pagesize > pt->pt_attr.__stacksize) {
      _pthread_free(pt, false);
      return EINVAL;
    }
    if (pt->pt_attr.__guardsize == pagesize &&
        !(IsAarch64() && IsLinux() && IsQemuUser())) {
      // MAP_GROWSDOWN doesn't work very well on qemu-aarch64
      pt->pt_attr.__stackaddr =
          mmap(0, pt->pt_attr.__stacksize, PROT_READ | PROT_WRITE,
               MAP_STACK | MAP_ANONYMOUS, -1, 0);
    } else {
      pt->pt_attr.__stackaddr =
          mmap(0, pt->pt_attr.__stacksize, PROT_READ | PROT_WRITE,
               MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
      if (pt->pt_attr.__stackaddr != MAP_FAILED) {
        if (IsOpenbsd() &&
            __sys_mmap(
                pt->pt_attr.__stackaddr, pt->pt_attr.__stacksize,
                PROT_READ | PROT_WRITE,
                MAP_PRIVATE | MAP_FIXED | MAP_ANON_OPENBSD | MAP_STACK_OPENBSD,
                -1, 0, 0) != pt->pt_attr.__stackaddr) {
          notpossible;
        }
        if (pt->pt_attr.__guardsize) {
          if (!IsWindows()) {
            if (mprotect(pt->pt_attr.__stackaddr, pt->pt_attr.__guardsize,
                         PROT_NONE)) {
              notpossible;
            }
          } else {
            uint32_t oldattr;
            if (!VirtualProtect(pt->pt_attr.__stackaddr,
                                pt->pt_attr.__guardsize,
                                kNtPageReadwrite | kNtPageGuard, &oldattr)) {
              notpossible;
            }
          }
        }
      }
    }
    if (!pt->pt_attr.__stackaddr || pt->pt_attr.__stackaddr == MAP_FAILED) {
      rc = errno;
      _pthread_free(pt, false);
      errno = e;
      if (rc == EINVAL || rc == EOVERFLOW) {
        return EINVAL;
      } else {
        return EAGAIN;
      }
    }
    pt->pt_flags |= PT_OWNSTACK;
    if (IsAsan() && !IsWindows() && pt->pt_attr.__guardsize) {
      __asan_poison(pt->pt_attr.__stackaddr, pt->pt_attr.__guardsize,
                    kAsanStackOverflow);
    }
  }

  // set initial status
  pt->tib->tib_pthread = (pthread_t)pt;
  atomic_store_explicit(&pt->tib->tib_sigmask, -1, memory_order_relaxed);
  if (!pt->pt_attr.__havesigmask) {
    pt->pt_attr.__havesigmask = true;
    pt->pt_attr.__sigmask = oldsigs;
  }
  switch (pt->pt_attr.__detachstate) {
    case PTHREAD_CREATE_JOINABLE:
      atomic_store_explicit(&pt->pt_status, kPosixThreadJoinable,
                            memory_order_relaxed);
      break;
    case PTHREAD_CREATE_DETACHED:
      atomic_store_explicit(&pt->pt_status, kPosixThreadDetached,
                            memory_order_relaxed);
      break;
    default:
      _pthread_free(pt, false);
      return EINVAL;
  }

  // add thread to global list
  // we add it to the beginning since zombies go at the end
  _pthread_lock();
  dll_make_first(&_pthread_list, &pt->list);
  _pthread_unlock();

  // launch PosixThread(pt) in new thread
  if ((rc = clone(PosixThread, pt->pt_attr.__stackaddr,
                  pt->pt_attr.__stacksize - (IsOpenbsd() ? 16 : 0),
                  CLONE_VM | CLONE_THREAD | CLONE_FS | CLONE_FILES |
                      CLONE_SIGHAND | CLONE_SYSVSEM | CLONE_SETTLS |
                      CLONE_PARENT_SETTID | CLONE_CHILD_SETTID |
                      CLONE_CHILD_CLEARTID,
                  pt, &pt->ptid, __adj_tls(pt->tib), &pt->tib->tib_tid))) {
    _pthread_lock();
    dll_remove(&_pthread_list, &pt->list);
    _pthread_unlock();
    _pthread_free(pt, false);
    return rc;
  }

  *thread = (pthread_t)pt;
  return 0;
}

static const char *DescribeHandle(char buf[12], errno_t err, pthread_t *th) {
  if (err) return "n/a";
  if (!th) return "NULL";
  FormatInt32(buf, _pthread_tid((struct PosixThread *)*th));
  return buf;
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
 * @returnserrno
 */
errno_t pthread_create(pthread_t *thread, const pthread_attr_t *attr,
                       void *(*start_routine)(void *), void *arg) {
  errno_t err;
  _pthread_decimate();
  BLOCK_SIGNALS;
  err = pthread_create_impl(thread, attr, start_routine, arg, _SigMask);
  ALLOW_SIGNALS;
  STRACE("pthread_create([%s], %p, %t, %p) → %s",
         DescribeHandle(alloca(12), err, thread), attr, start_routine, arg,
         DescribeErrno(err));
  return err;
}
