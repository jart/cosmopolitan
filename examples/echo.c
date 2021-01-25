#if 0
/*─────────────────────────────────────────────────────────────────╗
│ To the extent possible under law, Justine Tunney has waived      │
│ all copyright and related or neighboring rights to this file,    │
│ as it is written in the following disclaimers:                   │
│   • http://unlicense.org/                                        │
│   • http://creativecommons.org/publicdomain/zero/1.0/            │
╚─────────────────────────────────────────────────────────────────*/
#endif
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"

int main(int argc, char *argv[]) {
  int i, j;
  bool wantnewline;
  if (argc > 1 && !strcmp(argv[1], "-n")) {
    i = 2;
    wantnewline = false;
  } else {
    i = 1;
    wantnewline = true;
  }
  for (j = 0; i + j < argc; ++j) {
    if (j) fputc(' ', stdout);
    fputs(argv[i + j], stdout);
  }
  if (wantnewline) fputc('\n', stdout);
  return 0;
}
