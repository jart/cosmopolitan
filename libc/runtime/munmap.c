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
#include "libc/calls/state.internal.h"
#include "libc/calls/syscall-sysv.internal.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/intrin/asan.internal.h"
#include "libc/intrin/directmap.internal.h"
#include "libc/intrin/likely.h"
#include "libc/intrin/strace.internal.h"
#include "libc/log/backtrace.internal.h"
#include "libc/log/libfatal.internal.h"
#include "libc/log/log.h"
#include "libc/macros.internal.h"
#include "libc/runtime/internal.h"
#include "libc/runtime/memtrack.internal.h"
#include "libc/runtime/runtime.h"
#include "libc/str/str.h"
#include "libc/sysv/errfuns.h"

#define IP(X)      (intptr_t)(X)
#define ALIGNED(p) (!(IP(p) & (FRAMESIZE - 1)))
#define FRAME(x)   ((int)((intptr_t)(x) >> 16))

static void __munmap_shadow(char *p, size_t n) {
  intptr_t a, b, x, y;
  KERNTRACE("__munmap_shadow(%p, %'zu)", p, n);
  a = ((intptr_t)p >> 3) + 0x7fff8000;
  b = a + (n >> 3);
  if (IsMemtracked(FRAME(a), FRAME(b - 1))) {
    x = ROUNDUP(a, FRAMESIZE);
    y = ROUNDDOWN(b, FRAMESIZE);
    if (0 && x < y) {
      // delete shadowspace if unmapping ≥512kb. in practice it has
      // to be >1mb since we can only unmap it if it's aligned, and
      // as such we poison the edges if there are any.
      __repstosb((void *)a, kAsanUnmapped, x - a);
      __munmap_unlocked((void *)x, y - x);
      __repstosb((void *)y, kAsanUnmapped, b - y);
    } else {
      // otherwise just poison and assume reuse
      __repstosb((void *)a, kAsanUnmapped, b - a);
    }
  } else {
    STRACE("unshadow(%.12p, %p) EFAULT", a, b - a);
  }
}

// our api supports doing things like munmap(0, 0x7fffffffffff) but some
// platforms (e.g. openbsd) require that we know the specific intervals
// or else it returns EINVAL. so we munmap a piecewise.
static void __munmap_impl(char *p, size_t n) {
  char *q;
  size_t m;
  intptr_t a, b, c;
  int i, l, r, beg, end;
  KERNTRACE("__munmap_impl(%p, %'zu)", p, n);
  l = FRAME(p);
  r = FRAME(p + n - 1);
  i = __find_memory(&_mmi, l);
  for (; i < _mmi.i && r >= _mmi.p[i].x; ++i) {
    if (l >= _mmi.p[i].x && r <= _mmi.p[i].y) {

      // it's contained within the entry
      beg = l;
      end = r;
    } else if (l <= _mmi.p[i].x && r >= _mmi.p[i].x) {

      // it overlaps with the lefthand side of the entry
      beg = _mmi.p[i].x;
      end = MIN(r, _mmi.p[i].y);
    } else if (l <= _mmi.p[i].y && r >= _mmi.p[i].y) {

      // it overlaps with the righthand side of the entry
      beg = MAX(_mmi.p[i].x, l);
      end = _mmi.p[i].y;
    } else {
      __builtin_unreachable();
    }
    // openbsd even requires that if we mapped, for instance a 5 byte
    // file, that we be sure to call munmap(file, 5). let's abstract!
    a = ADDR_32_TO_48(beg);
    b = ADDR_32_TO_48(end) + FRAMESIZE;
    c = ADDR_32_TO_48(_mmi.p[i].x) + _mmi.p[i].size;
    q = (char *)a;
    m = MIN(b, c) - a;
    if (!IsWindows()) {
      npassert(!sys_munmap(q, m));
    } else {
      // Handled by __untrack_memories() on Windows
    }
    if (IsAsan() && !OverlapsShadowSpace(p, n)) {
      __munmap_shadow(q, m);
    }
  }
}

int __munmap_unlocked(char *p, size_t n) {
  unassert(!__vforked);
  if (UNLIKELY(!n)) {
    STRACE("munmap n is 0");
    return einval();
  }
  if (UNLIKELY(!IsLegalSize(n))) {
    STRACE("munmap n isn't 48-bit");
    return einval();
  }
  if (UNLIKELY(!IsLegalPointer(p))) {
    STRACE("munmap p isn't 48-bit");
    return einval();
  }
  if (UNLIKELY(!IsLegalPointer(p + (n - 1)))) {
    STRACE("munmap p+(n-1) isn't 48-bit");
    return einval();
  }
  if (UNLIKELY(!ALIGNED(p))) {
    STRACE("munmap(%p) isn't 64kb aligned", p);
    return einval();
  }
  __munmap_impl(p, n);
  return __untrack_memories(p, n);
}

/**
 * Releases memory pages.
 *
 * @param p is the beginning of the memory region to unmap
 * @param n is the number of bytes to be unmapped
 * @return 0 on success, or -1 w/ errno
 * @raises EINVAL if `n == 0`
 * @raises EINVAL if `n` isn't 48-bit
 * @raises EINVAL if `p+(n-1)` isn't 48-bit
 * @raises EINVAL if `p` isn't 65536-byte aligned
 */
int munmap(void *p, size_t n) {
  int rc;
  __mmi_lock();
  rc = __munmap_unlocked(p, n);
  size_t toto = __strace > 0 ? __get_memtrack_size(&_mmi) : 0;
  __mmi_unlock();
  STRACE("munmap(%.12p, %'zu) → %d% m (%'zu bytes total)", p, n, rc, toto);
  return rc;
}
