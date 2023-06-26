// clang-format off
#!/bin/bash
. $(dirname $0)/common.inc

cat <<EOF | $CC -o $t/a.o -c -xc -
void foo() {}
EOF

cat <<EOF | $CC -o $t/b.o -c -xc -
void foo();
int main() { foo(); }
EOF

! $CC -B. -o $t/exe $t/a.o $t/b.o -Wl,--print-dependencies > $t/log 2> /dev/null
grep -q 'b\.o.*a\.o.*foo$' $t/log
