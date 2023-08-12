// clang-format off
#!/bin/bash
. $(dirname $0)/common.inc

echo 'void _start() {}' | $CC -c -o $t/a.o -xc -
./mold -o $t/exe $t/a.o -gdb-index
readelf -WS $t/exe > $t/log
! grep -Fq .gdb_index $t/log || false
