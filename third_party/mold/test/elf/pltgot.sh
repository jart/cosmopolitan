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

void ignore(void *foo) {}

void hello() {
  printf("Hello world\n");
}
EOF

$CC -B. -shared -o $t/b.so $t/a.o

cat <<EOF | $CC -o $t/c.o -c -xc - -fPIC
void ignore(void *);
int hello();

void foo() { ignore(hello); }

int main() { hello(); }
EOF

$CC -B. -o $t/exe $t/c.o $t/b.so
$QEMU $t/exe | grep -q 'Hello world'
