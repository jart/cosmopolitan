// clang-format off
#!/bin/bash
. $(dirname $0)/common.inc

cat <<EOF | $CC -c -o $t/a.o -xc -
void foo() {}
EOF

cat <<EOF | $CC -c -o $t/b.o -xc -
void bar() {}
EOF

mkdir -p $t/foo/bar
rm -f $t/foo/bar/libfoo.a
ar rcs $t/foo/bar/libfoo.a $t/a.o $t/b.o

cat <<EOF | $CC -c -o $t/c.o -xc -
void foo();
int main() {
  foo();
}
EOF

$CC -B. -o $t/exe $t/c.o -Wl,--sysroot=$t/ \
  -Wl,-L=foo/bar -lfoo

$CC -B. -o $t/exe $t/c.o -Wl,--sysroot=$t/ \
  -Wl,-L=/foo/bar -lfoo

$CC -B. -o $t/exe $t/c.o -Wl,--sysroot=$t/ \
  '-Wl,-L$SYSROOTfoo/bar' -lfoo

$CC -B. -o $t/exe $t/c.o -Wl,--sysroot=$t/ \
  '-Wl,-L$SYSROOT/foo/bar' -lfoo

! $CC -B. -o $t/exe $t/c.o -lfoo >& /dev/null

! $CC -B. -o $t/exe $t/c.o -Wl,--sysroot=$t \
  -Wl,-Lfoo/bar -lfoo >& /dev/null
