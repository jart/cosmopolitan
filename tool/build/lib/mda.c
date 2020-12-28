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
#include "libc/macros.h"
#include "libc/str/str.h"
#include "tool/build/lib/buffer.h"
#include "tool/build/lib/mda.h"

#define kBlink     1
#define kVisible   2
#define kUnderline 4
#define kBold      8
#define kReverse   16

/**
 * Decodes Monochrome Display Adapter attributes.
 * @see https://www.seasip.info/VintagePC/mda.html
 */
static uint8_t DecodeMdaAttributes(int8_t a) {
  uint8_t r = 0;
  if (a & 0x77) {
    if ((a & 0x77) == 0x70) r |= kReverse;
    if ((a & 0x07) == 0x01) r |= kUnderline;
    if (a & 0x08) r |= kBold;
    if (a < 0) r |= kBlink;
    r |= kVisible;
  }
  return r;
}

void DrawMda(struct Panel *p, uint8_t v[25][80][2]) {
  unsigned y, x, n, a, b;
  n = MIN(25, p->bottom - p->top);
  for (y = 0; y < n; ++y) {
    a = -1;
    for (x = 0; x < 80; ++x) {
      b = DecodeMdaAttributes(v[y][x][1]);
      if (a != b) {
        a = b;
        AppendStr(&p->lines[y], "\e[0");
        if (a & kBold) AppendStr(&p->lines[y], ";1");
        if (a & kUnderline) AppendStr(&p->lines[y], ";4");
        if (a & kBlink) AppendStr(&p->lines[y], ";5");
        if (a & kReverse) AppendStr(&p->lines[y], ";7");
        AppendChar(&p->lines[y], 'm');
      }
      if (a) {
        AppendWide(&p->lines[y], kCp437[v[y][x][0]]);
      } else {
        AppendChar(&p->lines[y], ' ');
      }
    }
  }
}
