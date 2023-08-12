// clang-format off
#!/bin/bash
. $(dirname $0)/common.inc

echo 'int main() { return 0; }' | $CC -c -o $t/a.o -xc -
echo 'int main() { return 1; }' | $CC -c -o $t/b.o -xc -

! $CC -B. -o $t/exe $t/a.o $t/b.o 2> /dev/null || false
$CC -B. -o $t/exe $t/a.o $t/b.o -Wl,-allow-multiple-definition
$CC -B. -o $t/exe $t/a.o $t/b.o -Wl,-z,muldefs
