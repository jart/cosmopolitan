asm(".ident\t\"hello\"");
/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
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
#include "third_party/chibicc/test/test.h"

char *asm_fn1(void) {
  asm("mov\t$50,%rax\n\t"
      "mov\t%rbp,%rsp\n\t"
      "leave\n\t"
      "ret");
}

char *asm_fn2(void) {
  asm inline volatile("mov\t$55,%rax\n\t"
                      "mov\t%rbp,%rsp\n\t"
                      "leave\n\t"
                      "ret");
}

void *repmovsb(void *dst, void *src, unsigned long n) {
  asm("rep movsb"
      : "=D"(dst), "=S"(src), "=c"(n), "=m"(*(char(*)[n])dst)
      : "0"(dst), "1"(src), "2"(n), "m"(*(const char(*)[n])src));
  return dst;
}

void *repmovsbeax(void *dst, void *src, unsigned long n) {
  void *res;
  asm("rep movsb\n\t"
      "xchg\t%0,%1"
      : "=a"(res), "=D"(dst), "=S"(src), "=c"(n), "=m"(*(char(*)[n])dst)
      : "1"(dst), "2"(src), "3"(n), "m"(*(const char(*)[n])src)
      : "rbx", "rbp", "r12", "r13", "r14", "r15", "cc");
  return res;
}

void testSmashStackFrame_clobberIsRestored(void) {
  asm volatile("xor\t%%ebp,%%ebp"
               : /* no outputs */
               : /* no inputs */
               : "rbp", "cc");
}

void testFlagOutputs(void) {
  bool zf, cf, sf;
  asm("xor\t%%eax,%%eax\n\t"
      "inc\t%%eax"
      : "=@ccz"(zf), "=@ccs"(cf)
      : /* no inputs */
      : "rax");
  ASSERT(false, zf);
  ASSERT(false, sf);
  asm("xor\t%%eax,%%eax\n\t"
      "dec\t%%eax"
      : "=@ccz"(zf), "=@ccs"(cf)
      : /* no inputs */
      : "rax");
  ASSERT(false, zf);
  ASSERT(true, sf);
  asm("xor\t%%eax,%%eax\n\t"
      "cmc"
      : "=@ccz"(zf), "=@ccc"(cf), "=@ccs"(sf)
      : /* no inputs */
      : "rax");
  ASSERT(true, zf);
  ASSERT(true, cf);
  ASSERT(false, sf);
}

void testAugmentLoByte_onlyModifiesLowerBits(void) {
  int x, y;
  x = 0x01020304;
  y = 0x00000005;
  asm("sub\t%b1,%b0" : "+q"(x) : "q"(y));
  ASSERT(0x010203ff, x);
  ASSERT(0x00000005, y);
}

void testAugmentHiByte_onlyModifiesHigherBits(void) {
  int x, y;
  x = 0x01020304;
  y = 0x00000400;
  asm("sub\t%h1,%h0" : "+Q"(x) : "Q"(y));
  ASSERT(0x0102ff04, x);
  ASSERT(0x00000400, y);
}

int main() {
  ASSERT(50, asm_fn1());
  ASSERT(55, asm_fn2());

  {
    char buf[] = "HELLO";
    char *s = "hello";
    char *p = repmovsb(buf, s, 4);
    ASSERT(4, p - buf);
    ASSERT('h', buf[0]);
    ASSERT('e', buf[1]);
    ASSERT('l', buf[2]);
    ASSERT('l', buf[3]);
    ASSERT('O', buf[4]);
  }

  {
    char buf[] = "HELLO";
    char *s = "hello";
    char *p = repmovsbeax(buf, s, 4);
    ASSERT(4, p - buf);
    ASSERT('h', buf[0]);
    ASSERT('e', buf[1]);
    ASSERT('l', buf[2]);
    ASSERT('l', buf[3]);
    ASSERT('O', buf[4]);
  }

  testSmashStackFrame_clobberIsRestored();

  short v1[8] = {0, 1, 2, 3, 4, 5, 6, 7};
  short v2[8] = {1, 1, 1, 1, 1, 1, 1, 1};
  short v3[8] = {2, 2, 2, 2, 2, 2, 2, 2};
  asm("paddsw\t%1,%0\n\t"
      "paddsw\t%2,%0"
      : "+x"(v1)
      : "xm"(v2), "xm"(v3));
  ASSERT(3, v1[0]);
  ASSERT(4, v1[1]);
  ASSERT(5, v1[2]);
  ASSERT(6, v1[3]);
  ASSERT(7, v1[4]);
  ASSERT(8, v1[5]);
  ASSERT(9, v1[6]);
  ASSERT(10, v1[7]);

  {
    char *p;
    asm("mov\t%1,%0" : "=r"(p) : "r"("hello"));
    ASSERT(1, !strcmp(p, "hello"));
  }

  testAugmentLoByte_onlyModifiesLowerBits();
  testAugmentHiByte_onlyModifiesHigherBits();

  return 0;
}
