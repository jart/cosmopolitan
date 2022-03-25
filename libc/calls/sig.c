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
#include "libc/bits/bits.h"
#include "libc/calls/calls.h"
#include "libc/calls/internal.h"
#include "libc/calls/sig.internal.h"
#include "libc/calls/sigbits.h"
#include "libc/calls/strace.internal.h"
#include "libc/calls/struct/sigset.h"
#include "libc/calls/typedef/sigaction_f.h"
#include "libc/macros.internal.h"
#include "libc/runtime/internal.h"
#include "libc/runtime/runtime.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/sa.h"
#include "libc/sysv/consts/sig.h"
#include "libc/sysv/errfuns.h"

/**
 * @fileoverview UNIX signals for the New Technology.
 * @threadsafe
 */

#define LOCK                                     \
  for (;;)                                       \
    if (lockcmpxchg(&__sig.lock, false, true)) { \
    asm volatile("" ::: "memory")

#define UNLOCK                   \
  asm volatile("" ::: "memory"); \
  __sig.lock = false;            \
  break;                         \
  }                              \
  else sched_yield()

struct Signal {
  struct Signal *next;
  bool used;
  int sig;
  int si_code;
};

struct Signals {
  bool lock;
  sigset_t mask;
  struct Signal *queue;
  struct Signal mem[__SIG_QUEUE_LENGTH];
};

struct Signals __sig;

/**
 * Allocates piece of memory for storing pending signal.
 */
static textwindows struct Signal *__sig_alloc(void) {
  int i;
  for (i = 0; i < ARRAYLEN(__sig.mem); ++i) {
    if (!__sig.mem[i].used && lockcmpxchg(&__sig.mem[i].used, false, true)) {
      return __sig.mem + i;
    }
  }
  return 0;
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
    LOCK;
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
        STRACE("%s is masked", strsignal(res->sig));
      }
    }
    UNLOCK;
  } else {
    res = 0;
  }
  return res;
}

/**
 * Delivers signal.
 * @note called from main thread
 * @return true if EINTR should be returned by caller
 */
static textwindows bool __sig_deliver(bool restartable, struct Signal *sig,
                                      unsigned rva) {
  unsigned flags;
  siginfo_t info;
  flags = __sighandflags[sig->sig];
  // TODO(jart): polyfill prevention of re-entry
  if (flags & SA_RESETHAND) {
    __sighandrvas[sig->sig] = (int32_t)(intptr_t)SIG_DFL;
  }
  STRACE("delivering %s", strsignal(sig->sig));
  bzero(&info, sizeof(info));
  info.si_signo = sig->sig;
  info.si_code = sig->si_code;
  ((sigaction_f)(_base + rva))(sig->sig, &info, 0);
  if (!restartable) {
    return true;  // always send EINTR for wait4(), poll(), etc.
  } else if (flags & SA_RESTART) {
    STRACE("restarting syscall on %s", strsignal(sig->sig));
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
 * Enqueues generic signal for delivery on New Technology.
 * @threadsafe
 */
textwindows int __sig_add(int sig, int si_code) {
  struct Signal *mem;
  if (1 <= sig && sig <= NSIG) {
    if ((mem = __sig_alloc())) {
      mem->sig = sig;
      mem->si_code = si_code;
      LOCK;
      mem->next = __sig.queue;
      __sig.queue = mem;
      UNLOCK;
      return 0;
    } else {
      return enomem();
    }
  } else {
    return einval();
  }
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
    switch ((rva = __sighandrvas[sig->sig])) {
      case (intptr_t)SIG_DFL:
        if (__sig_isfatal(sig->sig)) {
          STRACE("terminating on %s", strsignal(sig->sig));
          __restorewintty();
          _Exit(128 + sig->sig);
        }
        // fallthrough
      case (intptr_t)SIG_IGN:
        STRACE("ignoring %s", strsignal(sig->sig));
        break;
      default:
        delivered = __sig_deliver(restartable, sig, rva);
        break;
    }
    __sig_free(sig);
  }
  return delivered;
}

/**
 * Changes signal mask for main thread.
 * @return old mask
 */
textwindows sigset_t __sig_mask(const sigset_t *neu) {
  sigset_t old;
  LOCK;
  old = __sig.mask;
  if (neu) __sig.mask = *neu;
  UNLOCK;
  return old;
}
