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
#include "libc/calls/syscall-sysv.internal.h"
#include "libc/dce.h"
#include "libc/intrin/getauxval.internal.h"
#include "libc/intrin/strace.internal.h"
#include "libc/sysv/consts/auxv.h"

/**
 * Determines if process is tainted.
 *
 * This function returns 1 if process was launched as a result of an
 * execve() call on a binary that had the setuid or setgid bits set.
 * FreeBSD defines tainted as including processes that changed their
 * effective user / group ids at some point.
 *
 * @return always successful, 1 if yes, 0 if no
 */
int issetugid(void) {
  int rc;
  if (IsLinux()) {
    rc = !!__getauxval(AT_SECURE).value;
  } else if (IsMetal() || IsWindows()) {
    rc = 0;
  } else {
    rc = sys_issetugid();
  }
  STRACE("issetugid() → %d", rc);
  return rc;
}
