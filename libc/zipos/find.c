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
#include "ape/relocations.h"
#include "libc/assert.h"
#include "libc/runtime/rbx.h"
#include "libc/runtime/runtime.h"
#include "libc/str/str.h"
#include "libc/zip.h"
#include "libc/zipos/zipos.h"

ssize_t __zipos_find(struct Zipos *zipos, const struct ZiposUri *name) {
  size_t i, cf;
  assert(ZIP_CDIR_MAGIC(zipos->cdir) == kZipCdirHdrMagic);
  for (i = 0, cf = ZIP_CDIR_OFFSET(zipos->cdir);
       i < ZIP_CDIR_RECORDS(zipos->cdir);
       ++i, cf += ZIP_CFILE_HDRSIZE(zipos->map + cf)) {
    assert(ZIP_CFILE_MAGIC(zipos->map + cf) == kZipCfileHdrMagic);
    if (name->len == ZIP_CFILE_NAMESIZE(zipos->map + cf) &&
        memcmp(name->path, ZIP_CFILE_NAME(zipos->map + cf), name->len) == 0) {
      return cf;
    }
  }
  return -1;
}
