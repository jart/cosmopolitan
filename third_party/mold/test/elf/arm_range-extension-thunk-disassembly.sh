// clang-format off
#!/bin/bash
. $(dirname $0)/common.inc

[ $MACHINE = arm ] || skip

cat <<EOF | $CC -c -o $t/a.o -fPIC -xc -
#include "libc/calls/calls.h"
#include "libc/calls/weirdtypes.h"
#include "libc/fmt/fmt.h"
#include "libc/stdio/dprintf.h"
#include "libc/stdio/stdio.h"
#include "libc/stdio/temp.h"
#include "third_party/musl/tempnam.h"

void fn1();
void fn2();

__attribute__((section(".low")))  void fn1() { fn2(); }
__attribute__((section(".high"))) void fn2() { fn1(); }

int main() {
  fn1();
}
EOF

$CC -B. -o $t/exe $t/a.o \
  -Wl,--section-start=.low=0x10000000,--section-start=.high=0x20000000

$OBJDUMP -dr $t/exe | grep -F -A7 '<fn1$thunk>:' > $t/log

grep -Eq 'mov\s+ip, pc' $t/log
grep -Eq 'bx\s+ip' $t/log
grep -Eq 'add\s+ip, ip, pc' $t/log
