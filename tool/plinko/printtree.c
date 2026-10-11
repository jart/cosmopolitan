#/*─────────────────────────────────────────────────────────────────╗
 │ To the extent possible under law, Justin Tunney has waived       │
 │ all copyright and related or neighboring rights to this file,    │
 │ as it is written in the following disclaimers:                   │
 │   • http://unlicense.org/                                        │
 │   • http://creativecommons.org/publicdomain/zero/1.0/            │
 ╚─────────────────────────────────────────────────────────────────*/
#include "print.h"
#include "printf.h"
#include "tree.h"

void PrintTree(int fd, int N, int n) {
  if (N >= 0) {
    Print(fd, N);
  } else {
    Fnprintf(fd, n, L"%s %S%n%I", Red(N) ? L"RED" : L"BLK", Key(Ent(N)));
    PrintIndent(fd, n);
    Fnprintf(fd, n + 2, L"%I- %T%n%I", Lit(N));
    PrintIndent(fd, n);
    Fnprintf(fd, n + 2, L"%I- %T", Rit(N));
  }
}
