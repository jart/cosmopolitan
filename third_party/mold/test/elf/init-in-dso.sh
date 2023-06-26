// clang-format off
#!/bin/bash
. $(dirname $0)/common.inc

cat <<EOF | $CC -shared -o $t/a.so -xc -
void foo() {}
EOF

cat <<EOF | $CC -o $t/b.o -c -xc -
void foo();
int main() {}
EOF

$CC -B. -o $t/exe $t/a.so $t/b.o -Wl,-init,foo
readelf --dynamic $t/exe > $t/log
! grep -Fq '(INIT)' $t/log || false
