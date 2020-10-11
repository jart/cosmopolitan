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
#include "libc/macros.h"
#include "libc/runtime/runtime.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/auxv.h"

struct FindComBinary {
  bool once;
  const char *res;
  char buf[PATH_MAX];
};

static struct FindComBinary g_findcombinary;

/**
 * Returns path of binary without debug information, or null.
 *
 * @return path to non-debug binary, or -1 w/ errno
 */
const char *FindComBinary(void) {
  size_t len;
  const char *p;
  if (!g_findcombinary.once) {
    g_findcombinary.once = true;
    if ((p = (const char *)getauxval(AT_EXECFN)) &&
        (len = strlen(p)) < ARRAYLEN(g_findcombinary.buf)) {
      g_findcombinary.res = memcpy(g_findcombinary.buf, p, len + 1);
      if (len > 4 && memcmp(&g_findcombinary.buf[len - 4], ".dbg", 4) == 0) {
        g_findcombinary.buf[len - 4] = '\0';
      }
    }
  }
  return g_findcombinary.res;
}
