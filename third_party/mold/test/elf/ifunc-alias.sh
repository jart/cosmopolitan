// clang-format off
#!/bin/bash
. $(dirname $0)/common.inc

supports_ifunc || skip

cat <<EOF | $CC -fPIC -o $t/a.o -c -xc -
#include "libc/calls/calls.h"
#include "libc/calls/weirdtypes.h"
#include "libc/fmt/fmt.h"
#include "libc/stdio/dprintf.h"
#include "libc/stdio/stdio.h"
#include "libc/stdio/temp.h"
#include "third_party/musl/tempnam.h"

void foo() {}
int bar() __attribute__((ifunc("resolve_bar")));
void *resolve_bar() { return foo; }
void *bar2 = bar;

int main() {
  printf("%p %p\n", bar, bar2);
}
EOF

$CC -B. -o $t/exe $t/a.o
$QEMU $t/exe | grep -Eq '^(\S+) \1$'
