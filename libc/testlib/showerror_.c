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
#include "libc/bits/safemacros.h"
#include "libc/calls/internal.h"
#include "libc/errno.h"
#include "libc/fmt/fmt.h"
#include "libc/log/color.internal.h"
#include "libc/log/internal.h"
#include "libc/log/log.h"
#include "libc/math.h"
#include "libc/nt/runtime.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/stdio.h"
#include "libc/testlib/testlib.h"

STATIC_YOINK("__isfdkind");

const char *testlib_showerror_errno;
const char *testlib_showerror_file;
const char *testlib_showerror_func;
const char *testlib_showerror_isfatal;
const char *testlib_showerror_macro;
const char *testlib_showerror_symbol;

testonly void testlib_showerror_(int line, const char *wantcode,
                                 const char *gotcode, char *FREED_want,
                                 char *FREED_got, const char *fmt, ...) {
  va_list va;

  getpid$sysv(); /* make strace easier to read */
  getpid$sysv();

  fflush(stdout);
  fflush(stderr);

  /* TODO(jart): Pay off tech debt re duplication */

  fprintf(stderr, "%s%s%s%s:%s:%d%s: %s(%s)\n\t%s(%s", RED2, "error", UNBOLD,
          BLUE1, testlib_showerror_file, line, RESET, testlib_showerror_func,
          g_fixturename, testlib_showerror_macro, wantcode);

  if (wantcode) {
    fprintf(stderr,
            ", %s)\n"
            "\t\t%s %s %s\n"
            "\t\t%s %s\n",
            gotcode, "need", FREED_want, testlib_showerror_symbol, " got",
            FREED_got);
  } else {
    fprintf(stderr,
            ", %s)\n"
            "\t\t→ %s%s\n",
            gotcode, testlib_showerror_symbol, FREED_want);
  }

  if (!isempty(fmt)) {
    fputc('\t', stderr);
    va_start(va, fmt);
    vfprintf(stderr, fmt, va);
    va_end(va);
    fputc('\n', stderr);
  }

  fprintf(stderr,
          "\t%s%s\n"
          "\t%s%s\n",
          SUBTLE, strerror(errno), program_invocation_name, RESET);

  free_s(&FREED_want);
  free_s(&FREED_got);

  ++g_testlib_failed;
  if (testlib_showerror_isfatal) testlib_abort();
}
