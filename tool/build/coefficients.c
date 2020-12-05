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
#include "dsp/core/half.h"
#include "dsp/core/q.h"
#include "libc/conv/conv.h"
#include "libc/log/check.h"
#include "libc/log/log.h"
#include "libc/macros.h"
#include "libc/math.h"
#include "libc/mem/mem.h"
#include "libc/runtime/gc.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/ex.h"
#include "libc/sysv/consts/exit.h"
#include "libc/x/x.h"
#include "third_party/gdtoa/gdtoa.h"
#include "third_party/getopt/getopt.h"

#define USAGE \
  " [FLAGS] [INT|FLOAT...]\n\
\n\
Example:\n\
\n\
  coefficients -n -- 1 4 6 4 1              # Gaussian Blur\n\
  coefficients -L16 -H235 -- .299 .587 .114 # BT.601 RGB→Y\n\
\n\
Flags:\n\
  -v         verbose\n\
  -n         normalize\n\
  -m FLEX    explicit Q(bits)\n\
  -L FLEX    low data value [default 0]\n\
  -H FLEX    high data value [default 255]\n\
  -?         shows this information\n\
\n"

static struct Flags {
  bool n;
  long L, H, m;
} flags_ = {
    .L = 0,
    .H = 255,
};

static wontreturn void PrintUsage(int rc, FILE *f) {
  fprintf(f, "Usage: %s%s", program_invocation_name, USAGE);
  exit(rc);
}

static void GetOpts(int argc, char *argv[]) {
  int opt;
  while ((opt = getopt(argc, argv, "?nvrL:H:m:")) != -1) {
    switch (opt) {
      case 'v':
        g_loglevel++;
        break;
      case 'n':
        flags_.n = true;
        break;
      case 'L':
        flags_.L = strtol(optarg, NULL, 0);
        break;
      case 'H':
        flags_.H = strtol(optarg, NULL, 0);
        break;
      case 'm':
        flags_.m = strtol(optarg, NULL, 0);
        break;
      case '?':
        PrintUsage(EXIT_SUCCESS, stdout);
      default:
        PrintUsage(EX_USAGE, stderr);
    }
  }
}

static void *Normalize(int n, double A[static 8]) {
  int i;
  double sum, rnorm;
  for (sum = i = 0; i < n; ++i) {
    sum += A[i];
  }
  if (fabs(sum - 1) > DBL_MIN * n) {
    rnorm = 1 / sum;
    for (i = 0; i < n; ++i) {
      A[i] *= rnorm;
    }
  }
  return A;
}

static void GetLimits(int n, const long I[static 8], long m, long L, long H,
                      long res[2][2]) {
  int i, j[8];
  long x, p[2] = {L, H};
  DCHECK(0 < n && n <= 8);
  memset(res, 0, sizeof(long) * 2 * 2);
  for (j[0] = 0; j[0] < ARRAYLEN(p); ++j[0]) {
    for (j[1] = 0; j[1] < ARRAYLEN(p); ++j[1]) {
      for (j[2] = 0; j[2] < ARRAYLEN(p); ++j[2]) {
        for (j[3] = 0; j[3] < ARRAYLEN(p); ++j[3]) {
          for (j[4] = 0; j[4] < ARRAYLEN(p); ++j[4]) {
            for (j[5] = 0; j[5] < ARRAYLEN(p); ++j[5]) {
              for (j[6] = 0; j[6] < ARRAYLEN(p); ++j[6]) {
                for (j[7] = 0; j[7] < ARRAYLEN(p); ++j[7]) {
                  x = 0;
                  for (i = 0; i < ARRAYLEN(j); ++i) {
                    x += p[j[i]] * I[i];
                    if (x < res[0][0]) res[0][0] = x;
                    if (x > res[0][1]) res[0][1] = x;
                  }
                  x += 1l << (m - 1);
                  if (x < res[0][0]) res[0][0] = x;
                  if (x > res[0][1]) res[0][1] = x;
                  x >>= m;
                  if (x < res[1][0]) res[1][0] = x;
                  if (x > res[1][1]) res[1][1] = x;
                }
              }
            }
          }
        }
      }
    }
  }
}

static const char *GetFittingMachineWord(long L, long H) {
  if (-128 <= L && H <= 127) return "int8";
  if (0 <= L && H <= 255) return "uint8";
  if (-0x8000 <= L && H <= 0x7fff) return "int16";
  if (~0x7fffffff <= L && H <= 0x7fffffff) return "int32";
  if (0 <= L && H <= 0xffffffff) return "uint32";
  return "INT64";
}

static char *DescribeMachineWord(long L, long H) {
  return xasprintf("%s[%,ld..%,ld]", GetFittingMachineWord(L, H), L, H);
}

static void ShowBetterCoefficients(int n, double C[static 8], long L, long H) {
  long err, I[8], lim[2][2];
  char buf[32], kAlphabet[] = "abcdefgh";
  int i, j, m, emitted, count, indices[8];
  CHECK_LT(L, H);
  DCHECK(0 < n && n <= 8);
  for (m = 2; m < 40; ++m) {
    memset(I, 0, sizeof(I));
    if (C[6] || C[7]) {
      err = GetIntegerCoefficients8(I, C, m, L, H);
    } else {
      err = GetIntegerCoefficients(I, C, m, L, H);
    }
    GetLimits(n, I, m, L, H, lim);
    for (count = i = 0; i < n; ++i) {
      if (I[i]) {
        indices[count++] = i;
      }
    }
    if (count) {
      emitted = 0;
      if (m) emitted += printf("(");
      for (i = 0; i < count; ++i) {
        if (i) emitted += printf(" + ");
        if (I[indices[i]] != 1) {
          emitted += printf("%ld*%c", I[indices[i]], kAlphabet[indices[i]]);
        } else {
          emitted += printf("%c", kAlphabet[indices[i]]);
        }
      }
      if (m) {
        if (m > 1) {
          emitted += printf(" + %ld", 1l << (m - 1));
        }
        emitted += printf(")>>%d", m);
      }
      printf("%*s", MAX(0, 80 - emitted), " ");
      printf("/* %s %s %s ε=%,ld */\n", gc(DescribeMachineWord(L, H)),
             gc(DescribeMachineWord(lim[0][0], lim[0][1])),
             gc(DescribeMachineWord(lim[1][0], lim[1][1])), err);
    }
  }
}

static int ReadIdealCoefficients(double C[static 8], int sn, char *S[sn]) {
  int i, n;
  if ((n = MIN(8, sn)) > 0) {
    C[0] = C[1] = C[2] = C[3] = C[4] = C[5] = 0;
    for (i = 0; i < n; ++i) {
      C[i] = strtod(S[i], NULL);
    }
  }
  return n;
}

int ToolBuildCoefficients(int argc, char *argv[]) {
  int n;
  double C[8];
  GetOpts(argc, argv);
  setvbuf(stdout, malloc(PAGESIZE), _IOLBF, PAGESIZE);
  if ((n = ReadIdealCoefficients(C, argc - optind, argv + optind))) {
    if (flags_.n) Normalize(n, C);
    ShowBetterCoefficients(n, C, flags_.L, flags_.H);
  }
  return 0;
}

int main(int argc, char *argv[]) {
  g_loglevel = kLogWarn;
  showcrashreports();
  return ToolBuildCoefficients(argc, argv);
}
