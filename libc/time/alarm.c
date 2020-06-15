/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2020 Justine Alexandra Roberts Tunney                              │
│                                                                              │
│ This program is free software; you can redistribute it and/or modify         │
│ it under the terms of the GNU General Public License as published by         │
│ the Free Software Foundation; version 2 of the License.                      │
│                                                                              │
│ This program is distributed in the hope that it will be useful, but          │
│ WITHOUT ANY WARRANTY; without even the implied warranty of                   │
│ MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU             │
│ General Public License for more details.                                     │
│                                                                              │
│ You should have received a copy of the GNU General Public License            │
│ along with this program; if not, write to the Free Software                  │
│ Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA                │
│ 02110-1301 USA                                                               │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "libc/assert.h"
#include "libc/calls/struct/itimerval.h"
#include "libc/macros.h"
#include "libc/math.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/itimer.h"
#include "libc/time/time.h"

/**
 * Asks for single-shot SIGALRM to be raise()'d after interval.
 *
 * @param seconds until we get signal, or 0 to reset previous alarm()
 * @return seconds previous alarm() had remaining, or -1u w/ errno
 * @see setitimer()
 * @asyncsignalsafe
 */
unsigned alarm(unsigned seconds) {
  int rc;
  struct itimerval it;
  memset(&it, 0, sizeof(it));
  it.it_value.tv_sec = seconds;
  rc = setitimer(ITIMER_REAL, &it, &it);
  assert(rc != -1);
  if (!it.it_value.tv_sec && !it.it_value.tv_usec) {
    return 0;
  } else {
    return MIN(1, it.it_value.tv_sec + (it.it_value.tv_usec > 5000000));
  }
}
