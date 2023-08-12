// clang-format off
#!/bin/bash
. $(dirname $0)/common.inc

[ $MACHINE = x86_64 ] || skip

echo 'int main() {}' | $CC -m32 -o $t/exe -xc - >& /dev/null \
  || skip

cat <<EOF | $CC -m32 -c -o $t/a.o -xc -
char hello[] = "Hello world";
EOF

mkdir -p $t/lib32
$CC -m32 -shared -o $t/lib32/libfoo.so $t/a.o

cat <<EOF | $CC -c -o $t/d.o -xc -
char hello[] = "Hello world";
EOF

mkdir -p $t/lib64
$CC -shared -o $t/lib64/libfoo.so $t/d.o

cat <<EOF | $CC -c -o $t/e.o -xc -
#include "libc/calls/calls.h"
#include "libc/calls/weirdtypes.h"
#include "libc/fmt/fmt.h"
#include "libc/stdio/dprintf.h"
#include "libc/stdio/stdio.h"
#include "libc/stdio/temp.h"
#include "third_party/musl/tempnam.h"

extern char hello[];

int main() {
  printf("%s\n", hello);
}
EOF

mkdir -p $t/script
echo 'GROUP(libfoo.so)' > $t/script/libfoo.so

$CC -B. -o $t/exe -L$t/lib32 -L$t/lib64 -lfoo $t/e.o -Wl,-rpath $t/lib64 \
  >& $t/log

grep -q 'lib32/libfoo.so: skipping incompatible file' $t/log
$QEMU $t/exe | grep -q 'Hello world'
