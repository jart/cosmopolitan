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
#include "libc/assert.h"
#include "libc/dce.h"
#include "libc/fmt/internal.h"
#include "libc/limits.h"
#include "libc/macros.internal.h"
#include "libc/runtime/runtime.h"
#include "libc/str/str.h"

struct SprintfStr {
  char *p;
  size_t i;
  size_t n;
};

static int vsnprintfputchar(const char *s, struct SprintfStr *t, size_t n) {
  if (n) {
    if (n == 1 && t->i < t->n) {
      t->p[t->i] = s[0];
    } else if (t->i + n <= t->n) {
      memcpy(t->p + t->i, s, n);
    } else if (t->i < t->n) {
      memcpy(t->p + t->i, s, t->n - t->i);
    }
    t->i += n;
  }
  return 0;
}

/**
 * Formats string to buffer w/ preexisting vararg state.
 *
 * @param buf stores output
 * @param size is byte capacity buf
 * @return number of bytes written, excluding the NUL terminator; or,
 *     if the output buffer wasn't passed, or was too short, then the
 *     number of characters that *would* have been written is returned
 * @see __fmt() and printf() for detailed documentation
 * @asyncsignalsafe
 * @vforksafe
 */
int vsnprintf(char *buf, size_t size, const char *fmt, va_list va) {
  struct SprintfStr str = {buf, 0, size};
  int rc = __fmt(vsnprintfputchar, &str, fmt, va);
  if (rc < 0) return rc;
  if (str.n) str.p[MIN(str.i, str.n - 1)] = '\0';
  return str.i;
}
