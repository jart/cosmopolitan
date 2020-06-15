/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2020 Justine Alexandra Roberts Tunney                              │
│                                                                              │
│ This program is free software; you can redistribute it and/or modify         │
│ it under the terms of the GNU General Public License as published by         │
│ the Free Software Foundation; version 2 of the License.                      │
│                                                                              │
│ This program is distributed in the hope that it will be useful, but          │
│ WITHOUT ANY WARRANTY; without even the implied warranty of                   │
│ MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU             │
│ General Public License for more details.                                     │
│                                                                              │
│ You should have received a copy of the GNU General Public License            │
│ along with this program; if not, write to the Free Software                  │
│ Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA                │
│ 02110-1301 USA                                                               │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "libc/runtime/gc.h"
#include "libc/str/str.h"
#include "libc/x/x.h"
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
