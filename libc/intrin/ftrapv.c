/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2023 Justine Alexandra Roberts Tunney                              │
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
#include "libc/intrin/intrin.h"
#include "libc/limits.h"
#include "libc/runtime/internal.h"

/**
 * Returns -𝑥, aborting on overflow.
 *
 * @see __on_arithmetic_overflow()
 * @see -ftrapv to enable
 */
int __negvsi2(int x) {
  if (x == INT_MIN) {
    __on_arithmetic_overflow();
  }
  return -x;
}

/**
 * Returns -𝑥 on overflow.
 *
 * @see __on_arithmetic_overflow()
 * @see -ftrapv to enable
 */
long __negvdi2(long x) {
  if (x == LONG_MIN) {
    __on_arithmetic_overflow();
  }
  return -x;
}

/**
 * Returns -𝑥, aborting on overflow.
 *
 * @see __on_arithmetic_overflow()
 * @see -ftrapv to enable
 */
int128_t __negvti2(int128_t x) {
  if (x == INT128_MIN) {
    __on_arithmetic_overflow();
  }
  return -x;
}

/**
 * Returns 𝑥+𝑦, aborting on overflow.
 *
 * @see __on_arithmetic_overflow()
 * @see -ftrapv to enable
 */
int __addvsi3(int x, int y) {
  int z;
  if (__builtin_add_overflow(x, y, &z)) {
    __on_arithmetic_overflow();
  }
  return z;
}

/**
 * Returns 𝑥+𝑦, aborting on overflow.
 *
 * @see __on_arithmetic_overflow()
 * @see -ftrapv to enable
 */
long __addvdi3(long x, long y) {
  long z;
  if (__builtin_add_overflow(x, y, &z)) {
    __on_arithmetic_overflow();
  }
  return z;
}

/**
 * Returns 𝑥+𝑦, aborting on overflow.
 *
 * @see __on_arithmetic_overflow()
 * @see -ftrapv to enable
 */
int128_t __addvti3(int128_t x, int128_t y) {
  int128_t z;
  if (__builtin_add_overflow(x, y, &z)) {
    __on_arithmetic_overflow();
  }
  return z;
}

/**
 * Returns 𝑥-𝑦, aborting on overflow.
 *
 * @see __on_arithmetic_overflow()
 * @see -ftrapv to enable
 */
int __subvsi3(int x, int y) {
  int z;
  if (__builtin_sub_overflow(x, y, &z)) {
    __on_arithmetic_overflow();
  }
  return z;
}

/**
 * Returns 𝑥-𝑦, aborting on overflow.
 *
 * @see __on_arithmetic_overflow()
 * @see -ftrapv to enable
 */
long __subvdi3(long x, long y) {
  long z;
  if (__builtin_sub_overflow(x, y, &z)) {
    __on_arithmetic_overflow();
  }
  return z;
}

/**
 * Returns 𝑥-𝑦, aborting on overflow.
 *
 * @see __on_arithmetic_overflow()
 * @see -ftrapv to enable
 */
int128_t __subvti3(int128_t x, int128_t y) {
  int128_t z;
  if (__builtin_sub_overflow(x, y, &z)) {
    __on_arithmetic_overflow();
  }
  return z;
}

/**
 * Returns 𝑥*𝑦, aborting on overflow.
 *
 * @see __on_arithmetic_overflow()
 * @see -ftrapv to enable
 */
int __mulvsi3(int x, int y) {
  int z;
  if (__builtin_mul_overflow(x, y, &z)) {
    __on_arithmetic_overflow();
  }
  return z;
}

/**
 * Returns 𝑥*𝑦, aborting on overflow.
 *
 * @see __on_arithmetic_overflow()
 * @see -ftrapv to enable
 */
long __mulvdi3(long x, long y) {
  long z;
  if (__builtin_mul_overflow(x, y, &z)) {
    __on_arithmetic_overflow();
  }
  return z;
}

/**
 * Returns 𝑥*𝑦, aborting on overflow.
 *
 * @see __on_arithmetic_overflow()
 * @see -ftrapv to enable
 */
int128_t __mulvti3(int128_t x, int128_t y) {
  int128_t z;
  if (__builtin_mul_overflow(x, y, &z)) {
    __on_arithmetic_overflow();
  }
  return z;
}
