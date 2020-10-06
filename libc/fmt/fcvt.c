/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2009 Ian Piumarta                                                  │
│                                                                              │
│ Permission is hereby granted, free of charge, to any person obtaining a copy │
│ of this software and associated documentation files (the 'Software'), to     │
│ deal in the Software without restriction, including without limitation the   │
│ rights to use, copy, modify, merge, publish, distribute, and/or sell copies  │
│ of the Software, and to permit persons to whom the Software is furnished to  │
│ do so, provided that the above copyright notice(s) and this permission       │
│ notice appear in all copies of the Software.  Inclusion of the above         │
│ copyright notice(s) and this permission notice in supporting documentation   │
│ would be appreciated but is not required.                                    │
│                                                                              │
│ THE SOFTWARE IS PROVIDED 'AS IS'.  USE ENTIRELY AT YOUR OWN RISK.            │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "libc/assert.h"
#include "libc/fmt/fmt.h"
#include "libc/macros.h"
#include "libc/math.h"
#include "libc/runtime/runtime.h"
#include "libc/str/str.h"

asm(".ident\t\"\\n\\n\
ecvt, fcvt (MIT License)\\n\
Copyright 2009 Ian Piumarta\"");

/**
 * @fileoverview Replacements for the functions ecvt() and fcvt()
 *
 * These functions were recently deprecated in POSIX. The interface and
 * behaviour is identical to the functions that they replace and faster.
 *
 * For details on the use of these functions, see your ecvt(3) manual
 * page. If you don't have one handy, there might still be one available
 * here: http://opengroup.org/onlinepubs/007908799/xsh/ecvt.html
 *
 * @see https://www.piumarta.com/software/fcvt/
 */

static char *Fcvt(double value, int ndigit, int *decpt, int *sign, int fflag) {
  static char buf[128];
  double i;
  uint64_t l, mant;
  int exp2, exp10, ptr;
  memcpy(&l, &value, 8);
  exp2 = (0x7ff & (l >> 52)) - 1023;
  mant = l & 0x000fffffffffffffULL;
  if ((*sign = l >> 63)) value = -value;
  if (exp2 == 0x400) {
    *decpt = 0;
    return mant ? "nan" : "inf";
  }
  exp10 = (value == 0) ? !fflag : (int)ceil(log10(value));
  if (exp10 < -307) exp10 = -307; /* otherwise overflow in pow() */
  value *= pow(10.0, -exp10);
  if (value) {
    while (value < 0.1) {
      value *= 10;
      --exp10;
    }
    while (value >= 1.0) {
      value /= 10;
      ++exp10;
    }
  }
  assert(value == 0 || (0.1 <= value && value < 1.0));
  if (fflag) {
    if (ndigit + exp10 < 0) {
      *decpt = -ndigit;
      return "";
    }
    ndigit += exp10;
  }
  *decpt = exp10;
  if (ARRAYLEN(buf) < ndigit + 2) abort();
  ptr = 1;
#if 0 /* slow and safe (and dreadfully boring) */
  while (ptr <= ndigit) {
    i;
    value = modf(value * 10, &i);
    buf[ptr++] = '0' + (int)i;
  }
  if (value >= 0.5) {
    while (--ptr && ++buf[ptr] > '9') {
      buf[ptr] = '0';
    }
  }
#else /* faster */
  memcpy(&l, &value, 8);
  exp2 = (0x7ff & (l >> 52)) - 1023;
  assert(value == 0 || (-4 <= exp2 && exp2 <= -1));
  mant = l & 0x000fffffffffffffULL;
  if (exp2 == -1023) {
    ++exp2;
  } else {
    mant |= 0x0010000000000000ULL;
  }
  mant <<= (exp2 + 4); /* 56-bit denormalised signifier */
  while (ptr <= ndigit) {
    mant &= 0x00ffffffffffffffULL; /* mod 1.0 */
    mant = (mant << 1) + (mant << 3);
    buf[ptr++] = '0' + (mant >> 56);
  }
  if (mant & 0x0080000000000000ULL) /* 1/2 << 56 */
    while (--ptr && ++buf[ptr] > '9') buf[ptr] = '0';
#endif
  if (ptr) {
    buf[ndigit + 1] = 0;
    return buf + 1;
  }
  if (fflag) {
    ++ndigit;
    ++*decpt;
  }
  buf[0] = '1';
  buf[ndigit] = 0;
  return buf;
}

char *ecvt(double value, int ndigit, int *decpt, int *sign) {
  return Fcvt(value, ndigit, decpt, sign, 0);
}

char *fcvt(double value, int ndigit, int *decpt, int *sign) {
  return Fcvt(value, ndigit, decpt, sign, 1);
}
