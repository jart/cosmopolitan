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
  p = __fatalbuf;
  p = __stpcpy(p, RED2);
  p = __stpcpy(p, "error");
  p = __stpcpy(p, UNBOLD);
  p = __stpcpy(p, BLUE1);
  p = __stpcpy(p, ":");
  p = __stpcpy(p, file);
  p = __stpcpy(p, ":");
  p = __intcpy(p, line);
  p = __stpcpy(p, RESET);
  p = __stpcpy(p, ": ");
  p = __stpcpy(p, method);
  p = __stpcpy(p, "() in ");
  p = __stpcpy(p, func);
  p = __stpcpy(p, "(");
  p = __stpcpy(p, g_fixturename);
  p = __stpcpy(p, ")\n\t");
  p = __stpcpy(p, code);
  p = __stpcpy(p, "\n\t\tneed ");
  p = __stpcpy(p, v1);
  p = __stpcpy(p, " ");
  p = __stpcpy(p, symbol);
  p = __stpcpy(p, "\n\t\t got ");
  p = __stpcpy(p, v2);
  p = __stpcpy(p, "\n\t");
  p = __stpcpy(p, SUBTLE);
  p = __stpcpy(p, strerror(errno));
  p = __stpcpy(p, "\n\t");
  p = __stpcpy(p, program_invocation_name);
  p = __stpcpy(p, RESET);
  p = __stpcpy(p, "\n");
  __write(__fatalbuf, p - __fatalbuf);
  free_s(&v1);
  free_s(&v2);
}

/* TODO(jart): Pay off tech debt re duplication */
testonly void testlib_showerror_(int line, const char *wantcode,
                                 const char *gotcode, char *FREED_want,
                                 char *FREED_got, const char *fmt, ...) {
  int e;
  char *p;
  va_list va;
  char hostname[32];
  __getpid();
  __getpid();
  p = __fatalbuf;
  e = errno;
  p = __stpcpy(p, RED2);
  p = __stpcpy(p, "error");
  p = __stpcpy(p, UNBOLD);
  p = __stpcpy(p, ":");
  p = __stpcpy(p, BLUE1);
  p = __stpcpy(p, testlib_showerror_file);
  p = __stpcpy(p, ":");
  p = __intcpy(p, line);
  p = __stpcpy(p, RESET);
  p = __stpcpy(p, ": ");
  p = __stpcpy(p, testlib_showerror_func);
  p = __stpcpy(p, "(");
  p = __stpcpy(p, g_fixturename);
  p = __stpcpy(p, ")\n\t");
  p = __stpcpy(p, testlib_showerror_macro);
  p = __stpcpy(p, "(");
  p = __stpcpy(p, wantcode);
  p = __stpcpy(p, ", ");
  p = __stpcpy(p, gotcode);
  if (wantcode) {
    p = __stpcpy(p, ")\n\t\tneed ");
    p = __stpcpy(p, FREED_want);
    p = __stpcpy(p, " ");
    p = __stpcpy(p, testlib_showerror_symbol);
    p = __stpcpy(p, "\n\t\t got ");
    p = __stpcpy(p, FREED_got);
    p = __stpcpy(p, "\n");
  } else {
    p = __stpcpy(p, ")\n\t\t→ ");
    p = __stpcpy(p, testlib_showerror_symbol);
    p = __stpcpy(p, FREED_want);
    p = __stpcpy(p, "\n");
  }
  if (!isempty(fmt)) {
    *p++ = '\t';
    va_start(va, fmt);
    p += vsprintf(p, fmt, va);
    va_end(va);
    *p++ = '\n';
  }
  __stpcpy(hostname, "unknown");
  gethostname(hostname, sizeof(hostname));
  p = __stpcpy(p, "\t");
  p = __stpcpy(p, SUBTLE);
  p = __stpcpy(p, strerror(e));
  p = __stpcpy(p, RESET);
  p = __stpcpy(p, "\n\t");
  p = __stpcpy(p, SUBTLE);
  p = __stpcpy(p, program_invocation_name);
  p = __stpcpy(p, " @ ");
  p = __stpcpy(p, hostname);
  p = __stpcpy(p, RESET);
  p = __stpcpy(p, "\n");
  __write(__fatalbuf, p - __fatalbuf);
  free_s(&FREED_want);
  free_s(&FREED_got);
  ++g_testlib_failed;
  if (testlib_showerror_isfatal) testlib_abort();
}
