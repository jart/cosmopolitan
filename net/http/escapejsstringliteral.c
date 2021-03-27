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
 * Things that can't be decoded will fall back to binary. Things that
 * can't be encoded will use invalid codepoint markers. This function is
 * agnostic to numbers that have been used with malicious intent in the
 * past under buggy software. Noncanonical encodings such as overlong
 * NUL are canonicalized as NUL.
 */
struct EscapeResult EscapeJsStringLiteral(const char *data, size_t size) {
  char *p;
  uint64_t w;
  unsigned i, n;
  wint_t x, a, b;
  const char *d, *e;
  struct EscapeResult r;
  d = data;
  e = data + size;
  p = r.data = xmalloc(size * 6 + 6 + 1);
  while (d < e) {
    x = *d++ & 0xff;
    if (x >= 0300) {
      a = ThomPikeByte(x);
      n = ThomPikeLen(x) - 1;
      if (d + n <= e) {
        for (i = 0;;) {
          b = d[i] & 0xff;
          if (!ThomPikeCont(b)) break;
          a = ThomPikeMerge(a, b);
          if (++i == n) {
            x = a;
            d += i;
            break;
          }
        }
      }
    }
    switch (x) {
      case ' ':
      case '!':
      case '#':
      case '$':
      case '%':
      case '(':
      case ')':
      case '*':
      case '+':
      case ',':
      case '-':
      case '.':
      case '0':
      case '1':
      case '2':
      case '3':
      case '4':
      case '5':
      case '6':
      case '7':
      case '8':
      case '9':
      case ':':
      case ';':
      case '?':
      case '@':
      case 'A':
      case 'B':
      case 'C':
      case 'D':
      case 'E':
      case 'F':
      case 'G':
      case 'H':
      case 'I':
      case 'J':
      case 'K':
      case 'L':
      case 'M':
      case 'N':
      case 'O':
      case 'P':
      case 'Q':
      case 'R':
      case 'S':
      case 'T':
      case 'U':
      case 'V':
      case 'W':
      case 'X':
      case 'Y':
      case 'Z':
      case '[':
      case ']':
      case '^':
      case '_':
      case '`':
      case 'a':
      case 'b':
      case 'c':
      case 'd':
      case 'e':
      case 'f':
      case 'g':
      case 'h':
      case 'i':
      case 'j':
      case 'k':
      case 'l':
      case 'm':
      case 'n':
      case 'o':
      case 'p':
      case 'q':
      case 'r':
      case 's':
      case 't':
      case 'u':
      case 'v':
      case 'w':
      case 'x':
      case 'y':
      case 'z':
      case '{':
      case '|':
      case '}':
      case '~':
        *p++ = x;
        break;
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
      case '<':
      case '>':
      case '&':
      case '=':
      default:
        w = EncodeUtf16(x);
        do {
          p[0] = '\\';
          p[1] = 'u';
          p[2] = "0123456789abcdef"[(w & 0xF000) >> 014];
          p[3] = "0123456789abcdef"[(w & 0x0F00) >> 010];
          p[4] = "0123456789abcdef"[(w & 0x00F0) >> 004];
          p[5] = "0123456789abcdef"[(w & 0x000F) >> 000];
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
