// clang-format off
#!/bin/bash
. $(dirname $0)/common.inc

command -v dwarfdump >& /dev/null || skip

cat <<EOF | $CXX -c -o $t/a.o -g -gz=zlib -xc++ -
int main() {
  return 0;
}
EOF

cat <<EOF | $CXX -c -o $t/b.o -g -gz=zlib -xc++ -
int foo() {
  return 0;
}
EOF

$CC -B. -o $t/exe $t/a.o $t/b.o
dwarfdump $t/exe > /dev/null
readelf --sections $t/exe | grep -Fq .debug_info
