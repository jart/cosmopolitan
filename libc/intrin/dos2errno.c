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
#include "libc/errno.h"
#include "libc/intrin/dos2errno.internal.h"
#include "libc/nt/errors.h"
#include "libc/sock/sock.h"

/**
 * Translates Windows error using superset of consts.sh.
 *
 * This function is called by __winerr(). It can only be used on
 * Windows, because it returns an errno. Normally, errnos will be
 * programmed to be the same as DOS errnos, per consts.sh. But since
 * there's so many more errors in DOS, this function provides an added
 * optional benefit mapping additional constants onto the errnos in
 * consts.sh.
 */
privileged errno_t __dos2errno(uint32_t error) {
  int i;
  if (error) {
    for (i = 0; kDos2Errno[i].doscode; ++i) {
      if (error == kDos2Errno[i].doscode) {
        return *(const int *)((intptr_t)kDos2Errno[i].systemv);
      }
    }
  }
  return error;
}
