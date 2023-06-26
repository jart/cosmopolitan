// clang-format off
#!/bin/bash
. $(dirname $0)/common.inc

cat <<EOF | $CC -o $t/a.o -c -xc -
int main() {}
EOF

mkdir -p $t/foo/bar
rm -f $t/foo/bar/libfoo.a
ar rcs $t/foo/bar/libfoo.a $t/a.o

cat <<EOF > $t/b.script
INPUT(-lfoo)
EOF

$CC -B. -o $t/exe -L$t/foo/bar $t/b.script
