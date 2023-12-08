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
#include "libc/intrin/strace.internal.h"

/**
 * Sets real, effective, and "saved" user ids.
 *
 * @param real sets real user id or -1 to do nothing
 * @param effective sets effective user id or -1 to do nothing
 * @param saved sets saved user id or -1 to do nothing
 * @see setresgid(), getauxval(AT_SECURE)
 * @raise ENOSYS on Windows NT
 */
int setresuid(uint32_t real, uint32_t effective, uint32_t saved) {
  int rc;
  if (saved != -1) {
    rc = sys_setresuid(real, effective, saved);
  } else {
    // polyfill xnu and netbsd
    rc = sys_setreuid(real, effective);
  }
  STRACE("setresuid(%d, %d, %d) → %d% m", real, effective, saved, rc);
  return rc;
}
