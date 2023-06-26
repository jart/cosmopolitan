// clang-format off
#!/bin/bash
. $(dirname $0)/common.inc

cat <<EOF | $CC -fPIC -c -o $t/a.o -xc -
void foo1() {}
void foo2() {}
void foo3() {}

__asm__(".symver foo1, foo@VER1");
__asm__(".symver foo2, foo@VER2");
__asm__(".symver foo3, foo@@VER3");

void foo();

void bar() {
  foo();
}
EOF

echo 'VER1 { local: *; }; VER2 { local: *; }; VER3 { local: *; };' > $t/b.ver
$CC -B. -shared -o $t/c.so $t/a.o -Wl,--version-script=$t/b.ver
readelf --symbols $t/c.so > $t/log

grep -Fq 'foo@VER1' $t/log
grep -Fq 'foo@VER2' $t/log
grep -Fq 'foo@@VER3' $t/log
