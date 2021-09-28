/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2020 Justine Alexandra Roberts Tunney                              │
│                                                                              │
│ Permission to use, copy, modify, and/or distribute this software for         │
│ any purpose with or without fee is hereby granted, provided that the         │
│ above copyright notice and this permission notice appear in all copies.      │
│                                                                              │
│ THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL                │
│ WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED                │
│ WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE             │
│ AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL         │
│ DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR        │
│ PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER               │
│ TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR             │
│ PERFORMANCE OF THIS SOFTWARE.                                                │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "libc/bits/safemacros.internal.h"
#include "libc/calls/calls.h"
#include "libc/errno.h"
#include "libc/fmt/fmt.h"
#include "libc/log/check.h"
#include "libc/log/color.internal.h"
#include "libc/log/internal.h"
#include "libc/log/libfatal.internal.h"
#include "libc/log/log.h"
#include "libc/runtime/memtrack.internal.h"

/**
 * Handles failure of CHECK_xx() macros.
 */
relegated void __check_fail(const char *suffix, const char *opstr,
                            uint64_t want, const char *wantstr, uint64_t got,
                            const char *gotstr, const char *file, int line,
                            const char *fmt, ...) {
  int e;
  char *p;
  size_t i;
  va_list va;
  char hostname[32];
  e = errno;
  p = __fatalbuf;
  __start_fatal(file, line);
  __stpcpy(hostname, "unknown");
  gethostname(hostname, sizeof(hostname));
  p = __stpcpy(p, "check failed on ");
  p = __stpcpy(p, hostname);
  p = __stpcpy(p, " pid ");
  p = __intcpy(p, __getpid());
  p = __stpcpy(p, "\n");
  p = __stpcpy(p, "\tCHECK_");
  for (; *suffix; ++suffix) {
    *p++ = *suffix - ('a' <= *suffix && *suffix <= 'z') * 32;
  }
  p = __stpcpy(p, "(");
  p = __stpcpy(p, wantstr);
  p = __stpcpy(p, ", ");
  p = __stpcpy(p, gotstr);
  p = __stpcpy(p, ");\n\t\t → 0x");
  p = __hexcpy(p, want);
  p = __stpcpy(p, " (");
  p = __stpcpy(p, wantstr);
  p = __stpcpy(p, ")\n\t\t");
  p = __stpcpy(p, opstr);
  p = __stpcpy(p, " 0x");
  p = __hexcpy(p, got);
  p = __stpcpy(p, " (");
  p = __stpcpy(p, gotstr);
  p = __stpcpy(p, ")\n");
  if (!isempty(fmt)) {
    *p++ = '\t';
    va_start(va, fmt);
    p += (vsprintf)(p, fmt, va);
    va_end(va);
    *p++ = '\n';
  }
  p = __stpcpy(p, "\t");
  p = __stpcpy(p, strerror(e));
  p = __stpcpy(p, "\n\t");
  p = __stpcpy(p, SUBTLE);
  p = __stpcpy(p, program_invocation_name);
  if (__argc > 1) p = __stpcpy(p, " \\");
  p = __stpcpy(p, RESET);
  p = __stpcpy(p, "\n");
  __write(__fatalbuf, p - __fatalbuf);
  for (i = 1; i < __argc; ++i) {
    p = __fatalbuf;
    p = __stpcpy(p, "\t\t");
    p = __stpcpy(p, __argv[i]);
    if (i < __argc - 1) p = __stpcpy(p, " \\");
    p = __stpcpy(p, "\n");
  }
  if (!IsTiny() && e == ENOMEM) {
    __write("\n", 1);
    PrintMemoryIntervals(2, &_mmi);
  }
  __die();
  unreachable;
}
