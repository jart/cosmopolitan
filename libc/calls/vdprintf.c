/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=8 sts=2 sw=2 fenc=utf-8                                :vi│
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
#include "libc/calls/internal.h"
#include "libc/dce.h"
#include "libc/fmt/fmt.h"
#include "libc/limits.h"
#include "libc/macros.h"
#include "libc/nt/files.h"
#include "libc/sysv/errfuns.h"

struct VdprintfState {
  int n;
  int fd;
  unsigned char buf[1024];
};

static int vdprintf_flush(struct VdprintfState *df, int n) {
  int i, rc;
  for (i = 0; i < n; i += rc) {
    if ((rc = write(df->fd, df->buf + i, n - i)) == -1) {
      return -1;
    }
  }
  return 0;
}

static int vdprintfputchar(int c, struct VdprintfState *df) {
  df->buf[df->n++ & (ARRAYLEN(df->buf) - 1)] = c & 0xff;
  if ((df->n & (ARRAYLEN(df->buf) - 1))) {
    return 0;
  } else {
    return vdprintf_flush(df, ARRAYLEN(df->buf));
  }
}

/**
 * Formats string directly to system i/o device.
 */
int(vdprintf)(int fd, const char *fmt, va_list va) {
  struct VdprintfState df;
  df.n = 0;
  df.fd = fd;
  if (palandprintf(vdprintfputchar, &df, fmt, va) == -1) return -1;
  if (vdprintf_flush(&df, df.n & (ARRAYLEN(df.buf) - 1)) == -1) return -1;
  return df.n;
}
