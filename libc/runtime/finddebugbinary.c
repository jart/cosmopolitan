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
#include "libc/bits/bits.h"
#include "libc/calls/calls.h"
#include "libc/errno.h"
#include "libc/macros.h"
#include "libc/runtime/runtime.h"
#include "libc/runtime/symbols.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/auxv.h"

/**
 * Returns path of binary with the debug information, or null.
 *
 * @return path to debug binary, or -1 w/ errno
 */
const char *finddebugbinary(void) {
  static char buf[PATH_MAX];
  if (buf[0]) return &buf[0];
  const char *const trybins[] = {program_invocation_name,
                                 (const char *)getauxval(AT_EXECFN)};
  for (unsigned i = 0; i < ARRAYLEN(trybins); ++i) {
    const char *res = trybins[i];
    unsigned len = strlen(res);
    if (4 < len && len < sizeof(buf) - 5) {
      if (strcmp(res + len - 4, ".dbg") == 0) return res;
      /* try suffixing extension, e.g. .com → .com.dbg */
      memcpy(mempcpy(buf, res, len), ".dbg", 5);
      if (fileexists(buf)) return &buf[0];
      buf[0] = '\0';
    }
  }
  errno = ENOENT;
  return NULL;
}
