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
#include "libc/mem/mem.h"
#include "libc/runtime/runtime.h"
#include "libc/str/str.h"

/**
 * Replaces tilde in path w/ user home folder.
 *
 * @param path is NULL propagating
 * @return must be free()'d
 */
char *replaceuser(const char *path) {
  char *res, *p;
  const char *home;
  size_t pathlen, homelen;
  res = NULL;
  if (path && *path++ == '~' && !isempty((home = getenv("HOME")))) {
    while (*path == '/') path++;
    pathlen = strlen(path);
    homelen = strlen(home);
    while (homelen && home[homelen - 1] == '/') homelen--;
    if ((p = res = malloc(pathlen + 1 + homelen + 1))) {
      p = mempcpy(p, home, homelen);
      *p++ = '/';
      memcpy(p, path, pathlen + 1);
    }
  }
  return res;
}
