// clang-format off
#!/bin/bash
. $(dirname $0)/common.inc

cat <<EOF | $CC -o $t/a.o -c -xc -
int main() {}
EOF

$CC -B. -o $t/exe $t/a.o
readelf --dynamic $t/exe > $t/log
grep -Fq '(DEBUG)' $t/log

cat <<EOF | $CC -o $t/b.o -c -xc -
void foo() {}
EOF

$CC -B. -o $t/c.so $t/b.o -shared
readelf --dynamic $t/c.so > $t/log
! grep -Fq '(DEBUG)' $t/log || false
