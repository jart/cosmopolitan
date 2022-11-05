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
#include "libc/errno.h"
#include "libc/intrin/atomic.h"
#include "libc/macros.internal.h"
#include "libc/mem/mem.h"
#include "libc/thread/posixthread.internal.h"
#include "libc/thread/spawn.h"
#include "libc/thread/thread.h"

/**
 * Asks POSIX thread to free itself automatically on termination.
 *
 * @return 0 on success, or errno with error
 * @raise EINVAL if thread is null or already detached
 * @returnserrno
 * @threadsafe
 */
errno_t pthread_detach(pthread_t thread) {
  struct PosixThread *pt;
  enum PosixThreadStatus status, transition;
  if (!(pt = (struct PosixThread *)thread)) return EINVAL;
  for (;;) {
    status = atomic_load_explicit(&pt->status, memory_order_acquire);
    if (status == kPosixThreadDetached || status == kPosixThreadZombie) {
      // these two states indicate the thread was already detached, in
      // which case it's already listed under _pthread_zombies.
      return EINVAL;
    } else if (status == kPosixThreadJoinable) {
      transition = kPosixThreadDetached;
    } else if (status == kPosixThreadTerminated) {
      transition = kPosixThreadZombie;
    } else {
      notpossible;
    }
    if (atomic_compare_exchange_weak_explicit(&pt->status, &status, transition,
                                              memory_order_release,
                                              memory_order_relaxed)) {
      _pthread_zombies_add(pt);
      break;
    }
  }
  return 0;
}
