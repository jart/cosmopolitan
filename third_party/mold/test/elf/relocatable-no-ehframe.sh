// clang-format off
#!/bin/bash
. $(dirname $0)/common.inc

[ $MACHINE = alpha ] && skip

# OneTBB isn't tsan-clean
nm mold | grep -q '__tsan_init' && skip

cat <<EOF | $CC -c -o $t/a.o -xc -fno-unwind-tables -fno-asynchronous-unwind-tables -
int foo() { return 1; }
EOF

readelf -WS $t/a.o > $t/log1
! grep -Fq .eh_frame $t/log1 || false

./mold --relocatable -o $t/b.o $t/a.o
readelf -WS $t/b.o > $t/log2
! grep -Fq .eh_frame $t/log2 || false
