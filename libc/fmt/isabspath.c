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
#include "libc/fmt/conv.h"
#include "libc/fmt/isslash.internal.h"

/**
 * Returns true if pathname could be absolute on any known platform.
 *
 * The ones we know about are System V (/foo/bar), DOS (C:\foo\bar),
 * Windows NT (\\.\C:\foo\bar), Google Cloud (gs://bucket/foo/bar), etc.
 */
bool isabspath(const char *path) {
  if (isslash(*path)) return true;
  for (; *path; ++path) {
    if (isslash(*path)) return false;
    if (*path == ':') {
      ++path;
      if (isslash(*path)) return true;
    }
  }
  return false;
}
