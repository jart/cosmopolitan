// clang-format off
#!/bin/bash
. $(dirname $0)/common.inc

cat <<EOF | $CC -c -o $t/a.o -xassembler -
.globl foo
.weak bar
foo:
  la a0, bar
  ret
EOF

cat <<EOF | $CC -c -o $t/b.o -xc -
#include "libc/calls/calls.h"
#include "libc/calls/weirdtypes.h"
#include "libc/fmt/fmt.h"
#include "libc/stdio/dprintf.h"
#include "libc/stdio/stdio.h"
#include "libc/stdio/temp.h"
#include "third_party/musl/tempnam.h"
long foo();
int main() { printf("%ld\n", foo()); }
EOF

$CC -B. -static -o $t/exe $t/a.o $t/b.o
$QEMU $t/exe | grep -q '^0$'
