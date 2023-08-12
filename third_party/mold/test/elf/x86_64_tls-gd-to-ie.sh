// clang-format off
#!/bin/bash
. $(dirname $0)/common.inc

cat <<EOF | $GCC -fPIC -c -o $t/a.o -xc - -mcmodel=large
#include "libc/calls/calls.h"
#include "libc/calls/weirdtypes.h"
#include "libc/fmt/fmt.h"
#include "libc/stdio/dprintf.h"
#include "libc/stdio/stdio.h"
#include "libc/stdio/temp.h"
#include "third_party/musl/tempnam.h"

__attribute__((tls_model("global-dynamic"))) static _Thread_local int x1 = 1;
__attribute__((tls_model("global-dynamic"))) _Thread_local int x2 = 2;
__attribute__((tls_model("global-dynamic"))) _Thread_local int x3;

int foo() {
  x3 = 3;

  printf("%d %d %d\n", x1, x2, x3);
  return 0;
}
EOF

cat <<EOF | $CC -fPIC -c -o $t/b.o -xc -
int foo();
int main() { foo(); }
EOF

$CC -B. -shared -o $t/c.so $t/a.o
$CC -B. -o $t/exe1 $t/b.o $t/c.so
$QEMU $t/exe1 | grep -q '1 2 3'

$CC -B. -shared -o $t/d.so $t/a.o -Wl,-no-relax
$CC -B. -o $t/exe2 $t/b.o $t/d.so
$QEMU $t/exe2 | grep -q '1 2 3'

$CC -B. -shared -o $t/e.so $t/a.o -Wl,-z,nodlopen
$CC -B. -o $t/exe3 $t/b.o $t/e.so
$QEMU $t/exe3 | grep -q '1 2 3'

$CC -B. -shared -o $t/f.so $t/a.o -Wl,-z,nodlopen -Wl,-no-relax
$CC -B. -o $t/exe4 $t/b.o $t/f.so
$QEMU $t/exe4 | grep -q '1 2 3'
