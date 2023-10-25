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
#include "libc/calls/termios.h"
#include "libc/isystem/unistd.h"
#include "libc/math.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/termios.h"

#ifdef __x86_64__

/**
 * @fileoverview Demo of program crash reporting with Bare Metal VGA TTY.
 *
 *     make -j8 o//examples/vga2.com
 *     qemu-system-x86_64 -hda o//examples/vga2.com -serial stdio
 */

__static_yoink("vga_console");
__static_yoink("_idt");
__static_yoink("_AcpiMadtFlags");
__static_yoink("_AcpiBootFlags");
__static_yoink("EfiMain");

int main(int argc, char *argv[]) {
  int i;
  volatile int x = 1;
  volatile int y = 2;
  printf("argc = %d\n", argc);
  for (i = 0; i < argc; ++i) {
    printf("argv[%d] = \"%s\"\n", i, argv[i]);
  }
  printf("\e[92;44mHello World!\e[0m %d\n", 1 / (x + y - 3));
  for (;;)
    ;
}

#endif /* __x86_64__ */
