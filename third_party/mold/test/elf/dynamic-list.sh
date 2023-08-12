// clang-format off
#!/bin/bash
. $(dirname $0)/common.inc

cat <<EOF | $CC -o $t/a.o -c -xc -
void foo() {}
void bar() {}
int main() {}
EOF

$CC -B. -o $t/exe $t/a.o

readelf --dyn-syms $t/exe > $t/log
! grep -q ' foo$' $t/log || false
! grep -q ' bar$' $t/log || false

cat <<EOF > $t/dyn
{ foo; bar; };
EOF

$CC -B. -o $t/exe1 $t/a.o -Wl,-dynamic-list=$t/dyn

readelf --dyn-syms $t/exe1 > $t/log1
grep -q ' foo$' $t/log1
grep -q ' bar$' $t/log1

$CC -B. -o $t/exe2 $t/a.o -Wl,--export-dynamic-symbol-list=$t/dyn

readelf --dyn-syms $t/exe2 > $t/log2
grep -q ' foo$' $t/log2
grep -q ' bar$' $t/log2

$CC -B. -o $t/exe3 $t/a.o -Wl,--export-dynamic-symbol=foo,--export-dynamic-symbol=bar

readelf --dyn-syms $t/exe3 > $t/log3
grep -q ' foo$' $t/log3
grep -q ' bar$' $t/log3
