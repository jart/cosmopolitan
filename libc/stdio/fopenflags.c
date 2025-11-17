/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=8 sts=2 sw=2 fenc=utf-8                               :vi │
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
#include "libc/calls/calls.h"
#include "libc/dce.h"
#include "libc/stdio/stdio.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/errfuns.h"

/**
 * Turns stdio flags description string into bitmask.
 */
int fopenflags(const char *mode) {
  int oflags;

  switch (*mode++) {
    case 'r':
      oflags = O_RDONLY;
      break;
    case 'w':
      oflags = O_WRONLY | O_CREAT | O_TRUNC;
      break;
    case 'a':
      oflags = O_WRONLY | O_CREAT | O_APPEND;
      break;
    default:
      return einval();
  }

  for (;;) {
    switch (*mode++) {
      case 0:
        return oflags;
      case '+':
        oflags &= ~O_ACCMODE;
        oflags |= O_RDWR;
        break;
      case 'x':
        oflags |= O_EXCL;
        break;
      case 'e':
        oflags |= O_CLOEXEC;
        break;
      case 'b':
        break;
      default:
        if (IsModeDbg())
          unassert(!"stdio mode character not defined by posix");
        break;
    }
  }
}
