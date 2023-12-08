/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
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
#include "libc/limits.h"
#include "libc/runtime/internal.h"
#include "libc/stdckdint.h"

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
long long __negvdi2(long long x) {
  if (x == LLONG_MIN) {
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
  if (ckd_add(&z, x, y)) {
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
long long __addvdi3(long long x, long long y) {
  long long z;
  if (ckd_add(&z, x, y)) {
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
  if (ckd_add(&z, x, y)) {
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
  if (ckd_sub(&z, x, y)) {
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
long long __subvdi3(long long x, long long y) {
  long long z;
  if (ckd_sub(&z, x, y)) {
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
  if (ckd_sub(&z, x, y)) {
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
  if (ckd_mul(&z, x, y)) {
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
long long __mulvdi3(long long x, long long y) {
  long long z;
  if (ckd_mul(&z, x, y)) {
    __on_arithmetic_overflow();
  }
  return z;
}
