#/*─────────────────────────────────────────────────────────────────╗
 │ To the extent possible under law, Justin Tunney has waived       │
 │ all copyright and related or neighboring rights to this file,    │
 │ as it is written in the following disclaimers:                   │
 │   • http://unlicense.org/                                        │
 │   • http://creativecommons.org/publicdomain/zero/1.0/            │
 ╚─────────────────────────────────────────────────────────────────*/
#include "histo.h"
#include "plinko.h"
#include "printf.h"

void PrintHistogram(int fd, const wchar_t *s, const long *h, size_t n) {
  long t;
  bool islast;
  int j, p, m;
  wchar_t buf[101];
  size_t i, logos;
  if (!(t = GetLongSum(h, n)))
    return;
  Fprintf(fd, L"%s%n", s);
  for (i = 0; i < n; ++i) {
    if (!h[i])
      continue;
    p = h[i] * 1000000 / t;
    assert(0 <= p && p <= 1000000);
    for (j = 0, m = p / 10000; j < m; ++j) {
      buf[j] = '#';
    }
    buf[j] = 0;
    if (i) {
      logos = 1;
      logos <<= i - 1;
    } else {
      logos = 0;
    }
    islast = i == n - 1;
    Fprintf(fd, L"%'16lu%s %'*ld %3d.%04d%% %s%n", logos, islast ? "+" : "",
            16 - islast, h[i], p / 10000, p % 10000, buf);
  }
}
