// clang-format off
#!/bin/bash
. $(dirname $0)/common.inc

cat <<EOF | $CC -o $t/a.o -c -xc - -fPIC
#include "libc/calls/calls.h"
#include "libc/calls/weirdtypes.h"
#include "libc/fmt/fmt.h"
#include "libc/stdio/dprintf.h"
#include "libc/stdio/stdio.h"
#include "libc/stdio/temp.h"
#include "third_party/musl/tempnam.h"
extern char *msg;
void hello() { printf("%s\n", msg); }
EOF

cat <<EOF | $CC -o $t/b.o -c -xc - -fPIC
char *msg = "Hello world";
void hello();
int main() { hello(); }
EOF

$CC -B. -o $t/exe $t/a.o $t/b.o
$QEMU $t/exe | grep -q 'Hello world'
