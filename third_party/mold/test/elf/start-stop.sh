// clang-format off
#!/bin/bash
. $(dirname $0)/common.inc

cat <<EOF | $CC -c -o $t/a.o -xc -
int main() {}
EOF

$CC -B. -o $t/exe1 $t/a.o -Wl,--start-stop
readelf -sW $t/exe1 | grep -q __start_text
readelf -sW $t/exe1 | grep -q __stop_text
