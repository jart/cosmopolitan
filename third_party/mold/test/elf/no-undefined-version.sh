// clang-format off
#!/bin/bash
. $(dirname $0)/common.inc

echo 'ver_x { global: foo; };' > $t/a.ver

cat <<EOF | $CC -o $t/b.o -c -xc -
int main() {}
EOF

$CC -B. -o $t/exe -Wl,--version-script,$t/a.ver $t/b.o 2> $t/log
grep -Fq 'a.ver: cannot assign version `ver_x` to symbol `foo`: symbol not found' $t/log
