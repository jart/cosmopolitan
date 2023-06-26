// clang-format off
#!/bin/bash
. $(dirname $0)/common.inc

test_cflags -static || skip

cat <<EOF | $CC -o $t/a.o -c -xc -
#include "libc/calls/calls.h"
#include "libc/calls/weirdtypes.h"
#include "libc/fmt/fmt.h"
#include "libc/stdio/dprintf.h"
#include "libc/stdio/stdio.h"
#include "libc/stdio/temp.h"
#include "third_party/musl/tempnam.h"

__attribute__((aligned(512)))
char hello[] = "Hello";

__attribute__((aligned(512)))
char world[] = "world";

int main() {
  printf("%s %s\n", hello, world);
}
EOF

$CC -B. -static -Wl,--filler,0xfe -o $t/exe1 $t/a.o
sed -i -e 's/--filler 0xfe/--filler 0x00/' $t/exe1
od -x $t/exe1 > $t/txt1

$CC -B. -static -Wl,--filler,0x00 -o $t/exe2 $t/a.o
od -x $t/exe2 > $t/txt2

diff -q $t/txt1 $t/txt2
