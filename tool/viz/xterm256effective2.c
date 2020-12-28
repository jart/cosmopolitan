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
#include "dsp/tty/quant.h"
#include "libc/fmt/fmt.h"
#include "libc/log/check.h"
#include "libc/macros.h"
#include "libc/math.h"
#include "libc/mem/mem.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/ex.h"
#include "libc/sysv/consts/exit.h"
#include "third_party/getopt/getopt.h"

#define USAGE \
  " [FLAGS] [PATH...]\n\
\n\
Flags:\n\
  -o PATH    output path\n\
  -h         shows this information\n\
\n"

static size_t linecap_;
static FILE *in_, *out_;
static char *inpath_, *outpath_, *line_;

void PrintUsage(int rc, FILE *f) {
  fputs("Usage: ", f);
  fputs(program_invocation_name, f);
  fputs(USAGE, f);
  exit(rc);
}

void GetOpts(int *argc, char *argv[]) {
  int opt;
  outpath_ = "-";
  while ((opt = getopt(*argc, argv, "?ho:")) != -1) {
    switch (opt) {
      case 'o':
        outpath_ = optarg;
        break;
      case '?':
      case 'h':
        PrintUsage(EXIT_SUCCESS, stdout);
      default:
        PrintUsage(EX_USAGE, stderr);
    }
  }
  if (optind == *argc) {
    argv[(*argc)++] = "-";
  }
}

#define U256F1(X) ((float)((X)&0xffu) * 256.0f)
#define F1U256(X) MAX(MIN((int)rintl(roundl(256.0f * (X))), 255), 0)

forceinline struct TtyRgb getquant(unsigned xt) {
  return g_ansi2rgb_[xt];
}

forceinline unsigned dist(int x, int y) {
  return x - y;
}

forceinline unsigned sqr(int x) {
  return x * x;
}

static unsigned rgb2hsl(unsigned rgba) {
  /* this is broken */
  unsigned h8, s8, l8;
  float r, g, b, h, s, d, l, cmax, cmin;
  r = U256F1(rgba);
  g = U256F1(rgba >> 010);
  b = U256F1(rgba >> 020);
  cmax = MAX(MAX(r, g), b);
  cmin = MIN(MIN(r, g), b);
  h = 0.0f;
  s = 0.0f;
  d = cmax - cmin;
  l = (cmax + cmin) / 2.0f;
  if (cmax != cmin) {
    s = l > 0.5L ? d / (2.0f - cmax - cmin) : d / (cmax + cmin);
    if (cmax == r) {
      h = (g - b) / d + (g < b ? 6.0f : 0.0f);
    } else if (cmax == g) {
      h = (b - r) / d + 2.0f;
    } else if (cmax == b) {
      h = (r - g) / d + 4.0f;
    }
    h /= 6.0f;
  }
  h8 = F1U256(h);
  s8 = F1U256(s);
  l8 = F1U256(l);
  return ((rgba >> 030) & 255) << 030 | l8 << 020 | s8 << 010 | h8;
}
static struct TtyRgb rgb2hsl2(struct TtyRgb rgb) {
  unsigned x =
      (unsigned)rgb.b << 020 | (unsigned)rgb.g << 010 | (unsigned)rgb.r;
  unsigned y = rgb2hsl(x);
  return (struct TtyRgb){
      .r = y & 0xff, .g = (y >> 010) & 0xff, .b = (y >> 020) & 0xff};
}
static unsigned rgbdist(struct TtyRgb x, struct TtyRgb y) {
  x = rgb2hsl2(x);
  y = rgb2hsl2(y);
  return sqrt(sqr(dist(x.r, y.r)) + sqr(dist(x.g, y.g)) + sqr(dist(x.b, y.b)));
}

static unsigned xtdist(unsigned x, unsigned y) {
  return rgbdist(getquant(x), getquant(y));
}

void Show(unsigned color, unsigned bg, unsigned fg, unsigned glyph) {
  uint8_t r, g, b;
  b = (color >> 020) & 0xff;
  g = (color >> 010) & 0xff;
  r = color & 0xff;
  printf("\tmix\t0x%04x,%3d,%3d,%3d,%3d,%3d,%3d\t# \e[48;2;%d;%d;%dm   \e[0m\n",
         rgbdist((struct TtyRgb){r, g, b, 0}, (struct TtyRgb){0, 0, 0, 0}), r,
         g, b, bg, fg, glyph, r, g, b);
}

void ProcessFile(void) {
  char *p;
  unsigned color1, bg1, fg1, glyph1;
  unsigned color, bg, fg, glyph;
  color1 = -1u;
  bg1 = -1u;
  fg1 = -1u;
  glyph1 = -1u;
  while ((getline(&line_, &linecap_, in_)) != -1) {
    p = chomp(line_);
    sscanf(p, "%x, %u,%u,%u", &color, &bg, &fg, &glyph);
    if (color != color1) {
      if (color1 != -1u) {
        Show(color1, bg1, fg1, glyph1);
      }
      color1 = color;
      bg1 = bg;
      fg1 = fg;
      glyph1 = glyph;
    }
    if ((fg1 && !fg) || (fg && fg1 && xtdist(fg, bg) < xtdist(fg1, bg1))) {
      color1 = color;
      bg1 = bg;
      fg1 = fg;
      glyph1 = glyph;
    }
  }
  Show(color1, bg1, fg1, glyph1);
}

int main(int argc, char *argv[]) {
  size_t i;
  GetOpts(&argc, argv);
  CHECK_NOTNULL((out_ = fopen(outpath_, "w")));
  for (i = optind; i < argc; ++i) {
    CHECK_NOTNULL((in_ = fopen((inpath_ = argv[i]), "r")));
    ProcessFile();
    CHECK_NE(-1, fclose_s(&in_));
  }
  CHECK_NE(-1, fclose_s(&out_));
  free(line_);
  return 0;
}
