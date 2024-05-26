/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2022 Justine Alexandra Roberts Tunney                              │
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
#include "dsp/scale/scale.h"
#include "libc/calls/calls.h"
#include "libc/calls/termios.h"
#include "libc/fmt/conv.h"
#include "libc/mem/mem.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/stdio.h"
#include "libc/stdio/sysparam.h"
#include "third_party/getopt/getopt.internal.h"
#include "third_party/stb/stb_image.h"

#define SQR(X)    ((X) * (X))
#define UNCUBE(x) x < 48 ? 0 : x < 115 ? 1 : (x - 35) / 40

#define USAGE \
  " IMAGE...\n\
\n\
SYNOPSIS\n\
\n\
  Prints Image\n\
\n\
FLAGS\n\
\n\
  -?      help\n\
  -t      true color mode\n\
  -f      print image at full size\n\
  -w INT  specify printed width in pixels (or cols)\n\
  -h INT  specify printed height in pixels (or rows*2)\n\
\n"

int FLAG_width;
int FLAG_height;
bool FLAG_true;
bool FLAG_full;
const char *prog;

/**
 * Quantizes 24-bit RGB to xterm256 code range [16,256).
 */
int rgb2xterm256(int r, int g, int b) {
  unsigned char cube[] = {0, 0137, 0207, 0257, 0327, 0377};
  int av, ir, ig, ib, il, qr, qg, qb, ql;
  av = r * .299 + g * .587 + b * .114 + .5;
  ql = (il = av > 238 ? 23 : (av - 3) / 10) * 10 + 8;
  qr = cube[(ir = UNCUBE(r))];
  qg = cube[(ig = UNCUBE(g))];
  qb = cube[(ib = UNCUBE(b))];
  if (SQR(qr - r) + SQR(qg - g) + SQR(qb - b) <=
      SQR(ql - r) + SQR(ql - g) + SQR(ql - b))
    return ir * 36 + ig * 6 + ib + 020;
  return il + 0350;
}

/**
 * Prints raw packed 8-bit RGB data from memory.
 */
void PrintImage(int yn, int xn, unsigned char rgb[yn][xn][3]) {
  unsigned y, x;
  for (y = 0; y < yn; y += 2) {
    for (x = 0; x < xn; ++x) {
      if (FLAG_true) {
        printf("\033[48;2;%hhu;%hhu;%hhu;38;2;%hhu;%hhu;%hhum▄",
               rgb[y + 0][x][0], rgb[y + 0][x][1], rgb[y + 0][x][2],
               rgb[MIN(y + 1, yn - 1)][x][0], rgb[MIN(y + 1, yn - 1)][x][1],
               rgb[MIN(y + 1, yn - 1)][x][2]);
      } else {
        printf(
            "\033[48;5;%hhu;38;5;%hhum▄",
            rgb2xterm256(rgb[y + 0][x][0], rgb[y + 0][x][1], rgb[y + 0][x][2]),
            rgb2xterm256(rgb[MIN(y + 1, yn - 1)][x][0],
                         rgb[MIN(y + 1, yn - 1)][x][1],
                         rgb[MIN(y + 1, yn - 1)][x][2]));
      }
    }
    printf("\e[0m\n");
  }
}

void Deblinterlace(int zn, int yn, int xn, unsigned char dst[zn][yn][xn],
                   const unsigned char src[yn][xn][zn]) {
  int y, x, z;
  for (y = 0; y < yn; ++y)
    for (x = 0; x < xn; ++x)
      for (z = 0; z < zn; ++z)
        dst[z][y][x] = src[y][x][z];
}

void Reblinterlace(int zn, int yn, int xn, unsigned char dst[yn][xn][zn],
                   const unsigned char src[zn][yn][xn]) {
  int y, x, z;
  for (y = 0; y < yn; ++y)
    for (x = 0; x < xn; ++x)
      for (z = 0; z < zn; ++z)
        dst[y][x][z] = src[z][y][x];
}

wontreturn void PrintUsage(int rc, int fd) {
  tinyprint(fd, "usage: ", prog, USAGE, NULL);
  exit(rc);
}

int main(int argc, char *argv[]) {

  // get program name
  prog = argv[0];
  if (!prog)
    prog = "basicidea";

  // get flags
  int opt;
  while ((opt = getopt(argc, argv, "?tfw:h:")) != -1) {
    switch (opt) {
      case 't':
        FLAG_true = true;
        break;
      case 'f':
        FLAG_full = true;
        break;
      case 'w':
        FLAG_width = atoi(optarg);
        break;
      case 'h':
        FLAG_height = atoi(optarg);
        break;
      case '?':
        PrintUsage(0, 1);
      default:
        PrintUsage(1, 2);
    }
  }
  if (argc == optind) {
    tinyprint(2, prog, ": missing image path (pass -? for help)\n", NULL);
    exit(1);
  }

  // process arguments
  for (int i = optind; i < argc; ++i) {
    const char *path = argv[i];

    // open image
    void *data;
    int cn, iyn, ixn;
    if (!(data = stbi_load(path, &ixn, &iyn, &cn, 3))) {
      perror(path);
      exit(1);
    }

    // determine size
    int yn, xn;
    struct winsize ws;
    if (FLAG_full) {
      yn = iyn;
      xn = ixn;
    } else if (FLAG_width > 0 || FLAG_height > 0) {
      if (FLAG_width <= 0) {
        yn = FLAG_height;
        xn = (double)ixn / iyn * FLAG_height + .5;
      } else if (FLAG_height <= 0) {
        yn = (double)iyn / ixn * FLAG_width + .5;
        xn = FLAG_width;
      } else {
        yn = FLAG_height;
        xn = FLAG_width;
      }
    } else if (!tcgetwinsize(1, &ws)) {
      yn = (ws.ws_row - 1) * 2;
      xn = ws.ws_col;
    } else {
      yn = iyn;
      xn = ixn;
    }

    // scale image
    if (yn != iyn || xn != ixn) {
      void *data2 = memalign(32, 3l * iyn * ixn);
      Deblinterlace(3, iyn, ixn, data2, data);
      free(data);
      data = memalign(32, 3l * yn * xn);
      EzGyarados(3, yn, xn, data, 3, iyn, ixn, data2, 0, 3, yn, xn, iyn, ixn, 0,
                 0, 0, 0);
      free(data2);
      data2 = memalign(32, 3l * yn * xn);
      Reblinterlace(3, yn, xn, data2, data);
      free(data);
      data = data2;
    }

    // print image
    PrintImage(yn, xn, data);
    free(data);
  }
}
