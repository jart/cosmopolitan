// clang-format off
#!/bin/bash
. $(dirname $0)/common.inc

cat <<EOF | $CC -c -o $t/a.o -xc -
#include "libc/calls/calls.h"
#include "libc/calls/weirdtypes.h"
#include "libc/fmt/fmt.h"
#include "libc/stdio/dprintf.h"
#include "libc/stdio/stdio.h"
#include "libc/stdio/temp.h"
#include "third_party/musl/tempnam.h"

__attribute__((weak)) int foo();
int bar();

int main() {
  bar();
  printf("%d\n", foo ? foo() : -1);
}
EOF

cat <<EOF | $CC -c -o $t/b.o -xc -
#include "libc/calls/calls.h"
#include "libc/calls/weirdtypes.h"
#include "libc/fmt/fmt.h"
#include "libc/stdio/dprintf.h"
#include "libc/stdio/stdio.h"
#include "libc/stdio/temp.h"
#include "third_party/musl/tempnam.h"

__attribute__((weak)) int foo();

int main() {
  printf("%d\n", foo ? foo() : -1);
}
EOF

cat <<EOF | $CC -fcommon -xc -c -o $t/c.o -
int foo() { return 2; }
void bar() {}
EOF

ar rcs $t/d.a $t/c.o

$CC -B. -o $t/exe1 $t/a.o $t/d.a
$CC -B. -o $t/exe2 $t/b.o $t/d.a

$QEMU $t/exe1 | grep -q '^2$'
$QEMU $t/exe2 | grep -q '^-1$'
