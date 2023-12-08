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
#include "libc/calls/blockcancel.internal.h"
#include "libc/calls/struct/timespec.h"
#include "libc/errno.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/clock.h"
#include "libc/thread/posixthread.internal.h"

/**
 * Sleeps for specified delay.
 *
 * This function may be canceled except when using masked mode in which
 * case cancelation is temporarily disabled, because there is no way to
 * report the ECANCELED state.
 *
 * @return unslept time which may be non-zero if the call was interrupted
 * @cancelationpoint
 */
struct timespec timespec_sleep(struct timespec delay) {
  int cs = -1;
  errno_t err;
  struct timespec remain;
  remain = timespec_zero;
  if (_pthread_self()->pt_flags & PT_MASKED) {
    cs = _pthread_block_cancelation();
  }
  if ((err = clock_nanosleep(CLOCK_REALTIME, 0, &delay, &remain))) {
    unassert(err == EINTR);
  }
  if (cs != -1) {
    _pthread_allow_cancelation(cs);
  }
  return remain;
}
