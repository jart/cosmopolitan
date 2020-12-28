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
#include "libc/assert.h"
#include "libc/calls/calls.h"
#include "libc/calls/struct/stat.h"
#include "libc/limits.h"
#include "libc/log/check.h"
#include "libc/macros.h"
#include "libc/math.h"
#include "libc/mem/mem.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/madv.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/prot.h"
#include "libc/x/x.h"
#include "third_party/stb/stb_image.h"

#define C13(A, B)     (((A) + 3 * (B)) / 4)
#define S3T(A, B, C)  MAX(0, ((24 * (B)) - (4 * ((A) + (C)))) / 16)
#define LERP(A, B, P) ((A) * (1 - (P)) + (B) * (P))

float ByteToFloat(int b) {
  return 1 / 255.f * b;
}

int FloatToByte(float f) {
  return MAX(0, MIN(255, roundf(255 * f)));
}

float ChessBoard(unsigned y, unsigned x, float a, float b) {
  return !((y ^ x) & (1u << 2)) ? a : b;
}

float AlphaBackground(unsigned y, unsigned x) {
  return ChessBoard(y, x, 1.f, .7f);
}

float OutOfBoundsBackground(unsigned y, unsigned x) {
  return ChessBoard(y, x, .01f, .02f);
}

float Opacify(size_t yn, size_t xn, const float P[yn][xn],
              const float A[yn][xn], long y, long x) {
  if ((0 <= y && y < yn) && (0 <= x && x < xn)) {
    return LERP(AlphaBackground(y, x), P[y][x], A[y][x]);
  } else {
    return OutOfBoundsBackground(y, x);
  }
}

void PrintImage(size_t yn, size_t xn, float R[yn][xn], float G[yn][xn],
                float B[yn][xn], float A[yn][xn]) {
  unsigned y, x;
  for (y = 0; y < yn; y += 2) {
    if (y) printf("\e[0m\n");
    for (x = 0; x < xn; ++x) {
      printf("\e[48;2;%d;%d;%d;38;2;%d;%d;%dm▄",
             FloatToByte(Opacify(yn, xn, R, A, y + 0, x)),
             FloatToByte(Opacify(yn, xn, G, A, y + 0, x)),
             FloatToByte(Opacify(yn, xn, B, A, y + 0, x)),
             FloatToByte(Opacify(yn, xn, R, A, y + 1, x)),
             FloatToByte(Opacify(yn, xn, G, A, y + 1, x)),
             FloatToByte(Opacify(yn, xn, B, A, y + 1, x)));
    }
  }
  printf("\e[0m\n");
}

void DeblinterlaceRgba(size_t dyn, size_t dxn, float R[dyn][dxn],
                       float G[dyn][dxn], float B[dyn][dxn], float A[dyn][dxn],
                       size_t syn, size_t sxn,
                       const unsigned char src[syn][sxn][4]) {
  unsigned y, x;
  for (y = 0; y < syn; ++y) {
    for (x = 0; x < sxn; ++x) {
      R[y][x] = ByteToFloat(src[y][x][0]);
      G[y][x] = ByteToFloat(src[y][x][1]);
      B[y][x] = ByteToFloat(src[y][x][2]);
      A[y][x] = ByteToFloat(src[y][x][3]);
    }
  }
}

void SharpenX(size_t yw, size_t xw, float dst[yw][xw], const float src[yw][xw],
              size_t yn, size_t xn) {
  int y, x;
  for (y = 0; y < yn; ++y) {
    for (x = 0; x < xn; ++x) {
      dst[y][x] =
          S3T(src[y][MAX(0, x - 1)], src[y][x], src[y][MIN(xn - 1, x + 1)]);
    }
  }
}

void SharpenY(size_t yw, size_t xw, float dst[yw][xw], const float src[yw][xw],
              size_t yn, size_t xn) {
  int y, x;
  for (y = 0; y < yn; ++y) {
    for (x = 0; x < xn; ++x) {
      dst[y][x] =
          S3T(src[MAX(0, y - 1)][x], src[y][x], src[MIN(yn - 1, y + 1)][x]);
    }
  }
}

void UpscaleX(size_t yw, size_t xw, float img[yw][xw], size_t yn, size_t xn) {
  unsigned y, x;
  for (y = yn; y--;) {
    for (x = xn; --x;) {
      img[y][x] =
          C13(img[y][MIN(xn / 2 - 1, x / 2 - 1 + x % 2 * 2)], img[y][x / 2]);
    }
  }
}

void UpscaleY(size_t yw, size_t xw, float img[yw][xw], size_t yn, size_t xn) {
  unsigned y, x;
  for (y = yn; --y;) {
    for (x = xn; x--;) {
      img[y][x] =
          C13(img[MIN(yn / 2 - 1, y / 2 - 1 + y % 2 * 2)][x], img[y / 2][x]);
    }
  }
}

void Upscale(size_t yw, size_t xw, float img[yw][xw], float tmp[yw][xw],
             size_t yn, size_t xn) {
  UpscaleY(yw, xw, img, yn, xn / 2);
  SharpenY(yw, xw, tmp, img, yn, xn / 2);
  UpscaleX(yw, xw, tmp, yn, xn);
  SharpenX(yw, xw, img, tmp, yn, xn);
}

void ProcessImageDouble(size_t yn, size_t xn, unsigned char img[yn][xn][4]) {
  void *t = xmemalign(32, sizeof(float) * yn * 2 * xn * 2);
  void *R = xmemalign(32, sizeof(float) * yn * 2 * xn * 2);
  void *G = xmemalign(32, sizeof(float) * yn * 2 * xn * 2);
  void *B = xmemalign(32, sizeof(float) * yn * 2 * xn * 2);
  void *A = xmemalign(32, sizeof(float) * yn * 2 * xn * 2);
  DeblinterlaceRgba(yn * 2, xn * 2, R, G, B, A, yn, xn, img);
  Upscale(yn * 2, xn * 2, R, t, yn * 2, xn * 2);
  Upscale(yn * 2, xn * 2, G, t, yn * 2, xn * 2);
  Upscale(yn * 2, xn * 2, B, t, yn * 2, xn * 2);
  Upscale(yn * 2, xn * 2, A, t, yn * 2, xn * 2);
  free(t);
  PrintImage(yn * 2, xn * 2, R, G, B, A);
  free(R);
  free(G);
  free(B);
  free(A);
}

void ProcessImage(size_t yn, size_t xn, unsigned char img[yn][xn][4]) {
  void *R = xmemalign(32, sizeof(float) * yn * xn);
  void *G = xmemalign(32, sizeof(float) * yn * xn);
  void *B = xmemalign(32, sizeof(float) * yn * xn);
  void *A = xmemalign(32, sizeof(float) * yn * xn);
  DeblinterlaceRgba(yn, xn, R, G, B, A, yn, xn, img);
  PrintImage(yn, xn, R, G, B, A);
  free(R);
  free(G);
  free(B);
  free(A);
}

void WithImageFile(const char *path, void fn(size_t yn, size_t xn,
                                             unsigned char img[yn][xn][4])) {
  struct stat st;
  int fd, yn, xn;
  void *map, *data;
  CHECK_NE(-1, (fd = open(path, O_RDONLY)), "%s", path);
  CHECK_NE(-1, fstat(fd, &st));
  CHECK_GT(st.st_size, 0);
  CHECK_LE(st.st_size, INT_MAX);
  fadvise(fd, 0, 0, MADV_WILLNEED | MADV_SEQUENTIAL);
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

int main(int argc, char *argv[]) {
  int i;
  for (i = 1; i < argc; ++i) {
    WithImageFile(argv[i], ProcessImageDouble);
  }
  return 0;
}
