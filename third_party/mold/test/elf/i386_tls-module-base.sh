// clang-format off
#!/bin/bash
. $(dirname $0)/common.inc

[ $MACHINE = i386 ] || skip

cat <<'EOF' | $CC -fPIC -o $t/a.o -c -xassembler -
.globl get_foo
.type get_foo, @function
get_foo:
  push %ebx
  call __x86.get_pc_thunk.bx
1:
  addl $_GLOBAL_OFFSET_TABLE_ - 1b, %ebx
  lea _TLS_MODULE_BASE_@TLSDESC(%ebx), %eax
  call *_TLS_MODULE_BASE_@TLSCALL(%eax)
  lea foo@dtpoff(%eax), %eax
  mov %gs:(%eax), %eax
  pop %ebx
  ret
.section .tdata, "awT", @progbits
foo:
.long 20
.section .note.GNU-stack, "", @progbits
EOF

cat <<EOF | $CC -fPIC -o $t/b.o -c -xc -
_Thread_local int bar = 3;
EOF

cat <<EOF | $CC -fPIC -o $t/c.o -c -xc -
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

$CC -o $t/exe1 $t/a.o $t/b.o $t/c.o -pie
$QEMU $t/exe1 | grep -q '^20 3$'

$CC -o $t/exe2 $t/a.o $t/b.o $t/c.o -Wl,-no-relax -pie
$QEMU $t/exe2 | grep -q '^20 3$'

$CC -o $t/d.so $t/a.o -shared
$CC -o $t/exe3 $t/b.o $t/c.o $t/d.so -pie
$QEMU $t/exe3 | grep -q '^20 3$'
