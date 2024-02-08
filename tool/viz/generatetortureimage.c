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
#include "libc/mem/gc.h"
#include "libc/str/str.h"
#include "libc/x/x.h"
#include "libc/x/xasprintf.h"
#include "third_party/stb/stb_image_write.h"

#define YN (1080 / YM)
#define XN (1920 / XM)
#define YM 2
#define XM 2

void *GenerateTortureImage(unsigned char RGB[YN][YM][XN][XM][3]) {
  long y, x, i, j, b, r, g;
  for (y = 0; y < YN; ++y) {
    for (x = 0; x < XN; ++x) {
      if ((y & 1) ^ (x & 1)) {
        r = 255;
        g = 255;
        b = 0;
      } else {
        r = 0;
        g = 0;
        b = 255;
      }
      for (i = 0; i < YM; ++i) {
        for (j = 0; j < XM; ++j) {
          RGB[y][i][x][j][0] = r;
          RGB[y][i][x][j][1] = g;
          RGB[y][i][x][j][2] = b;
        }
      }
    }
  }
  return RGB;
}

int main(int argc, char *argv[]) {
  stbi_write_png(
      gc(xasprintf("maxwell_%dx%d_%dx%d.png", XN * XM, YN * YM, XM, YM)),
      XN * XM, YN * YM, 3,
      GenerateTortureImage(
          gc(xmemalign(32, sizeof(unsigned char) * XN * XM * YN * YM * 3))),
      XN * XM * 3);
  return 0;
}
