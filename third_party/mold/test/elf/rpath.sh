// clang-format off
#!/bin/bash
. $(dirname $0)/common.inc

cat <<EOF | $CC -o $t/a.o -c -x assembler -
  .text
  .globl main
main:
  nop
EOF

$CC -B. -o $t/exe1 $t/a.o -Wl,-rpath,/foo,-rpath,/bar,-R/no/such/directory,-R/
readelf --dynamic $t/exe1 | grep -Fq 'Library runpath: [/foo:/bar:/no/such/directory:/]'

$CC -B. -o $t/exe2 $t/a.o -Wl,-rpath,/foo,-rpath,/bar,-rpath,/foo,-rpath,/baz
readelf --dynamic $t/exe2 | grep -Fq 'Library runpath: [/foo:/bar:/baz]'
