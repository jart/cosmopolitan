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
#include "libc/calls/syscall-sysv.internal.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/intrin/atomic.h"
#include "libc/limits.h"
#include "libc/runtime/syslib.internal.h"
#include "libc/sysv/errfuns.h"
#include "libc/thread/semaphore.h"

/**
 * Locks semaphore only if it's currently not locked.
 *
 * @return 0 on success, or -1 w/ errno
 * @raise EAGAIN if semaphore is locked
 * @raise EDEADLK if deadlock was detected
 * @raise EINVAL if `sem` is invalid
 */
int sem_trywait(sem_t *sem) {
  int v;

#if 0
  if (IsXnuSilicon() && sem->sem_magic == SEM_MAGIC_KERNEL) {
    return _sysret(__syslib->__sem_trywait(sem->sem_kernel));
  }
#endif

  v = atomic_load_explicit(&sem->sem_value, memory_order_relaxed);
  do {
    unassert(v > INT_MIN);
    if (!v) return eagain();
    if (v < 0) return einval();
  } while (!atomic_compare_exchange_weak_explicit(
      &sem->sem_value, &v, v - 1, memory_order_acquire, memory_order_relaxed));
  return 0;
}
