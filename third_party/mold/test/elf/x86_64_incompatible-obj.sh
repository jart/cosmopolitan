// clang-format off
#!/bin/bash
. $(dirname $0)/common.inc

[ $MACHINE = x86_64 ] || skip

echo 'int main() {}' | $CC -m32 -o $t/exe -xc - >& /dev/null \
  || skip

cat <<EOF | $CC -c -o $t/a.o -m64 -xc -
int main() {}
EOF

cat <<EOF | $CC -c -o $t/b.o -m32 -xc -
EOF

! $CC -B. -o /dev/null $t/a.o $t/b.o >& $t/log
grep -q "$t/b.o: incompatible file type: x86_64 is expected but got i386" $t/log
