// clang-format off
#!/bin/bash
. $(dirname $0)/common.inc

cat <<EOF > $t/a.ver
ver1 {
  global: ?oo;
  local: *;
};

ver2 {
  global: b?r;
};
EOF

cat <<EOF | $CC -B. -xc -shared -o $t/b.so -Wl,-version-script,$t/a.ver -
void foo() {}
void bar() {}
void baz() {}
EOF

cat <<EOF | $CC -xc -c -o $t/c.o -
void foo();
void bar();

int main() {
  foo();
  bar();
  return 0;
}
EOF

$CC -B. -o $t/exe $t/c.o $t/b.so
$QEMU $t/exe

readelf --dyn-syms $t/b.so > $t/log
grep -Fq 'foo@@ver1' $t/log
grep -Fq 'bar@@ver2' $t/log
! grep -Fq 'baz' $t/log || false
