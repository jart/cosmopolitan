// clang-format off
#!/bin/bash
. $(dirname $0)/common.inc

cat <<EOF | $GCC -c -o $t/a.o -xc -
#include "libc/calls/calls.h"
#include "libc/calls/weirdtypes.h"
#include "libc/fmt/fmt.h"
#include "libc/stdio/dprintf.h"
#include "libc/stdio/stdio.h"
#include "libc/stdio/temp.h"
#include "third_party/musl/tempnam.h"

__attribute__((section(".gnu.warning.foo")))
static const char foo[] = "foo is deprecated";

__attribute__((section(".gnu.warning.bar")))
const char bar[] = "bar is deprecated";

int main() {
  printf("Hello world\n");
}
EOF

# Make sure that we do not copy .gnu.warning.* sections.
$CC -B. -o $t/exe $t/a.o
$QEMU $t/exe | grep -q 'Hello world'
