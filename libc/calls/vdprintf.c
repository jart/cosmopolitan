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
#include "libc/nt/files.h"
#include "libc/sysv/errfuns.h"

#define DBUFSIZ 1460 /* tcp ethernet frame < -Wframe-larger-than=4096 */

struct dfile {
  int fd;
  unsigned idx;
  unsigned toto;
  unsigned char buf[DBUFSIZ];
};

static int vdprintf_flush(struct dfile *df) {
  ssize_t wrote;
  do {
    wrote = write(df->fd, &df->buf[0], df->idx);
    if (wrote == -1) return -1;
    df->toto += (unsigned)wrote;
    df->idx -= (unsigned)wrote;
    if (df->toto > INT_MAX) return eoverflow();
  } while (df->idx);
  return 0;
}

static int vdprintfputchar(unsigned char c, struct dfile *df) {
  df->buf[df->idx++] = c;
  if (df->idx == DBUFSIZ && vdprintf_flush(df) == -1) return -1;
  return 0;
}

/**
 * Formats string directly to system i/o device.
 */
int(vdprintf)(int fd, const char *fmt, va_list va) {
  struct dfile df;
  df.fd = fd;
  df.idx = 0;
  df.toto = 0;
  if (palandprintf(vdprintfputchar, &df, fmt, va) == -1) return -1;
  if (df.idx && vdprintf_flush(&df) == -1) return -1;
  return df.toto;
}
