// clang-format off
#!/bin/bash
. $(dirname $0)/common.inc

cat <<EOF > $t/a.cc
#include "libc/calls/calls.h"
#include "libc/calls/weirdtypes.h"
#include "libc/fmt/fmt.h"
#include "libc/stdio/dprintf.h"
#include "libc/stdio/stdio.h"
#include "libc/stdio/temp.h"
#include "third_party/musl/tempnam.h"

int two() { return 2; }

int live_var1 = 1;
int live_var2 = two();
int dead_var1 = 3;
int dead_var2 = 4;

void live_fn1() {}
void live_fn2() { live_fn1(); }
void dead_fn1() {}
void dead_fn2() { dead_fn1(); }

int main() {
  printf("%d %d\n", live_var1, live_var2);
  live_fn2();
}
EOF

$CXX -B. -o $t/exe1 $t/a.cc -ffunction-sections -fdata-sections

readelf --symbols $t/exe1 > $t/log.1
grep -qv live_fn1 $t/log.1
grep -qv live_fn2 $t/log.1
grep -qv dead_fn1 $t/log.1
grep -qv dead_fn2 $t/log.1
grep -qv live_var1 $t/log.1
grep -qv live_var2 $t/log.1
grep -qv dead_var1 $t/log.1
grep -qv dead_var2 $t/log.1
$QEMU $t/exe1 | grep -q '1 2'

$CXX -B. -o $t/exe2 $t/a.cc -ffunction-sections -fdata-sections -Wl,-gc-sections

readelf --symbols $t/exe2 > $t/log.2
grep -q  live_fn1 $t/log.2
grep -q  live_fn2 $t/log.2
grep -qv dead_fn1 $t/log.2
grep -qv dead_fn2 $t/log.2
grep -q  live_var1 $t/log.2
grep -q  live_var2 $t/log.2
grep -qv dead_var1 $t/log.2
grep -qv dead_var2 $t/log.2
$QEMU $t/exe2 | grep -q '1 2'
