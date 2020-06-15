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
#include "libc/calls/calls.h"
#include "libc/calls/internal.h"
#include "libc/calls/struct/utsname.h"
#include "libc/str/str.h"

/**
 * Asks kernel to give us the `uname -a` data.
 * @return 0 on success, or -1 w/ errno
 */
int uname(struct utsname *lool) {
  char *out;
  size_t i, j, len;
  char tmp[sizeof(struct utsname)];
  memset(tmp, 0, sizeof(tmp));
  if (uname$sysv(tmp) != -1) {
    out = (char *)lool;
    i = 0;
    j = 0;
    for (;;) {
      len = strlen(&tmp[j]);
      if (len >= sizeof(struct utsname) - i) break;
      memcpy(&out[i], &tmp[j], len + 1);
      i += SYS_NMLN;
      j += len;
      while (j < sizeof(tmp) && tmp[j] == '\0') ++j;
      if (j == sizeof(tmp)) break;
    }
    return 0;
  } else {
    memset(lool, 0, sizeof(struct utsname));
    return -1;
  }
}
