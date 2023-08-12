// clang-format off
#!/bin/bash
. $(dirname $0)/common.inc

cat <<EOF | $CC -c -xc -o $t/a.o -
int main() {}
EOF

$CC -B. -o $t/exe $t/a.o -Wl,-z,execstack
readelf --segments -W $t/exe | grep -q 'GNU_STACK.* RWE '

$CC -B. -o $t/exe $t/a.o -Wl,-z,execstack -Wl,-z,noexecstack
readelf --segments -W $t/exe | grep -q 'GNU_STACK.* RW '

$CC -B. -o $t/exe $t/a.o
readelf --segments -W $t/exe | grep -q 'GNU_STACK.* RW '
