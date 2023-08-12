// clang-format off
#!/bin/bash
. $(dirname $0)/common.inc

# On PPC64, a given entry point address is set to .opd, and the
# address in .opd address is set to the ELF header.
[ $MACHINE = ppc64 ] && skip

cat <<EOF | $CC -o $t/a.o -c -x assembler -
.globl foo, bar
foo = 0x1000
bar = 0x2000
EOF

cat <<EOF | $CC -o $t/b.o -c -xc -
int main() {}
EOF

$CC -B. -o $t/exe1 -Wl,-e,foo $t/a.o $t/b.o
readelf -e $t/exe1 > $t/log
grep -q "Entry point address:.*0x1000$" $t/log

$CC -B. -o $t/exe2 -Wl,-e,bar $t/a.o $t/b.o
readelf -e $t/exe2 > $t/log
grep -q "Entry point address:.*0x2000$" $t/log
