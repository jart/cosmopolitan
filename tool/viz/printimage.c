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
#include "libc/assert.h"
#include "libc/bits/bits.h"
#include "libc/bits/safemacros.h"
#include "libc/calls/calls.h"
#include "libc/calls/struct/stat.h"
#include "libc/limits.h"
#include "libc/log/check.h"
#include "libc/log/log.h"
#include "libc/macros.h"
#include "libc/math.h"
#include "libc/mem/mem.h"
#include "libc/rand/rand.h"
#include "libc/runtime/gc.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/ex.h"
#include "libc/sysv/consts/exit.h"
#include "libc/sysv/consts/madv.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/prot.h"
#include "libc/x/x.h"
#include "third_party/getopt/getopt.h"
#include "third_party/stb/stb_image.h"

#define LERP(X, Y, P) (((X) + (SAR((P) * ((Y) - (X)), 8))) & 0xff)

static struct Flags {
  const char *out;
  bool subpixel;
} g_flags;

static noreturn void PrintUsage(int rc, FILE *f) {
  fprintf(f, "Usage: %s%s", program_invocation_name, "\
 [FLAGS] [PATH]\n\
\n\
Flags:\n\
  -o PATH    output path\n\
  -p         convert to subpixel layout\n\
  -v         increases verbosity\n\
  -?         shows this information\n\
\n");
  exit(rc);
}

static void GetOpts(int *argc, char *argv[]) {
  int opt;
  if (*argc == 2 &&
      (strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-help") == 0)) {
    PrintUsage(EXIT_SUCCESS, stdout);
  }
  while ((opt = getopt(*argc, argv, "?vpo:")) != -1) {
    switch (opt) {
      case 'o':
        g_flags.out = optarg;
        break;
      case 'v':
        ++g_loglevel;
        break;
      case 'p':
        g_flags.subpixel = true;
        break;
      case '?':
        PrintUsage(EXIT_SUCCESS, stdout);
      default:
        PrintUsage(EX_USAGE, stderr);
    }
  }
  if (optind == *argc) {
    if (!g_flags.out) g_flags.out = "-";
    argv[(*argc)++] = "-";
  }
}

static unsigned char ChessBoard(unsigned y, unsigned x, unsigned char a,
                                unsigned char b) {
  return !((y ^ x) & (1u << 2)) ? a : b;
}

static unsigned char AlphaBackground(unsigned y, unsigned x) {
  return ChessBoard(y, x, 255, 200);
}

static unsigned char OutOfBoundsBackground(unsigned y, unsigned x) {
  return ChessBoard(y, x, 40, 80);
}

static unsigned char Opacify(long yn, long xn, const unsigned char P[yn][xn],
                             const unsigned char A[yn][xn], long y, long x) {
  if ((0 <= y && y < yn) && (0 <= x && x < xn)) {
    return LERP(AlphaBackground(y, x), P[y][x], A[y][x]);
  } else {
    return OutOfBoundsBackground(y, x);
  }
}

static void PrintRulerRight(long yn, long xn, long y, long x,
                            bool *inout_didhalfy) {
  if (y == 0) {
    printf("\e[0m‾0");
  } else if (yn / 2 <= y && y <= yn / 2 + 1 && !*inout_didhalfy) {
    printf("\e[0m‾%s%s", "yn/2", y % 2 ? "+1" : "");
    *inout_didhalfy = true;
  } else if (y + 1 == yn / 2 && !*inout_didhalfy) {
    printf("\e[0m⎯yn/2");
    *inout_didhalfy = true;
  } else if (y + 1 == yn) {
    printf("\e[0m⎯yn");
  } else if (y + 2 == yn) {
    printf("\e[0m_yn");
  } else if (!(y % 10)) {
    printf("\e[0m‾%,u", y);
  }
}

static void PrintImageImpl(long syn, long sxn, unsigned char RGB[3][syn][sxn],
                           long y0, long yn, long x0, long xn, bool rule,
                           long dy, long dx) {
  long y, x;
  bool didhalfy, didfirstx;
  unsigned char a[3], b[3];
  didhalfy = false;
  for (y = y0; y < yn; y += dy) {
    didfirstx = false;
    if (y) printf("\e[0m\n");
    for (x = x0; x < xn; x += dx) {
      a[0] = RGB[0][y][x];
      a[1] = RGB[1][y][x];
      a[2] = RGB[2][y][x];
      if (y + dy < yn && x + dx < xn) {
        b[0] = RGB[0][y + dy][x + dx];
        b[1] = RGB[1][y + dy][x + dx];
        b[2] = RGB[2][y + dy][x + dx];
      } else {
        b[2] = b[1] = b[0] = OutOfBoundsBackground(y + dy, x + dx);
      }
      printf("\e[48;2;%d;%d;%d;38;2;%d;%d;%dm%lc", a[0], a[1], a[2], b[0], b[1],
             b[2], dy > 1 ? u'▄' : u'▐');
      didfirstx = true;
    }
    if (rule) PrintRulerRight(yn, xn, y, x, &didhalfy);
  }
  printf("\e[0m\n");
}

static void PrintImage(long syn, long sxn, unsigned char RGB[3][syn][sxn],
                       long y0, long yn, long x0, long xn, bool rule) {
  PrintImageImpl(syn, sxn, RGB, y0, yn, x0, xn, rule, 2, 1);
}

static void PrintImageLR(long syn, long sxn, unsigned char RGB[3][syn][sxn],
                         long y0, long yn, long x0, long xn, bool rule) {
  PrintImageImpl(syn, sxn, RGB, y0, yn, x0, xn, rule, 1, 2);
}

static void *Deblinterlace(long dyn, long dxn, unsigned char dst[3][dyn][dxn],
                           long syn, long sxn,
                           const unsigned char src[syn][sxn][4], long y0,
                           long yn, long x0, long xn) {
  long y, x;
  for (y = y0; y < yn; ++y) {
    for (x = x0; x < xn; ++x) {
      dst[0][y][x] = src[y][x][0];
      dst[1][y][x] = src[y][x][1];
      dst[2][y][x] = src[y][x][2];
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

static void ProcessImage(long syn, long sxn, unsigned char RGB[syn][sxn][4],
                         long cn) {
  if (g_flags.subpixel) {
    PrintImageLR(
        syn, sxn * 3,
        DeblinterlaceSubpixelBgr(
            syn, sxn,
            gc(memalign(32, sizeof(unsigned char) * syn * sxn * 3 * 3)), syn,
            sxn, RGB, 0, syn, 0, sxn),
        0, syn, 0, sxn * 3, true);
  } else {
    PrintImage(
        syn, sxn,
        Deblinterlace(syn, sxn,
                      gc(memalign(32, sizeof(unsigned char) * syn * sxn * 3)),
                      syn, sxn, RGB, 0, syn, 0, sxn),
        0, syn, 0, sxn, true);
  }
}

void WithImageFile(const char *path,
                   void fn(long syn, long sxn, unsigned char RGB[syn][sxn][4],
                           long cn)) {
  struct stat st;
  void *map, *data;
  int fd, yn, xn, cn;
  CHECK_NE(-1, (fd = open(path, O_RDONLY)), "%s", path);
  CHECK_NE(-1, fstat(fd, &st));
  CHECK_GT(st.st_size, 0);
  CHECK_LE(st.st_size, INT_MAX);
  fadvise(fd, 0, 0, MADV_WILLNEED | MADV_SEQUENTIAL);
  CHECK_NE(MAP_FAILED,
           (map = mmap(NULL, st.st_size, PROT_READ, MAP_SHARED, fd, 0)));
  CHECK_NOTNULL(
      (data = stbi_load_from_memory(map, st.st_size, &xn, &yn, &cn, 4)), "%s",
      path);
  CHECK_NE(-1, munmap(map, st.st_size));
  CHECK_NE(-1, close(fd));
  fn(yn, xn, data, 4);
  free(data);
}

int main(int argc, char *argv[]) {
  int i;
  showcrashreports();
  GetOpts(&argc, argv);
  stbi_set_unpremultiply_on_load(true);
  for (i = optind; i < argc; ++i) {
    WithImageFile(argv[i], ProcessImage);
  }
  return 0;
}
