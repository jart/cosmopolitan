// clang-format off
#!/bin/bash
. $(dirname $0)/common.inc

[ $MACHINE = riscv64 -o $MACHINE = riscv32 ] && skip

# musl libc does not support init/fini on ARM
# https://github.com/rui314/mold/issues/951
[ $MACHINE = arm -o $MACHINE = aarch64 ] && ldd --help 2>&1 | grep -q musl && skip

cat <<EOF | $CC -c -fPIC -o $t/a.o -xc -
void keep();

int main() {
  keep();
}
EOF

cat <<EOF | $CC -c -fPIC -o $t/b.o -xc -
#include "libc/calls/calls.h"
#include "libc/calls/weirdtypes.h"
#include "libc/fmt/fmt.h"
#include "libc/stdio/dprintf.h"
#include "libc/stdio/stdio.h"
#include "libc/stdio/temp.h"
#include "third_party/musl/tempnam.h"

void init() {
  printf("init\n");
}

void fini() {
  printf("fini\n");
}

void keep() {}
EOF

$CC -B. -o $t/c.so -shared $t/b.o
$CC -B. -o $t/d.so -shared $t/b.o -Wl,-init,init -Wl,-fini,fini

$CC -B. -o $t/exe1 $t/a.o $t/c.so
$CC -B. -o $t/exe2 $t/a.o $t/d.so

$QEMU $t/exe1 > $t/log1
$QEMU $t/exe2 > $t/log2

! grep -q init $t/log1 || false
! grep -q fini $t/log1 || false

grep -q init $t/log2
grep -q fini $t/log2
