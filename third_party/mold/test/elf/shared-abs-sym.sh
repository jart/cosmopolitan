// clang-format off
#!/bin/bash
. $(dirname $0)/common.inc

cat <<EOF | $CC -fPIC -c -o $t/a.o -xassembler -
.globl foo
foo = 3;
EOF

$CC -B. -shared -o $t/b.so $t/a.o

cat <<EOF > $t/c.c
#include "libc/calls/calls.h"
#include "libc/calls/weirdtypes.h"
#include "libc/fmt/fmt.h"
#include "libc/stdio/dprintf.h"
#include "libc/stdio/stdio.h"
#include "libc/stdio/temp.h"
#include "third_party/musl/tempnam.h"
extern char foo;
int main() { printf("foo=%p\n", &foo); }
EOF

$CC -fPIC -c -o $t/d.o $t/c.c
$CC -B. -o $t/exe1 -pie $t/d.o $t/b.so
$QEMU $t/exe1 | grep -q 'foo=0x3'

nm -D $t/exe1 > $t/log1
! grep -q foo $t/log1 || false

$CC -fPIC -c -o $t/e.o $t/c.c
$CC -B. -o $t/exe2 -no-pie $t/e.o $t/b.so
$QEMU $t/exe2 | grep -q 'foo=0x3'

nm -D $t/exe2 > $t/log2
! grep -q foo $t/log2 || false
