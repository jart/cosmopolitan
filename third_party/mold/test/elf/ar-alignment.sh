// clang-format off
#!/bin/bash
. $(dirname $0)/common.inc

cat <<EOF | $CC -o $t/a.o -c -xc -
int two() { return 2; }
EOF

head -c 1 /dev/zero >> $t/a.o

cat <<EOF | $CC -o $t/b.o -c -xc -
int three() { return 3; }
EOF

cat <<EOF | $CC -o $t/c.o -c -xc -
#include "libc/calls/calls.h"
#include "libc/calls/weirdtypes.h"
#include "libc/fmt/fmt.h"
#include "libc/stdio/dprintf.h"
#include "libc/stdio/stdio.h"
#include "libc/stdio/temp.h"
#include "third_party/musl/tempnam.h"

int two();
int three();

int main() {
  printf("%d\n", two() + three());
}
EOF

rm -f $t/d.a
ar rcs $t/d.a $t/a.o $t/b.o

$CC -B. -o $t/exe $t/c.o $t/d.a
