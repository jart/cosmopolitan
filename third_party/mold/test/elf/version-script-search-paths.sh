// clang-format off
#!/bin/bash
. $(dirname $0)/common.inc

mkdir -p $t/foo/bar
echo 'ver_x { global: *; };' > $t/foo/bar/a.ver

cat <<EOF > $t/b.s
.globl foo, bar, baz
foo:
  nop
bar:
  nop
baz:
  nop
EOF

$CC -B. -shared -o $t/c.so -Wl,-L$t/foo/bar -Wl,-version-script,a.ver $t/b.s
readelf --version-info $t/c.so > $t/log

grep -Fq 'Rev: 1  Flags: none  Index: 2  Cnt: 1  Name: ver_x' $t/log
