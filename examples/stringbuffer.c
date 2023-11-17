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
#include "libc/log/check.h"
#include "libc/mem/mem.h"
#include "libc/stdio/append.h"
#include "libc/str/str.h"

/**
 * @fileoverview Fast Growable Strings Tutorial
 */

int main(int argc, char *argv[]) {
  char *b = 0;
  appendf(&b, "hello ");  // guarantees nul terminator
  CHECK_EQ(6, strlen(b));
  CHECK_EQ(6, appendz(b).i);
  appendf(&b, " world\n");
  CHECK_EQ(13, strlen(b));
  CHECK_EQ(13, appendz(b).i);
  appendd(&b, "\0", 1);  // supports binary
  CHECK_EQ(13, strlen(b));
  CHECK_EQ(14, appendz(b).i);
  appendf(&b, "%d arg%s\n", argc, argc == 1 ? "" : "s");
  appendf(&b, "%s\n", "have a nice day");
  write(1, b, appendz(b).i);
  free(b);
  return 0;
}
