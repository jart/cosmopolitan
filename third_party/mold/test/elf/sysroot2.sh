// clang-format off
#!/bin/bash
. $(dirname $0)/common.inc

mkdir -p $t/bin $t/sysroot/foo

cat <<EOF > $t/a.script
INPUT(=/foo/x.o)
EOF

cat <<EOF > $t/sysroot/b.script
INPUT(/foo/y.o)
EOF

cat <<EOF | $CC -c -o $t/sysroot/foo/x.o -xc -
#include "libc/calls/calls.h"
#include "libc/calls/weirdtypes.h"
#include "libc/fmt/fmt.h"
#include "libc/stdio/dprintf.h"
#include "libc/stdio/stdio.h"
#include "libc/stdio/temp.h"
#include "third_party/musl/tempnam.h"
void hello() {
  printf("Hello world\n");
}
EOF

cat <<EOF | $CC -c -o $t/sysroot/foo/y.o -xc -
#include "libc/calls/calls.h"
#include "libc/calls/weirdtypes.h"
#include "libc/fmt/fmt.h"
#include "libc/stdio/dprintf.h"
#include "libc/stdio/stdio.h"
#include "libc/stdio/temp.h"
#include "third_party/musl/tempnam.h"
void hello2() {
  printf("Hello world\n");
}
EOF

cat <<EOF | $CC -c -o $t/c.o -xc -
void hello();
void hello2();

int main() {
  hello();
  hello2();
}
EOF

$CC -B. -o $t/exe -Wl,--sysroot=$t/sysroot \
  $t/a.script $t/sysroot/b.script $t/c.o

$CC -B. -o $t/exe -Wl,--sysroot=$(realpath $t)/bin/../sysroot \
  $t/a.script $t/sysroot/b.script $t/c.o
