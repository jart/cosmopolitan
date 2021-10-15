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
#include "libc/bits/likely.h"
#include "libc/calls/internal.h"
#include "libc/calls/sysdebug.internal.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/intrin/asan.internal.h"
#include "libc/log/libfatal.internal.h"
#include "libc/macros.internal.h"
#include "libc/runtime/directmap.internal.h"
#include "libc/runtime/memtrack.internal.h"
#include "libc/runtime/runtime.h"
#include "libc/str/str.h"
#include "libc/sysv/errfuns.h"

#define IP(X)      (intptr_t)(X)
#define SMALL(n)   ((n) <= 0xffffffffffff)
#define ALIGNED(p) (!(IP(p) & (FRAMESIZE - 1)))
#define ADDR(x)    ((int64_t)((uint64_t)(x) << 32) >> 16)
#define SHADE(x)   (((intptr_t)(x) >> 3) + 0x7fff8000)
#define FRAME(x)   ((int)((intptr_t)(x) >> 16))

/**
 * Releases memory pages.
 *
 * This function may be used to punch holes in existing mappings, but
 * your mileage may vary on Windows.
 *
 * @param p is a pointer within any memory mapped region the process
 *     has permission to control, such as address ranges returned by
 *     mmap(), the program image itself, etc.
 * @param n is the number of bytes to be unmapped, and needs to be a
 *     multiple of FRAMESIZE for anonymous mappings, because windows
 *     and for files size needs to be perfect to the byte bc openbsd
 * @return 0 on success, or -1 w/ errno
 */
noasan int munmap(void *v, size_t n) {
  /* asan runtime depends on this function */
  int rc;
  char poison, *p = v;
  intptr_t a, b, x, y;
  if (UNLIKELY(!n)) {
    SYSDEBUG("munmap(0x%p, 0x%x) %s (n=0)", p, n);
    return einval();
  }
  if (UNLIKELY(!SMALL(n))) {
    SYSDEBUG("munmap(0x%p, 0x%x) EINVAL (n isn't 48-bit)", p, n);
    return einval();
  }
  if (UNLIKELY(!IsLegalPointer(p))) {
    SYSDEBUG("munmap(0x%p, 0x%x) EINVAL (p isn't 48-bit)", p, n);
    return einval();
  }
  if (UNLIKELY(!IsLegalPointer(p + (n - 1)))) {
    SYSDEBUG("munmap(0x%p, 0x%x) EINVAL (p+(n-1) isn't 48-bit)", p, n);
    return einval();
  }
  if (UNLIKELY(!ALIGNED(p))) {
    SYSDEBUG("munmap(0x%p, 0x%x) EINVAL (p isn't 64kb aligned)", p, n);
    return einval();
  }
  if (!IsMemtracked(FRAME(p), FRAME(p + (n - 1)))) {
    SYSDEBUG("munmap(0x%p, 0x%x) EFAULT (interval not tracked)", p, n);
    return efault();
  }
  if (UntrackMemoryIntervals(p, n) != -1) {
    if (!IsWindows()) {
      rc = sys_munmap(p, n);
      if (rc != -1) {
        if (IsAsan() && !OverlapsShadowSpace(p, n)) {
          a = SHADE(p);
          b = a + (n >> 3);
          if (IsMemtracked(FRAME(a), FRAME(b - 1))) {
            x = ROUNDUP(a, FRAMESIZE);
            y = ROUNDDOWN(b, FRAMESIZE);
            if (x < y) {
              /* delete shadowspace if unmapping ≥512kb */
              __repstosb((void *)a, kAsanUnmapped, x - a);
              munmap((void *)x, y - x);
              __repstosb((void *)y, kAsanUnmapped, b - y);
            } else {
              /* otherwise just poison and assume reuse */
              __repstosb((void *)a, kAsanUnmapped, b - a);
            }
          } else {
            SYSDEBUG("unshadow(0x%x, 0x%x) EFAULT", a, b - a);
          }
        }
      }
    } else {
      rc = 0; /* UntrackMemoryIntervals does it for NT */
    }
  } else {
    rc = -1;
  }
  SYSDEBUG("munmap(0x%p, 0x%x) -> %d %s", p, n, (long)rc,
           rc == -1 ? strerror(errno) : "");
  return rc;
}
