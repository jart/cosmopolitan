// clang-format off
#!/bin/bash
. $(dirname $0)/common.inc

command -v dwarfdump >& /dev/null || skip

cat <<EOF | $CC -c -g -o $t/a.o -xc -
#include "libc/calls/calls.h"
#include "libc/calls/weirdtypes.h"
#include "libc/fmt/fmt.h"
#include "libc/stdio/dprintf.h"
#include "libc/stdio/stdio.h"
#include "libc/stdio/temp.h"
#include "third_party/musl/tempnam.h"

int main() {
  printf("Hello world\n");
  return 0;
}
EOF

$CC -B. -o $t/exe $t/a.o -Wl,--compress-debug-sections=zlib
dwarfdump $t/exe > $t/log
grep -Fq '.debug_info SHF_COMPRESSED' $t/log
grep -Fq '.debug_str SHF_COMPRESSED' $t/log
