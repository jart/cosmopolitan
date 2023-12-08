/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2021 Justine Alexandra Roberts Tunney                              │
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
#include "libc/intrin/likely.h"
#include "libc/mem/mem.h"
#include "libc/str/str.h"
#include "libc/str/thompike.h"
#include "libc/str/utf16.h"
#include "libc/x/x.h"
#include "net/http/escape.h"

static const char kEscapeLiteral[128] = {
    9, 9, 9, 9, 9, 9, 9, 9, 9, 1, 2, 9, 4, 3, 9, 9,  // 0x00
    9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9,  // 0x10
    0, 0, 7, 0, 0, 0, 9, 9, 0, 0, 0, 0, 0, 0, 0, 6,  // 0x20
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 9, 9, 9, 0,  // 0x30
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // 0x40
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 0, 0, 0,  // 0x50
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // 0x60
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 9,  // 0x70
};

/**
 * Escapes UTF-8 data for JavaScript or JSON string literal.
 *
 * HTML entities and forward slash are escaped too for added safety.
 * Single quote (`'`) is \uxxxx-encoded for consistency, as it's
 * allowed in JavaScript, but not in JSON strings.
 *
 * We assume the UTF-8 is well-formed and can be represented as UTF-16.
 * Things that can't be decoded will fall back to binary. Things that
 * can't be encoded will use invalid codepoint markers. This function is
 * agnostic to numbers that have been used with malicious intent in the
 * past under buggy software. Noncanonical encodings such as overlong
 * NUL are canonicalized as NUL. Therefore it isn't necessary to say
 * EscapeJsStringLiteral(Underlong(𝑥)) since EscapeJsStringLiteral(𝑥)
 * will do the same thing.
 *
 * @param r is realloc'able output buffer reused between calls
 * @param y is used to track byte length of `*r`
 * @param p is input value
 * @param n if -1 implies strlen
 * @param out_size if non-NULL receives output length
 * @return *r on success, or null w/ errno
 */
char *EscapeJsStringLiteral(char **r, size_t *y, const char *p, size_t n,
                            size_t *z) {
  char *q;
  uint64_t w;
  size_t i, j, m;
  wint_t x, a, b;
  if (z) *z = 0;  // TODO(jart): why is this here?
  if (n == -1) n = p ? strlen(p) : 0;
  q = *r;
  i = n * 8 + 6 + 1;  // only need *6 but *8 is faster
  if (i > *y) {
    if ((q = realloc(q, i))) {
      *r = q;
      *y = i;
    }
  }
  if (q) {
    for (i = 0; i < n;) {
      x = p[i++] & 0xff;
      if (x >= 0300) {
        a = ThomPikeByte(x);
        m = ThomPikeLen(x) - 1;
        if (i + m <= n) {
          for (j = 0;;) {
            b = p[i + j] & 0xff;
            if (!ThomPikeCont(b)) break;
            a = ThomPikeMerge(a, b);
            if (++j == m) {
              x = a;
              i += j;
              break;
            }
          }
        }
      }
      switch (0 <= x && x <= 127 ? kEscapeLiteral[x] : 9) {
        case 0:
          *q++ = x;
          break;
        case 1:
          q[0] = '\\';
          q[1] = 't';
          q += 2;
          break;
        case 2:
          q[0] = '\\';
          q[1] = 'n';
          q += 2;
          break;
        case 3:
          q[0] = '\\';
          q[1] = 'r';
          q += 2;
          break;
        case 4:
          q[0] = '\\';
          q[1] = 'f';
          q += 2;
          break;
        case 5:
          q[0] = '\\';
          q[1] = '\\';
          q += 2;
          break;
        case 6:
          q[0] = '\\';
          q[1] = '/';
          q += 2;
          break;
        case 7:
          q[0] = '\\';
          q[1] = '"';
          q += 2;
          break;
        case 9:
          w = EncodeUtf16(x);
          do {
            q[0] = '\\';
            q[1] = 'u';
            q[2] = "0123456789abcdef"[(w & 0xF000) >> 014];
            q[3] = "0123456789abcdef"[(w & 0x0F00) >> 010];
            q[4] = "0123456789abcdef"[(w & 0x00F0) >> 004];
            q[5] = "0123456789abcdef"[(w & 0x000F) >> 000];
            q += 6;
          } while ((w >>= 16));
          break;
        default:
          __builtin_unreachable();
      }
    }
    if (z) *z = q - *r;
    *q++ = '\0';
  }
  return *r;
}
