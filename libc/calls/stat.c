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
#include "libc/bits/weaken.h"
#include "libc/calls/calls.h"
#include "libc/calls/internal.h"
#include "libc/dce.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/at.h"
#include "libc/sysv/errfuns.h"
#include "libc/zipos/zipos.h"

/**
 * Returns information about thing.
 *
 * @see S_ISDIR(st.st_mode), S_ISREG(), etc.
 * @asyncsignalsafe
 */
int stat(const char *pathname, struct stat *st) {
  struct ZiposUri zipname;
  if (weaken(__zipos_stat) &&
      weaken(__zipos_parseuri)(pathname, &zipname) != -1) {
    return weaken(__zipos_stat)(&zipname, st);
  } else if (!IsWindows()) {
    return fstatat$sysv(AT_FDCWD, pathname, st, 0);
  } else {
    return stat$nt(pathname, st);
  }
}
