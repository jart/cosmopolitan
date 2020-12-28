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
    if (weaken(__die)) weaken(__die)();
  }
  abort();
  unreachable;
}
