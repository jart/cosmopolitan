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
#include "libc/intrin/atomic.h"
#include "libc/limits.h"
#include "libc/sysv/errfuns.h"
#include "libc/thread/semaphore.h"

/**
 * Destroys unnamed semaphore.
 *
 * If `sem` was successfully initialized by sem_init() then
 * sem_destroy() may be called multiple times, before it may be
 * reinitialized again by calling sem_init().
 *
 * Calling sem_destroy() on a semaphore created by sem_open() has
 * undefined behavior. Using `sem` after calling sem_destroy() is
 * undefined behavior that will cause semaphore APIs to either crash or
 * raise `EINVAL` until `sem` is passed to sem_init() again.
 *
 * @param sem was created by sem_init()
 * @return 0 on success, or -1 w/ errno
 * @raise EINVAL if `sem` wasn't valid
 * @raise EBUSY if `sem` has waiters
 */
int sem_destroy(sem_t *sem) {
  int waiters;
  npassert(sem->sem_magic != SEM_MAGIC_NAMED);
  if (sem->sem_magic != SEM_MAGIC_UNNAMED) return einval();
  waiters = atomic_load_explicit(&sem->sem_waiters, memory_order_relaxed);
  unassert(waiters >= 0);
  if (waiters) return ebusy();
  atomic_store_explicit(&sem->sem_value, INT_MIN, memory_order_relaxed);
  return 0;
}
