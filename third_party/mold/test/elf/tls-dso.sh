// clang-format off
#!/bin/bash
. $(dirname $0)/common.inc

cat <<EOF | $CC -fPIC -shared -o $t/a.so -xc -
extern _Thread_local int foo;
_Thread_local int bar;

int get_foo1() { return foo; }
int get_bar1() { return bar; }
EOF

cat <<EOF | $CC -c -o $t/b.o -xc -
#include "libc/calls/calls.h"
#include "libc/calls/weirdtypes.h"
#include "libc/fmt/fmt.h"
#include "libc/stdio/dprintf.h"
#include "libc/stdio/stdio.h"
#include "libc/stdio/temp.h"
#include "third_party/musl/tempnam.h"

_Thread_local int foo;
extern _Thread_local int bar;

int get_foo1();
int get_bar1();

int get_foo2() { return foo; }
int get_bar2() { return bar; }

int main() {
  foo = 5;
  bar = 3;
  printf("%d %d %d %d %d %d\n",
         foo, bar,
         get_foo1(), get_bar1(),
         get_foo2(), get_bar2());
  return 0;
}
EOF

$CC -B. -o $t/exe $t/a.so $t/b.o
$QEMU $t/exe | grep -q '5 3 5 3 5 3'
