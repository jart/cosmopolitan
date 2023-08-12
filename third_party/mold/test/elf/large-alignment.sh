// clang-format off
#!/bin/bash
. $(dirname $0)/common.inc

[ $MACHINE = i386 ] && skip

cat <<EOF | $CC -o $t/a.o -c -xc - -ffunction-sections
#include "libc/calls/calls.h"
#include "libc/calls/weirdtypes.h"
#include "libc/fmt/fmt.h"
#include "libc/stdio/dprintf.h"
#include "libc/stdio/stdio.h"
#include "libc/stdio/temp.h"
#include "third_party/musl/tempnam.h"
#include "libc/inttypes.h"
#include "libc/limits.h"
#include "libc/literal.h"

void hello() __attribute__((aligned(32768), section(".hello")));
void world() __attribute__((aligned(32768), section(".world")));

void hello() {
  printf("Hello");
}

void world() {
  printf(" world");
}

int main() {
  hello();
  world();

  // Linux kernel may ignore a riduculously large alignment requirement,
  // but we still want to verify that an executable with a large
  // alignment requirement can still run.
  //
  // printf(" %lu %lu\n",
  //       (unsigned long)((uintptr_t)hello % 32768),
  //       (unsigned long)((uintptr_t)world % 32768));
}
EOF

$CC -B. -o $t/exe $t/a.o
$QEMU $t/exe | grep -q 'Hello world'
