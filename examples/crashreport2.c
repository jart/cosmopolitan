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
#include "libc/calls/struct/sigset.h"
#include "libc/log/log.h"
#include "libc/stdio/stdio.h"

/**
 * @fileoverview CTRL+\ debugging example
 *
 *     make -j8 -O o//examples/crashreport2.com
 *     o//examples/crashreport2.com
 *
 * Assuming you call ShowCrashReports() from main(), you can press
 * `CTRL+\` at anny time to generate a `SIGQUIT` message that lets you
 * debug wrongness and freezups.
 *
 * On supported platforms, this will cause GDB to automatically attach.
 * The nice thing about this, is you can start stepping through your
 * code at the precise instruction where the interrupt happened. See
 * `libc/log/attachdebugger.c` to see how it works.
 *
 * If you wish to suppress the auto-GDB behavior, then:
 *
 *     export GDB=
 *
 * Or alternatively:
 *
 *    extern int __isworker;
 *    __isworker = true;
 *
 * Will cause your `SIGQUIT` handler to just print a crash report
 * instead. This is useful for production software that might be running
 * in a terminal environment like GNU Screen, but it's not desirable to
 * have ten worker processes trying to attach GDB at once.
 */

int main(int argc, char *argv[]) {
  volatile int64_t x;
  ShowCrashReports();
  printf("please press ctrl+\\ and see what happens...\n");
  sigsuspend(0);
  printf("\n\n");
  printf("congratulations! your program is now resuming\n");
  return 0;
}
