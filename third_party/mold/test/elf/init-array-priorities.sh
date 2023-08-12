// clang-format off
#!/bin/bash
. $(dirname $0)/common.inc

# musl does not support GNU-style init/fini priorities
ldd --help 2>&1 | grep -q musl && skip

cat <<'EOF' | $CC -c -o $t/a.o -xc -
#include "libc/calls/calls.h"
#include "libc/calls/weirdtypes.h"
#include "libc/fmt/fmt.h"
#include "libc/stdio/dprintf.h"
#include "libc/stdio/stdio.h"
#include "libc/stdio/temp.h"
#include "third_party/musl/tempnam.h"
__attribute__((constructor(10000))) void init4() { printf("1"); }
EOF

cat <<'EOF' | $CC -c -o $t/b.o -xc -
#include "libc/calls/calls.h"
#include "libc/calls/weirdtypes.h"
#include "libc/fmt/fmt.h"
#include "libc/stdio/dprintf.h"
#include "libc/stdio/stdio.h"
#include "libc/stdio/temp.h"
#include "third_party/musl/tempnam.h"
__attribute__((constructor(1000))) void init3() { printf("2"); }
EOF

cat <<'EOF' | $CC -c -o $t/c.o -xc -
#include "libc/calls/calls.h"
#include "libc/calls/weirdtypes.h"
#include "libc/fmt/fmt.h"
#include "libc/stdio/dprintf.h"
#include "libc/stdio/stdio.h"
#include "libc/stdio/temp.h"
#include "third_party/musl/tempnam.h"
__attribute__((constructor)) void init1() { printf("3"); }
EOF

cat <<'EOF' | $CC -c -o $t/d.o -xc -
#include "libc/calls/calls.h"
#include "libc/calls/weirdtypes.h"
#include "libc/fmt/fmt.h"
#include "libc/stdio/dprintf.h"
#include "libc/stdio/stdio.h"
#include "libc/stdio/temp.h"
#include "third_party/musl/tempnam.h"
__attribute__((constructor)) void init2() { printf("4"); }
EOF

cat <<'EOF' | $CC -c -o $t/e.o -xc -
#include "libc/calls/calls.h"
#include "libc/calls/weirdtypes.h"
#include "libc/fmt/fmt.h"
#include "libc/stdio/dprintf.h"
#include "libc/stdio/stdio.h"
#include "libc/stdio/temp.h"
#include "third_party/musl/tempnam.h"
__attribute__((destructor(10000))) void fini4() { printf("5"); }
EOF

cat <<'EOF' | $CC -c -o $t/f.o -xc -
#include "libc/calls/calls.h"
#include "libc/calls/weirdtypes.h"
#include "libc/fmt/fmt.h"
#include "libc/stdio/dprintf.h"
#include "libc/stdio/stdio.h"
#include "libc/stdio/temp.h"
#include "third_party/musl/tempnam.h"
__attribute__((destructor(1000))) void fini3() { printf("6"); }
EOF

cat <<'EOF' | $CC -c -o $t/g.o -xc -
#include "libc/calls/calls.h"
#include "libc/calls/weirdtypes.h"
#include "libc/fmt/fmt.h"
#include "libc/stdio/dprintf.h"
#include "libc/stdio/stdio.h"
#include "libc/stdio/temp.h"
#include "third_party/musl/tempnam.h"
__attribute__((destructor)) void fini1() { printf("7"); }
EOF

cat <<'EOF' | $CC -c -o $t/h.o -xc -
#include "libc/calls/calls.h"
#include "libc/calls/weirdtypes.h"
#include "libc/fmt/fmt.h"
#include "libc/stdio/dprintf.h"
#include "libc/stdio/stdio.h"
#include "libc/stdio/temp.h"
#include "third_party/musl/tempnam.h"
__attribute__((destructor)) void fini2() { printf("8"); }
EOF

cat <<EOF | $CC -c -o $t/i.o -xc -
int main() {}
EOF

$CC -B. -o $t/exe $t/a.o $t/b.o $t/c.o $t/d.o $t/e.o $t/f.o $t/g.o $t/h.o $t/i.o
$QEMU $t/exe | grep -q '21348756'
