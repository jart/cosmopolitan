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
#include "libc/thread/sem.h"
#include "libc/thread/wait.h"
#include "libc/thread/yield.h"

#define CTHREAD_THREAD_VAL_BITS 32

static void pause(int attempt) {
  if (attempt < 16) {
    for (int i = 0; i < (1 << attempt); ++i) {
      asm("pause");
    }
  } else {
    cthread_yield();
  }
}

int cthread_sem_init(cthread_sem_t* sem, int count) {
  sem->linux.count = count;
  return 0;
}
int cthread_sem_destroy(cthread_sem_t* sem) {
  (void)sem;
  return 0;
}

int cthread_sem_signal(cthread_sem_t* sem) {
  uint64_t count;
  asm volatile("lock xadd\t%1, %0"
               : "+m"(sem->linux.count), "=r"(count)
               : "1"(1)
               : "cc");

  if ((count >> CTHREAD_THREAD_VAL_BITS)) {
    // WARNING: an offset of 4 bytes would be required on little-endian archs
    void* wait_address = &sem->linux.count;
    cthread_memory_wake32(wait_address, 1);
  }

  return 0;
}

int cthread_sem_wait_futex(cthread_sem_t* sem, const struct timespec* timeout) {
  uint64_t count;

  // record current thread as waiter
  asm volatile("lock xadd\t%1, %0"
               : "+m"(sem->linux.count), "=r"(count)
               : "1"((uint64_t)1 << CTHREAD_THREAD_VAL_BITS)
               : "cc");

  for (;;) {
    // try to acquire the semaphore, as well as remove itself from waiters
    while ((uint32_t)count > 0) {
      // without spin, we could miss a futex wake
      if (atomic_compare_exchange_weak(
              &sem->linux.count, count,
              count - 1 - ((uint64_t)1 << CTHREAD_THREAD_VAL_BITS))) {
        return 0;
      }
    }
    
    // WARNING: an offset of 4 bytes would be required on little-endian archs
    void* wait_address = &sem->linux.count;
    cthread_memory_wait32(wait_address, count, timeout);
    count = atomic_load(&sem->linux.count);
  }

  return 0;
}

int cthread_sem_wait_spin(cthread_sem_t* sem, uint64_t count, int spin,
                          const struct timespec* timeout) {
  // spin on pause
  for (int attempt = 0; attempt < spin; ++attempt) {
    //if ((count >> CTHREAD_THREAD_VAL_BITS) != 0) break;
    while ((uint32_t)count > 0) {
      // spin is useful if multiple waiters can acquire the semaphore at the same time
      if (atomic_compare_exchange_weak(&sem->linux.count, count, count - 1)) {
        return 0;
      }
    }
    pause(attempt);
  }
  
  return cthread_sem_wait_futex(sem, timeout);
}

int cthread_sem_wait(cthread_sem_t* sem, int spin,
                     const struct timespec* timeout) {
  uint64_t count = atomic_load(&sem->linux.count);

  // uncontended
  while ((uint32_t)count > 0) {
    // spin is useful if multiple waiters can acquire the semaphore at the same time
    if (atomic_compare_exchange_weak(&sem->linux.count, count, count - 1)) {
      return 0;
    }
  }
  
  return cthread_sem_wait_spin(sem, count, spin, timeout);
}
