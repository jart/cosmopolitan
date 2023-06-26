// clang-format off
#!/bin/bash
. $(dirname $0)/common.inc

cat <<EOF | $CC -o $t/a.o -c -xc - -fno-PIE
void _start() {}
EOF

./mold -o $t/exe $t/a.o --oformat=binary -Ttext=0x4000 -Map=$t/map
grep -Eq '^\s+0x4000\s+[0-9]+\s+[0-9]+\s+\.text$' $t/map

! grep -Fq .strtab $t/map || false
! grep -Fq .shstrtab $t/map || false
! grep -Fq .symtab $t/map || false
! grep -Fq .comment $t/map || false
