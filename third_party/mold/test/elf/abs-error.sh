// clang-format off
#!/bin/bash
. $(dirname $0)/common.inc

[ $MACHINE = aarch64 ] && skip
[ $MACHINE = ppc64 ] && skip
[ $MACHINE = ppc64le ] && skip
[ $MACHINE = s390x ] && skip
[ $MACHINE = alpha ] && skip

cat <<EOF | $CC -fPIC -c -o $t/a.o -xassembler -
.globl foo
foo = 3;
EOF

cat <<EOF | $CC -fno-PIC -c -o $t/b.o -xc -
#include "libc/calls/calls.h"
#include "libc/calls/weirdtypes.h"
#include "libc/fmt/fmt.h"
#include "libc/stdio/dprintf.h"
#include "libc/stdio/stdio.h"
#include "libc/stdio/temp.h"
#include "third_party/musl/tempnam.h"
extern char foo;
int main() { printf("foo=%p\n", &foo); }
EOF

! $CC -B. -o $t/exe -pie $t/a.o $t/b.o -Wl,-z,text >& $t/log
grep -q 'recompile with -fPIC' $t/log
