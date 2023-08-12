// clang-format off
#!/bin/bash
. $(dirname $0)/common.inc

# On PPC64V1, function pointers do not refer function entry addresses
# but instead refers "function descriptors" in .opd.
[ $MACHINE = ppc64 ] && skip

[ $MACHINE = arm ] && flags=-marm

cat <<EOF | $CC -o $t/a.o -c -xc -fno-PIC $flags -
#include "libc/calls/calls.h"
#include "libc/calls/weirdtypes.h"
#include "libc/fmt/fmt.h"
#include "libc/stdio/dprintf.h"
#include "libc/stdio/stdio.h"
#include "libc/stdio/temp.h"
#include "third_party/musl/tempnam.h"

__attribute__((section(".fn1"))) void fn1() { printf(" fn1"); }
__attribute__((section(".fn2"))) void fn2() { printf(" fn2"); }

int main() {
  printf("main");
  fn1();
  fn2();
  printf(" %p %p\n", fn1, fn2);
}
EOF

$CC -B. -o $t/exe1 $t/a.o -no-pie \
  -Wl,--section-start=.fn1=0x10000000,--section-start=.fn2=0x20000000
$QEMU $t/exe1 | grep -q 'main fn1 fn2 0x10000000 0x20000000'

# PT_LOAD must be sorted on p_vaddr
readelf -W --segments $t/exe1 | grep ' LOAD ' | sed 's/0x[0-9a-f]*//' > $t/log1
diff $t/log1 <(sort $t/log1)

$CC -B. -o $t/exe2 $t/a.o -no-pie \
  -Wl,--section-start=.fn1=0x20000000,--section-start=.fn2=0x10000000
$QEMU $t/exe2 | grep -q 'main fn1 fn2 0x20000000 0x10000000'

readelf -W --segments $t/exe2 | grep ' LOAD ' | sed 's/0x[0-9a-f]*//' > $t/log2
diff $t/log2 <(sort $t/log2)
