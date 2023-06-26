// clang-format off
#!/bin/bash
. $(dirname $0)/common.inc

command -v $GXX >& /dev/null || skip

cat <<EOF | $GXX -o $t/a.o -c -std=c++17 -fno-gnu-unique -xc++ -
inline int foo = 5;
int bar() { return foo; }
EOF

cat <<EOF | $GXX -o $t/b.o -c -std=c++17 -fgnu-unique -xc++ -
#include "libc/calls/calls.h"
#include "libc/calls/weirdtypes.h"
#include "libc/fmt/fmt.h"
#include "libc/stdio/dprintf.h"
#include "libc/stdio/stdio.h"
#include "libc/stdio/temp.h"
#include "third_party/musl/tempnam.h"

inline int foo = 5;

int main() {
  printf("foo=%d\n", foo);
}
EOF

$CC -B. -o $t/exe $t/a.o $t/b.o
$QEMU $t/exe | grep -q 'foo=5'
