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
#include "tool/build/emubin/poke.h"
#include "tool/build/emubin/real.h"

#define signbit(x) __builtin_signbit(x)

static const unsigned char kBlocks[] = {
    [0b1111] = 0xdb,  // █
    [0b0110] = 0xdb,  // █
    [0b1001] = 0xdb,  // █
    [0b0111] = 0xdb,  // █
    [0b1011] = 0xdb,  // █
    [0b1110] = 0xdb,  // █
    [0b1101] = 0xdb,  // █
    [0b1100] = 0xdc,  // ▄
    [0b0100] = 0xdc,  // ▄
    [0b1000] = 0xdc,  // ▄
    [0b0101] = 0xdd,  // ▌
    [0b1010] = 0xde,  // ▐
    [0b0011] = 0xdf,  // ▀
    [0b0010] = 0xdf,  // ▀
    [0b0001] = 0xdf,  // ▀
};

forceinline void *memset(void *di, int al, unsigned long cx) {
  asm("rep stosb"
      : "=D"(di), "=c"(cx), "=m"(*(char(*)[cx])di)
      : "0"(di), "1"(cx), "a"(al));
  return di;
}

forceinline long double pi(void) {
  long double x;
  asm("fldpi" : "=t"(x));
  return x;
}

forceinline long double tau(void) {
  return pi() * 2;
}

forceinline void sincosl(long double x, long double *sin, long double *cos) {
  asm("fsincos" : "=t"(*sin), "=u"(*cos) : "0"(x));
}

forceinline long double atan2l(long double x, long double y) {
  asm("fpatan" : "+t"(x) : "u"(y) : "st(1)");
  return x;
}

forceinline long lrintl(long double x) {
  long i;
  asm("fistp%z0\t%0" : "=m"(i) : "t"(x) : "st");
  return i;
}

forceinline long double truncl(long double x) {
  asm("frndint" : "+t"(x));
  return x;
}

forceinline long double fabsl(long double x) {
  asm("fabs" : "+t"(x));
  return x;
}

forceinline long lroundl(long double x) {
  int s = signbit(x);
  x = truncl(fabsl(x) + .5);
  if (s) x = -x;
  return lrintl(x);
}

static unsigned short GetFpuControlWord(void) {
  unsigned short cw;
  asm("fnstcw\t%0" : "=m"(cw));
  return cw;
}

static void SetFpuControlWord(unsigned short cw) {
  asm volatile("fldcw\t%0" : /* no outputs */ : "m"(cw));
}

static void InitializeFpu(void) {
  asm("fninit");
}

static void SetTruncationRounding(void) {
  SetFpuControlWord(GetFpuControlWord() | 0x0c00);
}

static void spiral(unsigned char p[25][80][2], unsigned char B[25][80], int g) {
  int i, x, y;
  long double a, b, u, v, h;
  for (a = b = i = 0; i < 1000; ++i) {
    sincosl(a, &u, &v);
    h = atan2l(u, v) - .333L * g;
    x = lroundl(80 + u * b);
    y = lroundl(25 + v * b * (1. / ((266 / 64.) * (900 / 1600.))));
    B[y >> 1][x >> 1] |= 1 << ((y & 1) << 1 | (x & 1));
    POKE(p[y >> 1][x >> 1][0], kBlocks[B[y >> 1][x >> 1]]);
    POKE(p[y >> 1][x >> 1][1], (lrintl((h + tau()) * (8 / tau())) & 7) + 8);
    a += .05;
    b += .05;
  }
}

int main() {
  int g;
  InitializeFpu();
  SetTruncationRounding();
  SetVideoMode(3);
  for (g = 0;; ++g) {
    SetEs(0);
    memset((void *)(0x7c00 + 512), 0, 25 * 80);
    SetEs(0xb8000 >> 4);
    memset((void *)0, 0, 25 * 80 * 2);
    spiral((void *)0, (void *)(0x7c00 + 512), g);
  }
}
