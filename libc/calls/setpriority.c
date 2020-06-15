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
#include "libc/calls/calls.h"
#include "libc/calls/internal.h"

/**
 * Sets nice value of thing.
 *
 * @param which can be PRIO_PROCESS, PRIO_PGRP, PRIO_USER
 * @param who is the pid, pgid, or uid, 0 meaning current
 * @param value ∈ [-NZERO,NZERO) which is clamped automatically
 * @return nonzero on success or -1 w/ errno
 * @see getpriority(), nice()
 */
int setpriority(int which, unsigned who, int value) {
  if (!IsWindows()) {
    return setpriority$sysv(which, who, value); /* TODO(jart): -20 */
  } else {
    return getsetpriority$nt(which, who, value, setpriority$nt);
  }
}
