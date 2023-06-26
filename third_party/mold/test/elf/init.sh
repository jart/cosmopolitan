// clang-format off
#!/bin/bash
. $(dirname $0)/common.inc

cat <<EOF | $CC -o $t/a.o -c -xc -
void foo() {}
int main() {}
EOF

$CC -B. -o $t/exe $t/a.o -Wl,-init,foo
readelf --dynamic $t/exe | grep -Fq '(INIT)'

$CC -B. -o $t/exe $t/a.o -Wl,-init,no-such-symbol
readelf --dynamic $t/exe > $t/log
! grep -Fq '(INIT)' $t/log || false
