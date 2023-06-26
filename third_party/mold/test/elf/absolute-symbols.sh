// clang-format off
#!/bin/bash
. $(dirname $0)/common.inc

# This test crashes only on qemu-sparc64 running on GitHub Actions,
# even though it works on a local x86-64 machine and on an actual
# SPARC machine.
[ $MACHINE = sparc64 ] && skip

cat <<EOF | $CC -o $t/a.o -c -x assembler -
.globl foo
foo = 0x800008
EOF

cat <<EOF | $CC -o $t/b.o -c -fno-PIC -xc -
#define _GNU_SOURCE 1
#include "libc/calls/calls.h"
#include "libc/calls/sigtimedwait.h"
#include "libc/calls/struct/sigaction.h"
#include "libc/calls/struct/siginfo.h"
#include "libc/sysv/consts/sa.h"
#include "libc/sysv/consts/sicode.h"
#include "libc/sysv/consts/ss.h"
#include "libc/calls/calls.h"
#include "libc/calls/weirdtypes.h"
#include "libc/fmt/fmt.h"
#include "libc/stdio/dprintf.h"
#include "libc/stdio/stdio.h"
#include "libc/stdio/temp.h"
#include "third_party/musl/tempnam.h"
#include "libc/calls/calls.h"
#include "libc/calls/termios.h"
#include "libc/fmt/conv.h"
#include "libc/limits.h"
#include "libc/mem/alg.h"
#include "libc/mem/alloca.h"
#include "libc/mem/mem.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/dprintf.h"
#include "libc/stdio/rand.h"
#include "libc/stdio/temp.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/exit.h"
#include "third_party/getopt/getopt.h"
#include "third_party/musl/crypt.h"
#include "third_party/musl/rand48.h"
// MISSING #include <ucontext.h>

void handler(int signum, siginfo_t *info, void *ptr) {
  printf("ip=%p\n", info->si_addr);
  exit(0);
}

extern volatile int foo;

int main() {
  struct sigaction act;
  act.sa_flags = SA_SIGINFO | SA_RESETHAND;
  act.sa_sigaction = handler;
  sigemptyset(&act.sa_mask);
  sigaction(SIGSEGV, &act, 0);
  foo = 5;
}
EOF

$CC -B. -o $t/exe -no-pie $t/a.o $t/b.o
$QEMU $t/exe | grep -q '^ip=0x80000.$'
