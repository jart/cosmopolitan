// clang-format off
#!/bin/bash
. $(dirname $0)/common.inc

cat <<EOF | $CC -o $t/a.o -c -xc - -fPIC
char foo[4000L * 1000 * 1000];
char bar[1000 * 1000];
EOF

cat <<EOF | $CC -o $t/b.o -c -xc - -fPIC
#include "libc/calls/calls.h"
#include "libc/calls/weirdtypes.h"
#include "libc/fmt/fmt.h"
#include "libc/stdio/dprintf.h"
#include "libc/stdio/stdio.h"
#include "libc/stdio/temp.h"
#include "third_party/musl/tempnam.h"

extern char foo[5000L * 1000 * 1000];
extern char bar[1000 * 1000];

int main() {
  printf("%d %d\n", foo[0], bar[0]);
}
EOF

$CC -B. -o $t/exe $t/a.o $t/b.o

$QEMU $t/exe
