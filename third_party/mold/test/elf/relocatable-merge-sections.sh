// clang-format off
#!/bin/bash
. $(dirname $0)/common.inc

# OneTBB isn't tsan-clean
nm mold | grep -q '__tsan_init' && skip

cat <<EOF | $CC -c -o $t/a.o -xc -ffunction-sections -
void foo() {}
void bar() {}
EOF

./mold --relocatable -o $t/b.o $t/a.o
readelf -WS $t/b.o > $t/log1
grep -Fq .text.foo $t/log1
grep -Fq .text.bar $t/log1

./mold --relocatable -o $t/c.o $t/a.o --relocatable-merge-sections
readelf -WS $t/c.o > $t/log2
! grep -Fq .text.foo $t/log2 || false
! grep -Fq .text.bar $t/log2 || false
