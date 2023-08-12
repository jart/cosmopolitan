// clang-format off
#!/bin/bash
. $(dirname $0)/common.inc

[ $MACHINE = x86_64 ] || skip

cat <<EOF | $CXX -c -o $t/a.o -xc++ -fPIC -
int main() {
  try {
    throw 0;
  } catch (int x) {
    return x;
  }
  return 1;
}
EOF

$CXX -B. -o $t/exe $t/a.o -mcmodel=large
$QEMU $t/exe

if echo 'int main() {}' | $CC -o /dev/null -xc - -static >& /dev/null; then
  $CXX -B. -o $t/exe $t/a.o -static -mcmodel=large
  $QEMU $t/exe
fi
