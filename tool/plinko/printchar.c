#/*─────────────────────────────────────────────────────────────────╗
 │ To the extent possible under law, Justin Tunney has waived       │
 │ all copyright and related or neighboring rights to this file,    │
 │ as it is written in the following disclaimers:                   │
 │   • http://unlicense.org/                                        │
 │   • http://creativecommons.org/publicdomain/zero/1.0/            │
 ╚─────────────────────────────────────────────────────────────────*/
#include <wchar.h>
#include "char.h"
#include "plinko.h"

int PrintChar(int fd, wchar_t c) {
  FILE *f;
  static FILE *last;
  f = fd == 2 ? stderr : stdout;
  if (f != last) {
    // keep stdout and stderr in order when they go to the same place
    if (last)
      fflush(last);
    last = f;
  }
  fputwc(c, f);
  return GetMonospaceCharacterWidth(c);
}
