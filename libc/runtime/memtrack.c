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
#include "libc/bits/bits.h"
#include "libc/bits/weaken.h"
#include "libc/calls/calls.h"
#include "libc/calls/sysdebug.internal.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/intrin/asan.internal.h"
#include "libc/log/libfatal.internal.h"
#include "libc/macros.internal.h"
#include "libc/mem/mem.h"
#include "libc/runtime/directmap.internal.h"
#include "libc/runtime/memtrack.internal.h"
#include "libc/runtime/runtime.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/prot.h"
#include "libc/sysv/errfuns.h"

static noasan void *MoveMemoryIntervals(struct MemoryInterval *d,
                                        const struct MemoryInterval *s, int n) {
  /* asan runtime depends on this function */
  int i;
  assert(n >= 0);
  if (d > s) {
    for (i = n; i--;) {
      d[i] = s[i];
    }
  } else {
    for (i = 0; i < n; ++i) {
      d[i] = s[i];
    }
  }
  return d;
}

static noasan void RemoveMemoryIntervals(struct MemoryIntervals *mm, int i,
                                         int n) {
  /* asan runtime depends on this function */
  assert(i >= 0);
  assert(i + n <= mm->i);
  MoveMemoryIntervals(mm->p + i, mm->p + i + n, mm->i - (i + n));
  mm->i -= n;
}

static noasan void MapNewMappingArray(struct MemoryIntervals *mm) {
  /* asan runtime depends on this function */
  void *a;
  int i, x, y, g;
  size_t n, m, f;
  int prot, flags;
  struct DirectMap dm;
  struct MemoryInterval *p, *q;
  assert(mm->i);
  assert(AreMemoryIntervalsOk(mm));
  n = mm->n;
  n = n * sizeof(*mm->p);
  n = ROUNDUP(n, FRAMESIZE);
  if (mm->p == mm->s) {
    m = n;
    q = 0;
  } else {
    q = mm->p;
    m = n + (n >> 1);
    m = ROUNDUP(m, FRAMESIZE);
  }
  /*
   * find a hole in the automap range
   * we pad the sides for easier insertion logic
   * only time this fails is MODE=tiny which makes no stack
   */
  i = 0;
  f = m >> 16;
  for (;;) {
    if (++i == mm->i || mm->p[i].x - mm->p[i - 1].y >= 1 + f + 1) {
      x = mm->p[i - 1].y + 1;
      y = x + f - 1;
      a = (void *)((intptr_t)((uint64_t)x << 32) >> 16);
      if (i == mm->i || (kAutomapStart <= (intptr_t)a &&
                         (intptr_t)a + m < kAutomapStart + kAutomapSize)) {
        break;
      }
    }
  }
  flags = MAP_ANONYMOUS | MAP_PRIVATE;
  prot = PROT_READ | PROT_WRITE;
  SYSDEBUG("MapNewMappingArray(0x%x, 0x%x) %d/%d/%d", a, m, i, mm->i, mm->n);
  dm = sys_mmap(a, m, prot, flags | MAP_FIXED, -1, 0);
  if ((p = dm.addr) != MAP_FAILED) {
    MoveMemoryIntervals(p, mm->p, i);
    MoveMemoryIntervals(p + i + 1, mm->p + i, mm->i - i);
    mm->i += 1;
    mm->n = m / sizeof(*mm->p);
    mm->p = p;
    mm->p[i].x = x;
    mm->p[i].y = y;
    mm->p[i].h = dm.maphandle;
    mm->p[i].prot = prot;
    mm->p[i].flags = flags;
    if (q) {
      munmap(q, n);
    }
    if (IsAsan()) {
      __asan_map_shadow((uintptr_t)a, m);
    }
    SYSDEBUG("MapNewMappingArray() succeeded");
  } else {
    SYSDEBUG("MapNewMappingArray() failed: %s", strerror(errno));
  }
  assert(AreMemoryIntervalsOk(mm));
}

noasan int CreateMemoryInterval(struct MemoryIntervals *mm, int i) {
  /* asan runtime depends on this function */
  int rc;
  rc = 0;
  assert(i >= 0);
  assert(i <= mm->i);
  assert(mm->n >= 0);
  if (mm->i == mm->n) {
    SYSDEBUG("CreateMemoryInterval() failed");
    return enomem();
  }
  MoveMemoryIntervals(mm->p + i + 1, mm->p + i, mm->i++ - i);
  return 0;
}

static noasan int PunchHole(struct MemoryIntervals *mm, int x, int y, int i) {
  if (mm->i == mm->n) return enomem();
  if (CreateMemoryInterval(mm, i) == -1) return -1;
  mm->p[i].y = x - 1;
  mm->p[i + 1].x = y + 1;
  return 0;
}

noasan int ReleaseMemoryIntervals(struct MemoryIntervals *mm, int x, int y,
                                  void wf(struct MemoryIntervals *, int, int)) {
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
    if (IsWindows() && wf) {
      wf(mm, l, r);
    }
    RemoveMemoryIntervals(mm, l, r - l + 1);
  }
  return 0;
}

noasan int TrackMemoryInterval(struct MemoryIntervals *mm, int x, int y, long h,
                               int prot, int flags) {
  /* asan runtime depends on this function */
  unsigned i;
  assert(y >= x);
  assert(AreMemoryIntervalsOk(mm));
  i = FindMemoryInterval(mm, x);
  if (i && x == mm->p[i - 1].y + 1 && h == mm->p[i - 1].h &&
      prot == mm->p[i - 1].prot && flags == mm->p[i - 1].flags) {
    mm->p[i - 1].y = y;
    if (i < mm->i && y + 1 == mm->p[i].x && h == mm->p[i].h &&
        prot == mm->p[i].prot && flags == mm->p[i].flags) {
      mm->p[i - 1].y = mm->p[i].y;
      RemoveMemoryIntervals(mm, i, 1);
    }
  } else if (i < mm->i && y + 1 == mm->p[i].x && h == mm->p[i].h &&
             prot == mm->p[i].prot && flags == mm->p[i].flags) {
    mm->p[i].x = x;
  } else {
    if (mm->i == mm->n) return enomem();
    if (CreateMemoryInterval(mm, i) == -1) return -1;
    mm->p[i].x = x;
    mm->p[i].y = y;
    mm->p[i].h = h;
    mm->p[i].prot = prot;
    mm->p[i].flags = flags;
    if (mm->i >= mm->n / 2) {
      MapNewMappingArray(mm);
    }
  }
  return 0;
}
