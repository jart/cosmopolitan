/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
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
#include "libc/calls/struct/timespec.h"
#include "libc/errno.h"
#include "libc/sysv/consts/clock.h"
#include "libc/sysv/consts/utime.h"
#include "libc/sysv/errfuns.h"
#include "libc/time/time.h"

/**
 * Sleeps for particular number of microseconds.
 *
 * @return 0 on success, or -1 w/ errno
 * @raise EINTR if a signal was delivered while sleeping
 * @raise ECANCELED if thread was cancelled in masked mode
 * @see clock_nanosleep()
 * @cancelationpoint
 * @norestart
 */
int usleep(uint64_t micros) {
  errno_t err;
  struct timespec ts = timespec_frommicros(micros);
  err = clock_nanosleep(CLOCK_REALTIME, 0, &ts, 0);
  if (err) return errno = err, -1;
  return 0;
}
