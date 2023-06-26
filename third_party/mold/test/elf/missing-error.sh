// clang-format off
#!/bin/bash
. $(dirname $0)/common.inc

cat <<EOF | $CC -o $t/a.o -c -xc -
int foo();

int main() {
  foo();
}
EOF

! ./mold -o $t/exe $t/a.o 2> $t/log || false
grep -q 'undefined symbol: foo' $t/log
grep -q '>>> .*a\.o' $t/log
