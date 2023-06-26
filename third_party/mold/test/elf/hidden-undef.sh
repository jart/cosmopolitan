// clang-format off
#!/bin/bash
. $(dirname $0)/common.inc

cat <<EOF | $CC -o $t/a.so -shared -fPIC -xc -
void foo() {}
EOF

cat <<EOF | $CC -o $t/b.o -fPIC -c -xc -
__attribute__((visibility("hidden"))) void foo();
int main() { foo(); }
EOF

! $CC -B. -o $t/exe $t/a.so $t/b.o >& $t/log
grep -q 'undefined symbol: foo' $t/log
