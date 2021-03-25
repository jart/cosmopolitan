/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
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
#include "libc/str/thompike.h"
#include "libc/str/utf16.h"
#include "libc/x/x.h"
#include "net/http/escape.h"

/**
 * Escapes UTF-8 data for JavaScript or JSON string literal.
 *
 * HTML entities and forward slash are escaped too for added safety.
 *
 * We assume the UTF-8 is well-formed and can be represented as UTF-16.
 * Things that can't be decoded or encoded will be replaced with invalid
 * code-point markers. This function is agnostic to numbers that have
 * been used with malicious intent in the past under buggy software.
 * Noncanonical encodings such as overlong NUL are canonicalized as NUL.
 */
struct EscapeResult EscapeJsStringLiteral(const char *data, size_t size) {
  char *p;
  size_t i;
  unsigned n;
  uint64_t w;
  wint_t x, y;
  struct EscapeResult r;
  p = r.data = xmalloc(size * 6 + 6 + 1);
  for (i = 0; i < size;) {
    x = data[i++] & 0xff;
    if (x >= 0200) {
      if (x >= 0300) {
        n = ThomPikeLen(x);
        x = ThomPikeByte(x);
        while (--n) {
          if (i < size) {
            y = data[i++] & 0xff;
            if (ThomPikeCont(y)) {
              x = ThomPikeMerge(x, y);
            } else {
              x = 0xFFFD;
              break;
            }
          } else {
            x = 0xFFFD;
            break;
          }
        }
      } else {
        x = 0xFFFD;
      }
    }
    switch (x) {
      case '\t':
        p[0] = '\\';
        p[1] = 't';
        p += 2;
        break;
      case '\n':
        p[0] = '\\';
        p[1] = 'n';
        p += 2;
        break;
      case '\r':
        p[0] = '\\';
        p[1] = 'r';
        p += 2;
        break;
      case '\f':
        p[0] = '\\';
        p[1] = 'f';
        p += 2;
        break;
      case '\\':
        p[0] = '\\';
        p[1] = '\\';
        p += 2;
        break;
      case '/':
        p[0] = '\\';
        p[1] = '/';
        p += 2;
        break;
      case '"':
        p[0] = '\\';
        p[1] = '"';
        p += 2;
        break;
      case '\'':
        p[0] = '\\';
        p[1] = '\'';
        p += 2;
        break;
      default:
        if (0x20 <= x && x < 0x7F) {
          *p++ = x;
          break;
        }
        /* fallthrough */
      case '<':
      case '>':
      case '&':
      case '=':
        w = EncodeUtf16(x);
        do {
          p[0] = '\\';
          p[1] = 'u';
          p[2] = "0123456789ABCDEF"[(w & 0xF000) >> 014];
          p[3] = "0123456789ABCDEF"[(w & 0x0F00) >> 010];
          p[4] = "0123456789ABCDEF"[(w & 0x00F0) >> 004];
          p[5] = "0123456789ABCDEF"[(w & 0x000F) >> 000];
          p += 6;
        } while ((w >>= 16));
        break;
    }
  }
  r.size = p - r.data;
  r.data = xrealloc(r.data, r.size + 1);
  r.data[r.size] = '\0';
  return r;
}
