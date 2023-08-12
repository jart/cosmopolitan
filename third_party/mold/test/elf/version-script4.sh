// clang-format off
#!/bin/bash
. $(dirname $0)/common.inc

cat <<EOF > $t/a.ver
{
  global:
  extern "C++" {
    foo::bar;
  };

  local: *;
};
EOF

cat <<EOF | $CXX -fPIC -c -o $t/b.o -xc++ -
int bar = 5;
namespace foo {
int bar = 7;
}

int main() {
  return 0;
}
EOF

$CC -B. -shared -o $t/c.so -Wl,-version-script,$t/a.ver $t/b.o

readelf --dyn-syms $t/c.so > $t/log
grep -Fq _ZN3foo3barE $t/log
! grep -Fq ' bar' $t/log || false
