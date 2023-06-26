// clang-format off
#!/bin/bash
. $(dirname $0)/common.inc

[ $MACHINE = alpha ] && skip

# glibc 2.22 or prior have a bug that ld-linux.so.2 crashes on dlopen()
# if .rela.dyn and .rela.plt are not contiguous in a given DSO.
# This test verifies that these sections are contiguous in mold's output.

cat <<EOF | $CC -o $t/a.o -fPIC -c -xc -
#include "libc/calls/calls.h"
#include "libc/calls/weirdtypes.h"
#include "libc/fmt/fmt.h"
#include "libc/stdio/dprintf.h"
#include "libc/stdio/stdio.h"
#include "libc/stdio/temp.h"
#include "third_party/musl/tempnam.h"
int main() {
  printf("Hello world\n");
}
EOF

$CC -B. -o $t/b.so -shared $t/a.o
readelf -W --sections $t/b.so | grep -E -A1 '\.rela?\.dyn' | \
  grep -Eq '\.rela?\.plt'
