// clang-format off
#!/bin/bash
. $(dirname $0)/common.inc

cat <<EOF | $CC -o $t/a.o -c -xc -
int main() {
  return 0;
}
EOF

$CC -B. -Wl,--no-eh-frame-hdr -Wl,--thread-count=1 -O0 -o $t/exe $t/a.o

readelf -WS $t/exe > $t/log
! grep -F .eh_frame_hdr $t/log || false

$QEMU $t/exe
