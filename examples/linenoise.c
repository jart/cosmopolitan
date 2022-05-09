#if 0
/*─────────────────────────────────────────────────────────────────╗
│ To the extent possible under law, Justine Tunney has waived      │
│ all copyright and related or neighboring rights to this file,    │
│ as it is written in the following disclaimers:                   │
│   • http://unlicense.org/                                        │
│   • http://creativecommons.org/publicdomain/zero/1.0/            │
╚─────────────────────────────────────────────────────────────────*/
#endif
#include "libc/mem/mem.h"
#include "libc/stdio/stdio.h"
#include "third_party/linenoise/linenoise.h"

int main(int argc, char *argv[]) {
  char *line;
  while ((line = linenoiseWithHistory("IN> ", "foo"))) {
    fputs("OUT> ", stdout);
    fputs(line, stdout);
    fputs("\n", stdout);
    free(line);
  }
  return 0;
}
