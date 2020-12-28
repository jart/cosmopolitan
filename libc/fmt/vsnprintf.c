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
#include "libc/dce.h"
#include "libc/fmt/fmt.h"
#include "libc/limits.h"
#include "libc/runtime/runtime.h"

struct SprintfStr {
  char *p;
  size_t i;
  size_t n;
};

static int vsnprintfputchar(unsigned char c, struct SprintfStr *str) {
  if (str->i < str->n) str->p[str->i] = c;
  str->i++;
  return 0;
}

/**
 * Formats string to buffer w/ preexisting vararg state.
 *
 * @param buf stores output and a NUL-terminator is always written,
 *     provided buf!=NULL && size!=0
 * @param size is byte capacity buf
 * @return number of bytes written, excluding the NUL terminator; or,
 *     if the output buffer wasn't passed, or was too short, then the
 *     number of characters that *would* have been written is returned
 * @see palandprintf() and printf() for detailed documentation
 */
int(vsnprintf)(char *buf, size_t size, const char *fmt, va_list va) {
  struct SprintfStr str = {buf, 0, size};
  palandprintf(vsnprintfputchar, &str, fmt, va);
  if (str.n) str.p[min(str.i, str.n - 1)] = '\0';
  return str.i;
}
