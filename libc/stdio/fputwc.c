/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
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
#include "libc/limits.h"
#include "libc/stdio/fputc.h"
#include "libc/stdio/internal.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"

/**
 * Writes wide character to stream.
 *
 * @return wc if written or -1 w/ errno
 */
wint_t fputwc(wchar_t wc, FILE *f) {
  char buf[MB_LEN_MAX];
  unsigned len, i;
  if (wc != -1) {
    len = tpencode(buf, sizeof(buf), wc, false);
    for (i = 0; i < len; ++i) {
      if (__fputc(buf[i], f) == -1) return -1;
    }
    return wc;
  } else {
    return fseteof(f);
  }
}
