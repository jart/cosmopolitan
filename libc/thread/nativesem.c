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
#include "libc/bits/atomic.h"
#include "libc/sysv/consts/futex.h"
#include "libc/sysv/consts/nr.h"
#include "libc/thread/nativesem.h"
#include "libc/thread/yield.h"

#define CTHREAD_THREAD_VAL_BITS 32

int cthread_native_sem_init(cthread_native_sem_t* sem, int count) {
  sem->linux.count = count;
  return 0;
}
int cthread_native_sem_destroy(cthread_native_sem_t* sem) {
  (void)sem;
  return 0;
}

int cthread_native_sem_signal(cthread_native_sem_t* sem) {
  uint64_t count;
  asm volatile("lock xadd\t%1, %0"
               : "+m"(sem->linux.count), "=r"(count)
               : "1"(1)
               : "cc");

  if ((count >> CTHREAD_THREAD_VAL_BITS)) {
    int flags = FUTEX_WAKE;

    // WARNING: an offset of 4 bytes would be required on little-endian archs
    void* wait_address = &sem->linux.count;
    asm volatile("syscall"
                 : /* no outputs */
                 : "a"(__NR_futex), "D"(wait_address), "S"(flags), "d"(1)
                 : "rcx", "r11", "cc", "memory");
  }

  return 0;
}

int cthread_native_sem_wait_slow(cthread_native_sem_t* sem,
                                 const struct timespec* timeout) {
  uint64_t count;

  // record current thread as waiter
  asm volatile("lock xadd\t%1, %0"
               : "+m"(sem->linux.count), "=r"(count)
               : "1"((uint64_t)1 << CTHREAD_THREAD_VAL_BITS)
               : "cc");

  for (;;) {
    // try to acquire the semaphore, as well as remove itself from waiters
    if ((uint32_t)count > 0 &&
        atomic_compare_exchange_weak(
            &sem->linux.count, count,
            count - 1 - ((uint64_t)1 << CTHREAD_THREAD_VAL_BITS)))
      break;

    int flags = FUTEX_WAIT;
    register struct timespec* timeout_ asm("r10") = timeout;

    // WARNING: an offset of 4 bytes would be required on little-endian archs
    void* wait_address = &sem->linux.count;
    asm volatile("syscall"
                 : /* no outputs */
                 : "a"(__NR_futex), "D"(wait_address), "S"(flags), "d"(count),
                   "r"(timeout_)
                 : "rcx", "r11", "cc", "memory");
    count = atomic_load(&sem->linux.count);
  }

  return 0;
}

int cthread_native_sem_wait_spin_yield(cthread_native_sem_t* sem,
                                       uint64_t count, int yield,
                                       const struct timespec* timeout) {
  // spin on yield
  while (yield-- > 0) {
    if ((count >> CTHREAD_THREAD_VAL_BITS) != 0)
      break;  // a thread is already waiting in queue
    if ((uint32_t)count > 0 &&
        atomic_compare_exchange_weak(&sem->linux.count, count, count - 1))
      return 0;
    cthread_yield();
  }

  return cthread_native_sem_wait_slow(sem, timeout);
}

int cthread_native_sem_wait_spin(cthread_native_sem_t* sem, uint64_t count,
                                 int spin, int yield,
                                 const struct timespec* timeout) {
  // spin on pause
  while (spin-- > 0) {
    if ((count >> CTHREAD_THREAD_VAL_BITS) != 0) break;
    if ((uint32_t)count > 0 &&
        atomic_compare_exchange_weak(&sem->linux.count, count, count - 1))
      return 0;
    asm volatile("pause");
  }

  return cthread_native_sem_wait_spin_yield(sem, count, yield, timeout);
}

int cthread_native_sem_wait(cthread_native_sem_t* sem, int spin, int yield,
                            const struct timespec* timeout) {
  uint64_t count = atomic_load(&sem->linux.count);

  // uncontended
  if ((count >> 32) == 0 && (uint32_t)count > 0 &&
      atomic_compare_exchange_weak(&sem->linux.count, count, count - 1))
    return 0;

  return cthread_native_sem_wait_spin(sem, count, spin, yield, timeout);
}
