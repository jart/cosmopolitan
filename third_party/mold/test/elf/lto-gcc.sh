// clang-format off
#!/bin/bash
. $(dirname $0)/common.inc

echo 'int main() {}' | $GCC -flto -o /dev/null -xc - >& /dev/null \
  || skip

cat <<EOF | $GCC -flto -c -o $t/a.o -xc -
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

$GCC -B. -o $t/exe1 -flto $t/a.o
$QEMU $t/exe1 | grep -q 'Hello world'

# Test that LTO is used for FAT LTO objects
cat <<EOF | $GCC -flto -ffat-lto-objects -c -o $t/b.o -xc -
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

$GCC -B. -o $t/exe2 $t/b.o --verbose 2>&1 | grep -q -- -fwpa

# Test FAT objects if -fno-use-linker-plugin is used

cat <<EOF | $GCC -flto -fno-use-linker-plugin -c -o $t/c.o -xc -
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

$GCC -B. -o $t/exe3 -flto -fno-use-linker-plugin $t/c.o
$QEMU $t/exe3 | grep -q 'Hello world'
