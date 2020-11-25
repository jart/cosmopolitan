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
#include "libc/log/color.internal.h"
#include "libc/log/log.h"
#include "libc/mem/mem.h"
#include "libc/runtime/gc.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/exit.h"
#include "libc/sysv/consts/fileno.h"
#include "libc/sysv/consts/sig.h"

#define kTutorialMessage "This echos stdio until Ctrl+C is pressed.\n"
#define kVictoryMessage  "\rGot Ctrl+C and longjmp() ran dtors (>'.')>\n"

jmp_buf jb;

void GotCtrlC(int sig) {
  gclongjmp(jb, 1);
  unreachable;
}

size_t HowManyBytesOfHeapMemoryAreAllocated(void) {
  return mallinfo().uordblks;
}

void ReadAndPrintLinesLoop(void) {
  ssize_t got;
  unsigned char *buf;
  buf = gc(malloc(BUFSIZ));
  CHECK_NE(0, HowManyBytesOfHeapMemoryAreAllocated());
  for (;;) {
    CHECK_NE(-1, (got = read(STDIN_FILENO, buf, BUFSIZ)));
    CHECK_EQ(got, write(STDOUT_FILENO, buf, got));
  }
}

int main(int argc, char *argv[]) {
  int rc;
  rc = 0;
  showcrashreports();
  if (cancolor()) {
    CHECK_EQ(0 /* cosmo runtime doesn't link malloc */,
             HowManyBytesOfHeapMemoryAreAllocated());
    puts("This echos stdio until Ctrl+C is pressed.");
    if (!(rc = setjmp(jb))) {
      CHECK_NE(SIG_ERR, signal(SIGINT, GotCtrlC));
      ReadAndPrintLinesLoop();
      unreachable;
    } else {
      --rc;
    }
    CHECK_EQ(0, HowManyBytesOfHeapMemoryAreAllocated());
    puts("\rGot Ctrl+C and longjmp() ran dtors (>'.')>");
  }
  return rc;
}
