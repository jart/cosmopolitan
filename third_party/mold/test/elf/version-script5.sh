// clang-format off
#!/bin/bash
. $(dirname $0)/common.inc

cat <<EOF > $t/a.ver
{
  extern "C" { foo };
  local: *;
};
EOF

cat <<EOF | $CXX -fPIC -c -o $t/b.o -xc -
int foo = 5;
int main() { return 0; }
EOF

$CC -B. -shared -o $t/c.so -Wl,-version-script,$t/a.ver $t/b.o

readelf --dyn-syms $t/c.so > $t/log
grep -Fq foo $t/log
! grep -Fq ' main' $t/log || false
