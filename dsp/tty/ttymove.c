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
#include "dsp/tty/itoa8.h"
#include "dsp/tty/tty.h"
#include "libc/intrin/safemacros.internal.h"
#include "libc/limits.h"
#include "libc/log/check.h"

/**
 * Moves teletypewriter cursor to new coordinate.
 *
 * This uses codings defined by ANSI X3.4-1967 / X3.64-1979.
 *
 * @param c [in/out] tracks cursor coordinate
 * @param p [out] buffer receives ANSI/VT sequences
 * @param y is 0-indexed row
 * @param x is 0-indexed column
 * @return p + written, like mempcpy()
 */
char *ttymove(struct TtyCursor *c, char *p, int y, int x) {
  int d;
  DCHECK_GE(y, 0);
  DCHECK_GE(x, 0);
  DCHECK_LE(y, UINT16_MAX);
  DCHECK_LE(x, UINT16_MAX);
  if (y != c->y || x != c->x) {
    do {
      if (y != c->y && x != c->x) {
        if (y == c->y + 1 && x == 0) {
          p[0] = '\r';
          p[1] = '\n';
          p[2] = '\0';
          p[3] = '\0';
          p += 2;
          c->y++;
          c->x = 0;
          break;
        } else if (x < 256 && y < 256) {
          if (y == 0 && x == 0) {
            if (c->y == 0) {
              p[0] = '\r';
              p[1] = '\0';
              p += 1;
              c->x = 0;
            } else {
              p[0] = '\e'; /* CUP(1,1) */
              p[1] = '[';
              p[2] = 'H';
              p[3] = '\0';
              p += 3;
              c->y = 0;
              c->x = 0;
            }
          } else if (x == 0) {
            *p++ = '\e'; /* CUP(y,1) */
            *p++ = '[';
            p = itoa8(p, y + 1);
            *p++ = 'H';
            *p = '\0';
            c->y = y;
            c->x = 0;
          } else if (y == 0) {
            *p++ = '\e'; /* CUP(1,x) */
            *p++ = '[';
            *p++ = ';';
            *p = '\0';
            p = itoa8(p, x + 1);
            *p++ = 'H';
            *p = '\0';
            c->y = 0;
            c->x = x;
          } else {
            *p++ = '\e'; /* CUP(y,1) */
            *p++ = '[';
            p = itoa8(p, y + 1);
            *p++ = ';';
            p = itoa8(p, x + 1);
            *p++ = 'H';
            *p = '\0';
            c->y = y;
            c->x = x;
          }
          break;
        }
      }
      if (x != c->x) {
        if (!x) {
          p[0] = '\r'; /* goto beginning of line */
          p[1] = '\0';
          p += 1;
          c->x = 0;
        } else if (x > c->x) {
          d = min(255, x - c->x);
          if (d == 1) {
            p[0] = '\e';
            p[1] = '[';
            p[2] = 'C'; /* cursor forward (CUF) */
            p[3] = '\0';
            p += 3;
          } else {
            p[0] = '\e';
            p[1] = '[';
            p = itoa8(p + 2, d);
            p[0] = 'C'; /* cursor forward (CUF) */
            p[1] = '\0';
            p[2] = '\0';
            p[3] = '\0';
            p += 1;
          }
          c->x += d;
        } else {
          d = min(255, c->x - x);
          if (d == 1) {
            p[0] = '\e';
            p[1] = '[';
            p[2] = 'D'; /* cursor backward (CUB) */
            p[3] = '\0';
            p += 3;
          } else {
            p[0] = '\e';
            p[1] = '[';
            p = itoa8(p + 2, d);
            p[0] = 'D'; /* cursor backward (CUB) */
            p[1] = '\0';
            p[2] = '\0';
            p[3] = '\0';
            p += 1;
          }
          c->x -= d;
        }
      }
      if (y != c->y) {
        if (y > c->y) {
          d = min(255, y - c->y);
          if (d == 1) {
            p[0] = '\e';
            p[1] = 'D'; /* index down (IND) */
            p[2] = '\0';
            p[3] = '\0';
            p += 2;
          } else {
            p[0] = '\e';
            p[1] = '[';
            p = itoa8(p + 2, d);
            p[0] = 'B'; /* cursor down (CUD) */
            p[1] = '\0';
            p[2] = '\0';
            p[3] = '\0';
            p += 1;
          }
          c->y += d;
        } else {
          d = min(255, c->y - y);
          if (d == 1) {
            p[0] = '\e';
            p[1] = 'M'; /* reverse index (RI) */
            p[2] = '\0';
            p[3] = '\0';
            p += 2;
          } else {
            p[0] = '\e';
            p[1] = '[';
            p = itoa8(p + 2, d);
            p[0] = 'A'; /* cursor up (CUU) */
            p[1] = '\0';
            p[2] = '\0';
            p[3] = '\0';
            p += 1;
          }
          c->y -= d;
        }
      }
    } while (x != c->x || y != c->y);
  } else {
    p[0] = '\0';
  }
  DCHECK_EQ(y, c->y);
  DCHECK_EQ(x, c->x);
  DCHECK_EQ(p[0], '\0');
  return p;
}
