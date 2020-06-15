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
#include "libc/conv/conv.h"
#include "libc/str/str.h"
#include "libc/sysv/errfuns.h"

/**
 * Converts string to integer, the BSD way.
 */
long long strtonum(const char *nptr, long long minval, long long maxval,
                   const char **errstr) {
  char *end;
  intmax_t res = strtoimax(nptr, &end, 10);
  if (res < minval || res > maxval || end != nptr + strlen(nptr)) {
    if (errstr) *errstr = "bad number";
    return einval();
  } else {
    if (errstr) *errstr = NULL;
    return res;
  }
}
