// clang-format off
#!/bin/bash
. $(dirname $0)/common.inc

test_cflags -static || skip

if [ $MACHINE = x86_64 -o $MACHINE = arm ]; then
  dialect=gnu2
elif [ $MACHINE = aarch64 ]; then
  dialect=desc
else
  skip
fi

cat <<EOF | $GCC -fPIC -mtls-dialect=$dialect -c -o $t/a.o -xc -
#include "libc/calls/calls.h"
#include "libc/calls/weirdtypes.h"
#include "libc/fmt/fmt.h"
#include "libc/stdio/dprintf.h"
#include "libc/stdio/stdio.h"
#include "libc/stdio/temp.h"
#include "third_party/musl/tempnam.h"

extern _Thread_local int foo;

int main() {
  foo = 42;
  printf("%d\n", foo);
}
EOF

cat <<EOF | $GCC -fPIC -mtls-dialect=$dialect -c -o $t/b.o -xc -
_Thread_local int foo;
EOF

$CC -B. -o $t/exe $t/a.o $t/b.o -static
$QEMU $t/exe | grep -q 42

$CC -B. -o $t/exe $t/a.o $t/b.o -static -Wl,-no-relax
$QEMU $t/exe | grep -q 42
