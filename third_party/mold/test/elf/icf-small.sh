// clang-format off
#!/bin/bash
. $(dirname $0)/common.inc

cat <<EOF | $CC -o $t/a.o -c -ffunction-sections -xc -
void _start() {}
EOF

./mold -o $t/exe -icf=all $t/a.o
