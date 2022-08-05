#if 0
/*─────────────────────────────────────────────────────────────────╗
│ To the extent possible under law, Justine Tunney has waived      │
│ all copyright and related or neighboring rights to this file,    │
│ as it is written in the following disclaimers:                   │
│   • http://unlicense.org/                                        │
│   • http://creativecommons.org/publicdomain/zero/1.0/            │
╚─────────────────────────────────────────────────────────────────*/
#endif
#include "libc/assert.h"
#include "libc/calls/struct/timeval.h"
#include "libc/stdio/stdio.h"
#include "libc/time/struct/tm.h"
#include "libc/time/time.h"
#include "net/http/http.h"

int main(int argc, char *argv[]) {
  int rc;
  int64_t t;
  char p[30];
  struct tm tm;
  struct timeval tv;
  rc = gettimeofday(&tv, 0);
  assert(!rc);
  t = tv.tv_sec;
  gmtime_r(&t, &tm);
  FormatHttpDateTime(p, &tm);
  printf("%s\n", p);
}
