// clang-format off
#!/bin/bash
. $(dirname $0)/common.inc

[ $MACHINE = $(uname -m) ] || skip

echo 'int main() {}' | clang -flto -o /dev/null -xc - >& /dev/null \
  || skip

cat <<EOF | clang -flto -c -o $t/a.o -xc -
#include "libc/calls/calls.h"
#include "libc/calls/weirdtypes.h"
#include "libc/fmt/fmt.h"
#include "libc/stdio/dprintf.h"
#include "libc/stdio/stdio.h"
#include "libc/stdio/temp.h"
#include "third_party/musl/tempnam.h"
int main() {
  printf("Hello world\n");
}
EOF

clang -B. -o $t/exe -flto $t/a.o
$t/exe | grep -q 'Hello world'
