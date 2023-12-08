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
#include "libc/assert.h"
#include "libc/calls/calls.h"
#include "libc/calls/struct/itimerval.h"
#include "libc/calls/struct/timeval.h"
#include "libc/sysv/consts/itimer.h"

/**
 * Asks for single-shot SIGALRM to be raise()'d after interval.
 *
 * @param seconds is how long to wait before raising SIGALRM (which will
 *     only happen once) or zero to clear any previously scheduled alarm
 * @return seconds that were remaining on the previously scheduled
 *     alarm, or zero if there wasn't one (failure isn't possible)
 * @see setitimer() for a more powerful api
 * @asyncsignalsafe
 */
unsigned alarm(unsigned seconds) {
  struct itimerval it, old;
  it.it_value = timeval_fromseconds(seconds);
  it.it_interval = timeval_zero;
  npassert(!setitimer(ITIMER_REAL, &it, &old));
  return timeval_toseconds(old.it_value);
}
