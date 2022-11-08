#if 0
/*─────────────────────────────────────────────────────────────────╗
│ To the extent possible under law, Justine Tunney has waived      │
│ all copyright and related or neighboring rights to this file,    │
│ as it is written in the following disclaimers:                   │
│   • http://unlicense.org/                                        │
│   • http://creativecommons.org/publicdomain/zero/1.0/            │
╚─────────────────────────────────────────────────────────────────*/
#endif
#include "libc/calls/calls.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/time/time.h"
#include "libc/x/x.h"

/**
 * 16kb sleep executable that runs on all operating systems.
 * https://justine.lol/cosmopolitan/demos/sleep.c
 * https://justine.lol/cosmopolitan/demos/sleep.com
 * https://justine.lol/cosmopolitan/index.html
 */

#define WRITE(s) write(2, s, strlen(s))

int main(int argc, char *argv[]) {
  char *p;
  long double x, y;
  if (argc != 2) {
    WRITE("Usage: ");
    WRITE(argv[0]);
    WRITE(" SECONDS\n");
    exit(1);
  }
  x = 0;
  for (p = argv[1]; isdigit(*p); ++p) {
    x *= 10;
    x += *p - '0';
  }
  if (*p == '.') {
    y = 1;
    for (++p; isdigit(*p); ++p) {
      x += (*p - '0') * (y /= 10);
    }
  }
  switch (*p) {
    case 'm':
      x *= 60;
      break;
    case 'h':
      x *= 60 * 60;
      break;
    case 'd':
      x *= 60 * 60 * 24;
      break;
    default:
      break;
  }
  dsleep(x);
  return 0;
}
