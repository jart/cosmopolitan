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
#include "libc/bits/safemacros.h"
#include "libc/fmt/conv.h"
#include "libc/fmt/fmt.h"
#include "libc/limits.h"
#include "libc/log/check.h"
#include "libc/log/log.h"
#include "libc/macros.h"
#include "libc/math.h"
#include "libc/mem/mem.h"
#include "libc/rand/lcg.internal.h"
#include "libc/rand/rand.h"
#include "libc/runtime/gc.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/ex.h"
#include "libc/sysv/consts/exit.h"
#include "libc/x/x.h"
#include "third_party/gdtoa/gdtoa.h"
#include "third_party/getopt/getopt.h"
#include "tool/viz/lib/formatstringtable.h"

typedef double (*round_f)(double);
typedef unsigned long (*rand_f)(void);

struct Range {
  long double a;
  long double b;
};

short xn_ = 8;
short yn_ = 8;
double digs_ = 6;
rand_f rand_;
round_f rounder_;
const char *path_ = "-";
const char *name_ = "M";
const char *type_ = "float";
struct Range r1_ = {LONG_MIN, LONG_MAX};
struct Range r2_ = {0, 1};
StringTableFormatter *formatter_ = FormatStringTableAsCode;

static wontreturn void PrintUsage(int rc, FILE *f) {
  fprintf(f, "Usage: %s%s", program_invocation_name, "\
 [FLAGS] [FILE]\n\
\n\
Flags:\n\
  -u         unsigned\n\
  -c         char\n\
  -s         short\n\
  -i         int\n\
  -l         long\n\
  -d         double\n\
  -b         bytes [-uc]\n\
  -g         non-deterministic rng\n\
  -S         output assembly\n\
  -W         output whitespace\n\
  -o PATH    output path\n\
  -x FLEX\n\
  -w FLEX    width\n\
  -y FLEX\n\
  -h FLEX    height\n\
  -N NAME    name\n\
  -T NAME    type name\n\
  -A FLEX    min value\n\
  -B FLEX    max value\n\
  -R FUNC    round function for indexing\n\
  -D FLEX    decimal digits to printout\n\
  -v         increases verbosity\n\
  -?         shows this information\n\
\n");
  exit(rc);
}

static bool StringEquals(const char *a, const char *b) {
  return strcasecmp(a, b) == 0;
}

static wontreturn void ShowInvalidArg(const char *name, const char *s,
                                      const char *type) {
  fprintf(stderr, "error: invalid %s %s: %s\n", type, name, s);
  exit(EXIT_FAILURE);
}

static double ParseFlexidecimalOrDie(const char *name, const char *s,
                                     double min, double max) {
  double x;
  s = firstnonnull(s, "NULL");
  if (strchr(s, '.') || strchr(s, 'e')) {
    x = strtod(s, NULL);
  } else {
    x = strtol(s, NULL, 0);
  }
  if (min <= x && x <= max) {
    return x;
  } else {
    ShowInvalidArg(name, s, "flexidecimal");
  }
}

static round_f ParseRoundingFunctionOrDie(const char *s) {
  if (isempty(s) || StringEquals(s, "none") || StringEquals(s, "null")) {
    return NULL;
  } else if (StringEquals(s, "round")) {
    return round;
  } else if (StringEquals(s, "rint")) {
    return rint;
  } else if (StringEquals(s, "nearbyint")) {
    return nearbyint;
  } else if (StringEquals(s, "trunc")) {
    return trunc;
  } else if (StringEquals(s, "floor")) {
    return floor;
  } else if (StringEquals(s, "ceil")) {
    return ceil;
  } else {
    ShowInvalidArg("round", s, "func");
  }
}

static void ConfigureIntegralRange(const char *type, long min, long max) {
  type_ = type;
  r1_.a = min;
  r1_.b = max;
  r2_.a = min;
  r2_.b = max;
  if (!rounder_) rounder_ = round;
}

void GetOpts(int argc, char *argv[]) {
  int opt;
  bool want_unsigned, want_char, want_short, want_int, want_long, want_double;
  want_unsigned = false;
  want_char = false;
  want_short = false;
  want_int = false;
  want_long = false;
  want_double = false;
  if (argc == 2 &&
      (StringEquals(argv[1], "--help") || StringEquals(argv[1], "-help"))) {
    PrintUsage(EXIT_SUCCESS, stdout);
  }
  while ((opt = getopt(argc, argv, "?vubcsildgSWo:x:w:y:h:N:A:B:C:E:T:R:D:")) !=
         -1) {
    switch (opt) {
      case 'b':
        want_unsigned = true;
        want_char = true;
        break;
      case 'u':
        want_unsigned = true;
        break;
      case 'c':
        want_char = true;
        break;
      case 's':
        want_short = true;
        break;
      case 'i':
        want_int = true;
        break;
      case 'l':
        want_long = true;
        break;
      case 'd':
        want_double = true;
        break;
      case 'g':
        rand_ = rand64;
        break;
      case 'N':
        name_ = optarg;
        break;
      case 'o':
        path_ = optarg;
        break;
      case 'S':
        formatter_ = FormatStringTableAsAssembly;
        break;
      case 'W':
        formatter_ = FormatStringTableBasic;
        break;
      case 't':
        type_ = optarg;
        break;
      case 'x':
      case 'w':
        xn_ = ParseFlexidecimalOrDie("width", optarg, 1, SHRT_MAX);
        break;
      case 'y':
      case 'h':
        yn_ = ParseFlexidecimalOrDie("height", optarg, 1, SHRT_MAX);
        break;
      case 'D':
        digs_ = ParseFlexidecimalOrDie("digs", optarg, 0, 15.95);
        break;
      case 'r':
        rounder_ = ParseRoundingFunctionOrDie(optarg);
        break;
      case 'A':
        r1_.a = ParseFlexidecimalOrDie("r1_.a", optarg, INT_MIN, INT_MAX);
        break;
      case 'B':
        r1_.b = ParseFlexidecimalOrDie("r1_.b", optarg, INT_MIN, INT_MAX);
        break;
      case 'C':
        r2_.a = ParseFlexidecimalOrDie("r2_.a", optarg, INT_MIN, INT_MAX);
        break;
      case 'E':
        r2_.b = ParseFlexidecimalOrDie("r2_.b", optarg, INT_MIN, INT_MAX);
        break;
      case '?':
        PrintUsage(EXIT_SUCCESS, stdout);
      default:
        PrintUsage(EX_USAGE, stderr);
    }
  }
  if (want_unsigned && want_char) {
    ConfigureIntegralRange("unsigned char", 0, 255);
  } else if (want_char) {
    ConfigureIntegralRange("signed char", -128, 127);
  } else if (want_unsigned && want_short) {
    ConfigureIntegralRange("unsigned short", USHRT_MIN, USHRT_MAX);
  } else if (want_short) {
    ConfigureIntegralRange("short", SHRT_MIN, SHRT_MAX);
  } else if (want_unsigned && want_int) {
    ConfigureIntegralRange("unsigned", UINT_MIN, UINT_MAX);
  } else if (want_int) {
    ConfigureIntegralRange("int", INT_MIN, INT_MAX);
  } else if (want_unsigned && want_long) {
    ConfigureIntegralRange("unsigned long", ULONG_MIN, ULONG_MAX);
  } else if (want_long) {
    ConfigureIntegralRange("long", LONG_MIN, LONG_MAX);
  } else if (want_double) {
    type_ = "double";
    r1_.a = LONG_MIN;
    r1_.b = LONG_MAX;
    digs_ = 19;
  }
}

static void *SetRandom(long n, long p[n]) {
  long i;
  uint64_t r;
  if (rand_) {
    for (r = 1, i = 0; i < n; ++i) {
      p[i] = rand_();
    }
  } else {
    for (r = 1, i = 0; i < n; ++i) {
      p[i] = KnuthLinearCongruentialGenerator(&r) >> 32 |
             KnuthLinearCongruentialGenerator(&r) >> 32 << 32;
    }
  }
  return p;
}

static long double ConvertRange(long double x, long double a, long double b,
                                long double c, long double d) {
  return (d - c) / (b - a) * (x - a) + c;
}

static double Compand(long x, double a, double b, double c, double d) {
  return ConvertRange(ConvertRange(x, LONG_MIN, LONG_MAX, a, b), a, b, c, d);
}

static void GenerateMatrixImpl(long I[yn_][xn_], double M[yn_][xn_], FILE *f) {
  long y, x;
  for (y = 0; y < yn_; ++y) {
    for (x = 0; x < xn_; ++x) {
      M[y][x] = Compand(I[y][x], r1_.a, r1_.b, r2_.a, r2_.b);
    }
    if (rounder_) {
      for (x = 0; x < xn_; ++x) {
        M[y][x] = rounder_(M[y][x]);
      }
    }
  }
  FormatMatrixDouble(yn_, xn_, M, fputs, f, formatter_, type_, name_, NULL,
                     digs_, round);
}

void GenerateMatrix(FILE *f) {
  GenerateMatrixImpl(SetRandom(yn_ * xn_, gc(calloc(yn_ * xn_, sizeof(long)))),
                     gc(calloc(yn_ * xn_, sizeof(double))), f);
}

int main(int argc, char *argv[]) {
  int i;
  FILE *f;
  showcrashreports();
  GetOpts(argc, argv);
  CHECK_NOTNULL((f = fopen(path_, "w")));
  if (optind < argc) FATALF("TODO(jart): support input files");
  GenerateMatrix(f);
  return fclose(f);
}
