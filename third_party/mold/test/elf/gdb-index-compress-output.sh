// clang-format off
#!/bin/bash
. $(dirname $0)/common.inc

[ $MACHINE = $HOST ] || skip
[ $MACHINE = riscv64 -o $MACHINE = riscv32 -o $MACHINE = sparc64 ] && skip

command -v gdb >& /dev/null || skip

cat <<EOF | $CC -c -o $t/a.o -fPIC -g -ggnu-pubnames -gdwarf-4 -xc - -ffunction-sections
#include "libc/calls/calls.h"
#include "libc/calls/weirdtypes.h"
#include "libc/fmt/fmt.h"
#include "libc/stdio/dprintf.h"
#include "libc/stdio/stdio.h"
#include "libc/stdio/temp.h"
#include "third_party/musl/tempnam.h"

void trap() {}

static void hello() {
  printf("Hello world\n");
  trap();
}

void greet() {
  hello();
}
EOF

$CC -B. -shared -o $t/b.so $t/a.o -Wl,--gdb-index -Wl,--compress-debug-sections=zlib-gabi
readelf -WS $t/b.so 2> /dev/null | grep -Fq .gdb_index

cat <<EOF | $CC -c -o $t/c.o -fPIC -g -ggnu-pubnames -gdwarf-4 -xc - -gz
void greet();

int main() {
  greet();
}
EOF

$CC -B. -o $t/exe $t/b.so $t/c.o -Wl,--gdb-index -Wl,--compress-debug-sections=zlib
readelf -WS $t/exe 2> /dev/null | grep -Fq .gdb_index

$QEMU $t/exe | grep -q 'Hello world'

DEBUGINFOD_URLS= gdb $t/exe -nx -batch -ex 'b main' -ex r -ex 'b trap' \
  -ex c -ex bt -ex quit >& $t/log

grep -q 'hello () at .*<stdin>:7' $t/log
grep -q 'greet () at .*<stdin>:11' $t/log
grep -q 'main () at .*<stdin>:4' $t/log
