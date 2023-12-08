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
#include "libc/fmt/conv.h"

/**
 * Returns absolute value of 𝑥.
 *
 * This function is a footgun since your argument may be narrrowed.
 * Consider using labs(), llabs(), or better yet a macro like this:
 *
 *     #define ABS(X) ((X) >= 0 ? (X) : -(X))
 *
 * Note that passing `x` as `INT_MIN` is undefined behavior, which
 * depends on whether or not your c library as well as the objects
 * that call it were built using the `-fwrapv` or `-ftrapv` flags.
 */
int abs(int x) {
  return x < 0 ? -x : x;
}

/**
 * Returns absolute value of 𝑥.
 */
long labs(long x) {
  return x < 0 ? -x : x;
}

__weak_reference(labs, llabs);
__weak_reference(labs, imaxabs);
