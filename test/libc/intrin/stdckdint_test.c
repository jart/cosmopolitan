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
#include "libc/stdckdint.h"
#include "libc/macros.internal.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"

#define FAIL ++failed < 100

#define uintmax_t uint128_t

#define T    int
#define TBIT (sizeof(T) * CHAR_BIT - 1)
#define TMIN (((T) ~(T)0) > 1 ? (T)0 : (T)((uintmax_t)1 << TBIT))
#define TMAX (((T) ~(T)0) > 1 ? (T) ~(T)0 : (T)(((uintmax_t)1 << TBIT) - 1))
T Vint[] = {5,         4,           2,        77,       4,         7,
            0,         1,           2,        3,        4,         -1,
            -2,        -3,          -4,       TMIN,     TMIN + 1,  TMIN + 2,
            TMIN + 3,  TMIN + 5,    TMIN + 7, TMAX,     TMAX - 1,  TMAX - 2,
            TMAX - 77, TMAX - 3,    TMAX - 5, TMAX - 7, TMAX - 50, TMIN / 2,
            TMAX / 2,  TMAX / 2 - 3};
#undef TMAX
#undef TMIN
#undef TBIT
#undef T

#define T    long
#define TBIT (sizeof(T) * CHAR_BIT - 1)
#define TMIN (((T) ~(T)0) > 1 ? (T)0 : (T)((uintmax_t)1 << TBIT))
#define TMAX (((T) ~(T)0) > 1 ? (T) ~(T)0 : (T)(((uintmax_t)1 << TBIT) - 1))
T Vlong[] = {5,         4,           2,        77,       4,         7,
             0,         1,           2,        3,        4,         -1,
             -2,        -3,          -4,       TMIN,     TMIN + 1,  TMIN + 2,
             TMIN + 3,  TMIN + 5,    TMIN + 7, TMAX,     TMAX - 1,  TMAX - 2,
             TMAX - 77, TMAX - 3,    TMAX - 5, TMAX - 7, TMAX - 50, TMIN / 2,
             TMAX / 2,  TMAX / 2 - 3};
#undef TMAX
#undef TMIN
#undef TBIT
#undef T

#define T    __int128
#define TBIT (sizeof(T) * CHAR_BIT - 1)
#define TMIN (((T) ~(T)0) > 1 ? (T)0 : (T)((uintmax_t)1 << TBIT))
#define TMAX (((T) ~(T)0) > 1 ? (T) ~(T)0 : (T)(((uintmax_t)1 << TBIT) - 1))
T Vint128[] = {5,         4,           2,        77,       4,         7,
               0,         1,           2,        3,        4,         -1,
               -2,        -3,          -4,       TMIN,     TMIN + 1,  TMIN + 2,
               TMIN + 3,  TMIN + 5,    TMIN + 7, TMAX,     TMAX - 1,  TMAX - 2,
               TMAX - 77, TMAX - 3,    TMAX - 5, TMAX - 7, TMAX - 50, TMIN / 2,
               TMAX / 2,  TMAX / 2 - 3};
#undef TMAX
#undef TMIN
#undef TBIT
#undef T

int failed;

void test_ckd_add(void) {
  for (int i = 0; i < ARRAYLEN(Vint); ++i) {
    int x = Vint[i];
    for (int j = 0; j < ARRAYLEN(Vint); ++j) {
      int z1, z2, o1, o2, y = Vint[i];
      o1 = ckd_add(&z1, x, y);
      o2 = __builtin_add_overflow(x, y, &z2);
      if (z1 != z2 && FAIL) {
        fprintf(stderr, "add %d + %d = %d vs. %d\n", x, y, z1, z2);
      }
      if (o1 != o2 && FAIL) {
        fprintf(stderr, "add %d + %d overflow disagreement\n", x, y);
      }
    }
  }
}

void test_ckd_sub(void) {
  for (int i = 0; i < ARRAYLEN(Vint); ++i) {
    int x = Vint[i];
    for (int j = 0; j < ARRAYLEN(Vint); ++j) {
      int z1, z2, o1, o2, y = Vint[i];
      o1 = ckd_sub(&z1, x, y);
      o2 = __builtin_sub_overflow(x, y, &z2);
      if (z1 != z2 && FAIL) {
        fprintf(stderr, "sub %d - %d = %d vs. %d\n", x, y, z1, z2);
      }
      if (o1 != o2 && FAIL) {
        fprintf(stderr, "sub %d - %d overflow disagreement\n", x, y);
      }
    }
  }
}

void test_ckd_mul(void) {
  for (int i = 0; i < ARRAYLEN(Vint); ++i) {
    int x = Vint[i];
    for (int j = 0; j < ARRAYLEN(Vint); ++j) {
      int z1, z2, o1, o2, y = Vint[i];
      o1 = ckd_mul(&z1, x, y);
      o2 = __builtin_mul_overflow(x, y, &z2);
      if (z1 != z2 && FAIL) {
        fprintf(stderr, "mul %d * %d = %d vs. %d\n", x, y, z1, z2);
      }
      if (o1 != o2 && FAIL) {
        fprintf(stderr, "mul %d * %d overflow disagreement\n", x, y);
      }
    }
  }
}

void test_ckd_add_long(void) {
  for (long i = 0; i < ARRAYLEN(Vlong); ++i) {
    long x = Vlong[i];
    for (long j = 0; j < ARRAYLEN(Vlong); ++j) {
      long z1, z2, o1, o2, y = Vlong[i];
      o1 = ckd_add(&z1, x, y);
      o2 = __builtin_add_overflow(x, y, &z2);
      if (z1 != z2 && FAIL) {
        fprintf(stderr, "add %ld + %ld = %ld vs. %ld\n", x, y, z1, z2);
      }
      if (o1 != o2 && FAIL) {
        fprintf(stderr, "add %ld + %ld overflow disagreement\n", x, y);
      }
    }
  }
}

void test_ckd_sub_long(void) {
  for (long i = 0; i < ARRAYLEN(Vlong); ++i) {
    long x = Vlong[i];
    for (long j = 0; j < ARRAYLEN(Vlong); ++j) {
      long z1, z2, o1, o2, y = Vlong[i];
      o1 = ckd_sub(&z1, x, y);
      o2 = __builtin_sub_overflow(x, y, &z2);
      if (z1 != z2 && FAIL) {
        fprintf(stderr, "sub %ld - %ld = %ld vs. %ld\n", x, y, z1, z2);
      }
      if (o1 != o2 && FAIL) {
        fprintf(stderr, "sub %ld - %ld overflow disagreement\n", x, y);
      }
    }
  }
}

void test_ckd_mul_long(void) {
  for (long i = 0; i < ARRAYLEN(Vlong); ++i) {
    long x = Vlong[i];
    for (long j = 0; j < ARRAYLEN(Vlong); ++j) {
      long z1, z2, o1, o2, y = Vlong[i];
      o1 = ckd_mul(&z1, x, y);
      o2 = __builtin_mul_overflow(x, y, &z2);
      if (z1 != z2 && FAIL) {
        fprintf(stderr, "mul %ld * %ld = %ld vs. %ld\n", x, y, z1, z2);
      }
      if (o1 != o2 && FAIL) {
        fprintf(stderr, "mul %ld * %ld overflow disagreement\n", x, y);
      }
    }
  }
}

void test_ckd_add_int128(void) {
  for (int128_t i = 0; i < ARRAYLEN(Vint128); ++i) {
    int128_t x = Vint128[i];
    for (int128_t j = 0; j < ARRAYLEN(Vint128); ++j) {
      int128_t z1, z2, o1, o2, y = Vint128[i];
      o1 = ckd_add(&z1, x, y);
      o2 = __builtin_add_overflow(x, y, &z2);
      if (z1 != z2 && FAIL) {
        fprintf(stderr, "add %jjd * %jjd = %jjd vs. %jjd\n", x, y, z1, z2);
      }
      if (o1 != o2 && FAIL) {
        fprintf(stderr, "add %jjd * %jjd overflow disagreement\n", x, y);
      }
    }
  }
}

void test_ckd_sub_int128(void) {
  for (int128_t i = 0; i < ARRAYLEN(Vint128); ++i) {
    int128_t x = Vint128[i];
    for (int128_t j = 0; j < ARRAYLEN(Vint128); ++j) {
      int128_t z1, z2, o1, o2, y = Vint128[i];
      o1 = ckd_sub(&z1, x, y);
      o2 = __builtin_sub_overflow(x, y, &z2);
      if (z1 != z2 && FAIL) {
        fprintf(stderr, "sub %jjd * %jjd = %jjd vs. %jjd\n", x, y, z1, z2);
      }
      if (o1 != o2 && FAIL) {
        fprintf(stderr, "sub %jjd * %jjd overflow disagreement\n", x, y);
      }
    }
  }
}

void test_ckd_mul_int128(void) {
  for (int128_t i = 0; i < ARRAYLEN(Vint128); ++i) {
    int128_t x = Vint128[i];
    for (int128_t j = 0; j < ARRAYLEN(Vint128); ++j) {
      int128_t z1, z2, o1, o2, y = Vint128[i];
      o1 = ckd_mul(&z1, x, y);
      o2 = __builtin_mul_overflow(x, y, &z2);
      if (z1 != z2 && FAIL) {
        fprintf(stderr, "mul %jjd * %jjd = %jjd vs. %jjd\n", x, y, z1, z2);
      }
      if (o1 != o2 && FAIL) {
        fprintf(stderr, "mul %jjd * %jjd overflow disagreement\n", x, y);
      }
    }
  }
}

void test_ckd_addu(void) {
  for (int i = 0; i < ARRAYLEN(Vint); ++i) {
    unsigned int x = Vint[i];
    for (int j = 0; j < ARRAYLEN(Vint); ++j) {
      unsigned int z1, z2, o1, o2, y = Vint[i];
      o1 = ckd_add(&z1, x, y);
      o2 = __builtin_add_overflow(x, y, &z2);
      if (z1 != z2 && FAIL) {
        fprintf(stderr, "unsigned add %u + %u = %u vs. %u\n", x, y, z1, z2);
      }
      if (o1 != o2 && FAIL) {
        fprintf(stderr, "unsigned add %u + %u overflow disagreement\n", x, y);
      }
    }
  }
}

void test_ckd_subu(void) {
  for (int i = 0; i < ARRAYLEN(Vint); ++i) {
    unsigned int x = Vint[i];
    for (int j = 0; j < ARRAYLEN(Vint); ++j) {
      unsigned int z1, z2, o1, o2, y = Vint[i];
      o1 = ckd_sub(&z1, x, y);
      o2 = __builtin_sub_overflow(x, y, &z2);
      if (z1 != z2 && FAIL) {
        fprintf(stderr, "unsigned sub %u - %u = %u vs. %u\n", x, y, z1, z2);
      }
      if (o1 != o2 && FAIL) {
        fprintf(stderr, "unsigned sub %u - %u overflow disagreement\n", x, y);
      }
    }
  }
}

void test_ckd_mulu(void) {
  for (int i = 0; i < ARRAYLEN(Vint); ++i) {
    unsigned int x = Vint[i];
    for (int j = 0; j < ARRAYLEN(Vint); ++j) {
      unsigned int z1, z2, o1, o2, y = Vint[i];
      o1 = ckd_mul(&z1, x, y);
      o2 = __builtin_mul_overflow(x, y, &z2);
      if (z1 != z2 && FAIL) {
        fprintf(stderr, "unsigned mul %u * %u = %u vs. %u\n", x, y, z1, z2);
      }
      if (o1 != o2 && FAIL) {
        fprintf(stderr, "unsigned mul %u * %u overflow disagreement\n", x, y);
      }
    }
  }
}

void test_ckd_addul(void) {
  for (int i = 0; i < ARRAYLEN(Vlong); ++i) {
    unsigned long x = Vlong[i];
    for (int j = 0; j < ARRAYLEN(Vlong); ++j) {
      unsigned long z1, z2, o1, o2, y = Vlong[i];
      o1 = ckd_add(&z1, x, y);
      o2 = __builtin_add_overflow(x, y, &z2);
      if (z1 != z2 && FAIL) {
        fprintf(stderr, "ulong add %lu + %lu = %lu vs. %lu\n", x, y, z1, z2);
      }
      if (o1 != o2 && FAIL) {
        fprintf(stderr, "ulong add %lu + %lu overflow disagreement\n", x, y);
      }
    }
  }
}

void test_ckd_subul(void) {
  for (int i = 0; i < ARRAYLEN(Vlong); ++i) {
    unsigned long x = Vlong[i];
    for (int j = 0; j < ARRAYLEN(Vlong); ++j) {
      unsigned long z1, z2, o1, o2, y = Vlong[i];
      o1 = ckd_sub(&z1, x, y);
      o2 = __builtin_sub_overflow(x, y, &z2);
      if (z1 != z2 && FAIL) {
        fprintf(stderr, "ulong sub %lu - %lu = %lu vs. %lu\n", x, y, z1, z2);
      }
      if (o1 != o2 && FAIL) {
        fprintf(stderr, "ulong sub %lu - %lu overflow disagreement\n", x, y);
      }
    }
  }
}

void test_ckd_mulul(void) {
  for (int i = 0; i < ARRAYLEN(Vlong); ++i) {
    unsigned long x = Vlong[i];
    for (int j = 0; j < ARRAYLEN(Vlong); ++j) {
      unsigned long z1, z2, o1, o2, y = Vlong[i];
      o1 = ckd_mul(&z1, x, y);
      o2 = __builtin_mul_overflow(x, y, &z2);
      if (z1 != z2 && FAIL) {
        fprintf(stderr, "ulong mul %lu * %lu = %lu vs. %lu\n", x, y, z1, z2);
      }
      if (o1 != o2 && FAIL) {
        fprintf(stderr, "ulong mul %lu * %lu overflow disagreement\n", x, y);
      }
    }
  }
}

void test_ckd_addull(void) {
  for (int i = 0; i < ARRAYLEN(Vint128); ++i) {
    uint128_t x = Vint128[i];
    for (int j = 0; j < ARRAYLEN(Vint128); ++j) {
      uint128_t z1, z2, o1, o2, y = Vint128[i];
      o1 = ckd_add(&z1, x, y);
      o2 = __builtin_add_overflow(x, y, &z2);
      if (z1 != z2 && FAIL) {
        fprintf(stderr, "u128 add %jju + %jju = %jju vs. %jju\n", x, y, z1, z2);
      }
      if (o1 != o2 && FAIL) {
        fprintf(stderr, "u128 add %jju + %jju overflow disagreement\n", x, y);
      }
    }
  }
}

void test_ckd_subull(void) {
  for (int i = 0; i < ARRAYLEN(Vint128); ++i) {
    uint128_t x = Vint128[i];
    for (int j = 0; j < ARRAYLEN(Vint128); ++j) {
      uint128_t z1, z2, o1, o2, y = Vint128[i];
      o1 = ckd_sub(&z1, x, y);
      o2 = __builtin_sub_overflow(x, y, &z2);
      if (z1 != z2 && FAIL) {
        fprintf(stderr, "u128 sub %jju - %jju = %jju vs. %jju\n", x, y, z1, z2);
      }
      if (o1 != o2 && FAIL) {
        fprintf(stderr, "u128 sub %jju - %jju overflow disagreement\n", x, y);
      }
    }
  }
}

void test_ckd_mulull(void) {
  for (int i = 0; i < ARRAYLEN(Vint128); ++i) {
    uint128_t x = Vint128[i];
    for (int j = 0; j < ARRAYLEN(Vint128); ++j) {
      uint128_t z1, z2, o1, o2, y = Vint128[i];
      o1 = ckd_mul(&z1, x, y);
      o2 = __builtin_mul_overflow(x, y, &z2);
      if (z1 != z2 && FAIL) {
        fprintf(stderr, "u128 mul %jju * %jju = %jju vs. %jju\n", x, y, z1, z2);
      }
      if (o1 != o2 && FAIL) {
        fprintf(stderr, "u128 mul %jju * %jju overflow disagreement\n", x, y);
      }
    }
  }
}

int main(int argc, char *argv[]) {
  test_ckd_add();
  test_ckd_sub();
  test_ckd_mul();
  test_ckd_add_long();
  test_ckd_sub_long();
  test_ckd_mul_long();
  test_ckd_add_int128();
  test_ckd_sub_int128();
  test_ckd_mul_int128();
  test_ckd_addu();
  test_ckd_subu();
  test_ckd_mulu();
  test_ckd_addul();
  test_ckd_subul();
  test_ckd_mulul();
  test_ckd_addull();
  test_ckd_subull();
  test_ckd_mulull();
  return failed ? 1 : 0;
}
