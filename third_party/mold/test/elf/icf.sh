// clang-format off
#!/bin/bash
. $(dirname $0)/common.inc

# On PPC64V1, function pointers refer function descriptors in .opd
# instead of directly referring .text section. We create a .opd entry
# for each symbol. So function pointer comparison on two different
# symbols are always the same, even if their function body are at the
# same location.
[ $MACHINE = ppc64 ] && skip

cat <<EOF | $CC -c -o $t/a.o -ffunction-sections -fdata-sections -xc -
#include "libc/calls/calls.h"
#include "libc/calls/weirdtypes.h"
#include "libc/fmt/fmt.h"
#include "libc/stdio/dprintf.h"
#include "libc/stdio/stdio.h"
#include "libc/stdio/temp.h"
#include "third_party/musl/tempnam.h"

int bar() {
  return 5;
}

int foo1(int x) {
  return bar() + x;
}

int foo2(int x) {
  return bar() + x;
}

int foo3() {
  bar();
  return 5;
}

int main() {
  printf("%d %d\n", (long)foo1 == (long)foo2, (long)foo1 == (long)foo3);
  return 0;
}
EOF

$CC -B. -o $t/exe $t/a.o -Wl,-icf=all
$QEMU $t/exe | grep -q '1 0'
