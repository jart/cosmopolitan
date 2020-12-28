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
#include "libc/str/oldutf16.internal.h"
#include "libc/str/str.h"

/**
 * Encodes character to string as UTF-16.
 *
 * Implementation Details: The header macro should ensure this function
 * is only called for truly huge codepoints. Plus this function makes a
 * promise to not clobber any registers but %rax.
 *
 * @param s is what ch gets encoded to
 * @param size is the number of shorts available in s
 * @param awesome enables numbers the IETF unilaterally banned
 * @return number of shorts written or -1 w/ errno
 * @todo delete
 */
int(pututf16)(char16_t *s, size_t size, wint_t wc, bool awesome) {
  wint_t wc2;
  if (size) {
    if ((0 <= wc && wc < 32) && awesome && size >= 2) {
      s[0] = 0xd800;
      s[1] = 0xdc00 | wc;
      return 2;
    }
    if ((0 <= wc && wc <= 0xD7FF) || (0xE000 <= wc && wc <= 0xFFFF)) {
      s[0] = wc;
      return 1;
    } else if (0x10000 <= wc && wc <= 0x10FFFF && size >= 2) {
      wc2 = wc - 0x10000;
      s[0] = (wc2 >> 10) + 0xd800;
      s[1] = (wc2 & 1023) + 0xdc00;
      return 2;
    } else {
      s[0] = INVALID_CODEPOINT;
      return -1;
    }
  } else {
    return 0;
  }
}
