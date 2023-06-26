// clang-format off
#!/bin/bash
. $(dirname $0)/common.inc

echo 'int main() {}' | $CC -c -o $t/a.o -xc -

echo 'VER1 { foo[12; };' > $t/b.ver

! $CC -B. -shared -o $t/c.so -Wl,-version-script,$t/b.ver \
  $t/a.o >& $t/log || false
grep -q 'invalid version pattern' $t/log
