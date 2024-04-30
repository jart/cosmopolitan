// Copyright 2024 Justine Alexandra Roberts Tunney
//
// Permission to use, copy, modify, and/or distribute this software for
// any purpose with or without fee is hereby granted, provided that the
// above copyright notice and this permission notice appear in all copies.
//
// THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL
// WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE
// AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL
// DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
// PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
// TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
// PERFORMANCE OF THIS SOFTWARE.

#include <math.h>
#include <stdlib.h>

#define MAX_ERROR_ULP       1
#define GOTTA_TEST_THEM_ALL 0

unsigned rand32(void) {
  /* Knuth, D.E., "The Art of Computer Programming," Vol 2,
     Seminumerical Algorithms, Third Edition, Addison-Wesley, 1998,
     p. 106 (line 26) & p. 108 */
  static unsigned long long lcg = 1;
  lcg *= 6364136223846793005;
  lcg += 1442695040888963407;
  return lcg >> 32;
}

int main() {
#if GOTTA_TEST_THEM_ALL
#pragma omp parallel for
  for (long i = 0; i < 4294967296; ++i) {
#else
  for (long r = 0; r < 100000; ++r) {
    unsigned i = rand32();
#endif
    union {
      float f;
      unsigned i;
    } x, a, b;
    x.i = i;
    a.f = erf(x.f);
    b.f = erff(x.f);
    long ai = a.i;
    long bi = b.i;
    long e = bi - ai;
    if (e < 0)
      e = -e;
    if (e > MAX_ERROR_ULP)
      exit(99);
  }
}
