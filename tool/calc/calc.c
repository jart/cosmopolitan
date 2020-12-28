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
#include "libc/bits/bits.h"
#include "libc/bits/bswap.h"
#include "libc/bits/safemacros.h"
#include "libc/calls/calls.h"
#include "libc/errno.h"
#include "libc/fmt/conv.h"
#include "libc/log/log.h"
#include "libc/macros.h"
#include "libc/math.h"
#include "libc/mem/mem.h"
#include "libc/nexgen32e/bsf.h"
#include "libc/nexgen32e/bsr.h"
#include "libc/nexgen32e/ffs.h"
#include "libc/nexgen32e/x86feature.h"
#include "libc/rand/rand.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/o.h"
#include "libc/time/time.h"
#include "libc/x/x.h"
#include "o/tool/calc/calc.c.inc"
#include "o/tool/calc/calc.h.inc"
#include "third_party/gdtoa/gdtoa.h"
#include "tool/calc/calc.h"

/**
 * make -j8 o//tool/calc
 * rlwrap -A -H ~/.calc -f tool/calc/calc.lst -e\( o//tool/calc/calc.com
 * @see https://github.com/hanslub42/rlwrap
 */

static jmp_buf jb;
static int g_line;
static int g_column;
static const char *g_file;
static yyParser g_parser[1];

wontreturn static void Error(const char *msg) {
  fprintf(stderr, "%s:%d:%d: %s\n", g_file, g_line, g_column, msg);
  longjmp(jb, 1);
}

wontreturn static void SyntaxError(void) {
  Error("SYNTAX ERROR");
}

wontreturn static void LexError(void) {
  Error("LEX ERROR");
}

wontreturn static void MissingArgumentError(void) {
  Error("MISSING ARGUMENT");
}

wontreturn static void MissingFunctionError(void) {
  Error("MISSING FUNCTION");
}

wontreturn static void SyscallError(const char *name) {
  fprintf(stderr, "ERROR: %s[%s]: %d\n", name, g_file, errno);
  exit(1);
}

static void NumbersFree(struct Numbers *n) {
  if (n) {
    NumbersFree(n->n);
    free(n);
  }
}

static struct Numbers *NumbersAppend(struct Numbers *n, long double x) {
  struct Numbers *a;
  a = malloc(sizeof(struct Numbers));
  a->n = n;
  a->x = x;
  return a;
}

static long double ParseNumber(struct Token t) {
  char *ep;
  ep = t.s + t.n;
  if (t.s[0] == '0') {
    return strtoumax(t.s, &ep, 0);
  } else {
    return strtod(t.s, &ep);
  }
}

static long double FnRand(struct Numbers *a) {
  return rand();
}

static long double FnRand32(struct Numbers *a) {
  return rand32();
}

static long double FnRand64(struct Numbers *a) {
  return rand64();
}

static long double FnRdrand(struct Numbers *a) {
  if (X86_HAVE(RDRND)) {
    return rdrand();
  } else {
    return NAN;
  }
}

static long double FnRdseed(struct Numbers *a) {
  if (X86_HAVE(RDSEED)) {
    return rdseed();
  } else {
    return NAN;
  }
}

static long double FnAtan2(struct Numbers *a) {
  if (!a || !a->n) MissingArgumentError();
  return atan2l(a->n->x, a->x);
}

static long double FnLdexp(struct Numbers *a) {
  if (!a || !a->n) MissingArgumentError();
  return ldexpl(a->n->x, a->x);
}

static long double FnCopysign(struct Numbers *a) {
  if (!a || !a->n) MissingArgumentError();
  return copysignl(a->n->x, a->x);
}

static long double FnFmax(struct Numbers *a) {
  if (!a || !a->n) MissingArgumentError();
  return fmaxl(a->n->x, a->x);
}

static long double FnFmin(struct Numbers *a) {
  if (!a || !a->n) MissingArgumentError();
  return fminl(a->n->x, a->x);
}

static long double FnFmod(struct Numbers *a) {
  if (!a || !a->n) MissingArgumentError();
  return fmodl(a->n->x, a->x);
}

static long double FnHypot(struct Numbers *a) {
  if (!a || !a->n) MissingArgumentError();
  return hypotl(a->n->x, a->x);
}

static long double FnPowi(struct Numbers *a) {
  if (!a || !a->n) MissingArgumentError();
  return powil(a->n->x, a->x);
}

static long double FnPow(struct Numbers *a) {
  if (!a || !a->n) MissingArgumentError();
  return powl(a->n->x, a->x);
}

static long double FnScalb(struct Numbers *a) {
  if (!a || !a->n) MissingArgumentError();
  return scalbl(a->n->x, a->x);
}

static long double FnIsgreater(struct Numbers *a) {
  if (!a || !a->n) MissingArgumentError();
  return isgreater(a->n->x, a->x);
}

static long double FnRemainder(struct Numbers *a) {
  if (!a || !a->n) MissingArgumentError();
  return remainderl(a->n->x, a->x);
}

static long double FnIsgreaterequal(struct Numbers *a) {
  if (!a || !a->n) MissingArgumentError();
  return isgreaterequal(a->n->x, a->x);
}

static long double FnIsless(struct Numbers *a) {
  if (!a || !a->n) MissingArgumentError();
  return isless(a->n->x, a->x);
}

static long double FnIslessequal(struct Numbers *a) {
  if (!a || !a->n) MissingArgumentError();
  return islessequal(a->n->x, a->x);
}

static long double FnIslessgreater(struct Numbers *a) {
  if (!a || !a->n) MissingArgumentError();
  return islessgreater(a->n->x, a->x);
}

static long double FnIsunordered(struct Numbers *a) {
  if (!a || !a->n) MissingArgumentError();
  return isunordered(a->n->x, a->x);
}

static long double FnRounddown(struct Numbers *a) {
  if (!a || !a->n) MissingArgumentError();
  return ROUNDDOWN((int128_t)a->n->x, (int128_t)a->x);
}

static long double FnRoundup(struct Numbers *a) {
  if (!a || !a->n) MissingArgumentError();
  return ROUNDUP((int128_t)a->n->x, (int128_t)a->x);
}

static long double FnAcos(struct Numbers *a) {
  if (!a) MissingArgumentError();
  return acosl(a->x);
}

static long double FnAsin(struct Numbers *a) {
  if (!a) MissingArgumentError();
  return asinl(a->x);
}

static long double FnAtan(struct Numbers *a) {
  if (!a) MissingArgumentError();
  return atanl(a->x);
}

static long double FnCbrt(struct Numbers *a) {
  if (!a) MissingArgumentError();
  return cbrtl(a->x);
}

static long double FnCeil(struct Numbers *a) {
  if (!a) MissingArgumentError();
  return ceill(a->x);
}

static long double FnCos(struct Numbers *a) {
  if (!a) MissingArgumentError();
  return cosl(a->x);
}

static long double FnExp10(struct Numbers *a) {
  if (!a) MissingArgumentError();
  return exp10l(a->x);
}

static long double FnExp2(struct Numbers *a) {
  if (!a) MissingArgumentError();
  return exp2l(a->x);
}

static long double FnExp(struct Numbers *a) {
  if (!a) MissingArgumentError();
  return expl(a->x);
}

static long double FnExpm1(struct Numbers *a) {
  if (!a) MissingArgumentError();
  return expm1l(a->x);
}

static long double FnFabs(struct Numbers *a) {
  if (!a) MissingArgumentError();
  return fabsl(a->x);
}

static long double FnFloor(struct Numbers *a) {
  if (!a) MissingArgumentError();
  return floorl(a->x);
}

static long double FnIlogb(struct Numbers *a) {
  if (!a) MissingArgumentError();
  return ilogbl(a->x);
}

static long double FnLog10(struct Numbers *a) {
  if (!a) MissingArgumentError();
  return log10l(a->x);
}

static long double FnLog1p(struct Numbers *a) {
  if (!a) MissingArgumentError();
  return log1pl(a->x);
}

static long double FnLog2(struct Numbers *a) {
  if (!a) MissingArgumentError();
  return log2l(a->x);
}

static long double FnLogb(struct Numbers *a) {
  if (!a) MissingArgumentError();
  return logbl(a->x);
}

static long double FnLog(struct Numbers *a) {
  if (!a) MissingArgumentError();
  return logl(a->x);
}

static long double FnLrint(struct Numbers *a) {
  if (!a) MissingArgumentError();
  return lrintl(a->x);
}

static long double FnLround(struct Numbers *a) {
  if (!a) MissingArgumentError();
  return lroundl(a->x);
}

static long double FnNearbyint(struct Numbers *a) {
  if (!a) MissingArgumentError();
  return nearbyintl(a->x);
}

static long double FnRint(struct Numbers *a) {
  if (!a) MissingArgumentError();
  return rintl(a->x);
}

static long double FnRound(struct Numbers *a) {
  if (!a) MissingArgumentError();
  return roundl(a->x);
}

static long double FnSignificand(struct Numbers *a) {
  if (!a) MissingArgumentError();
  return significandl(a->x);
}

static long double FnSin(struct Numbers *a) {
  if (!a) MissingArgumentError();
  return sinl(a->x);
}

static long double FnSqrt(struct Numbers *a) {
  if (!a) MissingArgumentError();
  return sqrtl(a->x);
}

static long double FnTan(struct Numbers *a) {
  if (!a) MissingArgumentError();
  return tanl(a->x);
}

static long double FnTrunc(struct Numbers *a) {
  if (!a) MissingArgumentError();
  return truncl(a->x);
}

static long double FnIsinf(struct Numbers *a) {
  if (!a) MissingArgumentError();
  return isinf(a->x);
}

static long double FnIsnan(struct Numbers *a) {
  if (!a) MissingArgumentError();
  return isnan(a->x);
}

static long double FnIsfinite(struct Numbers *a) {
  if (!a) MissingArgumentError();
  return isfinite(a->x);
}

static long double FnIsnormal(struct Numbers *a) {
  if (!a) MissingArgumentError();
  return isnormal(a->x);
}

static long double FnSignbit(struct Numbers *a) {
  if (!a) MissingArgumentError();
  return signbit(a->x);
}

static long double FnFpclassify(struct Numbers *a) {
  if (!a) MissingArgumentError();
  return fpclassify(a->x);
}

static long double FnBswap16(struct Numbers *a) {
  if (!a) MissingArgumentError();
  return bswap_16((uint16_t)a->x);
}

static long double FnBswap32(struct Numbers *a) {
  if (!a) MissingArgumentError();
  return bswap_32((uint32_t)a->x);
}

static long double FnBswap64(struct Numbers *a) {
  if (!a) MissingArgumentError();
  return bswap_64((uint64_t)a->x);
}

static long double FnBsr(struct Numbers *a) {
  if (!a) MissingArgumentError();
  return bsr(a->x);
}

static long double FnBsrl(struct Numbers *a) {
  if (!a) MissingArgumentError();
  return bsrl(a->x);
}

static long double FnBsfl(struct Numbers *a) {
  if (!a) MissingArgumentError();
  return bsfl(a->x);
}

static long double FnFfs(struct Numbers *a) {
  if (!a) MissingArgumentError();
  return ffs(a->x);
}

static long double FnFfsl(struct Numbers *a) {
  if (!a) MissingArgumentError();
  return ffsl(a->x);
}

static long double FnGray(struct Numbers *a) {
  if (!a) MissingArgumentError();
  return gray(a->x);
}

static long double FnUngray(struct Numbers *a) {
  if (!a) MissingArgumentError();
  return ungray(a->x);
}

static long double FnRounddown2pow(struct Numbers *a) {
  if (!a) MissingArgumentError();
  return rounddown2pow(a->x);
}

static long double FnRoundup2pow(struct Numbers *a) {
  if (!a) MissingArgumentError();
  return roundup2pow(a->x);
}

static long double FnRoundup2log(struct Numbers *a) {
  if (!a) MissingArgumentError();
  return roundup2log(a->x);
}

static long double FnBitreverse8(struct Numbers *a) {
  if (!a) MissingArgumentError();
  return bitreverse8(a->x);
}

static long double FnBitreverse16(struct Numbers *a) {
  if (!a) MissingArgumentError();
  return bitreverse16(a->x);
}

static long double FnBitreverse32(struct Numbers *a) {
  if (!a) MissingArgumentError();
  return bitreverse32(a->x);
}

static long double FnBitreverse64(struct Numbers *a) {
  if (!a) MissingArgumentError();
  return bitreverse64(a->x);
}

static int8_t sarb(int8_t x, uint8_t y) {
  return x >> (y & 7);
}

static int16_t sarw(int16_t x, uint8_t y) {
  return x >> (y & 15);
}

static int32_t sarl(int32_t x, uint8_t y) {
  return x >> (y & 31);
}

static int64_t sarq(int64_t x, uint8_t y) {
  return x >> (y & 63);
}

static long double FnSarb(struct Numbers *a) {
  if (!a || !a->n) MissingArgumentError();
  return sarb(a->n->x, a->x);
}

static long double FnSarw(struct Numbers *a) {
  if (!a || !a->n) MissingArgumentError();
  return sarw(a->n->x, a->x);
}

static long double FnSarl(struct Numbers *a) {
  if (!a || !a->n) MissingArgumentError();
  return sarl(a->n->x, a->x);
}

static long double FnSarq(struct Numbers *a) {
  if (!a || !a->n) MissingArgumentError();
  return sarq(a->n->x, a->x);
}

static long double FnSar(struct Numbers *a) {
  if (!a || !a->n) MissingArgumentError();
  return sarq(a->n->x, a->x);
}

static uint8_t rorb(uint8_t x, uint8_t y) {
  return x >> (y & 7) | x << (8 - (y & 7));
}

static uint16_t rorw(uint16_t x, uint8_t y) {
  return x >> (y & 15) | x << (16 - (y & 15));
}

static uint32_t rorl(uint32_t x, uint8_t y) {
  return x >> (y & 31) | x << (32 - (y & 31));
}

static uint64_t rorq(uint64_t x, uint8_t y) {
  return x >> (y & 63) | x << (64 - (y & 63));
}

static long double FnRorb(struct Numbers *a) {
  if (!a || !a->n) MissingArgumentError();
  return rorb(a->n->x, a->x);
}

static long double FnRorw(struct Numbers *a) {
  if (!a || !a->n) MissingArgumentError();
  return rorw(a->n->x, a->x);
}

static long double FnRorl(struct Numbers *a) {
  if (!a || !a->n) MissingArgumentError();
  return rorl(a->n->x, a->x);
}

static long double FnRorq(struct Numbers *a) {
  if (!a || !a->n) MissingArgumentError();
  return rorq(a->n->x, a->x);
}

static long double FnRor(struct Numbers *a) {
  if (!a || !a->n) MissingArgumentError();
  return rorq(a->n->x, a->x);
}

static uint8_t rolb(uint8_t x, uint8_t y) {
  return x << (y & 7) | x >> (8 - (y & 7));
}

static uint16_t rolw(uint16_t x, uint8_t y) {
  return x << (y & 15) | x >> (16 - (y & 15));
}

static uint32_t roll(uint32_t x, uint8_t y) {
  return x << (y & 31) | x >> (32 - (y & 31));
}

static uint64_t rolq(uint64_t x, uint8_t y) {
  return x << (y & 63) | x >> (64 - (y & 63));
}

static long double FnRolb(struct Numbers *a) {
  if (!a || !a->n) MissingArgumentError();
  return rolb(a->n->x, a->x);
}

static long double FnRolw(struct Numbers *a) {
  if (!a || !a->n) MissingArgumentError();
  return rolw(a->n->x, a->x);
}

static long double FnRoll(struct Numbers *a) {
  if (!a || !a->n) MissingArgumentError();
  return roll(a->n->x, a->x);
}

static long double FnRolq(struct Numbers *a) {
  if (!a || !a->n) MissingArgumentError();
  return rolq(a->n->x, a->x);
}

static long double FnRol(struct Numbers *a) {
  if (!a || !a->n) MissingArgumentError();
  return rolq(a->n->x, a->x);
}

static long double FnTime(struct Numbers *a) {
  return nowl();
}

static long double FnBin(struct Numbers *a) {
  if (!a) MissingArgumentError();
  printf("0b%jb\n", (uint128_t)a->x);
  return 0;
}

static long double FnOct(struct Numbers *a) {
  if (!a) MissingArgumentError();
  printf("0%jo\n", (uint128_t)a->x);
  return 0;
}

static long double FnHex(struct Numbers *a) {
  if (!a) MissingArgumentError();
  printf("0x%jx\n", (uint128_t)a->x);
  return 0;
}

static void PrintNumber(long double x) {
  char buf[32];
  g_xfmt_p(buf, &x, 15, sizeof(buf), 0);
  fputs(buf, stdout);
}

static void Print(struct Numbers *a) {
  if (a) {
    Print(a->n);
    if (a->n) fputc(' ', stdout);
    PrintNumber(a->x);
  }
}

static long double FnPrint(struct Numbers *a) {
  Print(a);
  fputc('\n', stdout);
  return 0;
}

static const struct Fn {
  const char *s;
  long double (*f)(struct Numbers *);
} kFunctions[] = {
    {"abs", FnFabs},
    {"acos", FnAcos},
    {"asin", FnAsin},
    {"atan", FnAtan},
    {"atan2", FnAtan2},
    {"bin", FnBin},
    {"bitreverse16", FnBitreverse16},
    {"bitreverse32", FnBitreverse32},
    {"bitreverse64", FnBitreverse64},
    {"bitreverse8", FnBitreverse8},
    {"bsfl", FnBsfl},
    {"bsfl", FnBsfl},
    {"bsr", FnBsr},
    {"bsrl", FnBsrl},
    {"bswap16", FnBswap16},
    {"bswap32", FnBswap32},
    {"bswap64", FnBswap64},
    {"cbrt", FnCbrt},
    {"ceil", FnCeil},
    {"copysign", FnCopysign},
    {"cos", FnCos},
    {"exp", FnExp},
    {"exp10", FnExp10},
    {"exp2", FnExp2},
    {"expm1", FnExpm1},
    {"fabs", FnFabs},
    {"ffs", FnFfs},
    {"ffsl", FnFfsl},
    {"floor", FnFloor},
    {"fmax", FnFmax},
    {"fmin", FnFmin},
    {"fmod", FnFmod},
    {"fpclassify", FnFpclassify},
    {"gray", FnGray},
    {"hex", FnHex},
    {"hypot", FnHypot},
    {"ilogb", FnIlogb},
    {"isfinite", FnIsfinite},
    {"isgreater", FnIsgreater},
    {"isgreaterequal", FnIsgreaterequal},
    {"isinf", FnIsinf},
    {"isless", FnIsless},
    {"islessequal", FnIslessequal},
    {"islessgreater", FnIslessgreater},
    {"isnan", FnIsnan},
    {"isnormal", FnIsnormal},
    {"isunordered", FnIsunordered},
    {"ldexp", FnLdexp},
    {"ldexp", FnLdexp},
    {"log", FnLog},
    {"log10", FnLog10},
    {"log1p", FnLog1p},
    {"log2", FnLog2},
    {"logb", FnLogb},
    {"lrint", FnLrint},
    {"lround", FnLround},
    {"max", FnFmax},
    {"min", FnFmin},
    {"nearbyint", FnNearbyint},
    {"oct", FnOct},
    {"pow", FnPow},
    {"powi", FnPowi},
    {"print", FnPrint},
    {"rand", FnRand},
    {"rand32", FnRand32},
    {"rand64", FnRand64},
    {"rdrand", FnRdrand},
    {"rdseed", FnRdseed},
    {"remainder", FnRemainder},
    {"rint", FnRint},
    {"rol", FnRol},
    {"rolb", FnRolb},
    {"roll", FnRoll},
    {"rolq", FnRolq},
    {"rolw", FnRolw},
    {"ror", FnRor},
    {"rorb", FnRorb},
    {"rorl", FnRorl},
    {"rorq", FnRorq},
    {"rorw", FnRorw},
    {"round", FnRound},
    {"rounddown", FnRounddown},
    {"rounddown2pow", FnRounddown2pow},
    {"roundup", FnRoundup},
    {"roundup2log", FnRoundup2log},
    {"roundup2pow", FnRoundup2pow},
    {"sar", FnSar},
    {"sarb", FnSarb},
    {"sarl", FnSarl},
    {"sarq", FnSarq},
    {"sarw", FnSarw},
    {"scalb", FnScalb},
    {"signbit", FnSignbit},
    {"signbit", FnSignbit},
    {"significand", FnSignificand},
    {"sin", FnSin},
    {"sqrt", FnSqrt},
    {"tan", FnTan},
    {"time", FnTime},
    {"trunc", FnTrunc},
    {"ungray", FnUngray},
};

static long double CallFunction(struct Token fn, struct Numbers *args) {
  int l, r, m, p;
  l = 0;
  r = ARRAYLEN(kFunctions) - 1;
  while (l <= r) {
    m = (l + r) >> 1;
    p = strncmp(kFunctions[m].s, fn.s, fn.n);
    if (p < 0) {
      l = m + 1;
    } else if (p > 0) {
      r = m - 1;
    } else {
      return kFunctions[m].f(args);
    }
  }
  MissingFunctionError();
}

static void Tokenize(const char *s, size_t size) {
  size_t n;
  char *se;
  for (se = s + size; s < se; s += n, ++g_column) {
    n = 1;
    switch (*s & 0xff) {
      case ' ':
      case '\t':
      case '\v':
      case '\r':
        break;
      case '\n':
        ++g_line;
        g_column = 0;
        break;
      case 'A' ... 'Z':
      case 'a' ... 'z':
        n = strspn(s, "0123456789"
                      "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                      "abcdefghijklmnopqrstuvwxyz");
        Parse(g_parser, SYMBOL, (struct Token){s, n});
        break;
      case '0':
        n = strspn(s, "xXbB0123456789abcdefABCDEF");
        Parse(g_parser, NUMBER, (struct Token){s, n});
        n += strspn(s + n, "LUlu");
        break;
      case '1' ... '9':
        n = strspn(s, "0123456789.");
        if (s[n] == 'e' || s[n] == 'E') {
          ++n;
          if (s[n] == '+' || s[n] == '-') ++n;
          n += strspn(s + n, "0123456789");
        }
        Parse(g_parser, NUMBER, (struct Token){s, n});
        n += strspn(s + n, "LUlu");
        break;
      case '(':
        Parse(g_parser, LP, (struct Token){0, 0});
        break;
      case ')':
        Parse(g_parser, RP, (struct Token){0, 0});
        break;
      case ',':
        Parse(g_parser, COMMA, (struct Token){0, 0});
        break;
      case '^':
        Parse(g_parser, XOR, (struct Token){0, 0});
        break;
      case '%':
        Parse(g_parser, REM, (struct Token){0, 0});
        break;
      case '+':
        Parse(g_parser, PLUS, (struct Token){0, 0});
        break;
      case '-':
        Parse(g_parser, MINUS, (struct Token){0, 0});
        break;
      case '~':
        Parse(g_parser, NOT, (struct Token){0, 0});
        break;
      case '/':
        if (s[1] == '/') {
          Parse(g_parser, DDIV, (struct Token){0, 0});
          ++n;
        } else {
          Parse(g_parser, DIV, (struct Token){0, 0});
        }
        break;
      case '*':
        if (s[1] == '*') {
          Parse(g_parser, EXP, (struct Token){0, 0});
          ++n;
        } else {
          Parse(g_parser, MUL, (struct Token){0, 0});
        }
        break;
      case '|':
        if (s[1] == '|') {
          Parse(g_parser, LOR, (struct Token){0, 0});
          ++n;
        } else {
          Parse(g_parser, OR, (struct Token){0, 0});
        }
        break;
      case '&':
        if (s[1] == '&') {
          Parse(g_parser, LAND, (struct Token){0, 0});
          ++n;
        } else {
          Parse(g_parser, AND, (struct Token){0, 0});
        }
        break;
      case '!':
        if (s[1] == '=') {
          Parse(g_parser, NE, (struct Token){0, 0});
          ++n;
        } else {
          Parse(g_parser, LNOT, (struct Token){0, 0});
        }
        break;
      case '=':
        if (s[1] == '=') {
          Parse(g_parser, EQ, (struct Token){0, 0});
          ++n;
        } else {
          LexError();
        }
        break;
      case '>':
        if (s[1] == '=') {
          Parse(g_parser, GE, (struct Token){0, 0});
          ++n;
        } else if (s[1] == '>') {
          Parse(g_parser, SHR, (struct Token){0, 0});
          ++n;
        } else {
          Parse(g_parser, GT, (struct Token){0, 0});
        }
        break;
      case '<':
        if (s[1] == '=') {
          Parse(g_parser, LE, (struct Token){0, 0});
          ++n;
        } else if (s[1] == '<') {
          Parse(g_parser, SHL, (struct Token){0, 0});
          ++n;
        } else {
          Parse(g_parser, LT, (struct Token){0, 0});
        }
        break;
      default:
        LexError();
    }
  }
}

int main(int argc, char *argv[]) {
  int i;
  int ec;
  int fd;
  size_t n;
  char *buf;
  ssize_t rc;
  size_t bufcap;
  if (!(ec = setjmp(jb))) {
    if (argc > 1) {
      ParseInit(g_parser);
      bufcap = BIGPAGESIZE;
      buf = malloc(bufcap);
      for (i = 1; i < argc; ++i) {
        g_file = argv[i];
        g_line = 0;
        g_column = 0;
        n = 0; /* wut */
        if ((fd = open(g_file, O_RDONLY)) == -1) SyscallError("open");
        for (;;) {
          if ((rc = read(fd, buf, bufcap)) == -1) SyscallError("read");
          if (!(n = rc)) break;
          Tokenize(buf, n);
        }
        close(fd);
        Parse(g_parser, 0, (struct Token){0, 0});
      }
      ParseFinalize(g_parser);
    } else {
      g_file = "/dev/stdin";
      g_line = 0;
      g_column = 0;
      buf = NULL;
      bufcap = 0;
      while (getline(&buf, &bufcap, stdin) != -1) {
        if ((n = strlen(buf))) {
          ParseInit(g_parser);
          if (!setjmp(jb)) {
            Tokenize("print(", 6);
            Tokenize(buf, n);
            Tokenize(")", 1);
            Parse(g_parser, 0, (struct Token){0, 0});
          }
          ParseFinalize(g_parser);
        }
      }
    }
  }
  free(buf);
  return ec;
}
