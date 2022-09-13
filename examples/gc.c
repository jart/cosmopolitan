#if 0
/*─────────────────────────────────────────────────────────────────╗
│ To the extent possible under law, Justine Tunney has waived      │
│ all copyright and related or neighboring rights to this file,    │
│ as it is written in the following disclaimers:                   │
│   • http://unlicense.org/                                        │
│   • http://creativecommons.org/publicdomain/zero/1.0/            │
╚─────────────────────────────────────────────────────────────────*/
#endif
#include "libc/mem/gc.h"
#include "libc/mem/mem.h"
#include "libc/str/str.h"

int main(int argc, char *argv[]) {
  char *p = _gc(malloc(64));
  strcpy(p, "this memory is free'd when main() returns");
}
