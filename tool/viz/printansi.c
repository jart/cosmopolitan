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
#include "dsp/core/half.h"
#include "dsp/core/twixt8.h"
#include "dsp/scale/scale.h"
#include "dsp/tty/quant.h"
#include "dsp/tty/tty.h"
#include "libc/assert.h"
#include "libc/calls/calls.h"
#include "libc/calls/struct/stat.h"
#include "libc/calls/struct/winsize.h"
#include "libc/calls/termios.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/fmt/conv.h"
#include "libc/intrin/safemacros.internal.h"
#include "libc/limits.h"
#include "libc/log/check.h"
#include "libc/log/log.h"
#include "libc/macros.internal.h"
#include "libc/math.h"
#include "libc/mem/gc.h"
#include "libc/mem/mem.h"
#include "libc/stdio/rand.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/ex.h"
#include "libc/sysv/consts/exit.h"
#include "libc/sysv/consts/fileno.h"
#include "libc/sysv/consts/madv.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/prot.h"
#include "libc/sysv/consts/termios.h"
#include "libc/x/x.h"
#include "third_party/getopt/getopt.internal.h"
#include "third_party/stb/stb_image.h"
#include "tool/viz/lib/graphic.h"

#define SQR(x) ((x) * (x))

static struct Flags {
  const char *out;
  bool invert;
  bool subpixel;
  bool unsharp;
  bool dither;
  bool ruler;
  bool trailingnewline;
  long half;
  bool full;
  long width;
  long height;
  enum TtyBlocksSelection blocks;
  enum TtyQuantizationAlgorithm quant;
} g_flags;

static wontreturn void PrintUsage(int rc, int fd) {
  tinyprint(fd, "Usage: ", program_invocation_name, "\
 [FLAGS] [PATH]\n\
\n\
FLAGS\n\
\n\
  -w INT     width\n\
  -h INT     height\n\
  -i         invert\n\
  -?         shows this information\n\
\n\
EXAMPLES\n\
\n\
  printansi.com -w80 -h40 logo.png\n\
\n\
\n", NULL);
  exit(rc);
}

static int ParseNumberOption(const char *arg) {
  long x;
  x = strtol(arg, NULL, 0);
  if (!(1 <= x && x <= INT_MAX)) {
    fprintf(stderr, "invalid flexidecimal: %s\n\n", arg);
    exit(EXIT_FAILURE);
  }
  return x;
}

static void GetOpts(int *argc, char *argv[]) {
  int opt;
  struct winsize ws;
  g_flags.quant = kTtyQuantTrue;
  g_flags.blocks = IsWindows() ? kTtyBlocksCp437 : kTtyBlocksUnicode;
  if (*argc == 2 &&
      (strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-help") == 0)) {
    PrintUsage(EXIT_SUCCESS, STDOUT_FILENO);
  }
  while ((opt = getopt(*argc, argv, "?ivpfrtxads234o:w:h:")) != -1) {
    switch (opt) {
      case 'o':
        g_flags.out = optarg;
        break;
      case 'd':
        g_flags.dither = true;
        break;
      case 's':
        g_flags.unsharp = true;
        break;
      case 'w':
        g_flags.trailingnewline = true;
        g_flags.width = ParseNumberOption(optarg);
        break;
      case 'h':
        g_flags.trailingnewline = true;
        g_flags.height = ParseNumberOption(optarg);
        break;
      case 'f':
        g_flags.full = true;
        break;
      case 'i':
        g_flags.invert = true;
        break;
      case '2':
        g_flags.half = true;
        break;
      case 'r':
        g_flags.ruler = true;
        break;
      case 'p':
        g_flags.subpixel = true;
        break;
      case 'a':
        g_flags.quant = kTtyQuantAnsi;
        break;
      case 'x':
        g_flags.quant = kTtyQuantXterm256;
        break;
      case 't':
        g_flags.quant = kTtyQuantTrue;
        break;
      case '3':
        g_flags.blocks = kTtyBlocksCp437;
        break;
      case '4':
        g_flags.blocks = kTtyBlocksUnicode;
        break;
      case 'v':
        ++__log_level;
        break;
      case '?':
      default:
        if (opt == optopt) {
          PrintUsage(EXIT_SUCCESS, STDOUT_FILENO);
        } else {
          PrintUsage(EX_USAGE, STDERR_FILENO);
        }
    }
  }
  if (optind == *argc) {
    if (!g_flags.out) g_flags.out = "-";
    argv[(*argc)++] = "-";
  }
  if (!g_flags.full && (!g_flags.width || !g_flags.width)) {
    ws.ws_col = 80;
    ws.ws_row = 24;
    if (tcgetwinsize(STDIN_FILENO, &ws) != -1 ||
        tcgetwinsize(STDOUT_FILENO, &ws) != -1) {
      g_flags.width = ws.ws_col * (1 + !g_flags.half);
      g_flags.height = ws.ws_row * 2;
    }
  }
  ttyquantsetup(g_flags.quant, kTtyQuantRgb, g_flags.blocks);
}

static unsigned char AlphaBackground(unsigned y, unsigned x) {
  return 255;
}

static void *Deblinterlace(long dyn, long dxn, unsigned char dst[3][dyn][dxn],
                           long syn, long sxn, long scn,
                           const unsigned char src[syn][sxn][scn], long y0,
                           long yn, long x0, long xn) {
  long y, x;
  unsigned char c;
  for (y = y0; y < yn; ++y) {
    for (x = x0; x < xn; ++x) {
      switch (scn) {
        case 1:
          c = src[y][x][0];
          dst[0][y][x] = c;
          dst[1][y][x] = c;
          dst[2][y][x] = c;
          break;
        case 2:
          c = twixt8(AlphaBackground(y, x), src[y][x][0], src[y][x][1]);
          dst[0][y][x] = c;
          dst[1][y][x] = c;
          dst[2][y][x] = c;
          break;
        case 3:
          dst[0][y][x] = src[y][x][0];
          dst[1][y][x] = src[y][x][1];
          dst[2][y][x] = src[y][x][2];
          break;
        case 4:
          c = AlphaBackground(y, x);
          dst[0][y][x] = twixt8(c, src[y][x][0], src[y][x][3]);
          dst[1][y][x] = twixt8(c, src[y][x][1], src[y][x][3]);
          dst[2][y][x] = twixt8(c, src[y][x][2], src[y][x][3]);
          break;
      }
    }
  }
  return dst;
}

static void *DeblinterlaceSubpixelBgr(long dyn, long dxn,
                                      unsigned char dst[3][dyn][dxn][3],
                                      long syn, long sxn,
                                      const unsigned char src[syn][sxn][4],
                                      long y0, long yn, long x0, long xn) {
  long y, x;
  for (y = y0; y < yn; ++y) {
    for (x = x0; x < xn; ++x) {
      dst[0][y][x][0] = 0;
      dst[1][y][x][0] = 0;
      dst[2][y][x][0] = src[y][x][2];
      dst[0][y][x][1] = 0;
      dst[1][y][x][1] = src[y][x][1];
      dst[2][y][x][1] = 0;
      dst[0][y][x][2] = src[y][x][0];
      dst[1][y][x][2] = 0;
      dst[2][y][x][2] = 0;
    }
  }
  return dst;
}

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

static void *Raster(long yn, long xn, unsigned char Y[yn][xn]) {
  long y, x, i, j, k, s, bi, bs;
  for (y = 0; y + 4 <= yn; y += 4) {
    if (y) fputc('\n', stdout);
    for (x = 0; x + 2 <= xn; x += 2) {
      bi = 0;
      bs = LONG_MAX;
      for (k = 0; k < ARRAYLEN(kBlocks); ++k) {
        s = 0;
        for (i = 0; i < 4; ++i) {
          for (j = 0; j < 2; ++j) {
            s += SQR(Y[y + i][x + j] - kBlocks[k].b[i][j]);
          }
        }
        if (s < bs) {
          bi = k;
          bs = s;
        }
      }
      fputwc(kBlocks[bi].c, stdout);
    }
  }
  fputc('\n', stdout);
  return Y;
}

static void *Invert(long yn, long xn, unsigned char Y[yn][xn]) {
  long y, x;
  if (!g_flags.invert) {
    for (y = 0; y < yn; ++y) {
      for (x = 0; x < xn; ++x) {
        Y[y][x] = 255 - Y[y][x];
      }
    }
  }
  return Y;
}

static void *Grayify(long yn, long xn, unsigned char Y[yn][xn],
                     const unsigned char RGB[3][yn][xn]) {
  long y, x;
  for (y = 0; y < yn; ++y) {
    for (x = 0; x < xn; ++x) {
      Y[y][x] = RGB[0][y][x] * .299 + RGB[1][y][x] * .587 + RGB[2][y][x] * .114;
    }
  }
  return Y;
}

static void ProcessImage(long yn, long xn, unsigned char RGB[3][yn][xn]) {
  Raster(yn, xn,
         Invert(yn, xn, Grayify(yn, xn, gc(memalign(32, yn * xn)), RGB)));
}

void WithImageFile(const char *path,
                   void fn(long yn, long xn, unsigned char RGB[3][yn][xn])) {
  struct stat st;
  void *map, *data;
  int fd, yn, xn, cn, dyn, dxn, syn, sxn;
  CHECK_NE(-1, (fd = open(path, O_RDONLY)), "%s", path);
  CHECK_NE(-1, fstat(fd, &st));
  CHECK_GT(st.st_size, 0);
  CHECK_LE(st.st_size, INT_MAX);
  fadvise(fd, 0, 0, MADV_WILLNEED | MADV_SEQUENTIAL);
  CHECK_NE(MAP_FAILED,
           (map = mmap(NULL, st.st_size, PROT_READ, MAP_SHARED, fd, 0)));
  CHECK_NOTNULL(
      (data = gc(stbi_load_from_memory(map, st.st_size, &xn, &yn, &cn, 0))),
      "%s", path);
  CHECK_NE(-1, munmap(map, st.st_size));
  CHECK_NE(-1, close(fd));
  if (g_flags.subpixel) {
    data = DeblinterlaceSubpixelBgr(yn, xn, gc(memalign(32, yn * xn * 4 * 3)),
                                    yn, xn, data, 0, yn, 0, xn);
    xn *= 3;
  } else {
    data = Deblinterlace(yn, xn, gc(memalign(32, yn * xn * 4)), yn, xn, cn,
                         data, 0, yn, 0, xn);
    cn = 3;
  }
  if (g_flags.height && g_flags.width) {
    syn = yn;
    sxn = xn;
    dyn = g_flags.height * 4;
    dxn = g_flags.width * 2;
    while (HALF(syn) > dyn || HALF(sxn) > dxn) {
      if (HALF(sxn) > dxn) {
        Magikarp2xX(yn, xn, data, syn, sxn);
        Magikarp2xX(yn, xn, (char *)data + yn * xn, syn, sxn);
        Magikarp2xX(yn, xn, (char *)data + yn * xn * 2, syn, sxn);
        sxn = HALF(sxn);
      }
      if (HALF(syn) > dyn) {
        Magikarp2xY(yn, xn, data, syn, sxn);
        Magikarp2xY(yn, xn, (char *)data + yn * xn, syn, sxn);
        Magikarp2xY(yn, xn, (char *)data + yn * xn * 2, syn, sxn);
        syn = HALF(syn);
      }
    }
    data = EzGyarados(3, dyn, dxn, gc(memalign(32, dyn * dxn * 3)), cn, yn, xn,
                      data, 0, cn, dyn, dxn, syn, sxn, 0, 0, 0, 0);
    yn = dyn;
    xn = dxn;
  }
  fn(yn, xn, data);
}

int main(int argc, char *argv[]) {
  int i;
  GetOpts(&argc, argv);
  stbi_set_unpremultiply_on_load(true);
  for (i = optind; i < argc; ++i) {
    WithImageFile(argv[i], ProcessImage);
  }
  return 0;
}
