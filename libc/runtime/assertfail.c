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
#include "libc/assert.h"
#include "libc/bits/bits.h"
#include "libc/bits/weaken.h"
#include "libc/dce.h"
#include "libc/log/log.h"
#include "libc/macros.internal.h"
#include "libc/mem/alloca.h"
#include "libc/nt/runtime.h"
#include "libc/sysv/consts/nr.h"

static noasan size_t __assert_strlen(const char *s) {
  size_t i = 0;
  while (s[i]) ++i;
  return i;
}

static noasan char *__assert_stpcpy(char *d, const char *s) {
  size_t i;
  for (i = 0;; ++i) {
    if (!(d[i] = s[i])) {
      return d + i;
    }
  }
}

static privileged noinline noasan wontreturn void __assert_exit(int rc) {
  if (!IsWindows()) {
    asm volatile("syscall"
                 : /* no outputs */
                 : "a"(__NR_exit_group), "D"(rc)
                 : "memory");
    unreachable;
  } else {
    ExitProcess(rc);
  }
}

static privileged noinline noasan ssize_t __assert_write(const void *data,
                                                         size_t size) {
  ssize_t rc;
  uint32_t wrote;
  if (!IsWindows()) {
    asm volatile("syscall"
                 : "=a"(rc)
                 : "0"(__NR_write), "D"(2), "S"(data), "d"(size)
                 : "rcx", "r11", "memory");
    return rc;
  } else {
    if (WriteFile(GetStdHandle(kNtStdErrorHandle), data, size, &wrote, 0)) {
      return wrote;
    } else {
      return -1;
    }
  }
}

/**
 * Handles failure of assert() macro.
 */
relegated wontreturn noasan void __assert_fail(const char *expr,
                                               const char *file, int line) {
  static bool once;
  char *msg, *p, linebuf[16];
  unsigned i, exprlen, filelen;
  if (cmpxchg(&once, false, true)) {
    exprlen = expr ? __assert_strlen(expr) : 0;
    filelen = file ? __assert_strlen(file) : 0;
    p = msg = alloca(MIN(512, exprlen + filelen + 64));
    p = __assert_stpcpy(p, file);
    p = __assert_stpcpy(p, ":");
    if (line < 1) line = 1;
    for (i = 0; line; line /= 10) linebuf[i++] = '0' + line % 10;
    while (i) *p++ = linebuf[--i];
    p = __assert_stpcpy(p, ": assert(");
    p = __assert_stpcpy(p, expr);
    p = __assert_stpcpy(p, ")\r\n");
    __assert_write(msg, p - msg);
    if (weaken(__die)) weaken(__die)();
  }
  __assert_exit(23);
}
