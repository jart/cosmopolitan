// clang-format off
#!/bin/bash
. $(dirname $0)/common.inc

cat <<EOF | $CC -o $t/a.o -c -xc++ -
#include "third_party/libcxx/cstdio"
static struct Foo1 {
  Foo1() { printf("foo1 "); }
} x;

static struct Foo2 {
  Foo2() { printf("foo2 "); }
} y;
EOF

cat <<EOF | $CC -o $t/b.o -c -xc++ -
#include "third_party/libcxx/cstdio"
static struct Foo3 {
  Foo3() { printf("foo3 "); }
} x;

static struct Foo4 {
  Foo4() { printf("foo4 "); }
} y;
EOF

cat <<EOF | $CC -o $t/c.o -c -xc++ -
#include "third_party/libcxx/cstdio"
static struct Foo5 {
  Foo5() { printf("foo5 "); }
} x;

static struct Foo6 {
  Foo6() { printf("foo6 "); }
} y;

int main() { printf("\n"); }
EOF

$CXX -B. -o $t/exe1 $t/a.o $t/b.o $t/c.o
$QEMU $t/exe1 | grep -q 'foo1 foo2 foo3 foo4 foo5 foo6'

$CXX -B. -o $t/exe2 $t/a.o $t/b.o $t/c.o -Wl,--reverse-sections
$QEMU $t/exe2 | grep -q 'foo5 foo6 foo3 foo4 foo1 foo2'
