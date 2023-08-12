// clang-format off
#!/bin/bash
. $(dirname $0)/common.inc

cat <<EOF | $CC -O2 -o $t/a.o -c -xc -
int add1(int n) { return n + 1; }
EOF

cat <<EOF | $CC -O2 -o $t/b.o -c -xc -
int add1(int n);
int add2(int n) { n += 1; return add1(n); }
EOF

cat <<EOF | $CC -O2 -o $t/c.o -c -xc -
#include "libc/calls/calls.h"
#include "libc/calls/weirdtypes.h"
#include "libc/fmt/fmt.h"
#include "libc/stdio/dprintf.h"
#include "libc/stdio/stdio.h"
#include "libc/stdio/temp.h"
#include "third_party/musl/tempnam.h"
int add2(int n);

int main() {
  printf("%d\n", add2(40));
  return 0;
}
EOF

$CC -B. -o $t/exe $t/a.o $t/b.o $t/c.o
$QEMU $t/exe | grep -q '42'

if [ $MACHINE = riscv32 -o $MACHINE = riscv64 ]; then
  $OBJDUMP -d $t/exe | grep -q bfed # c.j pc - 6
fi
