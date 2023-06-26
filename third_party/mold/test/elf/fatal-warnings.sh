// clang-format off
#!/bin/bash
. $(dirname $0)/common.inc

cat <<EOF | $CC -fcommon -xc -c -o $t/a.o -
int foo;
EOF

cat <<EOF | $CC -fcommon -xc -c -o $t/b.o -
int foo;

int main() {
  return 0;
}
EOF

$CC -B. -o $t/exe $t/a.o $t/b.o \
  -Wl,-warn-common 2> /dev/null

! $CC -B. -o $t/exe $t/a.o $t/b.o \
  -Wl,-warn-common -Wl,-fatal-warnings 2> /dev/null || false
