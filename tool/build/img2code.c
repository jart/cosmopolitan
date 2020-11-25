/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2020 Justine Alexandra Roberts Tunney                              │
│                                                                              │
│ This program is free software; you can redistribute it and/or modify         │
│ it under the terms of the GNU General Public License as published by         │
│ the Free Software Foundation; version 2 of the License.                      │
│                                                                              │
│ This program is distributed in the hope that it will be useful, but          │
│ WITHOUT ANY WARRANTY; without even the implied warranty of                   │
│ MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU             │
│ General Public License for more details.                                     │
│                                                                              │
│ You should have received a copy of the GNU General Public License            │
│ along with this program; if not, write to the Free Software                  │
│ Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA                │
│ 02110-1301 USA                                                               │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "dsp/scale/scale.h"
#include "libc/calls/calls.h"
#include "libc/calls/struct/stat.h"
#include "libc/conv/conv.h"
#include "libc/fmt/bing.internal.h"
#include "libc/limits.h"
#include "libc/log/check.h"
#include "libc/log/log.h"
#include "libc/macros.h"
#include "libc/math.h"
#include "libc/mem/mem.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/ex.h"
#include "libc/sysv/consts/exit.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/prot.h"
#include "libc/testlib/testlib.h"
#include "third_party/dtoa/dtoa.h"
#include "third_party/getopt/getopt.h"
#include "third_party/stb/stb_image.h"
#include "third_party/xed/x86.h"

#define USAGE \
  " [FLAGS] [PATH]\n\
\n\
Example:\n\
\n\
  img2code -cw79 -p2 foo.png\n\
\n\
Flags:\n\
  -c         compress range\n\
  -o PATH    out\n\
  -r FLEX    ratio\n\
  -w FLEX    new width\n\
  -h FLEX    new height\n\
  -p FLEX    pixel y ratio\n\
  -?         shows this information\n\
\n"

static struct Flags {
  const char *o;
  double r, p;
  int w, h;
  bool c;
} flags_ = {
    .o = "-",
    .p = 1,
};

static noreturn void PrintUsage(int rc, FILE *f) {
  fprintf(f, "Usage: %s%s", program_invocation_name, USAGE);
  exit(rc);
}

static void GetOpts(int *argc, char *argv[]) {
  int opt;
  if (*argc == 2 &&
      (strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-help") == 0)) {
    PrintUsage(EXIT_SUCCESS, stdout);
  }
  while ((opt = getopt(*argc, argv, "?co:r:w:h:p:")) != -1) {
    switch (opt) {
      case 'c':
        flags_.c = true;
        break;
      case 'o':
        flags_.o = optarg;
        break;
      case 'r':
        flags_.r = strtod(optarg, NULL);
        break;
      case 'p':
        flags_.p = strtod(optarg, NULL);
        break;
      case 'w':
        flags_.w = strtol(optarg, NULL, 0);
        break;
      case 'h':
        flags_.h = strtol(optarg, NULL, 0);
        break;
      case '?':
        PrintUsage(EXIT_SUCCESS, stdout);
      default:
        PrintUsage(EX_USAGE, stderr);
    }
  }
  if (optind == *argc) {
    argv[(*argc)++] = "-";
  }
}

static void GetOutputGeometry(long syn, long sxn, long *out_dyn, long *out_dxn,
                              double *out_ry, double *out_rx) {
  double ry, rx;
  long dyn, dxn;
  if (!flags_.h && !flags_.w) {
    if (flags_.r) {
      ry = flags_.r * flags_.p;
      rx = flags_.r;
    } else {
      ry = flags_.p;
      rx = 1;
    }
    dyn = round(syn / ry);
    dxn = round(sxn / rx);
  } else if (flags_.w && !flags_.h) {
    if (flags_.r) {
      rx = 1. * sxn / flags_.w;
      ry = flags_.r * flags_.p;
      dxn = flags_.w;
      dyn = round(syn / ry);
    } else {
      rx = 1. * sxn / flags_.w;
      ry = flags_.p * rx;
      dxn = flags_.w;
      dyn = round(syn / ry);
    }
  } else if (flags_.h && !flags_.w) {
    if (flags_.r) {
      rx = flags_.r;
      ry = flags_.p * syn / flags_.h;
      dxn = flags_.w;
      dyn = round(syn / ry);
    } else {
      ry = flags_.p * syn / flags_.h;
      rx = ry;
      dyn = flags_.h;
      dxn = round(syn / rx);
    }
  } else {
    ry = flags_.p;
    rx = 1;
    dyn = round(flags_.h / ry);
    dxn = flags_.w;
  }
  *out_dyn = dyn;
  *out_dxn = dxn;
  *out_ry = ry;
  *out_rx = rx;
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

static void CompressRange(long cn, long yn, long xn,
                          unsigned char img[cn][yn][xn]) {
  static const char R[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
  double f;
  long c, y, x, L, H;
  L = R[0], H = R[strlen(R) - 1];
  for (c = 0; c < cn; ++c) {
    for (y = 0; y < yn; ++y) {
      for (x = 0; x < xn; ++x) {
        f = img[c][y][x];
        f /= 255;
        f *= H - L;
        f += L;
        img[c][y][x] = MIN(H, MAX(L, lround(f)));
      }
    }
  }
}

static void PrintCode(FILE *f, long cn, long yn, long xn,
                      unsigned char img[cn][yn][xn]) {
  long c, y, x;
  if (flags_.c) CompressRange(cn, yn, xn, img);
  for (c = 0; c < cn; ++c) {
    fputc('\n', f);
    for (y = 0; y < yn; ++y) {
      fputc('\n', f);
      for (x = 0; x < xn; ++x) {
        fputwc(bing(img[c][y][x], 0), f);
      }
    }
  }
  fputc('\n', f);
}

static void ProcessImage(const char *path, long scn, long syn, long sxn,
                         unsigned char img[syn][sxn][scn], FILE *f) {
  double ry, rx;
  long dyn, dxn, cn;
  GetOutputGeometry(syn, sxn, &dyn, &dxn, &ry, &rx);
  if (dyn == syn && dxn == sxn) {
    /* TODO(jart): Why doesn't Gyarados no-op? */
    PrintCode(f, scn, dyn, dxn,
              Deinterlace(scn, syn, sxn, gc(memalign(32, scn * syn * sxn)), syn,
                          sxn, scn, img, syn, sxn, 0, 0, 0));
  } else {
    PrintCode(
        f, scn, dyn, dxn,
        EzGyarados(scn, dyn, dxn, gc(memalign(32, scn * dyn * dxn)), scn, syn,
                   sxn,
                   Deinterlace(scn, syn, sxn, gc(memalign(32, scn * syn * sxn)),
                               syn, sxn, scn, img, syn, sxn, 0, 0, 0),
                   0, scn, dyn, dxn, syn, sxn, ry, rx, 0, 0));
  }
}

static void WithImageFile(const char *path, FILE *f,
                          void fn(const char *path, long cn, long yn, long xn,
                                  unsigned char src[yn][xn][cn], FILE *f)) {
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
      (data = stbi_load_from_memory(map, st.st_size, &xn, &yn, &cn, 0)), "%s",
      path);
  CHECK_NE(-1, munmap(map, st.st_size));
  CHECK_NE(-1, close(fd));
  fn(path, cn, yn, xn, data, f);
  free(data);
}

int main(int argc, char *argv[]) {
  int i;
  FILE *f;
  showcrashreports();
  GetOpts(&argc, argv);
  stbi_set_unpremultiply_on_load(true);
  CHECK_NOTNULL((f = fopen(flags_.o, "w")));
  for (i = optind; i < argc; ++i) {
    WithImageFile(argv[i], f, ProcessImage);
  }
  return fclose(f);
}
