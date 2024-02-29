/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
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
#include "ape/sections.internal.h"
#include "libc/assert.h"
#include "libc/calls/calls.h"
#include "libc/errno.h"
#include "libc/fmt/itoa.h"
#include "libc/intrin/atomic.h"
#include "libc/intrin/kprintf.h"
#include "libc/intrin/safemacros.internal.h"
#include "libc/intrin/weaken.h"
#include "libc/log/color.internal.h"
#include "libc/log/internal.h"
#include "libc/log/libfatal.internal.h"
#include "libc/mem/mem.h"
#include "libc/runtime/runtime.h"
#include "libc/str/str.h"
#include "libc/testlib/testlib.h"

const char *testlib_showerror_errno;
const char *testlib_showerror_file;
const char *testlib_showerror_func;
const char *testlib_showerror_macro;
const char *testlib_showerror_symbol;

static void Free(void *p) {
  if (_weaken(free) && (long)p >= (long)_end) {
    _weaken(free)(p);
  }
}

void testlib_showerror(const char *file, int line, const char *func,
                       const char *method, const char *symbol, const char *code,
                       char *v1, char *v2) {
  char hostname[128], linestr[12], pidstr[12], tidstr[12];
  stpcpy(hostname, "unknown");
  gethostname(hostname, sizeof(hostname));
  FormatInt32(linestr, line);
  FormatInt32(pidstr, getpid());
  FormatInt32(tidstr, gettid());
  tinyprint(2, RED2, "error", UNBOLD, ":", BLUE1,                     //
            file, ":", linestr, RESET, ": ",                          //
            method, "() in ", func, "(", g_fixturename, ") ",         //
            "on ", hostname, " pid ", pidstr, " tid ", tidstr, "\n",  //
            "\t", code, "\n",                                         //
            "\t\tneed ", v1, " ", symbol, "\n",                       //
            "\t\t got ", v2, "\n",                                    //
            "\t", SUBTLE, strerror(errno), "\n",                      //
            "\t", __argv[0], RESET, "\n",                             //
            NULL);
  Free(v1);
  Free(v2);
}

static void testlib_showerror_(int line,              //
                               const char *wantcode,  //
                               const char *gotcode,   //
                               char *FREED_want,      //
                               char *FREED_got,       //
                               const char *fmt,       //
                               va_list va) {
  int e = errno;
  char hostname[128], linestr[12], pidstr[12], tidstr[12];
  stpcpy(hostname, "unknown");
  gethostname(hostname, sizeof(hostname));
  FormatInt32(linestr, line);
  FormatInt32(pidstr, getpid());
  FormatInt32(tidstr, gettid());
  tinyprint(                                                               //
      2, RED2, "error", UNBOLD, BLUE1, ":",                                //
      testlib_showerror_file, ":", linestr, RESET, ": ",                   //
      testlib_showerror_func, "(", g_fixturename, ") ",                    //
      "on ", hostname, " pid ", pidstr, " tid ", tidstr, "\n",             //
      "\t", testlib_showerror_macro, "(", wantcode, ", ", gotcode, ")\n",  //
      NULL);
  if (wantcode) {
    tinyprint(2, "\t\tneed ", FREED_want, " ", testlib_showerror_symbol, "\n",
              "\t\t got ", FREED_got, "\n", NULL);
  } else {
    tinyprint(2, "\t\t→ ", testlib_showerror_symbol, FREED_want, "\n", NULL);
  }
  if (!isempty(fmt)) {
    if (_weaken(kvprintf)) {
      tinyprint(2, "\t", NULL);
      _weaken(kvprintf)(fmt, va);
      tinyprint(2, "\n", NULL);
    } else {
      tinyprint(2, "\t[missing kvprintf]\n", NULL);
    }
  }
  tinyprint(2, "\t", SUBTLE, strerror(e), RESET, "\n\t", SUBTLE,
            firstnonnull(program_invocation_name, "unknown"), " @ ", hostname,
            RESET, "\n", NULL);
  ++g_testlib_failed;
  Free(FREED_want);
  Free(FREED_got);
}

void testlib_showerror_assert_eq(int line,              //
                                 const char *wantcode,  //
                                 const char *gotcode,   //
                                 char *FREED_want,      //
                                 char *FREED_got,       //
                                 const char *fmt,       //
                                 ...) {
  va_list va;
  testlib_showerror_macro = "ASSERT_EQ";
  testlib_showerror_symbol = "=";
  va_start(va, fmt);
  testlib_showerror_(line, wantcode, gotcode, FREED_want, FREED_got, fmt, va);
  va_end(va);
  testlib_abort();
}

void testlib_showerror_expect_eq(int line,              //
                                 const char *wantcode,  //
                                 const char *gotcode,   //
                                 char *FREED_want,      //
                                 char *FREED_got,       //
                                 const char *fmt,       //
                                 ...) {
  va_list va;
  testlib_showerror_macro = "EXPECT_EQ";
  testlib_showerror_symbol = "=";
  va_start(va, fmt);
  testlib_showerror_(line, wantcode, gotcode, FREED_want, FREED_got, fmt, va);
  va_end(va);
}

void testlib_showerror_assert_ne(int line,              //
                                 const char *wantcode,  //
                                 const char *gotcode,   //
                                 char *FREED_want,      //
                                 char *FREED_got,       //
                                 const char *fmt,       //
                                 ...) {
  va_list va;
  testlib_showerror_macro = "ASSERT_NE";
  testlib_showerror_symbol = "≠";
  va_start(va, fmt);
  testlib_showerror_(line, wantcode, gotcode, FREED_want, FREED_got, fmt, va);
  va_end(va);
  testlib_abort();
}

void testlib_showerror_expect_ne(int line,              //
                                 const char *wantcode,  //
                                 const char *gotcode,   //
                                 char *FREED_want,      //
                                 char *FREED_got,       //
                                 const char *fmt,       //
                                 ...) {
  va_list va;
  testlib_showerror_macro = "EXPECT_NE";
  testlib_showerror_symbol = "≠";
  va_start(va, fmt);
  testlib_showerror_(line, wantcode, gotcode, FREED_want, FREED_got, fmt, va);
  va_end(va);
}

void testlib_showerror_assert_true(int line,              //
                                   const char *wantcode,  //
                                   const char *gotcode,   //
                                   char *FREED_want,      //
                                   char *FREED_got,       //
                                   const char *fmt,       //
                                   ...) {
  va_list va;
  testlib_showerror_macro = "ASSERT_TRUE";
  testlib_showerror_symbol = "";
  va_start(va, fmt);
  testlib_showerror_(line, wantcode, gotcode, FREED_want, FREED_got, fmt, va);
  va_end(va);
  testlib_abort();
}

void testlib_showerror_expect_true(int line,              //
                                   const char *wantcode,  //
                                   const char *gotcode,   //
                                   char *FREED_want,      //
                                   char *FREED_got,       //
                                   const char *fmt,       //
                                   ...) {
  va_list va;
  testlib_showerror_macro = "EXPECT_TRUE";
  testlib_showerror_symbol = "";
  va_start(va, fmt);
  testlib_showerror_(line, wantcode, gotcode, FREED_want, FREED_got, fmt, va);
  va_end(va);
}

void testlib_showerror_assert_false(int line,              //
                                    const char *wantcode,  //
                                    const char *gotcode,   //
                                    char *FREED_want,      //
                                    char *FREED_got,       //
                                    const char *fmt,       //
                                    ...) {
  va_list va;
  testlib_showerror_macro = "ASSERT_FALSE";
  testlib_showerror_symbol = "!";
  va_start(va, fmt);
  testlib_showerror_(line, wantcode, gotcode, FREED_want, FREED_got, fmt, va);
  va_end(va);
  testlib_abort();
}

void testlib_showerror_expect_false(int line,              //
                                    const char *wantcode,  //
                                    const char *gotcode,   //
                                    char *FREED_want,      //
                                    char *FREED_got,       //
                                    const char *fmt,       //
                                    ...) {
  va_list va;
  testlib_showerror_macro = "EXPECT_FALSE";
  testlib_showerror_symbol = "!";
  va_start(va, fmt);
  testlib_showerror_(line, wantcode, gotcode, FREED_want, FREED_got, fmt, va);
  va_end(va);
}

void testlib_showerror_expect_matrixeq(int line,              //
                                       const char *wantcode,  //
                                       const char *gotcode,   //
                                       char *FREED_want,      //
                                       char *FREED_got,       //
                                       const char *fmt,       //
                                       ...) {
  va_list va;
  testlib_showerror_macro = "EXPECT_MATRIXEQ";
  testlib_showerror_symbol = "=";
  va_start(va, fmt);
  testlib_showerror_(line, wantcode, gotcode, FREED_want, FREED_got, fmt, va);
  va_end(va);
}
