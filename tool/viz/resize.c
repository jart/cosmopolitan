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
#include "libc/calls/calls.h"
#include "libc/calls/ioctl.h"
#include "libc/calls/struct/stat.h"
#include "libc/calls/termios.h"
#include "libc/fmt/fmt.h"
#include "libc/log/check.h"
#include "libc/log/log.h"
#include "libc/mem/mem.h"
#include "libc/runtime/gc.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/fileno.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/prot.h"
#include "third_party/zlib/zlib.h"

void ProcessFile(const char *path) {
  /* int fd; */
  /* void *map; */
  /* z_stream zs; */
  /* uint8_t *rgb; */
  /* struct stat st; */
  /* const char *ext; */
  /* int width, height; */
  /* size_t pathlen, rgbbytes; */
  /* CHECK_NOTNULL((ext = memrchr(path, '-', (pathlen = strlen(path))))); */
  /* CHECK_EQ(2, sscanf(ext, "-%dx%d.rgb.gz", &width, &height)); */
  /* CHECK_NE(-1, (fd = open(path, O_RDONLY))); */
  /* CHECK_NE(-1, fstat(fd, &st)); */
  /* CHECK_NOTNULL((rgb = malloc((rgbbytes = width * height * 3)))); */
  /* CHECK_NE(MAP_FAILED, */
  /*          (map = mmap(NULL, st.st_size, PROT_READ, MAP_SHARED, fd, 0))); */
  /* zs.next_in = map; */
  /* zs.zalloc = Z_NULL; */
  /* zs.zfree = Z_NULL; */
  /* zs.avail_in = st.st_size; */
  /* zs.total_in = st.st_size; */
  /* zs.next_out = rgb; */
  /* zs.avail_out = rgbbytes; */
  /* zs.total_out = rgbbytes; */
  /* CHECK_EQ(Z_OK, inflateInit2(&zs, 16 + MAX_WBITS)); */
  /* CHECK_NE(Z_BUF_ERROR, inflate(&zs, Z_NO_FLUSH)); */
  /* CHECK_EQ(Z_OK, inflateEnd(&zs)); */

  /* struct winsize ws; */
  /* struct Resizer *rz; */
  /* ws.ws_row = 25; */
  /* ws.ws_col = 80; */
  /* LOGIFNEG1(ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws)); */
  /* ws.ws_row *= 2; */
  /* ws.ws_col /= 2; */
  /* ws.ws_row /= 2; */

  /* int y, x, i; */
  /* uint8_t *rgb2, *u8p; */
  /* float *frgba, *frgba2, *f32p; */
  /* CHECK_NOTNULL((rz = NewResizer())); */
  /* CHECK_NOTNULL((frgba = malloc(width * height * 4 * sizeof(float)))); */
  /* CHECK_NOTNULL((frgba2 = malloc(ws.ws_row * ws.ws_col * 4 *
   * sizeof(float)))); */
  /* CHECK_NOTNULL((rgb2 = (uint8_t *)malloc(ws.ws_row * ws.ws_col * 3))); */
  /* for (u8p = rgb, f32p = frgba, y = 0; y < height; ++y) { */
  /*   for (x = 0; x < width; ++x, u8p += 3, f32p += 4) { */
  /*     f32p[0] = (int)u8p[0], f32p[1] = (int)u8p[1]; */
  /*     f32p[2] = (int)u8p[2], f32p[3] = 1; */
  /*     for (i = 0; i < 4; ++i) f32p[i] /= 256; */
  /*   } */
  /* } */
  /* ResizeImage(rz, frgba, ws.ws_row, ws.ws_col, frgba, height, width); */
  /* for (u8p = rgb2, f32p = frgba2, y = 0; y < ws.ws_row; ++y) { */
  /*   for (x = 0; x < ws.ws_col; ++x, u8p += 3, f32p += 4) { */
  /*     for (i = 0; i < 4; ++i) f32p[i] *= 256; */
  /*     u8p[0] = (int)f32p[0]; */
  /*     u8p[1] = (int)f32p[1]; */
  /*     u8p[2] = (int)f32p[2]; */
  /*   } */
  /* } */
  /* free(frgba2); */
  /* free(frgba); */

  /* int y, x; */
  /* uint8_t *rgb2; */
  /* CHECK_NOTNULL((rz = NewResizer())); */
  /* CHECK_NOTNULL((rgb2 = (uint8_t *)malloc(ws.ws_row * ws.ws_col * 3))); */
  /* printf("%d %d %d %d %d %d\n", rgb2, ws.ws_row, ws.ws_col, rgb, height,
   * width); */
  /* ResizeImage8(rz, rgb2, ws.ws_row, ws.ws_col, rgb, height, width); */

  /* uint8_t *p; */
  /* /\* printf("\e[H"); *\/ */
  /* for (p = rgb2, y = 0; y < ws.ws_row / 2; ++y) { */
  /*   for (x = 0; x < ws.ws_col; ++x, p += 3) { */
  /*     printf("\e[48;2;%hhu;%hhu;%hhu;38;2;%hhu;%hhu;%hhum▄", p[0], p[1],
   * p[2], */
  /*            p[ws.ws_col * 3 + 0], p[ws.ws_col * 3 + 1], p[ws.ws_col * 3 +
   * 2]); */
  /*   } */
  /*   for (x = 0; x < ws.ws_col; ++x, p += 3) { */
  /*   } */
  /*   printf("\n"); */
  /* } */
  /* printf("\e[0m"); */

  /* uint8_t *rgb2; */
  /* struct winsize ws; */
  /* struct Resizer *rz; */
  /* ws.ws_row = 25; */
  /* ws.ws_col = 80; */
  /* LOGIFNEG1(ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws)); */
  /* CHECK_NOTNULL((rz = NewResizer())); */
  /* CHECK_NOTNULL((rgb2 = malloc(ws.ws_row * ws.ws_col * 3))); */
  /* printf("%d %d %d %d %d %d\n", rgb2, ws.ws_row, ws.ws_col, rgb, height,
   * width); */
  /* ResizeImage8(rz, rgb2, ws.ws_row, ws.ws_col, rgb, height, width); */

  /* int y, x; */
  /* uint8_t *p; */
  /* printf("\e[H"); */
  /* for (p = rgb2, y = 0; y < ws.ws_row; ++y) { */
  /*   for (x = 0; x < ws.ws_col; ++x, p += 3) { */
  /*     if (p[0] || p[1] || p[2]) { */
  /*       printf("\e[48;2;%hhu;%hhu;%hhum ", p[0], p[1], p[2]); */
  /*     } */
  /*   } */
  /*   printf("\n"); */
  /* } */
  /* printf("\e[0m"); */

  /* CHECK_NE(-1, munmap(map, st.st_size)); */
  /* CHECK_NE(-1, close(fd)); */
  /* FreeResizer(rz); */
  /* free(rgb2); */
  /* free(rgb); */
}

int main(int argc, char *argv[]) {
  int i;
  for (i = 1; i < argc; ++i) {
    ProcessFile(argv[i]);
  }
  return 0;
}
