/*
 * Copyright (c) The mldsa-native project authors
 * SPDX-License-Identifier: LicenseRef-PD-hp OR CC0-1.0 OR 0BSD OR MIT-0 OR MIT
 */

/* References
 * ==========
 *
 * - [surf]
 *   SURF: Simple Unpredictable Random Function
 *   Daniel J. Bernstein
 *   https://cr.yp.to/papers.html#surf
 */

/* * Based on @[surf]*/

/**
 * WARNING
 *
 * The randombytes() implementation in this file is for TESTING ONLY.
 * You MUST NOT use this implementation outside of testing.
 *
 */

#include <stdint.h>
#include <string.h>

#include "notrandombytes.h"

#ifdef ENABLE_CT_TESTING
#include <valgrind/memcheck.h>
#endif

static uint32_t seed[32] = {3, 1, 4, 1, 5, 9, 2, 6, 5, 3, 5, 8, 9, 7, 9, 3,
                            2, 3, 8, 4, 6, 2, 6, 4, 3, 3, 8, 3, 2, 7, 9, 5};
static uint32_t in[12];
static uint32_t out[8];
static int32_t outleft = 0;

void randombytes_reset(void)
{
  memset(in, 0, sizeof(in));
  memset(out, 0, sizeof(out));
  outleft = 0;
}

#define ROTATE(x, b) (((x) << (b)) | ((x) >> (32 - (b))))
#define MUSH(i, b) x = t[i] += (((x ^ seed[i]) + sum) ^ ROTATE(x, b));

static void surf(void)
{
  uint32_t t[12];
  uint32_t x;
  uint32_t sum = 0;
  int32_t r;
  int32_t i;
  int32_t loop;

  for (i = 0; i < 12; ++i)
  {
    t[i] = in[i] ^ seed[12 + i];
  }
  for (i = 0; i < 8; ++i)
  {
    out[i] = seed[24 + i];
  }
  x = t[11];
  for (loop = 0; loop < 2; ++loop)
  {
    for (r = 0; r < 16; ++r)
    {
      sum += 0x9e3779b9;
      MUSH(0, 5)
      MUSH(1, 7)
      MUSH(2, 9)
      MUSH(3, 13)
      MUSH(4, 5)
      MUSH(5, 7)
      MUSH(6, 9)
      MUSH(7, 13)
      MUSH(8, 5)
      MUSH(9, 7)
      MUSH(10, 9)
      MUSH(11, 13)
    }
    for (i = 0; i < 8; ++i)
    {
      out[i] ^= t[i + 4];
    }
  }
}

int randombytes(uint8_t *buf, size_t n)
{
#ifdef ENABLE_CT_TESTING
  uint8_t *buf_orig = buf;
  size_t n_orig = n;
#endif

  while (n > 0)
  {
    if (!outleft)
    {
      if (!++in[0])
      {
        if (!++in[1])
        {
          if (!++in[2])
          {
            ++in[3];
          }
        }
      }
      surf();
      outleft = 8;
    }
    *buf = (uint8_t)out[--outleft];
    ++buf;
    --n;
  }

#ifdef ENABLE_CT_TESTING
  /*
   * Mark all randombytes output as secret (undefined).
   * Valgrind will propagate this to everything derived from it.
   */
  VALGRIND_MAKE_MEM_UNDEFINED(buf_orig, n_orig);
#endif /* ENABLE_CT_TESTING */
  return 0;
}
