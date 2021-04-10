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
#include "libc/runtime/fenv.h"

/**
 * Sets rounding mode.
 *
 * This configures the x87 FPU as well as SSE.
 *
 * @param mode may be FE_TONEAREST, FE_DOWNWARD, FE_UPWARD, or FE_TOWARDZERO
 * @return 0 on success, or -1 on error
 */
int fesetround(int mode) {
  uint16_t x87cw;
  uint32_t ssecw;
  switch (mode) {
    case FE_TONEAREST:
    case FE_DOWNWARD:
    case FE_UPWARD:
    case FE_TOWARDZERO:
      asm("fnstcw\t%0" : "=m"(x87cw));
      x87cw &= ~0x0c00;
      x87cw |= mode;
      asm volatile("fldcw\t%0" : /* no outputs */ : "m"(x87cw));
      asm("stmxcsr\t%0" : "=m"(ssecw));
      ssecw &= ~(0x0c00 << 3);
      ssecw |= (mode << 3);
      asm volatile("ldmxcsr\t%0" : /* no outputs */ : "m"(ssecw));
      return 0;
    default:
      return -1;
  }
}
