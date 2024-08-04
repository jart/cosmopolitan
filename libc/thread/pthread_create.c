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
#include "libc/calls/struct/sigaltstack.h"
#include "libc/calls/struct/sigset.h"
#include "libc/calls/struct/sigset.internal.h"
#include "libc/calls/syscall-sysv.internal.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/fmt/itoa.h"
#include "libc/intrin/bsr.h"
#include "libc/intrin/describeflags.h"
#include "libc/intrin/dll.h"
#include "libc/intrin/kprintf.h"
#include "libc/intrin/strace.h"
#include "libc/intrin/weaken.h"
#include "libc/log/internal.h"
#include "libc/macros.h"
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
#include "libc/str/locale.internal.h"
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
__static_yoink("_pthread_onfork_prepare");
__static_yoink("_pthread_onfork_parent");
__static_yoink("_pthread_onfork_child");

#define MAP_ANON_OPENBSD  0x1000
#define MAP_STACK_OPENBSD 0x4000

void _pthread_free(struct PosixThread *pt) {

  // thread must be removed from _pthread_list before calling
  unassert(dll_is_alone(&pt->list) && &pt->list != _pthread_list);

  // do nothing for the one and only magical statical posix thread
  if (pt->pt_flags & PT_STATIC)
    return;

  // unmap stack if the cosmo runtime was responsible for mapping it
  if (pt->pt_flags & PT_OWNSTACK)
    unassert(!munmap(pt->pt_attr.__stackaddr, pt->pt_attr.__stacksize));

  // free any additional upstream system resources
  // our fork implementation wipes this handle in child automatically
  uint64_t syshand =
      atomic_load_explicit(&pt->tib->tib_syshand, memory_order_acquire);
  if (syshand) {
    if (IsWindows())
      unassert(CloseHandle(syshand));  // non-inheritable
    else if (IsXnuSilicon())
      unassert(!__syslib->__pthread_join(syshand, 0));
  }

  // free heap memory associated with thread
  if (pt->pt_flags & PT_OWNSIGALTSTACK)
    free(pt->pt_attr.__sigaltstackaddr);
  free(pt->pt_tls);
  free(pt);
}

void _pthread_decimate(bool annihilation_only) {
  struct PosixThread *pt;
  struct Dll *e, *e2, *list = 0;
  enum PosixThreadStatus status;

  // acquire posix threads gil
  _pthread_lock();

  // swiftly remove every single zombie
  // that isn't being held by a killing thread
  for (e = dll_last(_pthread_list); e; e = e2) {
    e2 = dll_prev(_pthread_list, e);
    pt = POSIXTHREAD_CONTAINER(e);
    if (atomic_load_explicit(&pt->pt_refs, memory_order_acquire) > 0)
      continue;  // pthread_kill() has a lease on this thread
    status = atomic_load_explicit(&pt->pt_status, memory_order_acquire);
    if (status != kPosixThreadZombie)
      break;  // zombies only exist at the end of the linked list
    if (atomic_load_explicit(&pt->tib->tib_tid, memory_order_acquire))
      continue;  // undead thread that should stop existing soon
    dll_remove(&_pthread_list, e);
    dll_make_first(&list, e);
  }

  // code like pthread_exit() needs to call this in order to know if
  // it's appropriate to run exit() handlers however we really don't
  // want to have a thread exiting block on a bunch of __maps locks!
  // therefore we only take action if we'll destroy all but the self
  if (annihilation_only)
    if (!(_pthread_list == _pthread_list->prev &&
          _pthread_list == _pthread_list->next)) {
      dll_make_last(&_pthread_list, list);
      list = 0;
    }

  // release posix threads gil
  _pthread_unlock();

  // now free our thread local batch of zombies
  // because death is a release and not a punishment
  // this is advantaged by not holding locks over munmap
  while ((e = dll_first(list))) {
    pt = POSIXTHREAD_CONTAINER(e);
    dll_remove(&list, e);
    _pthread_free(pt);
  }
}

static int PosixThread(void *arg, int tid) {
  void *rc;
  struct PosixThread *pt = arg;

  // setup scheduling
  if (pt->pt_attr.__inheritsched == PTHREAD_EXPLICIT_SCHED) {
    unassert(_weaken(_pthread_reschedule));
    _weaken(_pthread_reschedule)(pt);  // yoinked by attribute builder
  }

  // setup signal stack
  if (pt->pt_attr.__sigaltstacksize) {
    struct sigaltstack ss;
    ss.ss_sp = pt->pt_attr.__sigaltstackaddr;
    ss.ss_size = pt->pt_attr.__sigaltstacksize;
    ss.ss_flags = 0;
    unassert(!sigaltstack(&ss, 0));
  }

  // set long jump handler so pthread_exit can bring control back here
  if (!setjmp(pt->pt_exiter)) {
    sigdelset(&pt->pt_attr.__sigmask, SIGTHR);
    if (IsWindows() || IsMetal()) {
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

static bool TellOpenbsdThisIsStackMemory(void *addr, size_t size) {
  return __sys_mmap(
             addr, size, PROT_READ | PROT_WRITE,
             MAP_PRIVATE | MAP_FIXED | MAP_ANON_OPENBSD | MAP_STACK_OPENBSD, -1,
             0, 0) == addr;
}

// OpenBSD only permits RSP to occupy memory that's been explicitly
// defined as stack memory, i.e. `lo <= %rsp < hi` must be the case
static errno_t FixupCustomStackOnOpenbsd(pthread_attr_t *attr) {

  // get interval
  uintptr_t lo = (uintptr_t)attr->__stackaddr;
  uintptr_t hi = lo + attr->__stacksize;

  // squeeze interval
  lo = (lo + __pagesize - 1) & -__pagesize;
  hi = hi & -__pagesize;

  // tell os it's stack memory
  errno_t olderr = errno;
  if (!TellOpenbsdThisIsStackMemory((void *)lo, hi - lo)) {
    errno_t err = errno;
    errno = olderr;
    return err;
  }

  // update attributes with usable stack address
  attr->__stackaddr = (void *)lo;
  attr->__stacksize = hi - lo;
  return 0;
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
  pt->pt_locale = &__global_locale;
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
        _pthread_free(pt);
        return rc;
      }
    }
  } else {
    // cosmo is managing the stack
    int pagesize = __pagesize;
    pt->pt_attr.__guardsize = ROUNDUP(pt->pt_attr.__guardsize, pagesize);
    pt->pt_attr.__stacksize = pt->pt_attr.__stacksize;
    if (pt->pt_attr.__guardsize + pagesize > pt->pt_attr.__stacksize) {
      _pthread_free(pt);
      return EINVAL;
    }
    pt->pt_attr.__stackaddr =
        mmap(0, pt->pt_attr.__stacksize, PROT_READ | PROT_WRITE,
             MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (pt->pt_attr.__stackaddr != MAP_FAILED) {
      if (IsOpenbsd())
        if (!TellOpenbsdThisIsStackMemory(pt->pt_attr.__stackaddr,
                                          pt->pt_attr.__stacksize))
          notpossible;
      if (pt->pt_attr.__guardsize)
        if (mprotect(pt->pt_attr.__stackaddr, pt->pt_attr.__guardsize,
                     PROT_NONE | PROT_GUARD))
          notpossible;
    }
    if (!pt->pt_attr.__stackaddr || pt->pt_attr.__stackaddr == MAP_FAILED) {
      rc = errno;
      _pthread_free(pt);
      errno = e;
      if (rc == EINVAL || rc == EOVERFLOW) {
        return EINVAL;
      } else {
        return EAGAIN;
      }
    }
    pt->pt_flags |= PT_OWNSTACK;
  }

  // setup signal stack
  if (pt->pt_attr.__sigaltstacksize) {
    if (!pt->pt_attr.__sigaltstackaddr) {
      if (!(pt->pt_attr.__sigaltstackaddr =
                malloc(pt->pt_attr.__sigaltstacksize))) {
        _pthread_free(pt);
        return errno;
      }
      pt->pt_flags |= PT_OWNSIGALTSTACK;
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
      _pthread_free(pt);
      return EINVAL;
  }

  // add thread to global list
  // we add it to the beginning since zombies go at the end
  _pthread_lock();
  dll_make_first(&_pthread_list, &pt->list);
  _pthread_unlock();

  // launch PosixThread(pt) in new thread
  if ((rc = clone(PosixThread, pt->pt_attr.__stackaddr, pt->pt_attr.__stacksize,
                  CLONE_VM | CLONE_THREAD | CLONE_FS | CLONE_FILES |
                      CLONE_SIGHAND | CLONE_SYSVSEM | CLONE_SETTLS |
                      CLONE_PARENT_SETTID | CLONE_CHILD_SETTID |
                      CLONE_CHILD_CLEARTID,
                  pt, &pt->ptid, __adj_tls(pt->tib), &pt->tib->tib_tid))) {
    _pthread_lock();
    dll_remove(&_pthread_list, &pt->list);
    _pthread_unlock();
    _pthread_free(pt);
    return rc;
  }

  *thread = (pthread_t)pt;
  return 0;
}

static const char *DescribeHandle(char buf[12], errno_t err, pthread_t *th) {
  if (err)
    return "n/a";
  if (!th)
    return "NULL";
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
  _pthread_decimate(false);
  BLOCK_SIGNALS;
  err = pthread_create_impl(thread, attr, start_routine, arg, _SigMask);
  ALLOW_SIGNALS;
  STRACE("pthread_create([%s], %p, %t, %p) → %s",
         DescribeHandle(alloca(12), err, thread), attr, start_routine, arg,
         DescribeErrno(err));
  return err;
}
