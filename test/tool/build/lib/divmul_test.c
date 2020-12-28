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
#include "libc/bits/progn.internal.h"
#include "libc/calls/struct/sigaction.h"
#include "libc/log/check.h"
#include "libc/runtime/gc.h"
#include "libc/sysv/consts/sa.h"
#include "libc/sysv/consts/sig.h"
#include "libc/testlib/ezbench.h"
#include "libc/testlib/testlib.h"
#include "libc/x/x.h"
#include "tool/build/lib/divmul.h"
#include "tool/build/lib/endian.h"
#include "tool/build/lib/flags.h"

#define CX     1
#define OSZ    00000000040
#define REXW   00000000100
#define RM(x)  (0000001600 & ((x) << 007))
#define MOD(x) (0060000000 & ((x) << 026))

jmp_buf sigfpejmp;
struct Machine m[1];
struct sigaction oldsigfpe[1];
struct XedDecodedInst xedd[1];

void OnSigFpe(void) {
  /* ProTip: gdb -ex 'handle SIGFPE nostop noprint pass' */
  longjmp(sigfpejmp, 1);
}

void SetUp(void) {
  m->xedd = xedd;
  CHECK_NE(-1, xsigaction(SIGFPE, OnSigFpe, SA_NODEFER, 0, oldsigfpe));
}

void TearDown(void) {
  m->xedd = xedd;
  CHECK_NE(-1, sigaction(SIGFPE, oldsigfpe, NULL));
}

TEST(imul8, test) {
  static const uint8_t A[] = {0x00, 0x01, 0x80, 0x7F, 0x81, 0x7E, 0xFF, 0xBF};
  int i, j;
  int16_t ax;
  bool cf, of;
  for (i = 0; i < ARRAYLEN(A); ++i) {
    for (j = 0; j < ARRAYLEN(A); ++j) {
      Write8(m->ax, A[i]);
      Write8(m->cx, A[j]);
      OpMulAxAlEbSigned(m, MOD(3) | RM(CX));
      asm volatile("imulb\t%3"
                   : "=a"(ax), "=@ccc"(cf), "=@cco"(of)
                   : "q"(A[j]), "0"(A[i])
                   : "cc");
      EXPECT_EQ(ax, (int16_t)Read16(m->ax));
      EXPECT_EQ(cf, GetFlag(m->flags, FLAGS_CF));
      EXPECT_EQ(of, GetFlag(m->flags, FLAGS_OF));
    }
  }
}

TEST(imul16, test) {
  static const uint16_t A[] = {0x0000, 0x0001, 0x8000, 0x7FFF, 0x8001, 0x7FFE,
                               0xFFFF, 0xBeef, 0x00b5, 0x00b6, 0xb504, 0xb505};
  int i, j;
  bool cf, of;
  uint16_t dx, ax;
  for (i = 0; i < ARRAYLEN(A); ++i) {
    for (j = 0; j < ARRAYLEN(A); ++j) {
      Write16(m->ax, A[i]);
      Write16(m->cx, A[j]);
      OpMulRdxRaxEvqpSigned(m, OSZ | MOD(3) | RM(CX));
      asm("imulw\t%4"
          : "=d"(dx), "=a"(ax), "=@ccc"(cf), "=@cco"(of)
          : "r"(A[j]), "1"(A[i])
          : "cc");
      EXPECT_EQ((int32_t)((uint32_t)dx << 16 | ax),
                (int32_t)((uint32_t)Read16(m->dx) << 16 | Read16(m->ax)));
      EXPECT_EQ(cf, GetFlag(m->flags, FLAGS_CF));
      EXPECT_EQ(of, GetFlag(m->flags, FLAGS_OF));
    }
  }
}

TEST(imul32, test) {
  static const uint32_t A[] = {0x00000000, 0x00000001, 0x80000000, 0x7FFFFFFF,
                               0x80000001, 0x7FFFFFFE, 0xFFFFFFFF, 0xDeadBeef,
                               0x000000b6, 0x0000b504, 0x0000b505, 0xb504f334};
  int i, j;
  bool cf, of;
  uint32_t dx, ax;
  for (i = 0; i < ARRAYLEN(A); ++i) {
    for (j = 0; j < ARRAYLEN(A); ++j) {
      Write32(m->ax, A[i]);
      Write32(m->cx, A[j]);
      OpMulRdxRaxEvqpSigned(m, MOD(3) | RM(CX));
      asm("imull\t%4"
          : "=d"(dx), "=a"(ax), "=@ccc"(cf), "=@cco"(of)
          : "r"(A[j]), "1"(A[i])
          : "cc");
      EXPECT_EQ((int64_t)((uint64_t)dx << 32 | ax),
                (int64_t)((uint64_t)Read32(m->dx) << 32 | Read32(m->ax)));
      EXPECT_EQ(cf, GetFlag(m->flags, FLAGS_CF));
      EXPECT_EQ(of, GetFlag(m->flags, FLAGS_OF));
    }
  }
}

TEST(imul64, test) {
  static const uint64_t A[] = {0x00000000, 0x00000001, 0x80000000, 0x7FFFFFFF,
                               0x80000001, 0x7FFFFFFE, 0xFFFFFFFF, 0xDeadBeef,
                               0x000000b6, 0x0000b504, 0x0000b505, 0xb504f334};
  int i, j;
  bool cf, of;
  uint64_t dx, ax;
  for (i = 0; i < ARRAYLEN(A); ++i) {
    for (j = 0; j < ARRAYLEN(A); ++j) {
      Write64(m->ax, A[i]);
      Write64(m->cx, A[j]);
      OpMulRdxRaxEvqpSigned(m, REXW | MOD(3) | RM(CX));
      asm("imulq\t%4"
          : "=d"(dx), "=a"(ax), "=@ccc"(cf), "=@cco"(of)
          : "r"(A[j]), "1"(A[i])
          : "cc");
      EXPECT_EQ((int128_t)((uint128_t)dx << 64 | ax),
                (int128_t)((uint128_t)Read64(m->dx) << 64 | Read64(m->ax)));
      EXPECT_EQ(cf, GetFlag(m->flags, FLAGS_CF));
      EXPECT_EQ(of, GetFlag(m->flags, FLAGS_OF));
    }
  }
}

TEST(mul8, test) {
  static const uint8_t A[] = {0x00, 0x01, 0x80, 0x7F, 0x81, 0x7E, 0xFF, 0xb6};
  int i, j;
  uint16_t ax;
  bool cf, of;
  for (i = 0; i < ARRAYLEN(A); ++i) {
    for (j = 0; j < ARRAYLEN(A); ++j) {
      Write8(m->ax, A[i]);
      Write8(m->cx, A[j]);
      OpMulAxAlEbUnsigned(m, MOD(3) | RM(CX));
      asm volatile("mulb\t%3"
                   : "=a"(ax), "=@ccc"(cf), "=@cco"(of)
                   : "q"(A[j]), "0"(A[i])
                   : "cc");
      EXPECT_EQ(ax, Read16(m->ax));
      EXPECT_EQ(cf, GetFlag(m->flags, FLAGS_CF));
      EXPECT_EQ(of, GetFlag(m->flags, FLAGS_OF));
    }
  }
}

TEST(mul16, test) {
  static const uint16_t A[] = {0x0000, 0x0001, 0x8000, 0x7FFF,
                               0x8001, 0x7FFE, 0xFFFF, 0x00b6};
  int i, j;
  bool cf, of;
  uint16_t dx, ax;
  for (i = 0; i < ARRAYLEN(A); ++i) {
    for (j = 0; j < ARRAYLEN(A); ++j) {
      Write16(m->ax, A[i]);
      Write16(m->cx, A[j]);
      OpMulRdxRaxEvqpUnsigned(m, OSZ | MOD(3) | RM(CX));
      asm("mulw\t%4"
          : "=d"(dx), "=a"(ax), "=@ccc"(cf), "=@cco"(of)
          : "r"(A[j]), "1"(A[i])
          : "cc");
      EXPECT_EQ((uint32_t)((uint32_t)dx << 16 | ax),
                (uint32_t)((uint32_t)Read16(m->dx) << 16 | Read16(m->ax)));
      EXPECT_EQ(cf, GetFlag(m->flags, FLAGS_CF));
      EXPECT_EQ(of, GetFlag(m->flags, FLAGS_OF));
    }
  }
}

TEST(mul32, test) {
  static const uint32_t A[] = {0x00000000, 0x00000001, 0x80000000, 0x7FFFFFFF,
                               0x80000001, 0x7FFFFFFE, 0xFFFFFFFF, 0x000000b5,
                               0x000000b6, 0x0000b504, 0x0000b505, 0xb504f334};
  int i, j;
  bool cf, of;
  uint32_t dx, ax;
  for (i = 0; i < ARRAYLEN(A); ++i) {
    for (j = 0; j < ARRAYLEN(A); ++j) {
      Write32(m->ax, A[i]);
      Write32(m->cx, A[j]);
      OpMulRdxRaxEvqpUnsigned(m, MOD(3) | RM(CX));
      asm("mull\t%4"
          : "=d"(dx), "=a"(ax), "=@ccc"(cf), "=@cco"(of)
          : "r"(A[j]), "1"(A[i])
          : "cc");
      EXPECT_EQ((uint64_t)((uint64_t)dx << 32 | ax),
                (uint64_t)((uint64_t)Read32(m->dx) << 32 | Read32(m->ax)));
      EXPECT_EQ(cf, GetFlag(m->flags, FLAGS_CF));
      EXPECT_EQ(of, GetFlag(m->flags, FLAGS_OF));
    }
  }
}

TEST(mul64, test) {
  static const uint64_t A[] = {0x00000000, 0x00000001, 0x80000000, 0x7FFFFFFF,
                               0x80000001, 0x7FFFFFFE, 0xFFFFFFFF, 0x000000b6,
                               0x0000b504, 0x0000b505, 0xb504f333, 0xb504f334};
  int i, j;
  bool cf, of;
  uint64_t dx, ax;
  for (i = 0; i < ARRAYLEN(A); ++i) {
    for (j = 0; j < ARRAYLEN(A); ++j) {
      Write64(m->ax, A[i]);
      Write64(m->cx, A[j]);
      OpMulRdxRaxEvqpUnsigned(m, REXW | MOD(3) | RM(CX));
      asm("mulq\t%4"
          : "=d"(dx), "=a"(ax), "=@ccc"(cf), "=@cco"(of)
          : "r"(A[j]), "1"(A[i])
          : "cc");
      EXPECT_EQ((uint128_t)((uint128_t)dx << 64 | ax),
                (uint128_t)((uint128_t)Read64(m->dx) << 64 | Read64(m->ax)));
      EXPECT_EQ(cf, GetFlag(m->flags, FLAGS_CF));
      EXPECT_EQ(of, GetFlag(m->flags, FLAGS_OF));
    }
  }
}

TEST(idiv8, test) {
  static const uint8_t A[] = {0x00, 0x01, 0x80, 0x7F, 0x81, 0x7E, 0xFF, 0xBF};
  uint16_t remquo;
  bool gotthrow, gotsigfpe;
  int8_t i, j, k, w, x, a, b;
  int8_t quotient, remainder;
  for (i = 0; i < ARRAYLEN(A); ++i) {
    for (j = 0; j < ARRAYLEN(A); ++j) {
      for (k = 0; k < ARRAYLEN(A); ++k) {
        m->ax[1] = A[i];
        m->ax[0] = A[j];
        m->cx[0] = A[k];
        gotthrow = false;
        gotsigfpe = false;
        if (!setjmp(m->onhalt)) {
          OpDivAlAhAxEbSigned(m, MOD(3) | RM(CX));
        } else {
          gotthrow = true;
        }
        if (!setjmp(sigfpejmp)) {
          asm("idivb\t%1"
              : "=a"(remquo)
              : "q"(A[k]), "0"((int16_t)(A[i] << 8 | A[j]))
              : "cc");
        } else {
          gotsigfpe = true;
        }
        EXPECT_EQ(gotsigfpe, gotthrow);
        if (!gotsigfpe && !gotthrow) {
          quotient = (int8_t)remquo;
          remainder = (int8_t)(remquo >> 8);
          EXPECT_EQ(quotient, (int8_t)m->ax[0]);
          EXPECT_EQ(remainder, (int8_t)m->ax[1]);
        }
      }
    }
  }
}

TEST(idiv16, test) {
  static const uint16_t A[] = {0x0000, 0x0001, 0x8000, 0x7FFF,
                               0x8001, 0x7FFE, 0xFFFF, 0xBeef};
  bool gotthrow, gotsigfpe;
  int16_t i, j, k, w, x, a, b;
  int16_t quotient, remainder;
  for (i = 0; i < ARRAYLEN(A); ++i) {
    for (j = 0; j < ARRAYLEN(A); ++j) {
      for (k = 0; k < ARRAYLEN(A); ++k) {
        memcpy(m->dx, &A[i], 2);
        memcpy(m->ax, &A[j], 2);
        memcpy(m->cx, &A[k], 2);
        if (!setjmp(m->onhalt)) {
          gotthrow = false;
          OpDivRdxRaxEvqpSigned(m, OSZ | MOD(3) | RM(CX));
        } else {
          gotthrow = true;
        }
        if (!setjmp(sigfpejmp)) {
          gotsigfpe = false;
          asm("idivw\t%2"
              : "=d"(remainder), "=a"(quotient)
              : "r"(A[k]), "0"(A[i]), "1"(A[j])
              : "cc");
        } else {
          gotsigfpe = true;
        }
        EXPECT_EQ(gotsigfpe, gotthrow);
        if (!gotsigfpe && !gotthrow) {
          EXPECT_EQ(quotient, (int16_t)Read16(m->ax));
          EXPECT_EQ(remainder, (int16_t)Read16(m->dx));
        }
      }
    }
  }
}

TEST(idiv32, test) {
  static const uint32_t A[] = {0x00000000, 0x00000001, 0x80000000, 0x7FFFFFFF,
                               0x80000001, 0x7FFFFFFE, 0xFFFFFFFF, 0xDeadBeef};
  bool gotthrow, gotsigfpe;
  int32_t i, j, k, w, x, a, b;
  int32_t quotient, remainder;
  for (i = 0; i < ARRAYLEN(A); ++i) {
    for (j = 0; j < ARRAYLEN(A); ++j) {
      for (k = 0; k < ARRAYLEN(A); ++k) {
        memcpy(m->dx, &A[i], 4);
        memcpy(m->ax, &A[j], 4);
        memcpy(m->cx, &A[k], 4);
        if (!setjmp(m->onhalt)) {
          gotthrow = false;
          OpDivRdxRaxEvqpSigned(m, MOD(3) | RM(CX));
        } else {
          gotthrow = true;
        }
        if (!setjmp(sigfpejmp)) {
          gotsigfpe = false;
          asm("idivl\t%2"
              : "=d"(remainder), "=a"(quotient)
              : "r"(A[k]), "0"(A[i]), "1"(A[j])
              : "cc");
        } else {
          gotsigfpe = true;
        }
        EXPECT_EQ(gotsigfpe, gotthrow);
        if (!gotsigfpe && !gotthrow) {
          EXPECT_EQ(quotient, (int32_t)Read32(m->ax));
          EXPECT_EQ(remainder, (int32_t)Read32(m->dx));
        }
      }
    }
  }
}

TEST(idiv64, test) {
  static const uint64_t A[] = {0x0000000000000000, 0x0000000000000001,
                               0x8000000000000000, 0x7FFFFFFFFFFFFFFF,
                               0x8000000000000001, 0x7FFFFFFFFFFFFFFE,
                               0xFFFFFFFFFFFFFFFF, 0x00DeadBeefCafe00};
  bool gotthrow, gotsigfpe;
  int64_t i, j, k, w, x, a, b;
  int64_t quotient, remainder;
  for (i = 0; i < ARRAYLEN(A); ++i) {
    for (j = 0; j < ARRAYLEN(A); ++j) {
      for (k = 0; k < ARRAYLEN(A); ++k) {
        memcpy(m->dx, &A[i], 8);
        memcpy(m->ax, &A[j], 8);
        memcpy(m->cx, &A[k], 8);
        if (!setjmp(m->onhalt)) {
          gotthrow = false;
          OpDivRdxRaxEvqpSigned(m, REXW | MOD(3) | RM(CX));
        } else {
          gotthrow = true;
        }
        if (!setjmp(sigfpejmp)) {
          gotsigfpe = false;
          asm("idivq\t%2"
              : "=d"(remainder), "=a"(quotient)
              : "r"(A[k]), "0"(A[i]), "1"(A[j])
              : "cc");
        } else {
          gotsigfpe = true;
        }
        EXPECT_EQ(gotsigfpe, gotthrow);
        if (!gotsigfpe && !gotthrow) {
          EXPECT_EQ(quotient, (int64_t)Read64(m->ax));
          EXPECT_EQ(remainder, (int64_t)Read64(m->dx));
        }
      }
    }
  }
}

TEST(div, test) {
  static const uint8_t A[] = {0x00, 0x01, 0x80, 0x7F, 0x81, 0x7E, 0xFF, 0xBF};
  uint16_t remquo;
  bool gotthrow, gotsigfpe;
  uint8_t i, j, k, w, x, a, b;
  uint8_t quotient, remainder;
  for (i = 0; i < ARRAYLEN(A); ++i) {
    for (j = 0; j < ARRAYLEN(A); ++j) {
      for (k = 0; k < ARRAYLEN(A); ++k) {
        m->ax[1] = A[i];
        m->ax[0] = A[j];
        m->cx[0] = A[k];
        gotthrow = false;
        gotsigfpe = false;
        if (!setjmp(m->onhalt)) {
          OpDivAlAhAxEbUnsigned(m, MOD(3) | RM(CX));
        } else {
          gotthrow = true;
        }
        if (!setjmp(sigfpejmp)) {
          asm("divb\t%1"
              : "=a"(remquo)
              : "q"(A[k]), "0"((uint16_t)(A[i] << 8 | A[j]))
              : "cc");
        } else {
          gotsigfpe = true;
        }
        EXPECT_EQ(gotsigfpe, gotthrow);
        if (!gotsigfpe && !gotthrow) {
          quotient = (uint8_t)remquo;
          remainder = (uint8_t)(remquo >> 8);
          EXPECT_EQ(quotient, (uint8_t)m->ax[0]);
          EXPECT_EQ(remainder, (uint8_t)m->ax[1]);
        }
      }
    }
  }
}

TEST(div16, test) {
  static const uint16_t A[] = {0x0000, 0x0001, 0x8000, 0x7FFF,
                               0x8001, 0x7FFE, 0xFFFF, 0xBeef};
  bool gotthrow, gotsigfpe;
  uint16_t i, j, k, w, x, a, b;
  uint16_t quotient, remainder;
  for (i = 0; i < ARRAYLEN(A); ++i) {
    for (j = 0; j < ARRAYLEN(A); ++j) {
      for (k = 0; k < ARRAYLEN(A); ++k) {
        memcpy(m->dx, &A[i], 2);
        memcpy(m->ax, &A[j], 2);
        memcpy(m->cx, &A[k], 2);
        if (!setjmp(m->onhalt)) {
          gotthrow = false;
          OpDivRdxRaxEvqpUnsigned(m, OSZ | MOD(3) | RM(CX));
        } else {
          gotthrow = true;
        }
        if (!setjmp(sigfpejmp)) {
          gotsigfpe = false;
          asm("divw\t%2"
              : "=d"(remainder), "=a"(quotient)
              : "r"(A[k]), "0"(A[i]), "1"(A[j])
              : "cc");
        } else {
          gotsigfpe = true;
        }
        EXPECT_EQ(gotsigfpe, gotthrow);
        if (!gotsigfpe && !gotthrow) {
          EXPECT_EQ(quotient, (uint16_t)Read16(m->ax));
          EXPECT_EQ(remainder, (uint16_t)Read16(m->dx));
        }
      }
    }
  }
}

TEST(div32, test) {
  static const uint32_t A[] = {0x00000000, 0x00000001, 0x80000000, 0x7FFFFFFF,
                               0x80000001, 0x7FFFFFFE, 0xFFFFFFFF, 0xDeadBeef};
  bool gotthrow, gotsigfpe;
  uint32_t i, j, k, w, x, a, b;
  uint32_t quotient, remainder;
  for (i = 0; i < ARRAYLEN(A); ++i) {
    for (j = 0; j < ARRAYLEN(A); ++j) {
      for (k = 0; k < ARRAYLEN(A); ++k) {
        memcpy(m->dx, &A[i], 4);
        memcpy(m->ax, &A[j], 4);
        memcpy(m->cx, &A[k], 4);
        if (!setjmp(m->onhalt)) {
          gotthrow = false;
          OpDivRdxRaxEvqpUnsigned(m, MOD(3) | RM(CX));
        } else {
          gotthrow = true;
        }
        if (!setjmp(sigfpejmp)) {
          gotsigfpe = false;
          asm("divl\t%2"
              : "=d"(remainder), "=a"(quotient)
              : "r"(A[k]), "0"(A[i]), "1"(A[j])
              : "cc");
        } else {
          gotsigfpe = true;
        }
        EXPECT_EQ(gotsigfpe, gotthrow);
        if (!gotsigfpe && !gotthrow) {
          EXPECT_EQ(quotient, (uint32_t)Read32(m->ax));
          EXPECT_EQ(remainder, (uint32_t)Read32(m->dx));
        }
      }
    }
  }
}

TEST(div64, test) {
  static const uint64_t A[] = {0x0000000000000000, 0x0000000000000001,
                               0x8000000000000000, 0x7FFFFFFFFFFFFFFF,
                               0x8000000000000001, 0x7FFFFFFFFFFFFFFE,
                               0xFFFFFFFFFFFFFFFF, 0x00DeadBeefCafe00};
  bool gotthrow, gotsigfpe;
  uint64_t i, j, k, w, x, a, b;
  uint64_t quotient, remainder;
  for (i = 0; i < ARRAYLEN(A); ++i) {
    for (j = 0; j < ARRAYLEN(A); ++j) {
      for (k = 0; k < ARRAYLEN(A); ++k) {
        memcpy(m->dx, &A[i], 8);
        memcpy(m->ax, &A[j], 8);
        memcpy(m->cx, &A[k], 8);
        if (!setjmp(m->onhalt)) {
          gotthrow = false;
          OpDivRdxRaxEvqpUnsigned(m, REXW | MOD(3) | RM(CX));
        } else {
          gotthrow = true;
        }
        if (!setjmp(sigfpejmp)) {
          gotsigfpe = false;
          asm("divq\t%2"
              : "=d"(remainder), "=a"(quotient)
              : "r"(A[k]), "0"(A[i]), "1"(A[j])
              : "cc");
        } else {
          gotsigfpe = true;
        }
        EXPECT_EQ(gotsigfpe, gotthrow);
        if (!gotsigfpe && !gotthrow) {
          EXPECT_EQ(quotient, (uint64_t)Read64(m->ax));
          EXPECT_EQ(remainder, (uint64_t)Read64(m->dx));
        }
      }
    }
  }
}

BENCH(imul, bench) {
  volatile register int8_t x8, y8;
  volatile register int16_t x16, y16;
  volatile register int32_t x32, y32;
  volatile register int64_t x64, y64;
  EZBENCH2("imul8", PROGN(x8 = 7, y8 = 18), y8 *= x8);
  EZBENCH2("imul16", PROGN(x16 = 123, y16 = 116), y16 *= x16);
  EZBENCH2("imul32", PROGN(x32 = 0x238943, y32 = 0x238), y32 *= x32);
  EZBENCH2("imul64", PROGN(x64 = 0x23894329838932, y64 = 0x238), y64 *= x64);
}

BENCH(idiv, bench) {
  volatile register int8_t x8, y8;
  volatile register int16_t x16, y16;
  volatile register int32_t x32, y32;
  volatile register int64_t x64, y64;
  EZBENCH2("idiv8", PROGN(x8 = 7, y8 = 18), x8 /= y8);
  EZBENCH2("idiv16", PROGN(x16 = 123, y16 = 116), x16 /= y16);
  EZBENCH2("idiv32", PROGN(x32 = 0x238943298, y32 = 0x238), x32 /= y32);
  EZBENCH2("idiv64", PROGN(x64 = 0x23894329838932, y64 = 0x238), x64 /= y64);
}

BENCH(mul, bench) {
  volatile register uint8_t x8, y8;
  volatile register uint16_t x16, y16;
  volatile register uint32_t x32, y32;
  volatile register uint64_t x64, y64;
  EZBENCH2("mul8", PROGN(x8 = 7, y8 = 18), y8 *= x8);
  EZBENCH2("mul16", PROGN(x16 = 123, y16 = 116), y16 *= x16);
  EZBENCH2("mul32", PROGN(x32 = 0x238943, y32 = 0x238), y32 *= x32);
  EZBENCH2("mul64", PROGN(x64 = 0x23894329838932, y64 = 0x238), y64 *= x64);
}

BENCH(div, bench) {
  volatile register uint8_t x8, y8;
  volatile register uint16_t x16, y16;
  volatile register uint32_t x32, y32;
  volatile register uint64_t x64, y64;
  EZBENCH2("div8", PROGN(x8 = 7, y8 = 18), x8 /= y8);
  EZBENCH2("div16", PROGN(x16 = 123, y16 = 116), x16 /= y16);
  EZBENCH2("div32", PROGN(x32 = 0x238943298, y32 = 0x238), x32 /= y32);
  EZBENCH2("div64", PROGN(x64 = 0x23894329838932, y64 = 0x238), x64 /= y64);
}
