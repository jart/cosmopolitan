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
#include "libc/bits/asmflag.h"
#include "libc/bits/atomic.h"
#include "libc/calls/calls.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/intrin/pthread.h"
#include "libc/intrin/spinlock.h"
#include "libc/linux/futex.h"
#include "libc/nexgen32e/threaded.h"
#include "libc/sysv/consts/futex.h"
#include "libc/sysv/consts/nr.h"

static inline int FutexWait(void *addr, int expect, struct timespec *timeout) {
  int ax;
  bool cf;
  asm volatile(CFLAG_ASM("mov\t%6,%%r10\n\t"
                         "clc\n\t"
                         "syscall")
               : CFLAG_CONSTRAINT(cf), "=a"(ax)
               : "1"(__NR_futex), "D"(addr), "S"(FUTEX_WAIT), "d"(expect),
                 "g"(timeout)
               : "rcx", "r10", "r11", "memory");
  if (cf) ax = -ax;
  return ax;
}

static int pthread_mutex_lock_spin(pthread_mutex_t *mutex, int tries) {
  volatile int i;
  struct timespec ts;
  if (tries < 7) {
    for (i = 0; i != 1 << tries; i++) {
    }
    tries++;
  } else if (IsLinux() || IsOpenbsd()) {
    atomic_fetch_add(&mutex->waits, 1);
    if (tries < 28) {
      ts.tv_sec = 0;
      ts.tv_nsec = 4 << tries;
      tries++;
    } else {
      ts.tv_sec = 1;
      ts.tv_nsec = 0;
    }
    FutexWait(&mutex->lock, 1, &ts);
    atomic_fetch_sub(&mutex->waits, 1);
  } else {
    sched_yield();
  }
  return tries;
}

/**
 * Locks mutex.
 *
 *     _spinlock()         l:   181,570c    58,646ns
 *     mutex normal        l:   297,965c    96,241ns
 *     mutex recursive     l: 1,112,166c   359,223ns
 *     mutex errorcheck    l: 1,449,723c   468,252ns
 *
 * @return 0 on success, or error number on failure
 */
int(pthread_mutex_lock)(pthread_mutex_t *mutex) {
  int me, owner, tries;
  switch (mutex->attr) {
    case PTHREAD_MUTEX_NORMAL:
      for (tries = 0;;) {
        if (!atomic_load_explicit(&mutex->lock, memory_order_relaxed) &&
            !atomic_exchange_explicit(&mutex->lock, 1, memory_order_acquire)) {
          break;
        }
        tries = pthread_mutex_lock_spin(mutex, tries);
      }
      return 0;
    case PTHREAD_MUTEX_RECURSIVE:
    case PTHREAD_MUTEX_ERRORCHECK:
      for (tries = 0, me = gettid();;) {
        owner = atomic_load_explicit(&mutex->lock, memory_order_relaxed);
        if (!owner && atomic_compare_exchange_weak_explicit(
                          &mutex->lock, &owner, me, memory_order_acquire,
                          memory_order_relaxed)) {
          break;
        } else if (owner == me) {
          if (mutex->attr != PTHREAD_MUTEX_ERRORCHECK) {
            break;
          } else {
            return EDEADLK;
          }
        }
        tries = pthread_mutex_lock_spin(mutex, tries);
      }
      ++mutex->reent;
      return 0;
    default:
      return EINVAL;
  }
}
