// clang-format off
#!/bin/bash
. $(dirname $0)/common.inc

cat <<EOF | $CC -c -o $t/a.o -fPIC -xc -
int foo = 4;

int get_foo() { return foo; }
void *bar() { return bar; }
EOF

$CC -B. -shared -o $t/b.so $t/a.o -Wl,-Bsymbolic-functions

cat <<EOF | $CC -c -o $t/c.o -xc - -fno-PIE
#include "libc/calls/calls.h"
#include "libc/calls/weirdtypes.h"
#include "libc/fmt/fmt.h"
#include "libc/stdio/dprintf.h"
#include "libc/stdio/stdio.h"
#include "libc/stdio/temp.h"
#include "third_party/musl/tempnam.h"

int foo = 3;
int x = 5;
int get_foo();
void *bar() { return &x; }

int main() {
  printf("%d %d %d\n", foo, get_foo(), bar == bar());
}
EOF

$CC -B. -no-pie -o $t/exe $t/c.o $t/b.so
$QEMU $t/exe | grep -q '3 3 0'
