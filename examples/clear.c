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

// clears teletypewriter display
//
//   - \e[H moves to top left of display
//   - \e[J erases whole display forward

int main(int argc, char *argv[]) {
  write(1, "\e[H\e[J", 6);
}
