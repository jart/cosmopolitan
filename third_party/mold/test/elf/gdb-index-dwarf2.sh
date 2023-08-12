// clang-format off
#!/bin/bash
. $(dirname $0)/common.inc

[ $MACHINE = $HOST ] || skip
[ $MACHINE = riscv64 -o $MACHINE = riscv32 -o $MACHINE = sparc64 ] && skip

command -v gdb >& /dev/null || skip

echo 'int main() {}' | $CC -o /dev/null -xc -gdwarf-2 -g - >& /dev/null || skip

cat <<EOF | $CC -c -o $t/a.o -fPIC -g -ggnu-pubnames -gdwarf-2 -xc - -ffunction-sections
void hello2();

static void hello() {
  hello2();
}

void greet() {
  hello();
}
EOF

cat <<EOF | $CC -c -o $t/b.o -fPIC -g -ggnu-pubnames -gdwarf-2 -xc - -ffunction-sections
#include "libc/calls/calls.h"
#include "libc/calls/weirdtypes.h"
#include "libc/fmt/fmt.h"
#include "libc/stdio/dprintf.h"
#include "libc/stdio/stdio.h"
#include "libc/stdio/temp.h"
#include "third_party/musl/tempnam.h"

void trap() {}

void hello2() {
  printf("Hello world\n");
  trap();
}
EOF

$CC -B. -shared -o $t/c.so $t/a.o $t/b.o -Wl,--gdb-index
readelf -WS $t/c.so 2> /dev/null | grep -Fq .gdb_index

cat <<EOF | $CC -c -o $t/d.o -fPIC -g -ggnu-pubnames -gdwarf-2 -xc - -gz
void greet();

int main() {
  greet();
}
EOF

$CC -B. -o $t/exe $t/c.so $t/d.o -Wl,--gdb-index
readelf -WS $t/exe 2> /dev/null | grep -Fq .gdb_index

$QEMU $t/exe | grep -q 'Hello world'

DEBUGINFOD_URLS= gdb $t/exe -nx -batch -ex 'b main' -ex r -ex 'b trap' \
  -ex c -ex bt -ex quit >& $t/log

grep -q 'hello2 () at .*<stdin>:7' $t/log
grep -q 'hello () at .*<stdin>:4' $t/log
grep -q 'greet () at .*<stdin>:8' $t/log
grep -q 'main () at .*<stdin>:4' $t/log
