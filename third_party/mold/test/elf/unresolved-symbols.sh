// clang-format off
#!/bin/bash
. $(dirname $0)/common.inc

cat <<EOF | $CC -c -o $t/a.o -xc -
int foo();
int main() { foo(); }
EOF

! $CC -B. -o $t/exe $t/a.o 2>&1 | grep -q 'undefined.*foo'

! $CC -B. -o $t/exe $t/a.o -Wl,-unresolved-symbols=report-all 2>&1 \
  | grep -q 'undefined.*foo'

$CC -B. -o $t/exe $t/a.o -Wl,-unresolved-symbols=ignore-all

! readelf --dyn-syms $t/exe | grep -w foo || false

$CC -B. -o $t/exe $t/a.o -Wl,-unresolved-symbols=report-all \
  -Wl,--warn-unresolved-symbols 2>&1 | grep -q 'undefined.*foo'

! $CC -B. -o $t/exe $t/a.o -Wl,-unresolved-symbols=ignore-in-object-files 2>&1 \
  | grep -q 'undefined.*foo'

! $CC -B. -o $t/exe $t/a.o -Wl,-unresolved-symbols=ignore-in-shared-libs 2>&1 \
  | grep -q 'undefined.*foo'
