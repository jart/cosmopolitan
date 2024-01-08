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
#include "dsp/scale/cdecimate2xuint8x8.h"
#include "libc/calls/struct/winsize.h"
#include "libc/calls/termios.h"
#include "libc/fmt/conv.h"
#include "libc/fmt/libgen.h"
#include "libc/intrin/bsr.h"
#include "libc/log/libfatal.internal.h"
#include "libc/log/log.h"
#include "libc/macros.internal.h"
#include "libc/mem/gc.h"
#include "libc/mem/mem.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/termios.h"
#include "libc/x/x.h"
#include "third_party/getopt/getopt.internal.h"
#include "third_party/stb/stb_truetype.h"

#define MAXCODE

int start;
int end = 0x03134A /* 0x10FFFD */;
int verbose;

struct winsize ws = {24, 80};
signed char kThePerfectKernel[8] = {-1, -1, 3, 15, 15, 3, -1, -1};

void PrintBar(unsigned char *p, size_t n) {
  size_t i, j;
  for (j = n; j > ws.ws_col - 32; j = (j + 1) >> 1) {
    cDecimate2xUint8x8(j, p, kThePerfectKernel);
  }
  for (i = 0; i < j; ++i) {
    if (p[i]) {
      fputwc(u"░░▒▒▓▓██"[_bsr(p[i])], stdout);
    } else {
      fputc(' ', stdout);
    }
  }
}

int main(int argc, char *argv[]) {
  size_t n, m;
  float scale;
  void *bitmap;
  size_t ttfsize;
  unsigned char *ttf;
  stbtt_fontinfo font;
  unsigned char *present;
  unsigned char *intotal;
  int w, h, i, c, arg, opt, errs, line, s = 40 * 4, rc = 0;
  ShowCrashReports();
  tcgetwinsize(0, &ws);
  while ((opt = getopt(argc, argv, "vs:e:")) != -1) {
    switch (opt) {
      case 'v':
        ++verbose;
        break;
      case 's':
        start = strtol(optarg, 0, 16);
        break;
      case 'e':
        end = strtol(optarg, 0, 16);
        break;
      default:
        return 1;
    }
  }
  n = end + 1 - start;
  m = ROUNDUP(n, 16);
  present = gc(malloc(m));
  intotal = gc(calloc(1, m));
  if (optind < argc) {
    for (arg = optind; arg < argc; ++arg) {
      ttf = xslurp(argv[arg], &ttfsize);
      if (!(line = setjmp(stbtt_jmpbuf))) {
        stbtt_InitFont(&font, ttf, stbtt_GetFontOffsetForIndex(ttf, 0));
      } else {
        rc = 1;
        fprintf(stderr, "%s: error loading font (assert @ stb_truetype.c:%d\n",
                argv[arg], line);
        free(ttf);
        continue;
      }
      bzero(present, m);
      for (errs = 0, c = start; c <= end; ++c) {
        if (!(line = setjmp(stbtt_jmpbuf))) {
          if ((i = stbtt_FindGlyphIndex(&font, c)) > 0) {
            w = h = 0;
            scale = stbtt_ScaleForPixelHeight(&font, s);
            bitmap = stbtt_GetGlyphBitmap(&font, 0, scale, i, &w, &h, 0, 0);
            if (w && h) {
              intotal[c - start] = present[c - start] = 255;
            }
            free(bitmap);
          }
        } else {
          ++errs;
        }
      }
      PrintBar(present, m);
      if (errs) {
        printf(" %s (%d errors)\n", basename(argv[arg]), errs);
      } else {
        printf(" %s (%,d kb)\n", basename(argv[arg]), ttfsize / 1024);
      }
      free(ttf);
    }
    PrintBar(intotal, m);
    fputc('\n', stdout);
  }
  return rc;
}
