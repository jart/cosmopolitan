// clang-format off
#!/bin/bash
. $(dirname $0)/common.inc

cat <<EOF | $CC -o $t/a.o -c -xc - -fno-PIC
int main() {}
EOF

$CC -B. -shared -o $t/b.so $t/a.o

! $CC -B. -o $t/b.so $t/a.o -Wl,-require-defined=no-such-sym >& $t/log1 || false
grep -q 'undefined symbol: no-such-sym' $t/log1

$CC -B. -shared -o $t/b.o $t/a.o -Wl,-require-defined=no-such-sym -Wl,-noinhibit-exec >& $t/log2
grep -q 'undefined symbol: no-such-sym' $t/log2
