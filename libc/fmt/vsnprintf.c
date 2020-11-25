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
#include "libc/bits/safemacros.internal.h"
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
