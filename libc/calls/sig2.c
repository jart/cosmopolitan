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
#include "libc/calls/sigbits.h"
#include "libc/calls/strace.internal.h"
#include "libc/intrin/cmpxchg.h"
#include "libc/intrin/spinlock.h"
#include "libc/macros.internal.h"
#include "libc/runtime/internal.h"
#include "libc/runtime/runtime.h"
#include "libc/sysv/consts/sa.h"
#include "libc/sysv/errfuns.h"

/**
 * @fileoverview UNIX signals for the New Technology, Part 2.
 * @threadsafe
 */

/**
 * Allocates piece of memory for storing pending signal.
 * @assume lock is held
 */
static textwindows struct Signal *__sig_alloc(void) {
  int i;
  struct Signal *res = 0;
  for (i = 0; i < ARRAYLEN(__sig.mem); ++i) {
    if (!__sig.mem[i].used) {
      __sig.mem[i].used = true;
      res = __sig.mem + i;
      break;
    }
  }
  return res;
}

/**
 * Returns signal memory to static pool.
 */
static textwindows void __sig_free(struct Signal *mem) {
  mem->used = false;
}

/**
 * Dequeues signal that isn't masked.
 * @return signal or null if empty or none unmasked
 */
static textwindows struct Signal *__sig_remove(void) {
  struct Signal *prev, *res;
  if (__sig.queue) {
    _spinlock(&__sig_lock);
    for (prev = 0, res = __sig.queue; res; prev = res, res = res->next) {
      if (!sigismember(&__sig.mask, res->sig)) {
        if (res == __sig.queue) {
          __sig.queue = res->next;
        } else if (prev) {
          prev->next = res->next;
        }
        res->next = 0;
        break;
      } else {
        STRACE("%G is masked", res->sig);
      }
    }
    _spunlock(&__sig_lock);
  } else {
    res = 0;
  }
  return res;
}

/**
 * Delivers signal to callback.
 * @note called from main thread
 * @return true if EINTR should be returned by caller
 */
static textwindows bool __sig_deliver(bool restartable, int sig, int si_code,
                                      ucontext_t *ctx) {
  unsigned rva, flags;
  siginfo_t info, *infop;
  STRACE("delivering %G", sig);

  // enter the signal
  _spinlock(&__sig_lock);
  rva = __sighandrvas[sig];
  flags = __sighandflags[sig];
  if ((~flags & SA_NODEFER) || (flags & SA_RESETHAND)) {
    // by default we try to avoid reentering a signal handler. for
    // example, if a sigsegv handler segfaults, then we'd want the
    // second signal to just kill the process. doing this means we
    // track state. that's bad if you want to longjmp() out of the
    // signal handler. in that case you must use SA_NODEFER.
    __sighandrvas[sig] = (int32_t)(intptr_t)SIG_DFL;
  }
  _spunlock(&__sig_lock);

  // setup the somewhat expensive information args
  // only if they're requested by the user in sigaction()
  if (flags & SA_SIGINFO) {
    bzero(&info, sizeof(info));
    info.si_signo = sig;
    info.si_code = si_code;
    infop = &info;
  } else {
    infop = 0;
    ctx = 0;
  }

  // handover control to user
  ((sigaction_f)(_base + rva))(sig, infop, ctx);

  if ((~flags & SA_NODEFER) && (~flags & SA_RESETHAND)) {
    // it's now safe to reenter the signal so we need to restore it.
    // since sigaction() is @asyncsignalsafe we only restore it if the
    // user didn't change it during the signal handler. we also don't
    // need to do anything if this was a oneshot signal or nodefer.
    _spinlock(&__sig_lock);
    _cmpxchg(__sighandrvas + sig, (int32_t)(intptr_t)SIG_DFL, rva);
    _spunlock(&__sig_lock);
  }

  if (!restartable) {
    return true;  // always send EINTR for wait4(), poll(), etc.
  } else if (flags & SA_RESTART) {
    STRACE("restarting syscall on %G", sig);
    return false;  // resume syscall for read(), write(), etc.
  } else {
    return true;  // default course is to raise EINTR
  }
}

/**
 * Returns true if signal default action is to end process.
 */
static textwindows bool __sig_isfatal(int sig) {
  return sig != SIGCHLD;
}

/**
 * Handles signal.
 *
 * @param restartable can be used to suppress true return if SA_RESTART
 * @return true if signal was delivered
 */
textwindows bool __sig_handle(bool restartable, int sig, int si_code,
                              ucontext_t *ctx) {
  bool delivered;
  switch (__sighandrvas[sig]) {
    case (intptr_t)SIG_DFL:
      if (__sig_isfatal(sig)) {
        STRACE("terminating on %G", sig);
        __restorewintty();
        _Exit(128 + sig);
      }
      // fallthrough
    case (intptr_t)SIG_IGN:
      STRACE("ignoring %G", sig);
      delivered = false;
      break;
    default:
      delivered = __sig_deliver(restartable, sig, si_code, ctx);
      break;
  }
  return delivered;
}

/**
 * Handles signal immediately if not blocked.
 *
 * @param restartable is for functions like read() but not poll()
 * @return true if EINTR should be returned by caller
 * @return 1 if delivered, 0 if enqueued, otherwise -1 w/ errno
 * @note called from main thread
 * @threadsafe
 */
textwindows int __sig_raise(int sig, int si_code) {
  int rc;
  int candeliver;
  _spinlock(&__sig_lock);
  candeliver = !sigismember(&__sig.mask, sig);
  _spunlock(&__sig_lock);
  switch (candeliver) {
    case 1:
      __sig_handle(false, sig, si_code, 0);
      return 0;
    case 0:
      STRACE("%G is masked", sig);
      return __sig_add(sig, si_code);
    default:
      return -1;  // sigismember() validates `sig`
  }
}

/**
 * Enqueues generic signal for delivery on New Technology.
 * @return 0 if enqueued, otherwise -1 w/ errno
 * @threadsafe
 */
textwindows int __sig_add(int sig, int si_code) {
  int rc;
  struct Signal *mem;
  if (1 <= sig && sig <= NSIG) {
    STRACE("enqueuing %G", sig);
    _spinlock(&__sig_lock);
    if ((mem = __sig_alloc())) {
      mem->sig = sig;
      mem->si_code = si_code;
      mem->next = __sig.queue;
      __sig.queue = mem;
      rc = 0;
    } else {
      rc = enomem();
    }
    _spunlock(&__sig_lock);
  } else {
    rc = einval();
  }
  return rc;
}

/**
 * Enqueues generic signal for delivery on New Technology.
 *
 * @param restartable is for functions like read() but not poll()
 * @return true if EINTR should be returned by caller
 * @note called from main thread
 * @threadsafe
 */
textwindows bool __sig_check(bool restartable) {
  unsigned rva;
  bool delivered;
  struct Signal *sig;
  delivered = false;
  while ((sig = __sig_remove())) {
    delivered |= __sig_handle(restartable, sig->sig, sig->si_code, 0);
    __sig_free(sig);
  }
  return delivered;
}
