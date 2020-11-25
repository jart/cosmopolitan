#if 0
/*─────────────────────────────────────────────────────────────────╗
│ To the extent possible under law, Justine Tunney has waived      │
│ all copyright and related or neighboring rights to this file,    │
│ as it is written in the following disclaimers:                   │
│   • http://unlicense.org/                                        │
│   • http://creativecommons.org/publicdomain/zero/1.0/            │
╚─────────────────────────────────────────────────────────────────*/
#endif
#include "dsp/core/core.h"
#include "dsp/core/twixt8.h"
#include "dsp/scale/scale.h"
#include "dsp/tty/itoa8.h"
#include "dsp/tty/quant.h"
#include "libc/alg/arraylist2.internal.h"
#include "libc/calls/calls.h"
#include "libc/calls/ioctl.h"
#include "libc/calls/struct/stat.h"
#include "libc/calls/struct/winsize.h"
#include "libc/limits.h"
#include "libc/log/check.h"
#include "libc/log/log.h"
#include "libc/macros.h"
#include "libc/math.h"
#include "libc/mem/mem.h"
#include "libc/runtime/gc.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/fileno.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/prot.h"
#include "libc/sysv/consts/termios.h"
#include "libc/x/x.h"
#include "third_party/stb/stb_image.h"

/**
 * Hat tips go out to Hornet, CRTC, DESiRE, Hans Petter Jansson, Guihua
 * Cui, Ming Ronnier Luoand anyone else Mister Rogers would describe as
 * the The Helpers Of This World who've helped us learn more about the
 * cool yet exceedingly difficult things possible to do with graphics.
 */

#define DIN99          1
#define GAMMA          4.1
#define GAMMA_A        .05
#define GAMMA_B        50.
#define GAMMA_C        .055
#define COLORSPACE_SRC kSrgbD65ToXyzD50
#define COLORSPACE_TTY kSrgbD65ToXyzD50

#define SQR(X) ((X) * (X))
#define DEG(X) ((X)*M_PI / 180)

const double kXyzKappa = 24389 / 27.;
const double kXyzEpsilon = 216 / 24389.;

const double kSrgbD65ToXyzD50[3][3] = {
    {.4360747, .3850649, .1430804},
    {.2225045, .7168786, .0606169},
    {.0139322, .0971045, .7141733},
};

const double kCieRgbEToXyz[3][3] = {
    {.4887180, .3106803, .2006017},
    {.1762044, .8129847, .0108109},
    {.0000000, .0102048, .9897952},
};

const unsigned char kShadePct[] = {0300, 0200, 0100};
const unsigned short kShadeRune[] = {u'▓', u'▒', u'░'};

const unsigned char kAnsiTango[16][3] = {
    {46, 52, 54},    {204, 0, 0},     {78, 154, 6},   {196, 160, 0},
    {52, 101, 164},  {117, 80, 123},  {6, 152, 154},  {211, 215, 207},
    {85, 87, 83},    {239, 41, 41},   {138, 226, 52}, {252, 233, 79},
    {114, 159, 207}, {173, 127, 168}, {52, 226, 226}, {238, 238, 236},
};

const unsigned char kCga[16][3] = {
    {0, 0, 0},     {170, 0, 0},    {0, 170, 0},    {170, 85, 0},
    {0, 0, 170},   {170, 0, 170},  {0, 170, 170},  {170, 170, 170},
    {85, 85, 85},  {255, 85, 85},  {85, 255, 85},  {255, 255, 85},
    {85, 85, 255}, {255, 85, 255}, {85, 255, 255}, {255, 255, 255},
};

struct Blends {
  size_t i, n;
  struct Blend {
    unsigned char color[4];
    unsigned char bg, fg;
    unsigned short rune;
  } * p;
} blends;

static double Gamma(double x) {
  if (x < GAMMA_A) return x / GAMMA_B;
  return pow(1 / (1 + GAMMA_C) * (x + GAMMA_C), GAMMA);
}

static void *LoadRgb(double rgb[3], const unsigned char pix[3]) {
  rgb[0] = Gamma(1. / 255 * pix[0]);
  rgb[1] = Gamma(1. / 255 * pix[1]);
  rgb[2] = Gamma(1. / 255 * pix[2]);
  return rgb;
}

static double Lab(double x) {
  return x > kXyzEpsilon ? cbrtf(x) : (kXyzKappa * x + 16) / 116;
}

static void XyzToLab(double lab[3], const double d50xyz[3]) {
  double Y, L, a, b;
  Y = Lab(d50xyz[1]);
  L = 116 * Y - 16;
  a = 500 * (Lab(d50xyz[0]) - Y);
  b = 200 * (Y - Lab(d50xyz[2]));
  lab[0] = L, lab[1] = a, lab[2] = b;
}

static void XyzToDin99d(unsigned char din99d[3], const double d50xyz[3]) {
  double e, f, G, C, h, xyz[3], lab[3];
  memcpy(xyz, d50xyz, sizeof(xyz));
  xyz[0] = 1.12f * xyz[0] - .12f * xyz[2];
  XyzToLab(lab, xyz);
  e = (+lab[1] * cosf(DEG(50)) + lab[2] * sinf(DEG(50)));
  f = (-lab[1] * sinf(DEG(50)) + lab[2] + cosf(DEG(50))) * 1.14;
  G = sqrtf(SQR(e) + SQR(f));
  C = logf(1 + .06 * G) * 22.5f;
  h = atan2f(f, e) + DEG(50);
  h = fmodf(h, DEG(360));
  if (h < 0) h += DEG(360);
  din99d[0] = MIN(255, MAX(0, 325.22f * logf(1 + .0036 * lab[0]) * 2.5f));
  din99d[1] = MIN(255, MAX(0, C * cos(h) * 2.5f + 128));
  din99d[2] = MIN(255, MAX(0, C * sin(h) * 2.5f + 128));
}

static void AddColor(int bg, int fg, int rune, int r, int g, int b, int x) {
  int i;
  if (blends.i) {
    for (i = blends.i; --i;) {
      if (blends.p[i].color[0] == r && blends.p[i].color[1] == g &&
          blends.p[i].color[2] == b) {
        return;
      }
    }
  }
  CHECK_NE(
      -1,
      APPEND(&blends.p, &blends.i, &blends.n,
             (&(struct Blend){
                 .bg = bg, .fg = fg, .rune = rune, .color = {r, g, b, x}})));
}

static void AddBlend(int bgxterm, int fgxterm, int rune, int shade,
                     const unsigned char bgrgb[4],
                     const unsigned char fgrgb[4]) {
  AddColor(bgxterm, fgxterm, rune, twixt8(bgrgb[0], fgrgb[0], shade),
           twixt8(bgrgb[1], fgrgb[1], shade), twixt8(bgrgb[2], fgrgb[2], shade),
           0);
}

static void MakeBlends(const unsigned char palette[16][3]) {
  int r, i, j, k;
  double rgb[3], xyz[3];
  unsigned char tab[256][4];
  unsigned char cube[6] = {0, 0137, 0207, 0257, 0327, 0377};
  unsigned char seqs[2][2] = {{16, 255}};
  for (i = 0; i < 16; ++i) {
    tab[i][0] = palette[i][0];
    tab[i][1] = palette[i][1];
    tab[i][2] = palette[i][2];
    tab[i][3] = i;
  }
  for (i = 16; i < 232; ++i) {
    tab[i][0] = cube[((i - 020) / 044) % 6];
    tab[i][1] = cube[((i - 020) / 6) % 6];
    tab[i][2] = cube[(i - 020) % 6];
    tab[i][3] = i;
  }
  for (i = 232; i < 256; ++i) {
    tab[i][0] = 8 + (i - 232) * 10;
    tab[i][1] = 8 + (i - 232) * 10;
    tab[i][2] = 8 + (i - 232) * 10;
    tab[i][3] = i;
  }
#if DIN99
  for (i = 0; i < 256; ++i) {
    LoadRgb(rgb, tab[i]);
    vmatmul3(xyz, rgb, COLORSPACE_TTY);
    XyzToDin99d(tab[i], xyz);
  }
#endif
  for (r = 0; r < ARRAYLEN(seqs); ++r) {
    for (i = seqs[r][0]; i <= seqs[r][1]; ++i) {
      for (j = seqs[r][0]; j <= seqs[r][1]; ++j) {
        if (i == j) {
          AddColor(i, 0, 0, tab[i][0], tab[i][1], tab[i][2], 0);
        } else {
          for (k = 0; k < ARRAYLEN(kShadeRune); ++k) {
            AddBlend(i, j, kShadeRune[k], kShadePct[k], tab[i], tab[j]);
            AddBlend(j, i, kShadeRune[k], kShadePct[k], tab[j], tab[i]);
          }
        }
      }
    }
  }
}

static int PickColor(unsigned char color[3]) {
  int i, best, least, dist;
  best = -1;
  least = INT_MAX;
  for (i = 0; i < blends.i; ++i) {
    dist = SQR(blends.p[i].color[0] - color[0]) +
           SQR(blends.p[i].color[1] - color[1]) +
           SQR(blends.p[i].color[2] - color[2]);
    if (dist < least) {
      least = dist;
      best = i;
    }
  }
  return best;
}

static void SetBg(int x) {
  if (0 <= x && x < 16) {
    if (x < 8) {
      x += 40;
    } else {
      x -= 8;
      x *= 100;
    }
    printf("\e[%dm", x);
  } else {
    /* assert(false); */
    printf("\e[48;5;%dm", x);
  }
}

static void SetFg(int x) {
  if (0 <= x && x < 16) {
    if (x < 8) {
      x += 30;
    } else {
      x -= 8;
      x *= 90;
    }
    printf("\e[%dm", x);
  } else {
    /* assert(false); */
    printf("\e[38;5;%dm", x);
  }
}

static void PrintImage(long yn, long xn, unsigned char src[4][yn][xn]) {
  int y, x, c;
  double rgb[3], xyz[3];
  unsigned char din99d[3];
  for (y = 0; y < yn; ++y) {
    printf("\n");
    for (x = 0; x < xn; ++x) {
      din99d[0] = src[0][y][x];
      din99d[1] = src[1][y][x];
      din99d[2] = src[2][y][x];
#if DIN99
      LoadRgb(rgb, din99d);
      vmatmul3(xyz, rgb, COLORSPACE_SRC);
      XyzToDin99d(din99d, xyz);
#endif
      c = PickColor(din99d);
      if (blends.p[c].rune) {
        SetBg(blends.p[c].bg);
        SetFg(blends.p[c].fg);
        printf("%hc", blends.p[c].rune);
      } else {
        SetBg(blends.p[c].bg);
        printf(" ");
      }
    }
    printf("\e[0m");
  }
  printf("\r");
}

static void *Deinterlace(long dcn, long dyn, long dxn,
                         unsigned char dst[dcn][dyn][dxn], long syw, long sxw,
                         long scw, const unsigned char src[syw][sxw][scw],
                         long syn, long sxn, long sy0, long sx0, long sc0) {
  long y, x, c;
  for (y = 0; y < dyn; ++y) {
    for (x = 0; x < dxn; ++x) {
      for (c = 0; c < dcn; ++c) {
        dst[c][y][x] = src[sy0 + y][sx0 + x][sc0 + c];
      }
    }
  }
  return dst;
}

static void GetTermSize(int out_rows[1], int out_cols[1]) {
  struct winsize ws;
  ws.ws_row = 24;
  ws.ws_col = 80;
  if (ioctl(STDIN_FILENO, TIOCGWINSZ, &ws) == -1) {
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws);
  }
  out_rows[0] = ws.ws_row;
  out_cols[0] = ws.ws_col;
}

static void ProcessImage(long syn, long sxn, unsigned char img[syn][sxn][4]) {
  int dyn, dxn;
  double ry, rx;
  GetTermSize(&dyn, &dxn);
  ry = syn, rx = sxn;
  ry /= dyn, rx /= dxn;
  PrintImage(
      dyn, dxn,
      EzGyarados(4, dyn, dxn, gc(xmemalign(32, dyn * dxn * 4)), 4, syn, sxn,
                 Deinterlace(4, syn, sxn, gc(xmemalign(32, syn * sxn * 4)), syn,
                             sxn, 4, img, syn, sxn, 0, 0, 0),
                 0, 4, dyn, dxn, syn, sxn, ry, rx, 0, 0));
}

static void WithImageFile(const char *path,
                          void fn(long yn, long xn,
                                  unsigned char src[yn][xn][4])) {
  struct stat st;
  void *map, *data;
  int fd, yn, xn, cn;
  CHECK_NE(-1, (fd = open(path, O_RDONLY)), "%s", path);
  CHECK_NE(-1, fstat(fd, &st));
  CHECK_GT(st.st_size, 0);
  CHECK_LE(st.st_size, INT_MAX);
  CHECK_NE(MAP_FAILED,
           (map = mmap(NULL, st.st_size, PROT_READ, MAP_SHARED, fd, 0)));
  CHECK_NOTNULL(
      (data = stbi_load_from_memory(map, st.st_size, &xn, &yn, NULL, 4)), "%s",
      path);
  CHECK_NE(-1, munmap(map, st.st_size));
  CHECK_NE(-1, close(fd));
  fn(yn, xn, data);
  free(data);
}

static void PrintData(void) {
  int i;
  for (i = 0; i < blends.i; ++i) {
    printf("%3d %3d %3d %3d %3d %d\n", blends.p[i].color[0],
           blends.p[i].color[1], blends.p[i].color[2], blends.p[i].bg,
           blends.p[i].fg, blends.p[i].rune);
  }
}

void *OpenRgbMap(const char *path) {
  int fd;
  void *map;
  size_t size;
  size = 256 * 256 * 256 * 3;
  CHECK_NE(-1, (fd = open(path, O_RDWR | O_CREAT | O_TRUNC, 0644)));
  CHECK_NE(-1, ftruncate(fd, size));
  CHECK_NE(MAP_FAILED,
           (map = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0)));
  CHECK_NE(-1, close(fd));
  return map;
}

int main(int argc, char *argv[]) {
  int i;
  MakeBlends(kCga);
  for (i = 1; i < argc; ++i) {
    WithImageFile(argv[i], ProcessImage);
  }
  return 0;
}
