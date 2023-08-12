// clang-format off
#!/bin/bash
. $(dirname $0)/common.inc

command -v dwarfdump >& /dev/null || skip

cat <<EOF | $CXX -c -o $t/a.o -g -xc++ -
extern const char *msg;
struct Foo {
  Foo() { msg = "Hello world"; }
};
Foo x;
EOF

cat <<EOF | $CXX -c -o $t/b.o -g -xc++ -
extern const char *msg;
struct Foo {
  Foo() { msg = "Hello world"; }
};
Foo y;
EOF

cat <<EOF | $CXX -o $t/c.o -c -xc++ -g -
#include "third_party/libcxx/cstdio"
const char *msg;
int main() { printf("%s\n", msg); }
EOF

$CXX -o $t/exe $t/a.o $t/b.o $t/c.o -g
$QEMU $t/exe | grep -q 'Hello world'

dwarfdump $t/exe > /dev/null
