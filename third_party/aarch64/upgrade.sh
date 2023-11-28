#!/bin/sh
# /opt/aarch64o2/lib/gcc/aarch64-linux-musl/9.2.0/include
# /opt/cross11portcosmo/lib/gcc/aarch64-linux-musl/11.2.0/include

# IMPORTANT NOTES
#
# 1. You also need:
#      #pragma GCC diagnostic ignored "-Wmissing-braces"
#    In third_party/aarch64/arm_neon.internal.h
#
# 2. You have to rewrite arm_fp16 to use `__funline`.
#
# 3. You should fix up the `#pragma GCC aarch64` things.
#

s=/opt/cross11portcosmo/lib/gcc/aarch64-linux-musl/11.2.0/include
d=third_party/aarch64

FILES='
arm_acle
arm_fp16
arm_neon
acc_prof
arm_bf16
arm_sve
acc_prof
openacc
'

strip_c_comments() {
  # https://stackoverflow.com/a/13062682/1653720
  [ $# -eq 2 ] && arg="$1" || arg=""
  eval file="\$$#"
  sed 's/a/aA/g; s/__/aB/g; s/#/aC/g' "$file" |
    gcc -P -E $arg - |
    sed 's/aC/#/g; s/aB/__/g; s/aA/a/g'
}

rm -f third_party/aarch64/*.h

for f in $FILES; do
  echo cp $s/$f.h $d/$f.internal.h
  cp $s/$f.h $d/$f.internal.h || exit
done

sed -i \
    -e 's/#  *include/#include/' \
    -e '/#include .std/d' \
    -e 's!#include [<"]!#include "third_party/aarch64/!' \
    -e 's!\.h[>"]$!.internal.h"!' \
    third_party/aarch64/*.h

# solve the pedantic gcc linter warning `'vmulxh_f16' is static but used
# in inline function 'vmulxh_laneq_f16' which is not static [-Werror]`
sed -i \
    -e 's/static/extern/g' \
    third_party/aarch64/arm_fp16.internal.h

for f in third_party/aarch64/*.h; do
  strip_c_comments $f >$f.tmp || exit
  mv $f.tmp $f
done

for f in third_party/aarch64/*.h; do
  (
    printf %s\\n '#if defined(__aarch64__) && !(__ASSEMBLER__ + __LINKER__ + 0)'
    cat $f
    printf %s\\n '#endif'
  ) >$f.tmp
  mv $f.tmp $f
done
