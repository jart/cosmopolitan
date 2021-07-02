/*bin/echo  ' -*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;coding:utf-8 -*-┤
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ To the extent possible under law, Justine Tunney has waived                  │
│ all copyright and related or neighboring rights to this file,                │
│ as it is written in the following disclaimers:                               │
│   • http://unlicense.org/                                                    │
│   • http://creativecommons.org/publicdomain/zero/1.0/                        │
╚────────────────────────────────────────────────────────────────────'>/dev/null
  if ! [ "${0%.*}.exe" -nt "$0" ]; then
    cc -O -o "${0%.*}.exe" "$0" || exit
  fi
  exec "${0%.*}.exe" "$@"
  exit

OVERVIEW

  Simple Terminal Image Printer

AUTHOR

  Justine Tunney <jtunney@gmail.com>

DESCRIPTION

  This program demonstrates a straightforward technique for displaying
  PNG/GIF/JPG/etc. files from the command line. This program supports
  xterm256 and 24-bit color w/ UNICODE half blocks. For example:

    apt install build-essential imagemagick
    ./printimage.c lemur.png

NOTES

  More advanced techniques exist for (1) gaining finer detail, as well
  as (2) reducing the number of bytes emitted; this program is here to
  get you started. */

#include "libc/calls/calls.h"
#include "libc/calls/ioctl.h"
#include "libc/fmt/fmt.h"
#include "libc/log/log.h"
#include "libc/runtime/runtime.h"
#include "libc/sysv/consts/exit.h"
#include "libc/sysv/consts/fileno.h"
#include "libc/sysv/consts/termios.h"
#include "libc/unicode/locale.h"

#define SQR(X)    ((X) * (X))
#define UNCUBE(x) x < 48 ? 0 : x < 115 ? 1 : (x - 35) / 40
#define ORDIE(X)                   \
  do {                             \
    if (!(X)) perror(#X), exit(1); \
  } while (0)

static int want24bit_;

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
      SQR(ql - r) + SQR(ql - g) + SQR(ql - b)) {
    return ir * 36 + ig * 6 + ib + 020;
  } else {
    return il + 0350;
  }
}

/**
 * Prints raw packed 8-bit RGB data from memory.
 */
void PrintImage(int yn, int xn, unsigned char rgb[yn][xn][3]) {
  unsigned y, x;
  for (y = 0; y < yn; y += 2) {
    if (y) printf("\r\n");
    for (x = 0; x < xn; ++x) {
      if (want24bit_) {
        printf("\033[48;2;%hhu;%hhu;%hhu;38;2;%hhu;%hhu;%hhum▄",
               rgb[y + 0][x][0], rgb[y + 0][x][1], rgb[y + 0][x][2],
               rgb[y + 1][x][0], rgb[y + 1][x][1], rgb[y + 1][x][2]);
      } else {
        printf(
            "\033[48;5;%hhu;38;5;%hhum▄",
            rgb2xterm256(rgb[y + 0][x][0], rgb[y + 0][x][1], rgb[y + 0][x][2]),
            rgb2xterm256(rgb[y + 1][x][0], rgb[y + 1][x][1], rgb[y + 1][x][2]));
      }
    }
  }
  printf("\033[0m\r");
}

/**
 * Determines dimensions of teletypewriter.
 */
void GetTermSize(int *out_rows, int *out_cols) {
  struct winsize ws;
  ws.ws_row = 20;
  ws.ws_col = 80;
  ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws);
  ioctl(STDIN_FILENO, TIOCGWINSZ, &ws);
  *out_rows = ws.ws_row;
  *out_cols = ws.ws_col;
}

void ReadAll(int fd, char *p, size_t n) {
  ssize_t rc;
  size_t got;
  do {
    ORDIE((rc = read(fd, p, n)) != -1);
    got = rc;
    if (!got && n) {
      fprintf(stderr, "error: expected eof\n");
      exit(EXIT_FAILURE);
    }
    p += got;
    n -= got;
  } while (n);
}

unsigned char *LoadImageOrDie(char *path, int yn, int xn) {
  size_t size;
  void *rgb;
  char dim[10 + 1 + 10 + 1 + 1];
  int pid, wstatus, readwrite[2];
  sprintf(dim, "%ux%u" /* jfc */ "!", xn, yn);
  pipe(readwrite);
  if (!(pid = fork())) {
    close(readwrite[0]);
    dup2(readwrite[1], STDOUT_FILENO);
    execvp("convert", /* apt install imagemagick */
           (char *const[]){"convert", path, "-resize", dim, "-colorspace",
                           "RGB", "-depth", "8", "rgb:-", NULL});
    _Exit(EXIT_FAILURE);
  }
  close(readwrite[1]);
  ORDIE((rgb = malloc((size = yn * xn * 3))));
  ReadAll(readwrite[0], rgb, size);
  ORDIE(close(readwrite[0]) != -1);
  ORDIE(waitpid(pid, &wstatus, 0) != -1);
  ORDIE(WEXITSTATUS(wstatus) == 0);
  return rgb;
}

int main(int argc, char *argv[]) {
  void *rgb;
  int i, yn, xn;
  setlocale(LC_ALL, "C.UTF-8");
  GetTermSize(&yn, &xn);
  yn *= 2;
  for (i = 1; i < argc; ++i) {
    if (strcmp(argv[i], "-t") == 0) {
      want24bit_ = 1;
    } else {
      rgb = LoadImageOrDie(argv[i], yn, xn);
      PrintImage(yn, xn, rgb);
      free(rgb);
    }
  }
  return 0;
}
