// clang-format off
#!/bin/bash
. $(dirname $0)/common.inc

cat <<EOF | $CC -shared -o $t/a.so -xc -
void fn() {}
EOF

cat <<EOF | $CC -o $t/b.o -c -xc -fno-PIC -
#include "libc/calls/calls.h"
#include "libc/calls/weirdtypes.h"
#include "libc/fmt/fmt.h"
#include "libc/stdio/dprintf.h"
#include "libc/stdio/stdio.h"
#include "libc/stdio/temp.h"
#include "third_party/musl/tempnam.h"

typedef void Func();

void fn();
Func *const ptr = fn;

int main() {
  printf("%d\n", fn == ptr);
}
EOF

$CC -B. -o $t/exe -no-pie $t/b.o $t/a.so
$QEMU $t/exe | grep -q 1
