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
#include "libc/fmt/fmt.h"
#include "libc/limits.h"
#include "libc/stdio/fputc.h"
#include "libc/stdio/stdio.h"
#include "libc/sysv/errfuns.h"

struct state {
  FILE *const f;
  unsigned toto;
};

static int vfprintfputchar(int c, struct state *st) {
  if (st->toto <= INT_MAX) {
    st->toto++;
    return __fputc(c, st->f);
  } else {
    return eoverflow();
  }
}

int(vfprintf)(FILE *f, const char *fmt, va_list va) {
  struct state st[1] = {{f, 0}};
  if (palandprintf(vfprintfputchar, st, fmt, va) != -1) {
    return st->toto;
  } else {
    return -1;
  }
}
