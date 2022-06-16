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
#include "libc/calls/calls.h"
#include "libc/calls/strace.internal.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/intrin/asan.internal.h"
#include "libc/runtime/runtime.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/futex.h"
#include "libc/sysv/consts/nr.h"
#include "libc/thread/thread.h"

/**
 * Waits for thread to terminate and frees its memory.
 *
 * @param td is thread descriptor memory
 * @param exitcode optionally receives value returned by thread
 * @return 0 on success, or error number on failure
 * @raises EDEADLK when trying to join this thread
 * @raises EINVAL if another thread is joining
 * @raises ESRCH if no such thread exists
 * @raises EINVAL if not joinable
 * @threadsafe
 */
int cthread_join(cthread_t td, void **exitcode) {
  int x, rc, tid;
  // otherwise, tid could be set to 0 even though `state` is not
  // finished mark thread as joining
  if (!td || (IsAsan() && !__asan_is_valid(td, sizeof(*td)))) {
    rc = ESRCH;
    tid = -1;
  } else if ((tid = td->tid) == gettid()) {  // tid must load before lock xadd
    rc = EDEADLK;
  } else if (atomic_load(&td->state) & (cthread_detached | cthread_joining)) {
    rc = EINVAL;
  } else {
    if (~atomic_fetch_add(&td->state, cthread_joining) & cthread_finished) {
      while ((x = atomic_load(&td->tid))) {
        // FUTEX_WAIT_PRIVATE makes it hang
        cthread_memory_wait32(&td->tid, x, 0);
      }
    }
    if (exitcode) {
      *exitcode = td->exitcode;
    }
    if (!munmap(td->alloc.bottom, td->alloc.top - td->alloc.bottom)) {
      rc = 0;
    } else {
      rc = errno;
    }
  }
  STRACE("cthread_join(%d, [%p]) → %s", tid, !rc && exitcode ? *exitcode : 0,
         !rc ? "0" : strerrno(rc));
  return rc;
}
