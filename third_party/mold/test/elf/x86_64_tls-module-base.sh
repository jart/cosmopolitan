// clang-format off
#!/bin/bash
. $(dirname $0)/common.inc

[ $MACHINE = x86_64 ] || skip

cat <<EOF | $CC -fPIC -o $t/a.o -c -xassembler -
.globl get_foo
.type get_foo, @function
get_foo:
  lea _TLS_MODULE_BASE_@TLSDESC(%rip), %rax
  call *_TLS_MODULE_BASE_@TLSCALL(%rax)
  lea foo@dtpoff(%rax), %rax
  mov %fs:(%rax), %eax
  ret
.section .tdata, "awT", @progbits
foo:
.long 20
EOF

cat <<EOF | $CC -o $t/b.o -c -xc -
_Thread_local int bar = 3;
EOF

cat <<EOF | $CC -o $t/c.o -c -xc -
#include "libc/calls/calls.h"
#include "libc/calls/weirdtypes.h"
#include "libc/fmt/fmt.h"
#include "libc/stdio/dprintf.h"
#include "libc/stdio/stdio.h"
#include "libc/stdio/temp.h"
#include "third_party/musl/tempnam.h"

int get_foo();
extern _Thread_local int bar;

int main() {
  printf("%d %d\n", get_foo(), bar);
}
EOF

$CC -B. -o $t/exe1 $t/a.o $t/b.o $t/c.o
$QEMU $t/exe1 | grep -q '^20 3$'

$CC -B. -o $t/exe2 $t/a.o $t/b.o $t/c.o -Wl,-no-relax
$QEMU $t/exe2 | grep -q '^20 3$'

$CC -B. -o $t/d.so $t/a.o -shared
$CC -B. -o $t/exe3 $t/b.o $t/c.o $t/d.so
$QEMU $t/exe3 | grep -q '^20 3$'
