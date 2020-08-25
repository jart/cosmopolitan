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
#include "libc/bits/pushpop.h"
#include "libc/bits/safemacros.h"
#include "libc/calls/hefty/ntspawn.h"
#include "libc/calls/internal.h"
#include "libc/conv/conv.h"
#include "libc/str/str.h"
#include "libc/str/tpdecode.h"
#include "libc/sysv/errfuns.h"

/**
 * Copies path for Windows NT.
 *
 * This entails (1) UTF-8 to UTF-16 conversion; (2) replacing
 * forward-slashes with backslashes; and (3) remapping several
 * well-known paths (e.g. /dev/null → NUL) for convenience.
 *
 * @param path16 is shortened so caller can prefix, e.g. \\.\pipe\, and
 *     due to a plethora of special-cases throughout the Win32 API
 * @param flags is used by open(), see fixntmagicpath2()
 * @return short count excluding NUL on success, or -1 w/ errno
 * @error ENAMETOOLONG
 */
forcealignargpointer textwindows int(mkntpath)(
    const char *path, unsigned flags,
    char16_t path16[hasatleast PATH_MAX - 16]) {
  /*
   * 1. Reserve +1 for NUL-terminator
   * 2. Reserve +1 for UTF-16 overflow
   * 3. Reserve ≥2 for SetCurrentDirectory trailing slash requirement
   * 4. Reserve ≥10 for CreateNamedPipe "\\.\pipe\" prefix requirement
   * 5. Reserve ≥13 for mkdir() i.e. 1+8+3+1, e.g. "\\ffffffff.xxx\0"
   */
  int rc;
  wint_t wc;
  size_t i, j;
  path = fixntmagicpath(path, flags);
  i = 0;
  j = 0;
  for (;;) {
    if ((rc = tpdecode(&path[i], &wc)) == -1) {
      path16[0] = u'\0';
      return -1;
    }
    if (!wc) break;
    i += (size_t)rc;
    if (wc == '/') wc = '\\';
    if (j + 1 /* utf-16 */ + 1 /* chdir() */ + 1 /* NUL */ < PATH_MAX - 16) {
      if ((rc = pututf16(&path16[j], 2, wc, false)) == -1) {
        path16[0] = u'\0';
        return -1;
      }
      j += (size_t)rc;
    } else {
      path16[0] = u'\0';
      return enametoolong();
    }
  }
  path16[j] = u'\0';
  return j;
}
