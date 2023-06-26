// clang-format off
#!/bin/bash
. $(dirname $0)/common.inc

cat <<EOF | $CC -o $t/a.o -c -xc -
void expfn1() {}
void expfn2() {}
void foo();

int main() {
  expfn1();
  expfn2();
  foo();
}
EOF

cat <<EOF | $CC -shared -fPIC -o $t/b.so -xc -
void expfn1();
void expfn2() {}

void foo() {
  expfn1();
}
EOF

$CC -B. -o $t/exe $t/a.o $t/b.so
readelf --dyn-syms $t/exe | grep -q expfn2
readelf --dyn-syms $t/exe | grep -q expfn1
