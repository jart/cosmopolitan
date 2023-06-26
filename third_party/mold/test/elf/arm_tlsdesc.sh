// clang-format off
#!/bin/bash
. $(dirname $0)/common.inc

[ $MACHINE = arm ] || skip

echo 'int main() {}' | $GCC -c -o /dev/null -xc - -O0 -mthumb >& /dev/null \
  || skip

cat <<EOF > $t/a.c
extern _Thread_local int foo;

__attribute__((section(".low")))
int get_foo() {
  int y = foo;
  return y;
}

static _Thread_local int bar = 5;

__attribute__((section(".high")))
int get_bar() {
  return bar;
}
EOF

cat <<EOF > $t/b.c
#include "libc/calls/calls.h"
#include "libc/calls/weirdtypes.h"
#include "libc/fmt/fmt.h"
#include "libc/stdio/dprintf.h"
#include "libc/stdio/stdio.h"
#include "libc/stdio/temp.h"
#include "third_party/musl/tempnam.h"

_Thread_local int foo;

int get_foo();
int get_bar();

int main() {
  foo = 42;
  printf("%d %d\n", get_foo(), get_bar());
  return 0;
}
EOF

$GCC -fPIC -mtls-dialect=gnu2 -c -o $t/c.o $t/a.c -marm
$GCC -fPIC -mtls-dialect=gnu2 -c -o $t/d.o $t/b.c -marm

$CC -B. -o $t/exe1 $t/c.o $t/d.o
$QEMU $t/exe1 | grep -q '42 5'

$CC -B. -o $t/exe2 $t/c.o $t/d.o -Wl,-no-relax
$QEMU $t/exe2 | grep -q '42 5'

$CC -B. -o $t/exe3 $t/c.o $t/d.o -Wl,-no-relax \
  -Wl,--section-start=.low=0x10000000,--section-start=.high=0x20000000
$QEMU $t/exe3 | grep -q '42 5'

$GCC -fPIC -mtls-dialect=gnu2 -c -o $t/e.o $t/a.c -mthumb
$GCC -fPIC -mtls-dialect=gnu2 -c -o $t/f.o $t/b.c -mthumb

$CC -B. -o $t/exe4 $t/e.o $t/f.o
$QEMU $t/exe4 | grep -q '42 5'

$CC -B. -o $t/exe5 $t/e.o $t/f.o -Wl,-no-relax
$QEMU $t/exe5 | grep -q '42 5'

$CC -B. -o $t/exe6 $t/e.o $t/f.o -Wl,-no-relax \
  -Wl,--section-start=.low=0x10000000,--section-start=.high=0x20000000
$QEMU $t/exe6 | grep -q '42 5'
