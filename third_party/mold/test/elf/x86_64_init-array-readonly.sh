// clang-format off
#!/bin/bash
. $(dirname $0)/common.inc

[ $MACHINE = x86_64 ] || skip

cat <<EOF | $CC -c -o $t/a.o -x assembler -
.section .init_array,"aw",@init_array
.p2align 3
.globl init1
.quad init1
EOF

# GNU assembler automatically turns on W bit if the section name
# is `.init_array`, so avoid using that name in assembly.
cat <<EOF | $CC -c -o $t/b.o -x assembler -
.section .init_xxxxx,"a",@progbits
.p2align 3
.globl init2
.quad init2
EOF

sed -i'' -e 's/init_xxxxx/init_array/g' $t/b.o

cat <<EOF | $CC -c -o $t/c.o -xc -
#include "libc/calls/calls.h"
#include "libc/calls/weirdtypes.h"
#include "libc/fmt/fmt.h"
#include "libc/stdio/dprintf.h"
#include "libc/stdio/stdio.h"
#include "libc/stdio/temp.h"
#include "third_party/musl/tempnam.h"

void init1() { printf("init1 "); }
void init2() { printf("init2 "); }

int main() {
  return 0;
}
EOF

$CC -B. -o $t/exe $t/a.o $t/b.o $t/c.o
$QEMU $t/exe | grep -q 'init1 init2'
