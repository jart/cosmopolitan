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
#include "libc/calls/calls.h"
#include "libc/calls/syscall-nt.internal.h"
#include "libc/calls/syscall-sysv.internal.h"
#include "libc/dce.h"
#include "libc/intrin/strace.h"
#include "libc/runtime/runtime.h"
#include "libc/sysv/errfuns.h"

static int __madvise(void *addr, size_t length, int advice) {

  // simulate linux behavior of validating alignment
  if ((uintptr_t)addr & (__pagesize - 1))
    return einval();

  // simulate linux behavior of checking for negative length
  if ((ssize_t)length < 0)
    return einval();

  // madvise(0, 0, advice) may be used to validate advice
  if (!length && (IsFreebsd() || IsNetbsd()))
    addr = (void *)65536l;

  if (!IsWindows())
    return sys_madvise(addr, length, advice);
  return sys_madvise_nt(addr, length, advice);
}

/**
 * Declares intent to OS on how memory region will be used.
 *
 * `madvise(0, 0, advice)` is recommended for validating `advise` and it
 * will always be the case that a `length` of zero is a no-op otherwise.
 *
 * Having the interval overlap unmapped pages has undefined behavior. On
 * Linux, this can be counted upon to raise ENOMEM. Other OSes vary much
 * in behavior here; they'll might ignore unmapped regions or they might
 * raise EINVAL, EFAULT, or ENOMEM.
 *
 * @param advice can be MADV_WILLNEED, MADV_SEQUENTIAL, MADV_FREE, etc.
 * @return 0 on success, or -1 w/ errno
 * @raise EINVAL if `advice` isn't valid or supported by system
 * @raise EINVAL if `addr` isn't getpagesize() aligned
 * @raise EINVAL if `length` is negative
 * @see libc/sysv/consts.sh
 * @see fadvise()
 */
int madvise(void *addr, size_t length, int advice) {
  int rc = __madvise(addr, length, advice);
  STRACE("madvise(%p, %'zu, %d) → %d% m", addr, length, advice, rc);
  return rc;
}
