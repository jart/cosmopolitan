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
#include "libc/fmt/conv.h"
#include "libc/limits.h"

/**
 * Decodes signed integer from ASCII string.
 *
 * @param s is a non-null nul-terminated string
 * @param endptr if non-null will always receive a pointer to the char
 *     following the last one this function processed, which is usually
 *     the NUL byte, or in the case of invalid strings, would point to
 *     the first invalid character
 * @param base can be anywhere between [2,36] or 0 to auto-detect based
 *     on the the prefixes 0 (octal), 0x (hexadecimal), 0b (binary), or
 *     decimal (base 10) by default
 * @return decoded signed saturated integer
 */
long long strtoll(const char *s, char **endptr, int base) {
  _Static_assert(LONG_MAX == LONG_LONG_MAX, "need strtoll impl");
  return strtol(s, endptr, base);
}
