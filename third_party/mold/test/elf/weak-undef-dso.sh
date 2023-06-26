// clang-format off
#!/bin/bash
. $(dirname $0)/common.inc

cat <<EOF | $CC -c -o $t/a.o -fPIC -xc -
__attribute__((weak)) int foo();
int bar() { return foo ? foo() : -1; }
EOF

$CC -B. -shared -o $t/b.so $t/a.o

cat <<EOF | $CC -xc -c -o $t/c.o -
#include "libc/calls/calls.h"
#include "libc/calls/weirdtypes.h"
#include "libc/fmt/fmt.h"
#include "libc/stdio/dprintf.h"
#include "libc/stdio/stdio.h"
#include "libc/stdio/temp.h"
#include "third_party/musl/tempnam.h"
int bar();
int main() { printf("bar=%d\n", bar()); }
EOF

$CC -B. -o $t/exe1 $t/c.o $t/b.so
$QEMU $t/exe1 | grep -q 'bar=-1'

cat <<EOF | $CC -xc -c -o $t/d.o -
#include "libc/calls/calls.h"
#include "libc/calls/weirdtypes.h"
#include "libc/fmt/fmt.h"
#include "libc/stdio/dprintf.h"
#include "libc/stdio/stdio.h"
#include "libc/stdio/temp.h"
#include "third_party/musl/tempnam.h"
int foo() { return 5; }
int bar();
int main() { printf("bar=%d\n", bar()); }
EOF

$CC -B. -o $t/exe2 $t/d.o $t/b.so
$QEMU $t/exe2 | grep -q 'bar=5'
