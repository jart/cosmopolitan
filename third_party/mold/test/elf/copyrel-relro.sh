// clang-format off
#!/bin/bash
. $(dirname $0)/common.inc

cat <<EOF | $CC -o $t/a.o -c -xc -fno-PIE -
#include "libc/runtime/runtime.h"
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

extern const char readonly[100];
extern char readwrite[100];

static int segv = 0;
static jmp_buf buf;

void handler(int sig) {
  segv = 1;
  longjmp(buf, 1);
}

int main() {
  signal(SIGSEGV, handler);

  readwrite[0] = 5;
  int x = segv;

  if (setjmp(buf) == 0)
    *(char *)readonly = 5;
  int y = segv;

  printf("sigsegv %d %d\n", x, y);
}
EOF

cat <<EOF | $CC -fPIC -shared -o $t/b.so -xc -
const char readonly[100] = "abc";
char readwrite[100] = "abc";
EOF

$CC -B. $t/a.o $t/b.so -o $t/exe -no-pie
$QEMU $t/exe | grep -q '^sigsegv 0 1$'
