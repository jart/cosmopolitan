// clang-format off
#!/bin/bash
. $(dirname $0)/common.inc

cat <<EOF | $CC -shared -fPIC -o $t/libfoo.so -Wl,--soname,libfoo.so -xc -
int foo() { return 3; }
EOF

cat <<EOF | $CC -shared -fPIC -o $t/libbar.so -Wl,--soname,libbar.so -xc -
int bar() { return 3; }
EOF

cat <<EOF | $CC -fPIC -c -o $t/a.o -xc -
int foo();
int baz() { return foo(); }
EOF

$CC -B. -shared -o $t/libbaz.so -Wl,--soname,libbaz.so -L$t $t/a.o -lfoo

cat <<EOF | $CC -c -o $t/b.o -xc -
#include "libc/calls/calls.h"
#include "libc/calls/weirdtypes.h"
#include "libc/fmt/fmt.h"
#include "libc/stdio/dprintf.h"
#include "libc/stdio/stdio.h"
#include "libc/stdio/temp.h"
#include "third_party/musl/tempnam.h"
int baz();
int main() {
  printf("%d\n", baz());
}
EOF

$CC -B. -o $t/exe $t/b.o -L$t -Wl,--as-needed -lbaz -lbar -lfoo

readelf --dynamic $t/exe > $t/log
grep -q libbaz $t/log || false
! grep -q libbar $t/log || false
grep -q libfoo $t/log || false
