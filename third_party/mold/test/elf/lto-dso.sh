// clang-format off
#!/bin/bash
. $(dirname $0)/common.inc

echo 'int main() {}' | $CC -flto -o /dev/null -xc - >& /dev/null \
  || skip

cat <<EOF | $CC -flto -c -fPIC -o $t/a.o -xc -
void foo() {}
EOF

$CC -B. -shared -o $t/b.so -flto $t/a.o

if [ $MACHINE = ppc64 ]; then
  # On PPC64V1, function symbol refers a function descriptor in .opd
  nm -D $t/b.so | grep -q 'D foo'
else
  nm -D $t/b.so | grep -q 'T foo'
fi
