// clang-format off
#!/bin/bash
. $(dirname $0)/common.inc

cat <<EOF | $CC -fPIC -ftls-model=initial-exec -c -o $t/a.o -xc -
#include "libc/calls/calls.h"
#include "libc/calls/weirdtypes.h"
#include "libc/fmt/fmt.h"
#include "libc/stdio/dprintf.h"
#include "libc/stdio/stdio.h"
#include "libc/stdio/temp.h"
#include "third_party/musl/tempnam.h"
static _Thread_local int foo = 5;
int bar() { return foo; }
EOF

$CC -B. -shared -o $t/b.so $t/a.o -Wl,--relax
readelf --dynamic $t/b.so | grep -q STATIC_TLS

$CC -B. -shared -o $t/c.so $t/a.o -Wl,--no-relax
readelf --dynamic $t/c.so | grep -q STATIC_TLS
