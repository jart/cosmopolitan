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

ssize_t __zipos_find(const struct ZiposUri *name) {
  size_t i, cf;
  assert(ZIP_CDIR_MAGIC(__zip_end) == kZipCdirHdrMagic);
  for (i = 0, cf = ZIP_CDIR_OFFSET(__zip_end); i < ZIP_CDIR_RECORDS(__zip_end);
       ++i, cf += ZIP_CFILE_HDRSIZE(&_base[0] + cf)) {
    assert(ZIP_CFILE_MAGIC(&_base[0] + cf) == kZipCfileHdrMagic);
    if (name->len == ZIP_CFILE_NAMESIZE(&_base[0] + cf) &&
        memcmp(name->path, ZIP_CFILE_NAME(&_base[0] + cf), name->len) == 0) {
      return cf;
    }
  }
  return -1;
}
