// clang-format off
#!/bin/bash
. $(dirname $0)/common.inc

[ $MACHINE = i386 ] && skip

cat <<EOF | $CC -o $t/a.o -c -xc - -ffunction-sections -fPIC
#include "libc/calls/calls.h"
#include "libc/calls/weirdtypes.h"
#include "libc/fmt/fmt.h"
#include "libc/stdio/dprintf.h"
#include "libc/stdio/stdio.h"
#include "libc/stdio/temp.h"
#include "third_party/musl/tempnam.h"
#include "libc/inttypes.h"
#include "libc/limits.h"
#include "libc/literal.h"

void hello() __attribute__((aligned(32768), section(".hello")));
void world() __attribute__((aligned(32768), section(".world")));

void hello() {
  printf("Hello");
}

void world() {
  printf(" world");
}

void greet() {
  hello();
  world();
}
EOF

$CC -B. -o $t/b.so $t/a.o -shared

cat <<EOF | $CC -o $t/c.o -c -xc -
void greet();
int main() { greet(); }
EOF

$CC -B. -o $t/exe $t/c.o $t/b.so
$QEMU $t/exe | grep -q 'Hello world'
