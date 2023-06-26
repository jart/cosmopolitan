// clang-format off
#!/bin/bash
. $(dirname $0)/common.inc

cat <<EOF | $CC -o $t/a.o -c -xc -
void foobar() {}
EOF

rm -f $t/b.a
ar rcs $t/b.a $t/a.o

cat <<EOF | $CC -o $t/c.o -c -xc -
int main() {}
EOF

$CC -B. -o $t/exe $t/c.o $t/b.a
! readelf --symbols $t/exe | grep -q foobar || false

$CC -B. -o $t/exe $t/c.o $t/b.a -Wl,-require-defined,foobar
readelf --symbols $t/exe | grep -q foobar

! $CC -B. -o $t/exe $t/c.o $t/b.a -Wl,-require-defined,xyz >& $t/log
grep -q 'undefined symbol: xyz' $t/log
