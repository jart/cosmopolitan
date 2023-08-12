// clang-format off
#!/bin/bash
. $(dirname $0)/common.inc

[ $MACHINE = x86_64 ] || skip

cat <<EOF | $CC -o $t/a.o -c -xc - 2> /dev/null || skip
volatile char arr[0x100000000];
int main() {
  return arr[2000];
}
EOF

$CC -B. -o $t/exe $t/a.o
$QEMU $t/exe
