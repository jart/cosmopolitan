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
#include "libc/fmt/fmt.h"
#include "libc/log/color.internal.h"
#include "libc/log/internal.h"
#include "libc/log/libfatal.internal.h"
#include "libc/testlib/testlib.h"

const char *testlib_showerror_errno;
const char *testlib_showerror_file;
const char *testlib_showerror_func;
const char *testlib_showerror_isfatal;
const char *testlib_showerror_macro;
const char *testlib_showerror_symbol;

testonly void testlib_showerror(const char *file, int line, const char *func,
                                const char *method, const char *symbol,
                                const char *code, char *v1, char *v2) {
  char *p;
  /* TODO(jart): Pay off tech debt re duplication */
  __getpid(); /* make strace easier to read */
  __getpid();
  __printf("%serror%s%s:%s:%d%s: %s() in %s(%s)\n"
           "\t%s\n"
           "\t\tneed %s %s\n"
           "\t\t got %s\n"
           "\t%s%s\n"
           "\t%s%s\n",
           RED2, UNBOLD, BLUE1, file, (long)line, RESET, method, func,
           g_fixturename, code, v1, symbol, v2, SUBTLE, strerror(errno),
           program_executable_name, RESET);
  free_s(&v1);
  free_s(&v2);
}

/* TODO(jart): Pay off tech debt re duplication */
testonly void testlib_showerror_(int line, const char *wantcode,
                                 const char *gotcode, char *FREED_want,
                                 char *FREED_got, const char *fmt, ...) {
  int e;
  va_list va;
  char hostname[32];
  e = errno;
  __getpid();
  __getpid();
  __printf("%serror%s:%s%s:%d%s: %s(%s)\n"
           "\t%s(%s, %s)\n",
           RED2, UNBOLD, BLUE1, testlib_showerror_file, line, RESET,
           testlib_showerror_func, g_fixturename, testlib_showerror_macro,
           wantcode, gotcode);
  if (wantcode) {
    __printf("\t\tneed %s %s\n"
             "\t\t got %s\n",
             FREED_want, testlib_showerror_symbol, FREED_got);
  } else {
    __printf("\t\t→ %s%s\n", testlib_showerror_symbol, FREED_want);
  }
  if (!isempty(fmt)) {
    __printf("\t");
    va_start(va, fmt);
    __vprintf(fmt, va);
    va_end(va);
    __printf("\n");
  }
  __stpcpy(hostname, "unknown");
  gethostname(hostname, sizeof(hostname));
  __printf("\t%s%s%s\n"
           "\t%s%s @ %s%s\n",
           SUBTLE, strerror(e), RESET, SUBTLE, program_invocation_name,
           hostname, RESET);
  free_s(&FREED_want);
  free_s(&FREED_got);
  ++g_testlib_failed;
  if (testlib_showerror_isfatal) testlib_abort();
}
