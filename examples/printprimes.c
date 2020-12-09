#if 0
/*─────────────────────────────────────────────────────────────────╗
│ To the extent possible under law, Justine Tunney has waived      │
│ all copyright and related or neighboring rights to this file,    │
│ as it is written in the following disclaimers:                   │
│   • http://unlicense.org/                                        │
│   • http://creativecommons.org/publicdomain/zero/1.0/            │
╚─────────────────────────────────────────────────────────────────*/
#endif
#include "libc/fmt/itoa.h"
#include "libc/math.h"
#include "libc/stdio/stdio.h"

int main(int argc, char *argv[]) {
  char buf[32];
  bool isprime;
  long i, j, k, n, m;
  k = 0;
  n = pow(2, 32);
  printf("2\n");
  for (i = 3; i < n; ++i) {
    isprime = true;
    for (m = sqrt(i) + 1, j = 2; j < m; ++j) {
      if (i % j == 0) {
        isprime = false;
        break;
      }
    }
    if (isprime) {
      int64toarray_radix10(i, buf);
      fputs(buf, stdout);
      fputc('\n', stdout);
      if (k++ % 100 == 0) {
        fprintf(stderr, "\r%20d", i);
      }
    }
  }
  return 0;
}
