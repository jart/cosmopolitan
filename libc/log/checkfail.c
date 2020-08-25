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
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/fmt/fmt.h"
#include "libc/log/check.h"
#include "libc/log/internal.h"
#include "libc/log/log.h"
#include "libc/runtime/memtrack.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/auxv.h"

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
  startfatal(file, line);

  if (!memccpy(sufbuf, suffix, '\0', sizeof(sufbuf))) strcpy(sufbuf, "?");
  strtoupper(sufbuf);

  fprintf(stderr,
          "check failed\n"
          "\tCHECK_%s(%s, %s);\n"
          "\t\t → %#lx (%s)\n"
          "\t\t%s %#lx (%s)\n",
          sufbuf, wantstr, gotstr, want, wantstr, opstr, got, gotstr);

  if (!isempty(fmt)) {
    fputc('\t', stderr);
    va_start(va, fmt);
    vfprintf(stderr, fmt, va);
    va_end(va);
    fputc('\n', stderr);
  }

  fprintf(stderr, "\t%s\n\t%s%s%s%s\n", strerror(lasterr), SUBTLE,
          getauxval(AT_EXECFN), g_argc > 1 ? " \\" : "", RESET);

  for (i = 1; i < g_argc; ++i) {
    fprintf(stderr, "\t\t%s%s\n", g_argv[i], i < g_argc - 1 ? " \\" : "");
  }

  if (!IsTiny() && lasterr == ENOMEM) {
    fprintf(stderr, "\n");
    fflush(stderr);
    PrintMemoryIntervals(fileno(stderr), &_mmi);
  }

  fflush(stderr);
  die();
  unreachable;
}
