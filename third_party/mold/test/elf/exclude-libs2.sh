// clang-format off
#!/bin/bash
. $(dirname $0)/common.inc

cat <<EOF | $CC -x assembler -c -o $t/a.o -
.globl foo
foo:
EOF

rm -f $t/b.a
ar crs $t/b.a $t/a.o

cat <<EOF | $CC -xc -c -o $t/c.o -
int foo() {
  return 3;
}
EOF

$CC -B. -shared -o $t/d.so $t/c.o $t/b.a -Wl,-exclude-libs=b.a
readelf --dyn-syms $t/d.so > $t/log
grep -Fq foo $t/log
