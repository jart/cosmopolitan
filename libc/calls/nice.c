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
#include "libc/bits/safemacros.h"
#include "libc/calls/calls.h"
#include "libc/conv/conv.h"
#include "libc/sysv/consts/prio.h"

static int clamp(int p) { return max(-NZERO, min(NZERO - 1, p)); }

/**
 * Changes process priority.
 *
 * @param delta is added to current priority w/ clamping
 * @return new priority, or -1 w/ errno
 * @see Linux claims ioprio_set() is tuned automatically by this
 */
int nice(int delta) {
  int p;
  if (abs(delta) >= NZERO * 2) {
    p = delta;
  } else {
    delta = clamp(delta);
    if ((p = getpriority(PRIO_PROCESS, 0)) == -1) return -1;
    p += delta;
  }
  p = clamp(p);
  if (setpriority(PRIO_PROCESS, 0, p) == -1) return -1;
  return p;
}
