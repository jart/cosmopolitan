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
#include "libc/bits/safemacros.internal.h"
#include "libc/calls/calls.h"
#include "libc/calls/struct/stat.h"
#include "libc/runtime/rbx.h"
#include "libc/str/str.h"
#include "libc/sysv/errfuns.h"
#include "libc/zip.h"
#include "libc/zipos/zipos.internal.h"

int __zipos_stat_impl(struct Zipos *zipos, size_t cf, struct stat *st) {
  if (zipos && st) {
    memset(st, 0, sizeof(*st));
    if (ZIP_CFILE_FILEATTRCOMPAT(zipos->map + cf) == kZipOsUnix) {
      st->st_mode = ZIP_CFILE_EXTERNALATTRIBUTES(zipos->map + cf) >> 16;
    } else {
      st->st_mode = 0100644;
    }
    st->st_size = ZIP_CFILE_UNCOMPRESSEDSIZE(zipos->map + cf);
    st->st_blocks =
        roundup(ZIP_CFILE_COMPRESSEDSIZE(zipos->map + cf), 512) / 512;
    return 0;
  } else {
    return einval();
  }
}
