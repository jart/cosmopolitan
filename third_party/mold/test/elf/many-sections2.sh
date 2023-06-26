// clang-format off
#!/bin/bash
. $(dirname $0)/common.inc

# OneTBB isn't tsan-clean
nm mold | grep -q '__tsan_init' && skip

seq 1 100000 | sed 's/.*/.section .data.\0,"aw"\n.globl x\0\nx\0: .word 0\n/g' | \
  $CC -c -xassembler -o $t/a.o -

./mold --relocatable -o $t/b.o $t/a.o
readelf -WS $t/b.o | grep -Fq .data.100000
readelf -Ws $t/b.o | grep -Fq 'GLOBAL DEFAULT 100000'
