// clang-format off
#!/bin/bash
. $(dirname $0)/common.inc

[ $MACHINE = x86_64 ] || skip

cat <<'EOF' | $CC -o $t/a.o -c -x assembler -
.globl _start
.text
_start:
  ret

.section .text.hot
.ascii ".text.hot "
.section .text.hot.foo
.ascii ".text.hot.foo "

.section .text.unknown
.ascii ".text.unknown "
.section .text.unknown.foo
.ascii ".text.unknown.foo "

.section .text.unlikely
.ascii ".text.unlikely "
.section .text.unlikely.foo
.ascii ".text.unlikely.foo "

.section .text.startup
.ascii ".text.startup "
.section .text.startup.foo
.ascii ".text.startup.foo "

.section .text.exit
.ascii ".text.exit "
.section .text.exit.foo
.ascii ".text.exit.foo "

.section .text
.ascii ".text "
.section .text.foo
.ascii ".text.foo "

.section .data.rel.ro
.ascii ".data.rel.ro "
.section .data.rel.ro.foo
.ascii ".data.rel.ro.foo "

.section .data
.ascii ".data "
.section .data.foo
.ascii ".data.foo "

.section .rodata
.ascii ".rodata "
.section .rodata.foo
.ascii ".rodata.foo "
EOF

./mold -o $t/exe $t/a.o -z keep-text-section-prefix

readelf -p .text.hot $t/exe | grep -Fq '.text.hot .text.hot.foo'
readelf -p .text.unknown $t/exe | grep -Fq '.text.unknown .text.unknown.foo'
readelf -p .text.unlikely $t/exe | grep -Fq '.text.unlikely .text.unlikely.foo'
readelf -p .text.startup $t/exe | grep -Fq '.text.startup .text.startup.foo'
readelf -p .text.exit $t/exe | grep -Fq '.text.exit .text.exit.foo'
readelf -p .text $t/exe | grep -Fq '.text .text.foo'
readelf -p .data.rel.ro $t/exe | grep -Fq '.data.rel.ro .data.rel.ro.foo'
readelf -p .data $t/exe | grep -Fq '.data .data.foo'
readelf -p .rodata $t/exe | grep -Fq '.rodata .rodata.foo'

./mold -o $t/exe $t/a.o
! readelf --sections $t/exe | grep -Fq .text.hot || false

./mold -o $t/exe $t/a.o -z nokeep-text-section-prefix
! readelf --sections $t/exe | grep -Fq .text.hot || false
