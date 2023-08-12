// clang-format off
#!/bin/bash
. $(dirname $0)/common.inc

cat <<EOF | $CC -o $t/long-long-long-filename.o -c -xc -
int three() { return 3; }
EOF

cat <<EOF | $CC -o $t/b.o -c -xc -
int five() { return 5; }
EOF

cat <<EOF | $CC -o $t/c.o -c -xc -
int seven() { return 7; }
EOF

cat <<EOF | $CC -o $t/d.o -c -xc -
#include "libc/calls/calls.h"
#include "libc/calls/weirdtypes.h"
#include "libc/fmt/fmt.h"
#include "libc/stdio/dprintf.h"
#include "libc/stdio/stdio.h"
#include "libc/stdio/temp.h"
#include "third_party/musl/tempnam.h"

int three();
int five();
int seven();

int main() {
  printf("%d\n", three() + five() + seven());
}
EOF

rm -f $t/d.a
(cd $t; ar rcsT d.a long-long-long-filename.o b.o "`pwd`"/c.o)

$CC -B. -Wl,--trace -o $t/exe $t/d.o $t/d.a > $t/log

grep -Eq 'thin-archive/d.a\(.*long-long-long-filename.o\)' $t/log
grep -Eq 'thin-archive/d.a\(.*/b.o\)' $t/log
grep -Fq thin-archive/d.o $t/log

$QEMU $t/exe | grep -q 15
