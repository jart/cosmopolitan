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
#define ShouldUseMsabiAttribute() 1
#include "libc/assert.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/nt/errors.h"
#include "libc/nt/runtime.h"
#include "libc/sock/internal.h"
#include "libc/sysv/errfuns.h"
#include "libc/thread/tls2.internal.h"

/**
 * Return path for failed Win32 API calls.
 *
 * @return -1 w/ few exceptions
 * @note this is a code-size saving device
 */
privileged int64_t __winerr(void) {
  errno_t e;
  if (IsWindows()) {
    e = __dos2errno(__imp_GetLastError());
  } else {
    e = ENOSYS;
  }
  if (__tls_enabled) {
    __get_tls_privileged()->tib_errno = e;
  } else {
    __errno = e;
  }
  return -1;
}
