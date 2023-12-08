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
#include "libc/sysv/consts/clock.h"
#include "libc/calls/struct/rusage.h"
#include "libc/calls/struct/timespec.h"
#include "libc/calls/struct/timeval.h"
#include "libc/errno.h"
#include "libc/sysv/consts/rusage.h"
#include "libc/time/time.h"

/**
 * Returns sum of CPU time consumed by current process since birth.
 *
 * This function provides a basic idea of how computationally expensive
 * your program is, in terms of both the userspace and kernel processor
 * resources it's hitherto consumed. Here's an example of how you might
 * display this information:
 *
 *     printf("consumed %g seconds of cpu time\n",
 *            (double)clock() / CLOCKS_PER_SEC);
 *
 * This function offers at best microsecond accuracy on all supported
 * platforms. Please note the reported values might be a bit chunkier
 * depending on the kernel scheduler sampling interval see `CLK_TCK`.
 *
 * @return units of CPU time consumed, where each unit's time length
 *     should be `1./CLOCKS_PER_SEC` seconds; Cosmopolitan currently
 *     returns the unit count in microseconds, i.e. `CLOCKS_PER_SEC`
 *     is hard-coded as 1000000. On failure this returns -1 / errno.
 * @raise ENOSYS should be returned currently if run on Bare Metal
 * @see clock_gettime() which polyfills this on Linux and BSDs
 * @see getrusage() which polyfills this on XNU and NT
 */
int64_t clock(void) {
  int e;
  struct rusage ru;
  struct timespec ts;
  e = errno;
  if (clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &ts)) {
    errno = e;
    if (getrusage(RUSAGE_SELF, &ru) != -1) {
      ts = timeval_totimespec(timeval_add(ru.ru_utime, ru.ru_stime));
    } else {
      return -1;
    }
  }
  // convert nanoseconds to microseconds w/ ceil rounding
  // this would need roughly ~7,019,309 years to overflow
  return ts.tv_sec * 1000000 + (ts.tv_nsec + 999) / 1000;
}
