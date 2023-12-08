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
#include "libc/intrin/asan.internal.h"
#include "libc/intrin/describeflags.internal.h"
#include "libc/intrin/strace.internal.h"
#include "libc/sysv/consts/at.h"
#include "libc/sysv/errfuns.h"

/**
 * Creates symbolic link.
 *
 * This is like link() but adds a tiny indirection to make the fact that
 * the file is a link obvious. It also enables certain other features,
 * like the ability to be broken.
 *
 * @param target can be relative and needn't exist
 * @param linkpath is what gets created
 * @return 0 on success, or -1 w/ errno
 * @raise EPERM if a non-admin on Windows NT tries to use this
 * @asyncsignalsafe
 */
int symlinkat(const char *target, int newdirfd, const char *linkpath) {
  int rc;
  if (IsAsan() &&
      (!__asan_is_valid_str(target) || !__asan_is_valid_str(linkpath))) {
    rc = efault();
  }
  if (!IsWindows()) {
    rc = sys_symlinkat(target, newdirfd, linkpath);
  } else {
    rc = sys_symlinkat_nt(target, newdirfd, linkpath);
  }
  STRACE("symlinkat(%#s, %s, %#s) → %d% m", target, DescribeDirfd(newdirfd),
         linkpath, rc);
  return rc;
}
