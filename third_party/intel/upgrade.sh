#!/bin/sh

s=/opt/include
d=third_party/intel

FILES='
adxintrin
ammintrin
amxbf16intrin
amxcomplexintrin
amxfp16intrin
amxint8intrin
amxtileintrin
avx2intrin
avx5124fmapsintrin
avx5124vnniwintrin
avx512bf16intrin
avx512bf16vlintrin
avx512bitalgintrin
avx512bitalgvlintrin
avx512bwintrin
avx512cdintrin
avx512dqintrin
avx512erintrin
avx512fintrin
avx512fp16intrin
avx512fp16vlintrin
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
avx512vp2intersectintrin
avx512vp2intersectvlintrin
avx512vpopcntdqintrin
avx512vpopcntdqvlintrin
avxifmaintrin
avxvnniint8intrin
avxvnniint16intrin
avxintrin
avxneconvertintrin
avxvnniintrin
bmi2intrin
bmmintrin
bmiintrin
cetintrin
cldemoteintrin
clflushoptintrin
clwbintrin
clzerointrin
cmpccxaddintrin
cpuid
emmintrin
enqcmdintrin
f16cintrin
fma4intrin
fmaintrin
fxsrintrin
gfniintrin
hresetintrin
ia32intrin
immintrin
keylockerintrin
lwpintrin
lzcntintrin
mm3dnow
mm_malloc
mmintrin
movdirintrin
mwaitintrin
mwaitxintrin
nmmintrin
pconfigintrin
pkuintrin
pmmintrin
popcntintrin
prfchiintrin
prfchwintrin
raointintrin
rdseedintrin
rtmintrin
serializeintrin
sgxintrin
sha512intrin
shaintrin
sm3intrin
sm4intrin
smmintrin
tbmintrin
tmmintrin
tsxldtrkintrin
uintrintrin
usermsrintrin
vaesintrin
vpclmulqdqintrin
waitpkgintrin
wbnoinvdintrin
wmmintrin
x86gprintrin
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
