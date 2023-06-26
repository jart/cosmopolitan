// clang-format off
#!/bin/bash
. $(dirname $0)/common.inc

# ARM assembler has a differnet grammar than the others.
# Concretely speaking, ARM as uses "@" as a start of a comment.
[ $MACHINE = arm ] && skip

cat <<'EOF' | $CC -c -o $t/a.o -x assembler -
.globl val1, val2, val3, val4, val5

.section .rodata.str1.1,"aMS",@progbits,1
val1:
.ascii "Hello \0"

.section .rodata.str4.4,"aMS",@progbits,4
.align 4
val2:
.ascii "world   \0\0\0\0"

.section .rodata.foo,"aMS",@progbits,1
val3:
.ascii "foobar\0"

.section .rodata.cst8,"aM",@progbits,8
.align 8
val4:
.ascii "abcdefgh"

.section .rodatabar,"aMS",@progbits,1
val5:
.ascii "bar\0"
EOF

cat <<'EOF' | $CC -c -o $t/b.o -xc -
#include "libc/calls/calls.h"
#include "libc/calls/weirdtypes.h"
#include "libc/fmt/fmt.h"
#include "libc/stdio/dprintf.h"
#include "libc/stdio/stdio.h"
#include "libc/stdio/temp.h"
#include "third_party/musl/tempnam.h"

extern char val1, val2, val3, val4, val5;

int main() {
  printf("%p %p %p %p %p\n", &val1, &val2, &val3, &val4, &val5);
}
EOF

$CC -B. -o $t/exe $t/a.o $t/b.o

readelf -p .rodata.str $t/exe | grep -q Hello
readelf -p .rodata.str $t/exe | grep -q world
readelf -p .rodata.str $t/exe | grep -q foobar
readelf -p .rodata.cst $t/exe | grep -q abcdefgh
readelf -p .rodatabar $t/exe | grep -q bar
