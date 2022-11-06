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
#include "libc/assert.h"
#include "libc/calls/struct/timespec.h"
#include "libc/errno.h"
#include "libc/limits.h"
#include "libc/sysv/consts/clock.h"
#include "libc/time/time.h"

/**
 * Sleeps for particular number of seconds.
 *
 * @return 0 if the full time elapsed, otherwise we assume an interrupt
 *     was delivered, in which case the errno condition is ignored, and
 *     this function shall return the number of unslept seconds rounded
 *     using the ceiling function, and finally `-1u` may be returned if
 *     thread was cancelled with `PTHREAD_CANCEL_MASKED` in play
 * @see clock_nanosleep()
 * @cancellationpoint
 * @asyncsignalsafe
 * @norestart
 */
unsigned sleep(unsigned seconds) {
  errno_t rc;
  unsigned unslept;
  struct timespec tv = {seconds};
  if (!(rc = clock_nanosleep(CLOCK_REALTIME, 0, &tv, &tv))) return 0;
  if (rc == ECANCELED) return -1u;
  _npassert(rc == EINTR);
  unslept = tv.tv_sec;
  if (tv.tv_nsec && unslept < UINT_MAX) {
    ++unslept;
  }
  return unslept;
}
