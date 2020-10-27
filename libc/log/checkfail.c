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
#include "libc/bits/safemacros.h"
#include "libc/calls/calls.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/fmt/fmt.h"
#include "libc/log/check.h"
#include "libc/log/color.h"
#include "libc/log/internal.h"
#include "libc/log/log.h"
#include "libc/runtime/memtrack.h"
#include "libc/runtime/runtime.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/auxv.h"
#include "libc/sysv/consts/fileno.h"

STATIC_YOINK("ntoa");
STATIC_YOINK("stoa");
STATIC_YOINK("ftoa");

/**
 * Handles failure of CHECK_xx() macros.
 */
relegated void __check_fail(const char *suffix, const char *opstr,
                            uint64_t want, const char *wantstr, uint64_t got,
                            const char *gotstr, const char *file, int line,
                            const char *fmt, ...) {
  size_t i;
  va_list va;
  char sufbuf[8];
  int lasterr = errno;
  __start_fatal(file, line);

  if (!memccpy(sufbuf, suffix, '\0', sizeof(sufbuf))) strcpy(sufbuf, "?");
  strtoupper(sufbuf);

  (dprintf)(STDERR_FILENO,
            "check failed\r\n"
            "\tCHECK_%s(%s, %s);\r\n"
            "\t\t → %#lx (%s)\r\n"
            "\t\t%s %#lx (%s)\r\n",
            sufbuf, wantstr, gotstr, want, wantstr, opstr, got, gotstr);

  if (!isempty(fmt)) {
    (dprintf)(STDERR_FILENO, "\t");
    va_start(va, fmt);
    (vdprintf)(STDERR_FILENO, fmt, va);
    va_end(va);
    (dprintf)(STDERR_FILENO, "\r\n");
  }

  (dprintf)(STDERR_FILENO, "\t%s\r\n\t%s%s%s%s\r\n", strerror(lasterr), SUBTLE,
            getauxval(AT_EXECFN), g_argc > 1 ? " \\" : "", RESET);

  for (i = 1; i < g_argc; ++i) {
    (dprintf)(STDERR_FILENO, "\t\t%s%s\r\n", g_argv[i],
              i < g_argc - 1 ? " \\" : "");
  }

  if (!IsTiny() && lasterr == ENOMEM) {
    (dprintf)(STDERR_FILENO, "\r\n");
    PrintMemoryIntervals(STDERR_FILENO, &_mmi);
  }

  die();
  unreachable;
}
