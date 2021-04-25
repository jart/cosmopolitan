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
 * NUL are canonicalized as NUL. Therefore it isn't necessary to say
 * EscapeJsStringLiteral(Underlong(𝑥)) since EscapeJsStringLiteral(𝑥)
 * will do the same thing.
 *
 * @param p is input value
 * @param n if -1 implies strlen
 * @param out_size if non-NULL receives output length
 * @return allocated NUL-terminated buffer, or NULL w/ errno
 */
char *EscapeJsStringLiteral(const char *p, size_t n, size_t *z) {
  uint64_t w;
  char *q, *r;
  size_t i, j, m;
  wint_t x, a, b;
  if (z) *z = 0;
  if (n == -1) n = p ? strlen(p) : 0;
  if ((q = r = malloc(n * 6 + 6 + 1))) {
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
          *q++ = x;
          break;
        case '\t':
          q[0] = '\\';
          q[1] = 't';
          q += 2;
          break;
        case '\n':
          q[0] = '\\';
          q[1] = 'n';
          q += 2;
          break;
        case '\r':
          q[0] = '\\';
          q[1] = 'r';
          q += 2;
          break;
        case '\f':
          q[0] = '\\';
          q[1] = 'f';
          q += 2;
          break;
        case '\\':
          q[0] = '\\';
          q[1] = '\\';
          q += 2;
          break;
        case '/':
          q[0] = '\\';
          q[1] = '/';
          q += 2;
          break;
        case '"':
          q[0] = '\\';
          q[1] = '"';
          q += 2;
          break;
        case '\'':
          q[0] = '\\';
          q[1] = '\'';
          q += 2;
          break;
        case '<':
        case '>':
        case '&':
        case '=':
        default:
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
      }
    }
    if (z) *z = q - r;
    *q++ = '\0';
    if ((q = realloc(r, q - r))) r = q;
  }
  return r;
}
