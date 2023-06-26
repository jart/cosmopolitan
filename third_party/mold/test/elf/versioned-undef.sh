// clang-format off
#!/bin/bash
. $(dirname $0)/common.inc

# Skip if libc is musl because musl does not fully support GNU-style
# symbol versioning.
ldd --help 2>&1 | grep -q musl && skip

cat <<EOF | $CC -fPIC -c -o $t/a.o -xc -
int foo1() { return 1; }
int foo2() { return 2; }
int foo3() { return 3; }

__asm__(".symver foo1, foo@VER1");
__asm__(".symver foo2, foo@VER2");
__asm__(".symver foo3, foo@@VER3");
EOF

echo 'VER1 { local: *; }; VER2 { local: *; }; VER3 { local: *; };' > $t/b.ver
$CC -B. -shared -o $t/c.so $t/a.o -Wl,--version-script=$t/b.ver

cat <<EOF | $CC -c -o $t/d.o -xc -
#include "libc/calls/calls.h"
#include "libc/calls/weirdtypes.h"
#include "libc/fmt/fmt.h"
#include "libc/stdio/dprintf.h"
#include "libc/stdio/stdio.h"
#include "libc/stdio/temp.h"
#include "third_party/musl/tempnam.h"

int foo1();
int foo2();
int foo3();
int foo();

__asm__(".symver foo1, foo@VER1");
__asm__(".symver foo2, foo@VER2");
__asm__(".symver foo3, foo@VER3");

int main() {
  printf("%d %d %d %d\n", foo1(), foo2(), foo3(), foo());
}
EOF

$CC -B. -o $t/exe $t/d.o $t/c.so
$QEMU $t/exe | grep -q '^1 2 3 3$'
