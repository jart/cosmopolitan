/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=8 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2020 Justine Alexandra Roberts Tunney                              │
│                                                                              │
│ This program is free software; you can redistribute it and/or modify         │
│ it under the terms of the GNU General Public License as published by         │
│ the Free Software Foundation; version 2 of the License.                      │
│                                                                              │
│ This program is distributed in the hope that it will be useful, but          │
│ WITHOUT ANY WARRANTY; without even the implied warranty of                   │
│ MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU             │
│ General Public License for more details.                                     │
│                                                                              │
│ You should have received a copy of the GNU General Public License            │
│ along with this program; if not, write to the Free Software                  │
│ Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA                │
│ 02110-1301 USA                                                               │
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
