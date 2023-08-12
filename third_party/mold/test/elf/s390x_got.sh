// clang-format off
#!/bin/bash
. $(dirname $0)/common.inc

[ $MACHINE = s390x ] || skip

# GOT[0] must be set to the link-time address of .dynamic on s390x.

cat <<EOF | $CC -c -fPIC -o $t/a.o -xc -
#include "libc/calls/calls.h"
#include "libc/calls/weirdtypes.h"
#include "libc/fmt/fmt.h"
#include "libc/stdio/dprintf.h"
#include "libc/stdio/stdio.h"
#include "libc/stdio/temp.h"
#include "third_party/musl/tempnam.h"

extern char _DYNAMIC;
extern void *got[];

int main() {
  printf("%p %p\n", &_DYNAMIC, got[0]);
}
EOF

$CC -B. -o $t/exe $t/a.o -Wl,-defsym=got=_GLOBAL_OFFSET_TABLE_ -no-pie
$QEMU $t/exe | grep -Eq '^(\S+) \1$'
