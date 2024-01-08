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
#include "libc/calls/struct/sigaction.h"
#include "libc/fmt/conv.h"
#include "libc/limits.h"
#include "libc/log/log.h"
#include "libc/macros.internal.h"
#include "libc/mem/gc.h"
#include "libc/mem/mem.h"
#include "libc/stdio/append.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/sig.h"
#include "libc/x/x.h"
#include "third_party/getopt/getopt.internal.h"
#include "third_party/stb/stb_truetype.h"

#define SQR(x) ((x) * (x))

int end;
int start;
int verbose;
int difference;

struct Block {
  char16_t c;
  unsigned char b[4][2];
} kBlocks[] = {
    {u' ', {{0000, 0000}, {0000, 0000}, {0000, 0000}, {0000, 0000}}},  //
    {u'░', {{0060, 0060}, {0060, 0060}, {0060, 0060}, {0060, 0060}}},  //
    {u'▒', {{0140, 0140}, {0140, 0140}, {0140, 0140}, {0140, 0140}}},  //
    {u'▓', {{0300, 0300}, {0300, 0300}, {0300, 0300}, {0300, 0300}}},  //
    {u'█', {{0377, 0377}, {0377, 0377}, {0377, 0377}, {0377, 0377}}},  //
    {u'▄', {{0000, 0000}, {0000, 0000}, {0377, 0377}, {0377, 0377}}},  //
    {u'▌', {{0377, 0000}, {0377, 0000}, {0377, 0000}, {0377, 0000}}},  //
    {u'▐', {{0000, 0377}, {0000, 0377}, {0000, 0377}, {0000, 0377}}},  //
    {u'▀', {{0377, 0377}, {0377, 0377}, {0000, 0000}, {0000, 0000}}},  //
};

static char *Raster(int yn, int xn, unsigned char Y[yn][xn], int *dw) {
  char *r = 0;
  unsigned char B[4][4];
  int y, x, i, j, k, s, w, bi, bs;
  *dw = 0;
  for (y = 0; y < yn; y += 4) {
    if (y) appendw(&r, '\n');
    for (w = x = 0; x < xn; x += 4) {
      for (i = 0; i < 4; ++i) {
        for (j = 0; j < 4; ++j) {
          if (y + i < yn && x + j < xn) {
            B[i][j] = Y[y + i][x + j];
          } else {
            B[i][j] = 0;
          }
        }
      }
      bi = 0;
      bs = INT_MAX;
      for (k = 0; k < ARRAYLEN(kBlocks); ++k) {
        s = 0;
        for (i = 0; i < 4; ++i) {
          for (j = 0; j < 4; ++j) {
            s += SQR(B[i][j] - kBlocks[k].b[i][j / 2]);
          }
        }
        if (s < bs) {
          bi = k;
          bs = s;
        }
      }
      appendw(&r, tpenc(kBlocks[bi].c));
      ++w;
    }
    if (w > *dw) *dw = w;
  }
  return r;
}

void OnSig(int sig) {
  exit(128 + sig);
}

int main(int argc, char *argv[]) {
  char *p;
  void *bmap;
  float scale;
  bool gotsome;
  char **rasters;
  char **fasters;
  size_t ttfsize;
  bool isdifferent;
  unsigned char **ttf;
  stbtt_fontinfo *font;
  int c, i, j, m, o, dw, maxw, *w, *h, s = 40 * 4;
  ShowCrashReports();
  signal(SIGPIPE, OnSig);
  start = 0;
  end = 0x10FFFD;
  while ((o = getopt(argc, argv, "vdc:s:e:S:")) != -1) {
    switch (o) {
      case 'v':
        ++verbose;
        break;
      case 'd':
        difference = 1;
        break;
      case 'c':
        start = end = strtol(optarg, 0, 16);
        break;
      case 's':
        start = strtol(optarg, 0, 16);
        break;
      case 'e':
        end = strtol(optarg, 0, 16);
        break;
      case 'S':
        s = strtol(optarg, 0, 0);
        break;
      default:
        return 1;
    }
  }
  m = argc - optind;
  w = gc(calloc(m, sizeof(*w)));
  h = gc(calloc(m, sizeof(*h)));
  ttf = gc(calloc(m, sizeof(*ttf)));
  font = gc(calloc(m, sizeof(*font)));
  rasters = gc(calloc(m, sizeof(*rasters)));
  fasters = gc(calloc(m, sizeof(*fasters)));
  for (j = 0; j < m; ++j) {
    ttf[j] = gc(xslurp(argv[optind + j], &ttfsize));
    if (!ttf[j]) {
      fprintf(stderr, "%s: not found\n", argv[optind + j]);
      exit(1);
    }
    stbtt_InitFont(font + j, ttf[j], stbtt_GetFontOffsetForIndex(ttf[j], 0));
    printf("%s\n", argv[optind + j]);
  }
  for (c = start; c <= end; ++c) {
    maxw = 0;
    gotsome = false;
    isdifferent = false;
    for (j = 0; j < m; ++j) {
      rasters[j] = 0;
      if ((i = stbtt_FindGlyphIndex(font + j, c)) > 0) {
        w[j] = 0;
        h[j] = 0;
        scale = stbtt_ScaleForPixelHeight(font + j, s);
        bmap = stbtt_GetGlyphBitmap(font + j, 0, scale, i, w + j, h + j, 0, 0);
        if (w[j] && h[j]) {
          gotsome = true;
          if (verbose) {
            rasters[j] = Raster(h[j], w[j], bmap, &dw);
            if (!isdifferent && j && rasters[j - 1] &&
                strcmp(rasters[j], rasters[j - 1])) {
              isdifferent = true;
            }
            if (dw > maxw) maxw = dw;
          }
        }
        free(bmap);
      }
    }
    if (gotsome) {
      memcpy(fasters, rasters, m * sizeof(*rasters));
      printf("%04X\n", c);
      if (verbose && (!difference || isdifferent)) {
        do {
          gotsome = false;
          for (j = 0; j < m; ++j) {
            if (!rasters[j]) {
              printf("%-*s    ", maxw, "");
              continue;
            }
            p = strchrnul(rasters[j], '\n');
            if (p - rasters[j]) gotsome = true;
            printf("%-*.*s    ", maxw, p - rasters[j], rasters[j]);
            rasters[j] = *p ? p + 1 : p;
          }
          printf("\n");
        } while (gotsome);
        printf("\n");
      }
      for (j = 0; j < m; ++j) {
        free(fasters[j]);
      }
    }
  }
  return 0;
}
