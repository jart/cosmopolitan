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
#include "libc/bits/safemacros.internal.h"
#include "libc/calls/internal.h"
#include "libc/calls/kntprioritycombos.internal.h"
#include "libc/nt/process.h"
#include "libc/nt/runtime.h"
#include "libc/nt/thread.h"

static textwindows struct NtPriorityCombo findntprio(int nice) {
  size_t l, r, m;
  l = 0;
  r = kNtPriorityCombosLen;
  while (l < r) {
    m = (l + r) >> 1;
    if (kNtPriorityCombos[m].nice > nice) {
      r = m;
    } else {
      l = m + 1;
    }
  }
  return kNtPriorityCombos[max(0, l - 1)];
}

textwindows int setpriority$nt(int nice) {
  uint32_t tier;
  struct NtPriorityCombo p;
  p = findntprio(nice);
  tier = 1 << (p.lg2tier - 1);
  if (SetPriorityClass(GetCurrentProcess(), tier) &&
      SetThreadPriority(GetCurrentThread(), p.wut)) {
    return p.nice;
  } else {
    return __winerr();
  }
}
