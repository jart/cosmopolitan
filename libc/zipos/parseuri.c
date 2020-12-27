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
#include "libc/str/str.h"
#include "libc/zipos/zipos.internal.h"

const char kZiposSchemePrefix[4] hidden = "zip:";

/**
 * Extracts information about ZIP URI if it is one.
 */
ssize_t __zipos_parseuri(const char *uri, struct ZiposUri *out) {
  size_t len;
  if ((len = strlen(uri)) >= sizeof(kZiposSchemePrefix) && len < PATH_MAX &&
      memcmp(uri, kZiposSchemePrefix, sizeof(kZiposSchemePrefix)) == 0) {
    out->path = uri + sizeof(kZiposSchemePrefix);
    return (out->len = len - sizeof(kZiposSchemePrefix));
  } else {
    return -1;
  }
}
