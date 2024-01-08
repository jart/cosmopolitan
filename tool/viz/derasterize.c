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
#include "libc/assert.h"
#include "libc/calls/calls.h"
#include "libc/calls/struct/stat.h"
#include "libc/calls/termios.h"
#include "libc/fmt/conv.h"
#include "libc/limits.h"
#include "libc/log/check.h"
#include "libc/log/log.h"
#include "libc/macros.internal.h"
#include "libc/math.h"
#include "libc/mem/gc.h"
#include "libc/mem/mem.h"
#include "libc/nexgen32e/x86feature.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/ex.h"
#include "libc/sysv/consts/exit.h"
#include "libc/sysv/consts/fileno.h"
#include "libc/sysv/consts/madv.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/prot.h"
#include "libc/x/x.h"
#include "third_party/getopt/getopt.internal.h"
#include "third_party/stb/stb_image.h"
#include "third_party/stb/stb_image_resize.h"
#ifdef __x86_64__

#define HELPTEXT \
  "\n\
NAME\n\
\n\
  derasterize - convert pictures to text using unicode ANSI art\n\
\n\
SYNOPSIS\n\
\n\
  derasterize [FLAGS] [PNG|JPG|ETC]...\n\
\n\
DESCRIPTION\n\
\n\
  This program converts pictures into unicode text and ANSI colors so\n\
  that images can be displayed within a terminal. It performs lots of\n\
  AVX2 optimized math to deliver the best quality on modern terminals\n\
  with 24-bit color support, e.g. Kitty, Gnome Terminal, CMD.EXE, etc\n\
\n\
  The default output if fullscreen but can be changed:\n\
  -w X\n\
  -x X\n\
          If X is positive, hardcode the width in tty cells to X\n\
          If X is negative, remove as much from the fullscreen width\n\
          X may be specified as base 10 decimal, octal, binary, or hex\n\
  -h Y\n\
  -y Y\n\
          If Y is positive, hardcode the height in tty cells to Y\n\
          If Y is negative, remove as much from the fullscreen height\n\
          May be specified as base 10 decimal, octal, binary, or hex\n\
  -m\n\
          Use ImageMagick `convert` command to load/scale graphics\n\
  -?\n\
  -H\n\
          Show this help information\n\
\n\
EXAMPLES\n\
\n\
  $ ./derasterize.com samples/wave.png > wave.uaart\n\
  $ cat wave.uaart\n\
\n\
AUTHORS\n\
\n\
  Csdvrx <csdvrx@outlook.com>\n\
  Justine Tunney <jtunney@gmail.com>\n\
"

int m_; /* -m        [use imagemagick] */
int x_; /* -x WIDTH  [in flexidecimal] */
int y_; /* -y HEIGHT [in flexidecimal] */

#define BEST   0
#define FAST   1
#define FASTER 2

#define Mode BEST

#if Mode == BEST
#define MC 9u  /* log2(#) of color combos to consider */
#define GN 35u /* # of glyphs to consider */
#elif Mode == FAST
#define MC 6u
#define GN 35u
#elif Mode == FASTER
#define MC 4u
#define GN 25u
#endif

#define CN 3u        /* # channels (rgb) */
#define YS 8u        /* row stride -or- block height */
#define XS 4u        /* column stride -or- block width */
#define GT 44u       /* total glyphs */
#define BN (YS * XS) /* # scalars in block/glyph plane */

#define PHIPRIME 0x9E3779B1u

extern const uint32_t kGlyphs[];
extern const char16_t kRunes[];

/*───────────────────────────────────────────────────────────────────────────│─╗
│ derasterize § encoding                                                   ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/

/**
 * Formats Thompson-Pike variable length integer to array.
 *
 * @param p needs at least 8 bytes
 * @return p + number of bytes written, cf. mempcpy
 * @note no NUL-terminator is added
 */
static char *tptoa(char *p, wchar_t x) {
  unsigned long w;
  for (w = tpenc(x); w; w >>= 010) *p++ = w & 0xff;
  return p;
}

/*───────────────────────────────────────────────────────────────────────────│─╗
│ derasterize § colors                                                     ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/

static float frgb2lin(float x) {
  float r1, r2;
  r1 = x / 12.92f;
  r2 = pow((x + 0.055) / (1 + 0.055), 2.4);
  return x < 0.04045f ? r1 : r2;
}

static float frgb2std(float x) {
  float r1, r2;
  r1 = x * 12.92f;
  r2 = 1.055 * pow(x, 1 / 2.4) - 0.055;
  return x < 0.0031308f ? r1 : r2;
}

/**
 * Converts 8-bit RGB samples to floating point.
 */
static void rgb2float(unsigned n, float *f, const unsigned char *u) {
  unsigned i;
  for (i = 0; i < n; ++i) f[i] = u[i];
  for (i = 0; i < n; ++i) f[i] /= 255;
}

/**
 * Converts floating point RGB samples to 8-bit.
 */
static void float2rgb(unsigned n, unsigned char *u, float *f) {
  unsigned i;
  for (i = 0; i < n; ++i) f[i] *= 256;
  for (i = 0; i < n; ++i) f[i] = roundf(f[i]);
  for (i = 0; i < n; ++i) u[i] = MAX(0, MIN(255, f[i]));
}

/**
 * Converts standard RGB to linear RGB.
 *
 * This makes subtraction look good by flattening out the bias curve
 * that PC display manufacturers like to use.
 */
static dontinline void rgb2lin(unsigned n, float *f, const unsigned char *u) {
  unsigned i;
  rgb2float(n, f, u);
  for (i = 0; i < n; ++i) f[i] = frgb2lin(f[i]);
}

/**
 * Converts linear RGB to standard RGB.
 */
static dontinline void rgb2std(unsigned n, unsigned char *u, float *f) {
  unsigned i;
  for (i = 0; i < n; ++i) f[i] = frgb2std(f[i]);
  float2rgb(n, u, f);
}

/*───────────────────────────────────────────────────────────────────────────│─╗
│ derasterize § blocks                                                     ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/

struct Cell {
  char16_t rune;
  unsigned char bg[CN], fg[CN];
};

/**
 * Serializes ANSI background, foreground, and UNICODE glyph to wire.
 */
static char *celltoa(char *p, struct Cell cell) {
  *p++ = 033;
  *p++ = '[';
  *p++ = '4';
  *p++ = '8';
  *p++ = ';';
  *p++ = '2';
  *p++ = ';';
  p = itoa8(p, cell.bg[0]);
  *p++ = ';';
  p = itoa8(p, cell.bg[1]);
  *p++ = ';';
  p = itoa8(p, cell.bg[2]);
  *p++ = ';';
  *p++ = '3';
  *p++ = '8';
  *p++ = ';';
  *p++ = '2';
  *p++ = ';';
  p = itoa8(p, cell.fg[0]);
  *p++ = ';';
  p = itoa8(p, cell.fg[1]);
  *p++ = ';';
  p = itoa8(p, cell.fg[2]);
  *p++ = 'm';
  p = tptoa(p, cell.rune);
  return p;
}

/**
 * Picks ≤2**MC unique (bg,fg) pairs from product of lb.
 */
static unsigned combinecolors(unsigned char bf[1u << MC][2],
                              const unsigned char bl[CN][YS * XS]) {
  uint64_t hv, ht[(1u << MC) * 2];
  unsigned i, j, n, b, f, h, hi, bu, fu;
  bzero(ht, sizeof(ht));
  for (n = b = 0; b < BN && n < (1u << MC); ++b) {
    bu = bl[2][b] << 020 | bl[1][b] << 010 | bl[0][b];
    hi = 0;
    hi = (((bu >> 000) & 0xff) + hi) * PHIPRIME;
    hi = (((bu >> 010) & 0xff) + hi) * PHIPRIME;
    hi = (((bu >> 020) & 0xff) + hi) * PHIPRIME;
    for (f = b + 1; f < BN && n < (1u << MC); ++f) {
      fu = bl[2][f] << 020 | bl[1][f] << 010 | bl[0][f];
      h = hi;
      h = (((fu >> 000) & 0xff) + h) * PHIPRIME;
      h = (((fu >> 010) & 0xff) + h) * PHIPRIME;
      h = (((fu >> 020) & 0xff) + h) * PHIPRIME;
      h = h & 0xffff;
      h = MAX(1, h);
      hv = 0;
      hv <<= 030;
      hv |= fu;
      hv <<= 030;
      hv |= bu;
      hv <<= 020;
      hv |= h;
      for (i = 0;; ++i) {
        j = (h + i * (i + 1) / 2) & (ARRAYLEN(ht) - 1);
        if (!ht[j]) {
          ht[j] = hv;
          bf[n][0] = b;
          bf[n][1] = f;
          n++;
          break;
        } else if (ht[j] == hv) {
          break;
        }
      }
    }
  }
  return n;
}

/**
 * Computes distance between synthetic block and actual.
 */
#define ADJUDICATE(SYMBOL, ARCH)                                  \
  ARCH static float SYMBOL(unsigned b, unsigned f, unsigned g,    \
                           const float lb[CN][YS * XS]) {         \
    unsigned i, k, gu;                                            \
    float p[BN], q[BN], fu, bu, r;                                \
    bzero(q, sizeof(q));                                          \
    for (k = 0; k < CN; ++k) {                                    \
      gu = kGlyphs[g];                                            \
      bu = lb[k][b];                                              \
      fu = lb[k][f];                                              \
      for (i = 0; i < BN; ++i) p[i] = (gu & (1u << i)) ? fu : bu; \
      for (i = 0; i < BN; ++i) p[i] -= lb[k][i];                  \
      for (i = 0; i < BN; ++i) p[i] *= p[i];                      \
      for (i = 0; i < BN; ++i) q[i] += p[i];                      \
    }                                                             \
    r = 0;                                                        \
    for (i = 0; i < BN; ++i) q[i] = sqrtf(q[i]);                  \
    for (i = 0; i < BN; ++i) r += q[i];                           \
    return r;                                                     \
  }

ADJUDICATE(adjudicate_avx2, _Microarchitecture("avx2,fma"))
ADJUDICATE(adjudicate_avx, _Microarchitecture("avx"))
ADJUDICATE(adjudicate_default, )

static float (*adjudicate_hook)(unsigned, unsigned, unsigned,
                                const float[CN][YS * XS]);

static float adjudicate2(unsigned b, unsigned f, unsigned g,
                         const float lb[CN][YS * XS]) {
  if (!adjudicate_hook) {
    if (X86_HAVE(AVX2) && X86_HAVE(FMA)) {
      adjudicate_hook = adjudicate_avx2;
    } else if (X86_HAVE(AVX)) {
      adjudicate_hook = adjudicate_avx;
    } else {
      adjudicate_hook = adjudicate_default;
    }
  }
  return adjudicate_hook(b, f, g, lb);
}

static float adjudicate(unsigned b, unsigned f, unsigned g,
                        const float lb[CN][YS * XS]) {
  unsigned i, k, gu;
  float p[BN], q[BN], fu, bu, r;
  bzero(q, sizeof(q));
  for (k = 0; k < CN; ++k) {
    gu = kGlyphs[g];
    bu = lb[k][b];
    fu = lb[k][f];
    for (i = 0; i < BN; ++i) p[i] = (gu & (1u << i)) ? fu : bu;
    for (i = 0; i < BN; ++i) p[i] -= lb[k][i];
    for (i = 0; i < BN; ++i) p[i] *= p[i];
    for (i = 0; i < BN; ++i) q[i] += p[i];
  }
  r = 0;
  for (i = 0; i < BN; ++i) q[i] = sqrtf(q[i]);
  for (i = 0; i < BN; ++i) r += q[i];
  return r;
}

/**
 * Converts tiny bitmap graphic into unicode glyph.
 */
static struct Cell derasterize(unsigned char block[CN][YS * XS]) {
  struct Cell cell;
  unsigned i, n, b, f, g;
  float r, best, lb[CN][YS * XS];
  unsigned char bf[1u << MC][2];
  rgb2lin(CN * YS * XS, lb[0], block[0]);
  n = combinecolors(bf, block);
  best = FLT_MAX;
  cell.rune = 0;
  for (i = 0; i < n; ++i) {
    b = bf[i][0];
    f = bf[i][1];
    for (g = 0; g < GN; ++g) {
      r = adjudicate(b, f, g, lb);
      if (r < best) {
        best = r;
        cell.rune = kRunes[g];
        cell.bg[0] = block[0][b];
        cell.bg[1] = block[1][b];
        cell.bg[2] = block[2][b];
        cell.fg[0] = block[0][f];
        cell.fg[1] = block[1][f];
        cell.fg[2] = block[2][f];
        if (!r) return cell;
      }
    }
  }
  return cell;
}

/*───────────────────────────────────────────────────────────────────────────│─╗
│ derasterize § graphics                                                   ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/

/**
 * Turns packed 8-bit RGB graphic into ANSI UNICODE text.
 */
static char *RenderImage(char *v, unsigned yn, unsigned xn,
                         const unsigned char srgb[yn][YS][xn][XS][CN]) {
  unsigned y, x, i, j, k;
  unsigned char copy[YS][XS][CN] forcealign(32);
  unsigned char block[CN][YS * XS] forcealign(32);
  DCHECK_ALIGNED(32, v);
  DCHECK_ALIGNED(32, srgb);
  for (y = 0; y < yn; ++y) {
    if (y) {
      *v++ = 033;
      *v++ = '[';
      *v++ = '0';
      *v++ = 'm';
      *v++ = '\n';
    }
    for (x = 0; x < xn; ++x) {
      for (i = 0; i < YS; ++i) {
        memcpy(copy[i], srgb[y][i][x], XS * CN);
      }
      for (i = 0; i < YS; ++i) {
        for (j = 0; j < XS; ++j) {
          for (k = 0; k < CN; ++k) {
            block[k][i * XS + j] = copy[i][j][k];
          }
        }
      }
      v = celltoa(v, derasterize(block));
    }
  }
  return v;
}

/*───────────────────────────────────────────────────────────────────────────│─╗
│ derasterize § systems                                                    ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/

static void PrintImage(unsigned yn, unsigned xn,
                       const unsigned char rgb[yn][YS][xn][XS][CN]) {
  size_t size;
  char *v, *vt;
  size = yn * (xn * (32 + (2 + (1 + 3) * 3) * 2 + 1 + 3)) * 1 + 5 + 1;
  size = ROUNDUP(size, FRAMESIZE);
  CHECK_NOTNULL((vt = _mapanon(size)));
  v = RenderImage(vt, yn, xn, rgb);
  *v++ = '\r';
  *v++ = 033;
  *v++ = '[';
  *v++ = '0';
  *v++ = 'm';
  CHECK_NE(-1, xwrite(1, vt, v - vt));
  CHECK_NE(-1, munmap(vt, size));
}

/**
 * Determines dimensions of teletypewriter.
 */
static void GetTermSize(unsigned out_rows[1], unsigned out_cols[1]) {
  struct winsize ws;
  ws.ws_row = 24;
  ws.ws_col = 80;
  if (tcgetwinsize(STDIN_FILENO, &ws) == -1) {
    tcgetwinsize(STDOUT_FILENO, &ws);
  }
  out_rows[0] = ws.ws_row;
  out_cols[0] = ws.ws_col;
}

static int ReadAll(int fd, void *data, size_t size) {
  char *p;
  ssize_t rc;
  size_t got, n;
  p = data;
  n = size;
  do {
    if ((rc = read(fd, p, n)) == -1) return -1;
    got = rc;
    assert(got || !n);
    p += got;
    n -= got;
  } while (n);
  return 0;
}

/**
 * Loads and scales image via ImageMagick `convert` command.
 *
 * @param path is filename of graphic
 * @param yn is desired height
 * @param xn is desired width
 * @param rgb is memory allocated by caller for image
 */
static void LoadFileViaImageMagick(const char *path, unsigned yn, unsigned xn,
                                   unsigned char rgb[yn][YS][xn][XS][CN]) {
  const char *convert;
  int pid, ws, pipefds[2];
  char pathbuf[PATH_MAX], dim[32];
  if (!(convert = commandv("convert", pathbuf, sizeof(pathbuf)))) {
    fputs("error: `convert` command not found\n"
          "try: apt-get install imagemagick\n",
          stderr);
    exit(EXIT_FAILURE);
  }
  sprintf(dim, "%ux%u!", xn * XS, yn * YS);
  CHECK_NE(-1, pipe2(pipefds, O_CLOEXEC));
  if (!(pid = vfork())) {
    dup2(pipefds[1], 1);
    execv(convert,
          (char *const[]){"convert", (char *)path, "-resize", dim, "-depth",
                          "8", "-colorspace", "sRGB", "rgb:-", NULL});
    abort();
  }
  CHECK_NE(-1, close(pipefds[1]));
  CHECK_NE(-1, ReadAll(pipefds[0], rgb, yn * YS * xn * XS * CN));
  CHECK_NE(-1, close(pipefds[0]));
  CHECK_NE(-1, waitpid(pid, &ws, 0));
  CHECK_EQ(0, WEXITSTATUS(ws));
}

static void LoadFile(const char *path, size_t yn, size_t xn, void *rgb) {
  struct stat st;
  void *map, *data;
  int fd, gotx, goty, channels_in_file;
  CHECK_NE(-1, (fd = open(path, O_RDONLY)), "%s", path);
  CHECK_NE(-1, fstat(fd, &st));
  CHECK_GT(st.st_size, 0);
  CHECK_LE(st.st_size, INT_MAX);
  /* LOGIFNEG1(fadvise(fd, 0, 0, MADV_WILLNEED | MADV_SEQUENTIAL)); */
  CHECK_NE(MAP_FAILED,
           (map = mmap(NULL, st.st_size, PROT_READ, MAP_SHARED, fd, 0)));
  CHECK_NOTNULL((data = stbi_load_from_memory(map, st.st_size, &gotx, &goty,
                                              &channels_in_file, CN)),
                "%s", path);
  CHECK_NE(-1, munmap(map, st.st_size));
  CHECK_NE(-1, close(fd));
#if 1
  stbir_resize_uint8(data, gotx, goty, 0, rgb, xn * XS, yn * YS, 0, CN);
#else
  CHECK_EQ(CN, 3);
  data2size = ROUNDUP(sizeof(float) * goty * gotx * CN, FRAMESIZE);
  data3size = ROUNDUP(sizeof(float) * yn * YS * xn * XS * CN, FRAMESIZE);
  CHECK_NOTNULL((data2 = _mapanon(data2size)));
  CHECK_NOTNULL((data3 = _mapanon(data3size)));
  rgb2lin(goty * gotx * CN, data2, data);
  lanczos3(yn * YS, xn * XS, data3, goty, gotx, data2, gotx * 3);
  rgb2std(yn * YS * xn * XS * CN, rgb, data3);
  CHECK_NE(-1, munmap(data2, data2size));
  CHECK_NE(-1, munmap(data3, data3size));
#endif
  free(data);
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

static void PrintUsage(int rc, int fd) {
  tinyprint(fd, HELPTEXT, NULL);
  exit(rc);
}

static void GetOpts(int argc, char *argv[]) {
  int opt;
  while ((opt = getopt(argc, argv, "?Hmx:y:w:h:")) != -1) {
    switch (opt) {
      case 'w':
      case 'x':
        x_ = ParseNumberOption(optarg);
        break;
      case 'h':
      case 'y':
        y_ = ParseNumberOption(optarg);
        break;
      case 'm':
        m_ = 1;
        break;
      case '?':
      case 'H':
      default:
        if (opt == optopt) {
          PrintUsage(EXIT_SUCCESS, STDOUT_FILENO);
        } else {
          PrintUsage(EX_USAGE, STDERR_FILENO);
        }
    }
  }
}

int main(int argc, char *argv[]) {
  int i;
  void *rgb;
  size_t size;
  unsigned yd, xd;
  ShowCrashReports();
  GetOpts(argc, argv);
  // if sizes are given, 2 cases:
  //  - positive values: use that as the target size
  //  - negative values: add, for ex to offset the command prompt size
  GetTermSize(&yd, &xd);
  if (y_ <= 0) {
    y_ += yd;
  }
  if (x_ <= 0) {
    x_ += xd;
  }
  // FIXME: on the conversion stage should do 2Y because of halfblocks
  // printf( "filename >%s<\tx >%d<\ty >%d<\n\n", filename, x_, y_);
  size = y_ * YS * x_ * XS * CN;
  CHECK_NOTNULL((rgb = _mapanon(ROUNDUP(size, FRAMESIZE))));
  for (i = optind; i < argc; ++i) {
    if (!argv[i]) continue;
    if (m_) {
      LoadFileViaImageMagick(argv[i], y_, x_, rgb);
    } else {
      LoadFile(argv[i], y_, x_, rgb);
    }
    PrintImage(y_, x_, rgb);
  }
  munmap(rgb, ROUNDUP(size, FRAMESIZE));
  return 0;
}

#else

int main(int argc, char *argv[]) {
}

#endif /* __x86_64__ */
