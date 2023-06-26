// clang-format off
#!/bin/bash
. $(dirname $0)/common.inc

[ $MACHINE = m68k ] && skip
[ $MACHINE = ppc ] && skip

command -v llvm-readelf >& /dev/null || skip

cat <<EOF | $CC -o $t/a.o -fPIC -c -xc -
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

$CC -B. -o $t/exe1 $t/a.o -pie
llvm-readelf -r $t/exe1 | grep RELATIVE | wc -l > $t/log1

$CC -B. -o $t/exe2 $t/a.o -pie -Wl,-pack-dyn-relocs=relr
llvm-readelf -r $t/exe2 | grep RELATIVE | wc -l > $t/log2

diff $t/log1 $t/log2

llvm-readelf --dynamic $t/exe2 > $t/log3
grep -wq RELR $t/log3
grep -wq RELRSZ $t/log3
grep -wq RELRENT $t/log3
