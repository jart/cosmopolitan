// clang-format off
#!/bin/bash
. $(dirname $0)/common.inc

ldd mold-wrapper.so | grep -q libasan && skip

nm mold | grep -q '__[at]san_init' && skip

rm -rf $t
mkdir -p $t/bin $t/lib/mold
cp mold $t/bin
cp mold-wrapper.so $t/bin

$t/bin/mold -run bash -c 'echo $LD_PRELOAD' | grep -q '/bin/mold-wrapper.so'
