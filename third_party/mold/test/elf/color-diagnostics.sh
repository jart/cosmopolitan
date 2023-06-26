// clang-format off
#!/bin/bash
. $(dirname $0)/common.inc

cat <<EOF | $CC -o $t/a.o -c -xc -
int foo();
int main() { foo(); }
EOF

! ./mold -o $t/exe $t/a.o --color-diagnostics 2> $t/log
! grep -q $'\033' $t/log || false

! ./mold -o $t/exe $t/a.o --color-diagnostics=always 2> $t/log
grep -q $'\033' $t/log

! ./mold -o $t/exe $t/a.o --color-diagnostics=never 2> $t/log
! grep -q $'\033' $t/log || false

! ./mold -o $t/exe $t/a.o --color-diagnostics=auto 2> $t/log
! grep -q $'\033' $t/log || false
