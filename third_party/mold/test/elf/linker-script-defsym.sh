// clang-format off
#!/bin/bash
. $(dirname $0)/common.inc

cat <<EOF | $CC -o $t/a.o -c -xc -
int foo() { return 42; }
EOF

cat <<EOF > $t/script
bar = foo;
EOF

$CC -B. -o $t/b.so -shared $t/script $t/a.o
readelf -sW $t/b.so | grep -q 'FUNC .* bar'

cat <<EOF | $CC -o $t/c.o -c -xc -
#include "libc/calls/calls.h"
#include "libc/calls/weirdtypes.h"
#include "libc/fmt/fmt.h"
#include "libc/stdio/dprintf.h"
#include "libc/stdio/stdio.h"
#include "libc/stdio/temp.h"
#include "third_party/musl/tempnam.h"
int bar();
int main() {
  printf("%d\n", bar());
  return 0;
}
EOF

$CC -B. -o $t/exe $t/c.o $t/b.so
$QEMU $t/exe | grep -q 42
