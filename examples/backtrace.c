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
#include "libc/log/backtrace.h"
#include "libc/log/log.h"
#include "libc/mem/mem.h"
#include "libc/runtime/gc.h"
#include "libc/runtime/runtime.h"
#include "libc/runtime/symbols.h"
#include "libc/stdio/stdio.h"
#include "libc/sysv/consts/fileno.h"

/*

  Your executables will try to find the addr2line tool, which is needed
  to decrypt gcc debug information, enabling backtraces like this:

    0x0000000000403a7a: Hello at examples/backtrace.c:31
    0x0000000000403ac3: World at examples/backtrace.c:38
    0x0000000000401608: main at examples/backtrace.c:42
    0x0000000000401379: _start at libc/crt/crt.S:61

  If that doesn't work, then your αcτµαlly pδrταblε εxεcµταblε will use
  its own builtin fallback solution:

    0000ac2fa7e0 000000403a9b Hello+0x49
    0000ac2fa7f0 000000403ac4 World+0x22
    0000ac2fa800 000000401609 main+0x7
    0000ac2fa810 00000040137a _start+0x63

  This is guaranteed to work if the .com.dbg file can be found.
  Otherwise it'll print numbers:

    0000268a8390 000000403a90 (null)+0x403a8f
    0000268a83a0 000000403aef (null)+0x403aee
    0000268a83b0 0000004015fe (null)+0x4015fd
    0000268a83c0 00000040137a (null)+0x401379

*/

void hello(void) {
  gc(malloc(1));
  ShowBacktrace(STDOUT_FILENO, NULL);
  setenv("ADDR2LINE", "", true);
  ShowBacktrace(STDOUT_FILENO, NULL);
}

void world(void) {
  gc(malloc(1));
  hello();
}

int main(int argc, char *argv[]) {
  world();
  return 0;
}
