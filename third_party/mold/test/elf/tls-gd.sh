// clang-format off
#!/bin/bash
. $(dirname $0)/common.inc

cat <<EOF | $GCC -fPIC -c -o $t/a.o -xc -
#include "libc/calls/calls.h"
#include "libc/calls/weirdtypes.h"
#include "libc/fmt/fmt.h"
#include "libc/stdio/dprintf.h"
#include "libc/stdio/stdio.h"
#include "libc/stdio/temp.h"
#include "third_party/musl/tempnam.h"

__attribute__((tls_model("global-dynamic"))) static _Thread_local int x1 = 1;
__attribute__((tls_model("global-dynamic"))) static _Thread_local int x2;
__attribute__((tls_model("global-dynamic"))) extern _Thread_local int x3;
__attribute__((tls_model("global-dynamic"))) extern _Thread_local int x4;

int get_x5();
int get_x6();

int main() {
  x2 = 2;

  printf("%d %d %d %d %d %d\n", x1, x2, x3, x4, get_x5(), get_x6());
  return 0;
}
EOF

cat <<EOF | $GCC -fPIC -c -o $t/b.o -xc -
__attribute__((tls_model("global-dynamic"))) _Thread_local int x3 = 3;
__attribute__((tls_model("global-dynamic"))) static _Thread_local int x5 = 5;
int get_x5() { return x5; }
EOF


cat <<EOF | $GCC -fPIC -c -o $t/c.o -xc -
__attribute__((tls_model("global-dynamic"))) _Thread_local int x4 = 4;
__attribute__((tls_model("global-dynamic"))) static _Thread_local int x6 = 6;
int get_x6() { return x6; }
EOF

$CC -B. -shared -o $t/d.so $t/b.o
$CC -B. -shared -o $t/e.so $t/c.o -Wl,--no-relax

$CC -B. -o $t/exe1 $t/a.o $t/d.so $t/e.so
$QEMU $t/exe1 | grep -q '1 2 3 4 5 6'

$CC -B. -o $t/exe2 $t/a.o $t/d.so $t/e.so -Wl,-no-relax
$QEMU $t/exe2 | grep -q '1 2 3 4 5 6'

if test_cflags -static; then
  $CC -B. -o $t/exe3 $t/a.o $t/b.o $t/c.o -static
  $QEMU $t/exe3 | grep -q '1 2 3 4 5 6'

  $CC -B. -o $t/exe4 $t/a.o $t/b.o $t/c.o -static -Wl,-no-relax
  $QEMU $t/exe4 | grep -q '1 2 3 4 5 6'
fi
