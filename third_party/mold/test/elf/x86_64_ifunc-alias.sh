// clang-format off
#!/bin/bash
. $(dirname $0)/common.inc

supports_ifunc || skip
test_cflags -static || skip

cat <<EOF | $CXX -o $t/a.o -c -xc++ - -fno-PIE
#include "libc/assert.h"
#include "libc/calls/calls.h"
#include "libc/calls/weirdtypes.h"
#include "libc/fmt/fmt.h"
#include "libc/stdio/dprintf.h"
#include "libc/stdio/stdio.h"
#include "libc/stdio/temp.h"
#include "third_party/musl/tempnam.h"

__attribute__((target("default")))
int foo() {
  return 0;
}

__attribute__((target("ssse3,avx2")))
int foo() {
  return 1;
}

int (*p)() = foo;

int main() {
  int val = foo();
  assert(val == p());
}
EOF

$CXX -B. -o $t/exe $t/a.o -static
$QEMU $t/exe
