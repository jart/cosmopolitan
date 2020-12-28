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
#include "libc/math.h"
#include "libc/runtime/gc.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/x/x.h"
#include "third_party/gdtoa/gdtoa.h"

const char kConfig[] = "\
/* FPU Control Word (x87) Exception Masks\n\
   @see Intel Manual V1 §8.1.5\n\
       IM: Invalid Operation ───────────────┐\n\
       DM: Denormal Operand ───────────────┐│\n\
       ZM: Zero Divide ───────────────────┐││\n\
       OM: Overflow ─────────────────────┐│││\n\
       UM: Underflow ───────────────────┐││││\n\
       PM: Precision ──────────────────┐│││││\n\
      PC: Precision Control ────────┐  ││││││\n\
       {float,∅,double,long double} │  ││││││\n\
      RC: Rounding Control ───────┐ │  ││││││\n\
       {even, →-∞, →+∞, →0}       │┌┤  ││││││\n\
                                 ┌┤││  ││││││\n\
                                d││││rr││││││*/\n\
                          ";

const char kStatus[] = "\
/* FPU Status Word (x87)\n\
   @see Intel Manual V1 §8.1.3\n\
   IE: Invalid Operation ────────────────┐\n\
   DE: Denormalized Operand ────────────┐│\n\
   ZE: Zero Divide ────────────────────┐││\n\
   OE: Overflow Flag ─────────────────┐│││\n\
   UE: Underflow Flag ───────────────┐││││\n\
   PE: Precision Flag ──────────────┐│││││\n\
   SF: Stack Fault ────────────────┐││││││\n\
   ES: Exception Summary Status ──┐│││││││\n\
   C0-3: Condition Codes ──┬────┐ ││││││││\n\
   TOP of Stack Pointer ─────┐  │ ││││││││\n\
   B: FPU Busy ───────────┐│ │  │ ││││││││\n\
                          ││┌┴┐┌┼┐││││││││\n\
                          │↓│ │↓↓↓││││││││*/\n\
                          ";

const char kRegister[] = "\
/\t┌sign\n\
/\t│ ┌exponent\n\
/\t│ │             ┌intpart\n\
/\t│ │             │ ┌fraction\n\
/\t│ │             │ │\n\
/\t│┌┴────────────┐│┌┴────────────────────────────────────────────────────────────┐\n";

void PrintRegister(long double x) {
  char buf[32];
  uint16_t lo, hi;
  memcpy(buf, &x, sizeof(x));
  memcpy(&lo, &buf[0], sizeof(lo));
  memcpy(&hi, &buf[8], sizeof(hi));
  g_xfmt_p(buf, &x, 15, 32, 0);
  printf("/\t%016lb%064lb %s\n", hi, lo, buf);
}

int main(int argc, char *argv[]) {
  double d;
  unsigned short sw;
  long double st0, st1;

#define ST0 1
#define ST1 2
  d = 0;
  st0 = ST0;
  st1 = ST1;
  asm("fstl\t%1\n\t"
      "fstsw"
      : "=a"(sw), "=m"(d) /*, "+t"(st0), "+u"(st1) */);
  printf("\n%s%016b\n\n%s", kStatus, sw, kRegister);
  PrintRegister(st0);
  PrintRegister(st1);
  printf("/\t%-80s %s\n", "d", gc(xdtoa(d)));

#if 0
#define ST0 2e18L
#define ST1 0.0000000000000123L
  st0 = ST0;
  st1 = ST1;
  asm("fldpi\n\t"
      "fadd\t%%st\n\t"
      "fxch\n2:\t"
      "fprem1\n\t"
      "fnstsw\n\t"
      "test\t$4,%%ah\n\t"
      "jnz\t2b\n\t"
      "fstp\t%%st(1)\n\t"
      "fsin\n\t"
      "fstsw"
      : "=a"(sw), "+t"(st0), "+u"(st1));
  printf("\n%s%016b\n\n%s", kStatus, sw, kRegister);
  PrintRegister(st0);
  PrintRegister(st1);
#endif

#if 0
  st0 = ST0;
  st1 = ST1;
  asm("fsin\n\t"
      "fstsw"
      : "=a"(sw), "+t"(st0), "+u"(st1));
  printf("\n%s%016b\n\n%s", kStatus, sw, kRegister);
  PrintRegister(st0);
  PrintRegister(st1);
#endif

#if 0
  printf("\n");
  PrintRegister(sinl(remainderl(2e40L, M_PI * 2)));
#endif

  return 0;
}
