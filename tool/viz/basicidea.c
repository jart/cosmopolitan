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
#include "dsp/core/core.h"
#include "libc/bits/safemacros.h"
#include "libc/calls/calls.h"
#include "libc/calls/hefty/spawn.h"
#include "libc/calls/ioctl.h"
#include "libc/calls/struct/winsize.h"
#include "libc/dce.h"
#include "libc/fmt/fmt.h"
#include "libc/log/check.h"
#include "libc/log/log.h"
#include "libc/macros.h"
#include "libc/math.h"
#include "libc/mem/mem.h"
#include "libc/runtime/gc.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/exit.h"
#include "libc/sysv/consts/fileno.h"
#include "libc/sysv/consts/ok.h"
#include "libc/sysv/consts/termios.h"
#include "libc/x/x.h"

#define SQR(X)     ((X) * (X))
#define DIST(X, Y) ((X) - (Y))

static int want24bit_;

const int kXtermCube[] = {0, 0137, 0207, 0257, 0327, 0377};

static int rgbdist(int a, int b, int c, int x, int y, int z) {
  return SQR(DIST(a, x)) + SQR(DIST(b, y)) + SQR(DIST(c, z));
}

static int uncube(int x) {
  return x < 48 ? 0 : x < 115 ? 1 : (x - 35) / 40;
}

static int DivideIntRound(int x, int y) {
  return (x + y / 2) / y;
}

static int XtermQuantizeLuma(int Y) {
  return DivideIntRound(Y - 8, 10);
}

static int XtermDequantizeLuma(int qY) {
  if (0 < qY && qY < 24) {
    return (qY * 10) + 8;
  } else if (qY > 0) {
    return 255;
  } else {
    return 0;
  }
}

static int XtermEncodeLuma(int qY) {
  if (0 < qY && qY < 24) {
    return qY + 232;
  } else if (qY > 0) {
    return 231;
  } else {
    return 16;
  }
}

static int XtermQuantizeChroma(int c) {
  return DivideIntRound(c - 55, 40);
}

static int XtermDequantizeChroma(int qc) {
  if (0 < qc && qc < 6) {
    return (qc * 40) + 55;
  } else if (qc > 0) {
    return 255;
  } else {
    return 0;
  }
}

static int XtermEncodeChromaComponent(int qC) {
  if (0 < qC && qC < 6) {
    return qC;
  } else if (qC > 0) {
    return 5;
  } else {
    return 0;
  }
}

static int XtermEncodeChroma(int qR, int qG, int qB) {
  int xt;
  xt = 16;
  xt += XtermEncodeChromaComponent(qR) * 6 * 6;
  xt += XtermEncodeChromaComponent(qG) * 6;
  xt += XtermEncodeChromaComponent(qB) * 1;
  return xt;
}

/**
 * Quantizes 24-bit sRGB to xterm256 code range [16,256).
 */
static int rgb2xterm256(unsigned char R, unsigned char G, unsigned char B) {
  double y, r, g, b, yr, yg, yb, ry, gy, by, gamma;
  int Y, qY, cY, qRY, qGY, qBY, qR, qG, qB, cR, cG, cB, xt;
  gamma = 2.4;
  yr = 871024 / 4096299.;
  yg = 8788810 / 12288897.;
  yb = 887015 / 12288897.;
  r = rgb2linpc(R / 255., gamma);
  g = rgb2linpc(G / 255., gamma);
  b = rgb2linpc(B / 255., gamma);
  y = yr * r + yg * g + yb * b;
  ry = (r - y) / (1 - yr + yg + yb);
  gy = (g - y) / (1 - yg + yr + yb);
  by = (b - y) / (1 - yb + yg + yr);
  Y = round(rgb2stdpc(y, gamma) * 255);
  qRY = round(rgb2stdpc(ry, gamma) * 6 + 3);
  qGY = round(rgb2stdpc(gy, gamma) * 6 + 3);
  qBY = round(rgb2stdpc(by, gamma) * 6 + 3);
  qY = XtermQuantizeLuma(Y);
  qR = XtermQuantizeChroma(qRY);
  qG = XtermQuantizeChroma(qGY);
  qB = XtermQuantizeChroma(qBY);
  cY = XtermDequantizeLuma(qY);
  cR = XtermDequantizeChroma(qRY);
  cG = XtermDequantizeChroma(qGY);
  cB = XtermDequantizeChroma(qBY);
#if 0
  LOGF("RGB(%3d,%3d,%3d)   rgb(%f,%f,%f) y=%f", R, G, B, r, g, b, y);
  LOGF("RGB(%3d,%3d,%3d) yΔrgb(%f,%f,%f) XCUBE(%d,%d,%d)", R, G, B, ry, gy, by,
       qRY, qGY, qBY);
  LOGF("RGB(%3d,%3d,%3d)  cRGB(%d,%d,%d) cY=%d qY=%d Y=%d", R, G, B, cR, cG, cB,
       cY, qY, Y);
#endif
  if (rgbdist(cR, cG, cB, R, G, B) <= rgbdist(cY, cY, cY, R, G, B)) {
    xt = XtermEncodeChroma(qR, qG, qB);
  } else {
    xt = XtermEncodeLuma(qY);
  }
  /* LOGF("xt=%d", xt); */
  return xt;
}

/**
 * Prints raw packed 8-bit RGB data from memory.
 */
static void PrintImage(long yn, long xn, unsigned char RGB[yn][xn][4]) {
  long y, x;
  for (y = 0; y < yn; y += 2) {
    if (y) printf("\r\n");
    for (x = 0; x < xn; ++x) {
      if (want24bit_) {
        printf("\033[48;2;%hhu;%hhu;%hhu;38;2;%hhu;%hhu;%hhum▄",
               RGB[y + 0][x][0], RGB[y + 0][x][1], RGB[y + 0][x][2],
               RGB[y + 1][x][0], RGB[y + 1][x][1], RGB[y + 1][x][2]);
      } else {
        printf(
            "\033[48;5;%hhu;38;5;%hhum▄",
            rgb2xterm256(RGB[y + 0][x][0], RGB[y + 0][x][1], RGB[y + 0][x][2]),
            rgb2xterm256(RGB[y + 1][x][0], RGB[y + 1][x][1], RGB[y + 1][x][2]));
      }
    }
  }
  if (IsWindows()) {
    printf("\033[0m\r\n");
  } else {
    printf("\033[0m\r");
  }
}

/**
 * Determines dimensions of teletypewriter.
 */
static void GetTermSize(unsigned *out_rows, unsigned *out_cols) {
  struct winsize ws;
  ws.ws_row = 20;
  ws.ws_col = 80;
  ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws);
  ioctl(STDIN_FILENO, TIOCGWINSZ, &ws);
  *out_rows = ws.ws_row;
  *out_cols = ws.ws_col;
}

static void ReadAll(int fd, void *buf, size_t n) {
  char *p;
  ssize_t rc;
  size_t got;
  p = buf;
  do {
    CHECK_NE(-1, (rc = read(fd, p, n)));
    got = rc;
    CHECK(!(!got && n));
    p += got;
    n -= got;
  } while (n);
}

static void LoadImageOrDie(const char *path, size_t size, long yn, long xn,
                           unsigned char RGB[yn][xn][4]) {
  int pid, ws, fds[3];
  char *convert, pathbuf[PATH_MAX];
  if (isempty((convert = getenv("CONVERT"))) &&
      !(IsWindows() && access((convert = "\\msys64\\mingw64\\bin\\convert.exe"),
                              X_OK) != -1) &&
      !(convert = commandv("convert", pathbuf))) {
    fputs("'convert' command not found\r\n"
          "please install imagemagick\r\n",
          stderr);
    exit(1);
  }
  fds[0] = STDIN_FILENO;
  fds[1] = -1;
  fds[2] = STDERR_FILENO;
  pid = spawnve(0, fds, convert,
                (char *const[]){"convert", path, "-resize",
                                gc(xasprintf("%ux%u!", xn, yn)), "-colorspace",
                                "RGB", "-depth", "8", "rgba:-", NULL},
                environ);
  CHECK_NE(-1, pid);
  ReadAll(fds[1], RGB, size);
  CHECK_NE(-1, close(fds[1]));
  CHECK_NE(-1, waitpid(pid, &ws, 0));
  CHECK_EQ(0, WEXITSTATUS(ws));
}

int main(int argc, char *argv[]) {
  int i;
  void *rgb;
  size_t size;
  unsigned yn, xn;
  GetTermSize(&yn, &xn);
  yn *= 2;
  size = yn * xn * 4;
  CHECK_NOTNULL((rgb = valloc(size)));
  for (i = 1; i < argc; ++i) {
    if (strcmp(argv[i], "-t") == 0) {
      want24bit_ = 1;
    } else {
      LoadImageOrDie(argv[i], size, yn, xn, rgb);
      PrintImage(yn, xn, rgb);
    }
  }
  return 0;
}
