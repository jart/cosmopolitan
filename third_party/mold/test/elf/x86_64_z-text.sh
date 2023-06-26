// clang-format off
#!/bin/bash
. $(dirname $0)/common.inc

# Skip if libc is musl
ldd --help 2>&1 | grep -q musl && skip

# Skip if target is not x86-64
[ $MACHINE = x86_64 ] || skip

cat <<'EOF' | $CC -c -o $t/a.o -x assembler -
.globl fn1
fn1:
  sub $8, %rsp
  movabs ptr, %rax
  call *%rax
  add $8, %rsp
  ret
EOF

cat <<EOF | $CC -c -o $t/b.o -fPIC -xc -
#include "libc/calls/calls.h"
#include "libc/calls/weirdtypes.h"
#include "libc/fmt/fmt.h"
#include "libc/stdio/dprintf.h"
#include "libc/stdio/stdio.h"
#include "libc/stdio/temp.h"
#include "third_party/musl/tempnam.h"

int fn1();

int fn2() {
  return 3;
}

void *ptr = fn2;

int main() {
  printf("%d\n", fn1());
}
EOF

$CC -B. -pie -o $t/exe $t/a.o $t/b.o
$QEMU $t/exe | grep -q 3

readelf --dynamic $t/exe | grep -Fq '(TEXTREL)'
readelf --dynamic $t/exe | grep -q '\(FLAGS\).*TEXTREL'
