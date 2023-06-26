// clang-format off
#!/bin/bash
. $(dirname $0)/common.inc
exit

[ $MACHINE = aarch64 ] && skip

cat <<EOF | $CC -fno-PIC -c -o $t/a.o -xc -
#include "libc/calls/calls.h"
#include "libc/calls/weirdtypes.h"
#include "libc/fmt/fmt.h"
#include "libc/stdio/dprintf.h"
#include "libc/stdio/stdio.h"
#include "libc/stdio/temp.h"
#include "third_party/musl/tempnam.h"

int foo;
int * const bar = &foo;

int main() {
  printf("%d\n", *bar);
}
EOF

! $CC -B. -o $t/exe $t/a.o -pie >& $t/log
grep -Eq 'relocation against symbol .+ can not be used; recompile with -fPIC' $t/log
