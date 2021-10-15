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
#include "libc/bits/likely.h"
#include "libc/bits/weaken.h"
#include "libc/calls/calls.h"
#include "libc/calls/internal.h"
#include "libc/calls/sysdebug.internal.h"
#include "libc/dce.h"
#include "libc/intrin/asan.internal.h"
#include "libc/log/libfatal.internal.h"
#include "libc/macros.internal.h"
#include "libc/nt/runtime.h"
#include "libc/runtime/directmap.internal.h"
#include "libc/runtime/memtrack.internal.h"
#include "libc/runtime/runtime.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/mremap.h"
#include "libc/sysv/errfuns.h"

#define IP(X)      (intptr_t)(X)
#define VIP(X)     (void *)IP(X)
#define SMALL(n)   ((n) <= 0xffffffffffff)
#define ALIGNED(p) (!(IP(p) & (FRAMESIZE - 1)))
#define ADDR(x)    ((int64_t)((uint64_t)(x) << 32) >> 16)
#define SHADE(x)   (((intptr_t)(x) >> 3) + 0x7fff8000)
#define FRAME(x)   ((int)((intptr_t)(x) >> 16))

static size_t GetMapSize(size_t i, size_t *j) {
  size_t n;
  n = (size_t)(_mmi.p[i].y - _mmi.p[i].x + 1) << 16;
  while (i + 1 < _mmi.i) {
    if (_mmi.p[i + 1].x != _mmi.p[i].y + 1) break;
    ++i;
    n += (size_t)(_mmi.p[i].y - _mmi.p[i].x + 1) << 16;
  }
  *j = i;
  return n;
}

static bool MustMoveMap(intptr_t y, size_t j) {
  return ADDR(_mmi.p[j].y) + FRAMESIZE > y ||
         (j + 1 < _mmi.i && ADDR(_mmi.p[j + 1].x) < y);
}

/**
 * Extends and/or relocates memory pages.
 *
 * @param p is old address
 * @param n is old size
 * @param m is new size
 * @param f should have MREMAP_MAYMOVE and may have MAP_FIXED
 * @param q is new address
 */
void *mremap(void *p, size_t n, size_t m, int f, ... /* void *q */) {
  enosys();
  return MAP_FAILED;
  void *q;
  va_list va;
  size_t i, j, k;
  struct DirectMap dm;
  int a, b, prot, flags;
  if (UNLIKELY(!m)) {
    SYSDEBUG("mremap(0x%p, 0x%x, 0x%x, 0x%x) EINVAL (m=0)", p, n, m, f);
    return VIP(einval());
  }
  if (UNLIKELY(!n)) {
    SYSDEBUG("mremap(0x%p, 0x%x, 0x%x, 0x%x) EOPNOTSUPP (n=0)", p, n, m, f);
    return VIP(eopnotsupp());
  }
  if (UNLIKELY(!ALIGNED(n))) {
    SYSDEBUG("mremap(0x%p, 0x%x, 0x%x, 0x%x) EOPNOTSUPP (n align)", p, n, m, f);
    return VIP(eopnotsupp());
  }
  if (UNLIKELY(!ALIGNED(m))) {
    SYSDEBUG("mremap(0x%p, 0x%x, 0x%x, 0x%x) EOPNOTSUPP (n align)", p, n, m, f);
    return VIP(eopnotsupp());
  }
  if (UNLIKELY(!ALIGNED(p))) {
    SYSDEBUG("mremap(0x%p, 0x%x, 0x%x, 0x%x) EINVAL (64kb align)", p, n, m, f);
    return VIP(einval());
  }
  if (UNLIKELY(!SMALL(n))) {
    SYSDEBUG("mremap(0x%p, 0x%x, 0x%x, 0x%x) EINVAL (n too big)", p, n, m, f);
    return VIP(enomem());
  }
  if (UNLIKELY(!SMALL(m))) {
    SYSDEBUG("mremap(0x%p, 0x%x, 0x%x, 0x%x) EINVAL (m too big)", p, n, m, f);
    return VIP(enomem());
  }
  if (f & ~(MREMAP_MAYMOVE | MREMAP_FIXED)) {
    SYSDEBUG("mremap(0x%p, 0x%x, 0x%x, 0x%x) EINVAL (bad flag)", p, n, m, f);
    return VIP(einval());
  }
  if (!IsMemtracked(FRAME(p), FRAME((intptr_t)p + (n - 1)))) {
    SYSDEBUG("munmap(0x%x, 0x%x) EFAULT (interval not tracked)", p, n);
    return VIP(efault());
  }
  SYSDEBUG("mremap(0x%p, 0x%x, 0x%x, 0x%x)", p, n, m, f);
  i = FindMemoryInterval(&_mmi, FRAME(p));
  if (i >= _mmi.i) return VIP(efault());
  flags = _mmi.p[i].flags;
  if (!(flags & MAP_ANONYMOUS)) {
    return VIP(eopnotsupp()); /* TODO */
  }
  if (f & MREMAP_FIXED) {
    va_start(va, f);
    q = va_arg(va, void *);
    va_end(va);
    if (!ALIGNED(q)) return VIP(einval());
    return VIP(eopnotsupp()); /* TODO */
  }
  prot = _mmi.p[i].prot;
  for (k = i + 1; k <= j; ++k) {
    prot |= _mmi.p[k].prot;
    if (_mmi.p[k].flags != flags) {
      return VIP(enomem());
    }
  }
  if (m == n) {
    return p;
  } else if (m < n) {
    if (munmap((char *)p + n, m - n) != -1) {
      return p;
    } else {
      return MAP_FAILED;
    }
  } else if (!MustMoveMap(j, (intptr_t)p + n)) {
    dm = sys_mmap((char *)p + n, m - n, prot, flags | MAP_FIXED, -1, 0);
    if (dm.addr == MAP_FAILED) return 0;
    if (TrackMemoryInterval(&_mmi, ((uintptr_t)p + n) >> 16,
                            ((uintptr_t)p + m - FRAMESIZE) >> 16, dm.maphandle,
                            prot, flags) != -1) {
      if (weaken(__asan_map_shadow)) {
        weaken(__asan_map_shadow)((uintptr_t)dm.addr, m - n);
      }
      return p;
    } else {
      abort();
    }
  } else if (!(f & MREMAP_MAYMOVE)) {
    return VIP(enomem());
  } else if (IsLinux()) {
    a = (uint32_t)(kAutomapStart >> 16);
    i = FindMemoryInterval(&_mmi, a);
    if (i < _mmi.i) {
      for (; i + 1 < _mmi.i; ++i) {
        if (((size_t)(_mmi.p[i + 1].x - _mmi.p[i].y - 1) << 16) >= m) {
          break;
        }
      }
      if (__builtin_add_overflow(_mmi.p[i].y, 1, &a)) {
        return VIP(enomem());
      }
    }
    if (__builtin_add_overflow(a, (int)((m >> 16) - 1), &b)) {
      return VIP(enomem());
    }
    q = sys_mremap((void *)p, n, m, MREMAP_MAYMOVE | MREMAP_FIXED,
                   (void *)ADDR(a));
    SYSDEBUG("sys_mremap(0x%p, 0x%x, 0x%x, 0x%x, 0x%x) -> 0x%p", p, n, m,
             MREMAP_MAYMOVE | MREMAP_FIXED, ADDR(a));
    if (q == MAP_FAILED) return 0;
    if (ReleaseMemoryIntervals(&_mmi, (uintptr_t)p >> 16,
                               ((uintptr_t)p + n - FRAMESIZE) >> 16, 0) != -1 &&
        TrackMemoryInterval(&_mmi, a, b, -1, prot, flags) != -1) {
      if (weaken(__asan_poison)) {
        if (!OverlapsShadowSpace(p, n)) {
          weaken(__asan_poison)((intptr_t)p, n, kAsanUnmapped);
        }
        if (!OverlapsShadowSpace(q, m)) {
          weaken(__asan_map_shadow)((intptr_t)q, m);
        }
      }
      return (void *)ADDR(a);
    } else {
      abort();
    }
  } else if ((q = mmap(0, m, prot, flags, -1, 0)) != MAP_FAILED) {
    memcpy(q, p, n);
    munmap(p, n);
    return q;
  } else {
    return q;
  }
}
