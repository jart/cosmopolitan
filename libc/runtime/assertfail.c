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
#include "libc/assert.h"
#include "libc/bits/weaken.h"
#include "libc/calls/calls.h"
#include "libc/log/log.h"
#include "libc/macros.h"
#include "libc/mem/alloca.h"
#include "libc/runtime/runtime.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/fileno.h"

/**
 * Handles failure of assert() macro.
 */
relegated void __assert_fail(const char *expr, const char *file, int line) {
  static bool once;
  char *msg, *p, linebuf[16];
  unsigned i, exprlen, filelen;
  if (!once) {
    once = true;
    exprlen = expr ? strlen(expr) : 0;
    filelen = file ? strlen(file) : 0;
    p = msg = alloca(MIN(512, exprlen + filelen + 64));
    p = mempcpy(p, file, filelen);
    p = stpcpy(p, ":");
    if (line < 1) line = 1;
    for (i = 0; line; line /= 10) linebuf[i++] = '0' + line % 10;
    while (i) *p++ = linebuf[--i];
    p = stpcpy(p, ":");
    p = mempcpy(p, expr, exprlen);
    p = stpcpy(p, "\r\n");
    write(STDERR_FILENO, msg, p - msg);
    if (weaken(die)) weaken(die)();
  }
  abort();
  unreachable;
}
