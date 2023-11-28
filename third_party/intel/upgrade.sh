#!/bin/sh

s=/opt/cross11portcosmo/lib/gcc/x86_64-linux-musl/11.2.0/include
d=third_party/intel

FILES='
amxbf16intrin
amxint8intrin
amxtileintrin
avx512bf16intrin
avx512bf16vlintrin
avx512vp2intersectintrin
avx512vp2intersectvlintrin
avxvnniintrin
enqcmdintrin
hresetintrin
keylockerintrin
serializeintrin
tsxldtrkintrin
uintrintrin
x86gprintrin
avx5124fmapsintrin
avx5124vnniwintrin
avx512bitalgintrin
avx512bwintrin
avx512cdintrin
avx512dqintrin
avx512erintrin
avx512fintrin
avx512ifmaintrin
avx512ifmavlintrin
avx512pfintrin
avx512vbmi2intrin
avx512vbmi2vlintrin
avx512vbmiintrin
avx512vbmivlintrin
avx512vlbwintrin
avx512vldqintrin
avx512vlintrin
avx512vnniintrin
avx512vnnivlintrin
avx512vpopcntdqintrin
avx512vpopcntdqvlintrin
adxintrin
ammintrin
avx2intrin
avxintrin
bmi2intrin
bmiintrin
cetintrin
cldemoteintrin
clflushoptintrin
clwbintrin
clzerointrin
cpuid
emmintrin
f16cintrin
fma4intrin
fmaintrin
fxsrintrin
gfniintrin
ia32intrin
immintrin
lwpintrin
lzcntintrin
mm3dnow
mm_malloc
mmintrin
movdirintrin
mwaitxintrin
nmmintrin
pconfigintrin
pkuintrin
pmmintrin
popcntintrin
prfchwintrin
rdseedintrin
rtmintrin
sgxintrin
shaintrin
smmintrin
tbmintrin
tmmintrin
vaesintrin
vpclmulqdqintrin
waitpkgintrin
wbnoinvdintrin
wmmintrin
x86intrin
xmmintrin
xopintrin
xsavecintrin
xsaveintrin
xsaveoptintrin
xsavesintrin
xtestintrin
'

strip_c_comments() {
  # https://stackoverflow.com/a/13062682/1653720
  [ $# -eq 2 ] && arg="$1" || arg=""
  eval file="\$$#"
  sed 's/a/aA/g; s/__/aB/g; s/#/aC/g' "$file" |
    gcc -P -E $arg - |
    sed 's/aC/#/g; s/aB/__/g; s/aA/a/g'
}

rm -f third_party/intel/*.h

for f in $FILES; do
  echo cp $s/$f.h $d/$f.internal.h
  cp $s/$f.h $d/$f.internal.h || exit
done

sed -i \
    -e 's/#  *include/#include/' \
    -e '/#include .std/d' \
    -e 's!#include [<"]!#include "third_party/intel/!' \
    -e 's!\.h[>"]$!.internal.h"!' \
    third_party/intel/*.h

for f in third_party/intel/*.h; do
  strip_c_comments $f >$f.tmp || exit
  mv $f.tmp $f
done

for f in third_party/intel/*.h; do
  (
    printf %s\\n '#if defined(__x86_64__) && !(__ASSEMBLER__ + __LINKER__ + 0)'
    cat $f
    printf %s\\n '#endif'
  ) >$f.tmp
  mv $f.tmp $f
done
