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
#include "libc/dce.h"
#include "libc/macros.h"
#include "libc/runtime/memtrack.h"
#include "libc/runtime/runtime.h"
#include "libc/str/str.h"
#include "libc/sysv/errfuns.h"

static void RemoveMemoryIntervals(struct MemoryIntervals *mm, int i, int n) {
  assert(i >= 0);
  assert(i + n <= mm->i);
  memcpy(mm->p + i, mm->p + i + n,
         (intptr_t)(mm->p + mm->i) - (intptr_t)(mm->p + i + n));
  memcpy(mm->h + i, mm->h + i + n,
         (intptr_t)(mm->h + mm->i) - (intptr_t)(mm->h + i + n));
  mm->i -= n;
}

static void CreateMemoryInterval(struct MemoryIntervals *mm, int i) {
  assert(i >= 0);
  assert(i <= mm->i);
  assert(mm->i < ARRAYLEN(mm->p));
  memmove(mm->p + i + 1, mm->p + i,
          (intptr_t)(mm->p + mm->i) - (intptr_t)(mm->p + i));
  memmove(mm->h + i + 1, mm->h + i,
          (intptr_t)(mm->h + mm->i) - (intptr_t)(mm->h + i));
  ++mm->i;
}

static int PunchHole(struct MemoryIntervals *mm, int x, int y, int i) {
  if (mm->i == ARRAYLEN(mm->p)) return enomem();
  CreateMemoryInterval(mm, i);
  mm->p[i].y = x - 1;
  mm->p[i + 1].x = y + 1;
  return 0;
}

int ReleaseMemoryIntervals(struct MemoryIntervals *mm, int x, int y,
                           void wincb(struct MemoryIntervals *, int, int)) {
  unsigned l, r;
  assert(y >= x);
  assert(AreMemoryIntervalsOk(mm));
  if (!mm->i) return 0;
  l = FindMemoryInterval(mm, x);
  if (l == mm->i) return 0;
  if (!l && y < mm->p[l].x) return 0;
  if (y < mm->p[l].x) return 0;
  r = FindMemoryInterval(mm, y);
  if (r == mm->i || (r > l && y < mm->p[r].x)) --r;
  assert(r >= l);
  assert(x <= mm->p[r].y);
  if (l == r && x > mm->p[l].x && y < mm->p[l].y) {
    return PunchHole(mm, x, y, l);
  }
  if (x > mm->p[l].x && x <= mm->p[l].y) {
    assert(y >= mm->p[l].y);
    if (IsWindows()) return einval();
    mm->p[l].y = x - 1;
    assert(mm->p[l].x <= mm->p[l].y);
    ++l;
  }
  if (y >= mm->p[r].x && y < mm->p[r].y) {
    assert(x <= mm->p[r].x);
    if (IsWindows()) return einval();
    mm->p[r].x = y + 1;
    assert(mm->p[r].x <= mm->p[r].y);
    --r;
  }
  if (l <= r) {
    if (IsWindows() && wincb) {
      wincb(mm, l, r);
    }
    RemoveMemoryIntervals(mm, l, r - l + 1);
  }
  return 0;
}

int TrackMemoryInterval(struct MemoryIntervals *mm, int x, int y, long h) {
  unsigned i;
  assert(y >= x);
  assert(AreMemoryIntervalsOk(mm));
  i = FindMemoryInterval(mm, x);
  if (i && x == mm->p[i - 1].y + 1 && h == mm->h[i - 1]) {
    mm->p[i - 1].y = y;
    if (i < mm->i && y + 1 == mm->p[i].x && h == mm->h[i]) {
      mm->p[i - 1].y = mm->p[i].y;
      RemoveMemoryIntervals(mm, i, 1);
    }
  } else if (i < mm->i && y + 1 == mm->p[i].x && h == mm->h[i]) {
    mm->p[i].x = x;
  } else {
    if (mm->i == ARRAYLEN(mm->p)) return enomem();
    CreateMemoryInterval(mm, i);
    mm->p[i].x = x;
    mm->p[i].y = y;
    mm->h[i] = h;
  }
  return 0;
}
