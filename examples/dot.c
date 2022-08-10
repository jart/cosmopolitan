#if 0
/*─────────────────────────────────────────────────────────────────╗
│ To the extent possible under law, Justine Tunney has waived      │
│ all copyright and related or neighboring rights to this file,    │
│ as it is written in the following disclaimers:                   │
│   • http://unlicense.org/                                        │
│   • http://creativecommons.org/publicdomain/zero/1.0/            │
╚─────────────────────────────────────────────────────────────────*/
#endif
#include "libc/macros.internal.h"
#include "libc/stdio/stdio.h"

typedef float xmm_t __attribute__((__vector_size__(16), __aligned__(4)));

float dotvector(float *x, float *y, size_t n) {
  size_t i;
  float res;
  if (n > 64) {
    return dotvector(x, y, n / 2) + dotvector(x + n / 2, y + n / 2, n - n / 2);
  }
  for (res = i = 0; i < n; ++i) {
    if (i + 4 <= n) {
      xmm_t res4 = (xmm_t){0};
      do {
        res4 += *(xmm_t *)(x + i) * *(xmm_t *)(y + i);
      } while ((i += 4) + 4 <= n);
      res += res4[0];
      res += res4[1];
      res += res4[2];
      res += res4[3];
      continue;
    }
    res += x[i] * y[i];
  }
  return res;
}

int main(int argc, char *argv[]) {
  float x[] = {1, 2, 3, 4, 1, 2, 3, 4};
  float y[] = {4, 3, 2, 1, 1, 2, 3, 4};
  printf("%g\n", dotvector(x, y, ARRAYLEN(x)));
}
