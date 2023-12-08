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
#include "third_party/nsync/futex.internal.h"

/**
 * Unlocks semaphore.
 *
 * @return 0 on success, or -1 w/ errno
 * @raise EINVAL if `sem` isn't valid
 */
int sem_post(sem_t *sem) {
  int rc, old, wakeups;

#if 0
  if (IsXnuSilicon() && sem->sem_magic == SEM_MAGIC_KERNEL) {
    return _sysret(__syslib->__sem_post(sem->sem_kernel));
  }
#endif

  old = atomic_fetch_add_explicit(&sem->sem_value, 1, memory_order_acq_rel);
  unassert(old > INT_MIN);
  if (old >= 0) {
    wakeups = nsync_futex_wake_(&sem->sem_value, 1, true);
    npassert(wakeups >= 0);
    rc = 0;
  } else {
    wakeups = 0;
    rc = einval();
  }
  return rc;
}
