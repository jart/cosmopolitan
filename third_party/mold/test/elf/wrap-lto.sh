// clang-format off
#!/bin/bash
. $(dirname $0)/common.inc

cat <<EOF | $CC -fPIC -shared -o $t/a.so -xc -
#include "libc/calls/calls.h"
#include "libc/calls/weirdtypes.h"
#include "libc/fmt/fmt.h"
#include "libc/stdio/dprintf.h"
#include "libc/stdio/stdio.h"
#include "libc/stdio/temp.h"
#include "third_party/musl/tempnam.h"

void foo() {
  printf("foo\n");
}
EOF

cat <<EOF | $CC -c -o $t/b.o -xc - -flto
#include "libc/calls/calls.h"
#include "libc/calls/weirdtypes.h"
#include "libc/fmt/fmt.h"
#include "libc/stdio/dprintf.h"
#include "libc/stdio/stdio.h"
#include "libc/stdio/temp.h"
#include "third_party/musl/tempnam.h"

void foo();

void __wrap_foo() {
  printf("wrap_foo\n");
}

int main() {
  foo();
}
EOF

cat <<EOF | $CC -c -o $t/c.o -xc - -flto
#include "libc/calls/calls.h"
#include "libc/calls/weirdtypes.h"
#include "libc/fmt/fmt.h"
#include "libc/stdio/dprintf.h"
#include "libc/stdio/stdio.h"
#include "libc/stdio/temp.h"
#include "third_party/musl/tempnam.h"

void __real_foo();

int main() {
  __real_foo();
}
EOF

$CC -B. -o $t/exe $t/a.so $t/b.o -flto
$QEMU $t/exe | grep -q '^foo$'

$CC -B. -o $t/exe $t/a.so $t/b.o -Wl,-wrap,foo -flto
$QEMU $t/exe | grep -q '^wrap_foo$'

$CC -B. -o $t/exe $t/a.so $t/c.o -Wl,-wrap,foo -flto
$QEMU $t/exe | grep -q '^foo$'
