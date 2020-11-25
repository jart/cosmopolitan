#if 0
/*─────────────────────────────────────────────────────────────────╗
│ To the extent possible under law, Justine Tunney has waived      │
│ all copyright and related or neighboring rights to this file,    │
│ as it is written in the following disclaimers:                   │
│   • http://unlicense.org/                                        │
│   • http://creativecommons.org/publicdomain/zero/1.0/            │
╚─────────────────────────────────────────────────────────────────*/
#endif
#include "libc/nt/enum/mb.h"
#include "libc/nt/messagebox.h"

int main(int argc, char *argv[]) {
  return MessageBox(0, u"hello world", u"cosmopolitan", kNtMbOk) ? 0 : 1;
}
