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
#include "libc/str/str.h"
#include "libc/sysv/consts/fileno.h"

#define kMessage "hello \e[1mworld\e[0m\r\n"

int main() {
  /*
   * Cosmopolitan "Hello World" using system calls.
   *
   * Another Cosmopolitan best practice is to use the standard symbolic
   * names for file descriptors 0, 1, and 2. This is a good idea because
   * on Windows the numbers are actually different. Because Cosmopolitan
   * is a zero-emulation library, we address that problem by turning
   * traditional #define's into variables, which the runtime determines
   * automatically.
   */
  return write(STDOUT_FILENO, kMessage, strlen(kMessage)) != -1 ? 0 : 1;
}
