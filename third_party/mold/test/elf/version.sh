// clang-format off
#!/bin/bash
. $(dirname $0)/common.inc

# OneTBB isn't tsan-clean
nm mold | grep -q '__tsan_init' && skip

./mold -v | grep -q '[ms]old .*compatible with GNU ld'
./mold --version | grep -q '[ms]old .*compatible with GNU ld'

./mold -V | grep -q '[ms]old .*compatible with GNU ld'
./mold -V | grep -q elf_x86_64
./mold -V | grep -q elf_i386

cat <<EOF | $CC -c -xc -o $t/a.o -
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

rm -f $t/exe
$CC -B. -Wl,--version -o $t/exe1 $t/a.o 2>&1 | grep -q '[ms]old'
! [ -f $t/exe1 ] || false

$CC -B. -Wl,-v -o $t/exe2 $t/a.o 2>&1 | grep -q '[ms]old'
$QEMU $t/exe2 | grep -q 'Hello world'

! ./mold --v >& $t/log
grep -q 'unknown command line option:' $t/log
