// clang-format off
#!/bin/bash
. $(dirname $0)/common.inc

cat <<EOF | $CC -o $t/a.o -c -xc -
int a = 0;
__attribute__((weak)) int weak_undef;
EOF

cat <<EOF | $CC -o $t/b.o -c -xc - -fcommon
int b = 0;
int common;
EOF

cat <<EOF | $CC -o $t/c.o -c -xc -
int c = 0;
__attribute__((weak)) int weak_def = 3;
EOF

cat <<EOF | $CC -o $t/d.o -c -xc - -fcommon
#include "libc/calls/calls.h"
#include "libc/calls/weirdtypes.h"
#include "libc/fmt/fmt.h"
#include "libc/stdio/dprintf.h"
#include "libc/stdio/stdio.h"
#include "libc/stdio/temp.h"
#include "third_party/musl/tempnam.h"

__attribute__((weak)) extern int a;
__attribute__((weak)) extern int b;
extern int c;

__attribute__((weak)) extern int weak_undef;
__attribute__((weak)) int weak_def = 5;

int common;

int main() {
  (void)common;
  (void)c;
  printf("%d %d %d %d\n", !!&a, !!&b, !!&weak_undef, weak_def);
}
EOF

rm -f $t/x.a
ar rcs $t/x.a $t/a.o $t/b.o $t/c.o

$CC -B. -o $t/exe $t/d.o $t/x.a
$QEMU $t/exe | grep -q '^0 0 0 5$'
