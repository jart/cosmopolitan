/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2021 Justine Alexandra Roberts Tunney                              │
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
#include "libc/mem/gc.internal.h"
#include "libc/testlib/testlib.h"
#include "libc/x/x.h"

void SetUp(void) {
  /* 8087 FPU Control Word
      IM: Invalid Operation ───────────────┐
      DM: Denormal Operand ───────────────┐│
      ZM: Zero Divide ───────────────────┐││
      OM: Overflow ─────────────────────┐│││
      UM: Underflow ───────────────────┐││││
      PM: Precision ──────────────────┐│││││
     PC: Precision Control ────────┐  ││││││
      {float,∅,double,long double} │  ││││││
     RC: Rounding Control ───────┐ │  ││││││
      {even, →-∞, →+∞, →0}       │┌┤  ││││││
                                ┌┤││  ││││││
                               d││││rr││││││*/
  int x87cw = 0b0000000000000000001101100001;
  asm volatile("fldcw\t%0" : /* no outputs */ : "m"(x87cw));
}

TEST(atanl, testLongDouble) {
  EXPECT_STREQ("NAN", gc(xdtoal(atanl(NAN))));
  EXPECT_STREQ(".7853981583974483", gc(xdtoal(atanl(.99999999))));
}
