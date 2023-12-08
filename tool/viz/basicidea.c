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
#include "libc/calls/struct/stat.h"
#include "libc/calls/struct/winsize.h"
#include "libc/calls/termios.h"
#include "libc/log/log.h"
#include "libc/macros.internal.h"
#include "libc/mem/mem.h"
#include "libc/runtime/runtime.h"
#include "libc/str/locale.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/exit.h"
#include "libc/sysv/consts/fileno.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/prot.h"
#include "libc/sysv/consts/termios.h"
#include "third_party/stb/stb_image.h"

#define SQR(X)    ((X) * (X))
#define UNCUBE(x) x < 48 ? 0 : x < 115 ? 1 : (x - 35) / 40
#define ORDIE(X)                   \
  do {                             \
    if (!(X)) perror(#X), exit(1); \
  } while (0)

int want24bit_;
int wantfullsize_;

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
    for (x = 0; x < xn; ++x) {
      if (want24bit_) {
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

/**
 * Determines dimensions of teletypewriter.
 */
void GetTermSize(int *out_rows, int *out_cols) {
  struct winsize ws;
  ws.ws_row = 20;
  ws.ws_col = 80;
  tcgetwinsize(STDOUT_FILENO, &ws);
  tcgetwinsize(STDIN_FILENO, &ws);
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

static void Deblinterlace(long zn, long yn, long xn,
                          unsigned char dst[zn][yn][xn],
                          const unsigned char src[yn][xn][zn]) {
  long y, x, z;
  for (y = 0; y < yn; ++y) {
    for (x = 0; x < xn; ++x) {
      for (z = 0; z < zn; ++z) {
        dst[z][y][x] = src[y][x][z];
      }
    }
  }
}

static void Reblinterlace(long zn, long yn, long xn,
                          unsigned char dst[yn][xn][zn],
                          const unsigned char src[zn][yn][xn]) {
  long y, x, z;
  for (y = 0; y < yn; ++y) {
    for (x = 0; x < xn; ++x) {
      for (z = 0; z < zn; ++z) {
        dst[y][x][z] = src[z][y][x];
      }
    }
  }
}

int main(int argc, char *argv[]) {
  struct stat st;
  void *map, *data, *data2;
  int i, fd, tyn, txn, yn, xn, cn;
  setlocale(LC_ALL, "C.UTF-8");
  for (i = 1; i < argc; ++i) {
    if (strcmp(argv[i], "-t") == 0) {
      want24bit_ = 1;
    } else if (strcmp(argv[i], "-f") == 0) {
      wantfullsize_ = 1;
    } else {
      fd = open(argv[i], O_RDONLY);
      if (fd == -1) {
        perror(argv[i]);
        exit(1);
      }
      fstat(fd, &st);
      map = mmap(0, st.st_size, PROT_READ, MAP_SHARED, fd, 0);
      data = stbi_load_from_memory(map, st.st_size, &xn, &yn, &cn, 3);
      munmap(map, st.st_size);
      close(fd);
      if (!wantfullsize_) {
        GetTermSize(&tyn, &txn);
        --tyn;
        tyn *= 2;
        data2 = memalign(32, 3 * yn * xn);
        Deblinterlace(3, yn, xn, data2, data);
        free(data);
        data = memalign(32, 3 * tyn * txn);
        EzGyarados(3, tyn, txn, data, 3, yn, xn, data2, 0, 3, tyn, txn, yn, xn,
                   0, 0, 0, 0);
        free(data2);
        data2 = memalign(32, 3 * yn * xn);
        Reblinterlace(3, tyn, txn, data2, data);
        free(data);
        data = data2;
        yn = tyn;
        xn = txn;
      }
      PrintImage(yn, xn, data);
      free(data);
    }
  }
  return 0;
}
