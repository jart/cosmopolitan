#!/bin/sh
# cosmopolitan toolchain packager
#
#     tool/cosmocc/package.sh
#     cd cosmocc
#     zip -ry9 ../cosmocc.zip .
#

set -ex

OUTDIR=${1:-cosmocc}

make -j8 m= \
  o//tool/build/apelink.com

make -j8 m=x86_64 \
  o/cosmocc.h.txt \
  o/x86_64/ape/ape.lds \
  o/x86_64/libc/crt/crt.o \
  o/x86_64/ape/ape.elf \
  o/x86_64/ape/ape.macho \
  o/x86_64/ape/ape.o \
  o/x86_64/ape/ape-copy-self.o \
  o/x86_64/ape/ape-no-modify-self.o \
  o/x86_64/cosmopolitan.a \
  o/x86_64/third_party/libcxx/libcxx.a \
  o/x86_64/tool/build/assimilate.com.dbg \
  o/x86_64/tool/build/march-native.com.dbg \
  o/x86_64/tool/build/mktemper.com.dbg \
  o/x86_64/tool/build/fixupobj.com.dbg \
  o/x86_64/tool/build/zipcopy.com.dbg \
  o/x86_64/tool/build/mkdeps.com.dbg \
  o/x86_64/tool/build/zipobj.com.dbg \
  o/x86_64/tool/build/apelink.com.dbg \
  o/x86_64/tool/build/pecheck.com.dbg \
  o/x86_64/third_party/make/make.com.dbg \
  o/x86_64/third_party/ctags/ctags.com.dbg

make -j8 m=aarch64 \
  o/aarch64/ape/ape.elf \
  o/aarch64/ape/aarch64.lds \
  o/aarch64/libc/crt/crt.o \
  o/aarch64/ape/ape-copy-self.o \
  o/aarch64/ape/ape-no-modify-self.o \
  o/aarch64/cosmopolitan.a \
  o/aarch64/third_party/libcxx/libcxx.a \
  o/aarch64/tool/build/assimilate.com.dbg \
  o/aarch64/tool/build/march-native.com.dbg \
  o/aarch64/tool/build/mktemper.com.dbg \
  o/aarch64/tool/build/fixupobj.com.dbg \
  o/aarch64/tool/build/zipcopy.com.dbg \
  o/aarch64/tool/build/mkdeps.com.dbg \
  o/aarch64/tool/build/zipobj.com.dbg \
  o/aarch64/tool/build/apelink.com.dbg \
  o/aarch64/tool/build/pecheck.com.dbg \
  o/aarch64/third_party/make/make.com.dbg \
  o/aarch64/third_party/ctags/ctags.com.dbg

mkdir -p "$OUTDIR/bin/"
cp tool/cosmocc/README.md "$OUTDIR/"
cp tool/cosmocc/LICENSE.* "$OUTDIR/"

mkdir -p "$OUTDIR/include/"
cp -R libc/isystem/* "$OUTDIR/include/"
cp -R libc/integral "$OUTDIR/include/libc/"
for x in $(cat o/cosmocc.h.txt); do
  mkdir -p "$OUTDIR/include/${x%/*}/"
  cp -f $x "$OUTDIR/include/${x%/*}/"
done

OLD=$PWD
cd "$OUTDIR/"
if [ ! -x bin/x86_64-linux-cosmo-gcc ]; then
  wget https://github.com/ahgamut/superconfigure/releases/download/z0.0.19/aarch64-gcc.zip
  unzip aarch64-gcc.zip
  rm -f aarch64-gcc.zip
  wget https://github.com/ahgamut/superconfigure/releases/download/z0.0.19/x86_64-gcc.zip
  unzip x86_64-gcc.zip
  rm -f x86_64-gcc.zip
fi
rm -f bin/*-cpp
rm -f bin/*-gcc-*
rm -f bin/*-gprof
rm -f bin/*-strings
for arch in aarch64 x86_64; do
  ln -sf $arch-linux-cosmo-addr2line bin/$arch-unknown-cosmo-addr2line
  ln -sf $arch-linux-cosmo-ar bin/$arch-unknown-cosmo-ar
  ln -sf $arch-linux-cosmo-as bin/$arch-unknown-cosmo-as
  ln -sf $arch-linux-cosmo-c++filt bin/$arch-unknown-cosmo-c++filt
  ln -sf $arch-linux-cosmo-g++ bin/$arch-linux-cosmo-c++
  ln -sf $arch-linux-cosmo-gcc bin/$arch-linux-cosmo-cc
  ln -sf $arch-linux-cosmo-gcc bin/$arch-linux-cosmo-cpp
  ln -sf $arch-linux-cosmo-nm bin/$arch-unknown-cosmo-nm
  ln -sf $arch-linux-cosmo-objcopy bin/$arch-unknown-cosmo-objcopy
  ln -sf $arch-linux-cosmo-objdump bin/$arch-unknown-cosmo-objdump
  ln -sf $arch-linux-cosmo-readelf bin/$arch-unknown-cosmo-readelf
  ln -sf $arch-linux-cosmo-strip bin/$arch-unknown-cosmo-strip
  cmp -s libexec/gcc/$arch-linux-cosmo/11.2.0/ld.bfd libexec/gcc/$arch-linux-cosmo/11.2.0/ld
  ln -sf ld.bfd libexec/gcc/$arch-linux-cosmo/11.2.0/ld
  cmp -s libexec/gcc/$arch-linux-cosmo/11.2.0/ld.bfd bin/$arch-linux-cosmo-ld
  ln -sf ../libexec/gcc/$arch-linux-cosmo/11.2.0/ld.bfd bin/$arch-linux-cosmo-ld
  cmp -s libexec/gcc/$arch-linux-cosmo/11.2.0/as bin/$arch-linux-cosmo-as
  ln -sf ../libexec/gcc/$arch-linux-cosmo/11.2.0/as bin/$arch-linux-cosmo-as
  cmp -s libexec/gcc/$arch-linux-cosmo/11.2.0/ld.bfd bin/$arch-linux-cosmo-ld.bfd
  ln -sf ../libexec/gcc/$arch-linux-cosmo/11.2.0/ld.bfd bin/$arch-linux-cosmo-ld.bfd
done
cd "$OLD"

for arch in x86_64 aarch64; do
  mkdir -p "$OUTDIR/$arch-linux-cosmo/lib/"
  cp -f o/$arch/libc/crt/crt.o "$OUTDIR/$arch-linux-cosmo/lib/"
  cp -f o/$arch/cosmopolitan.a "$OUTDIR/$arch-linux-cosmo/lib/libcosmo.a"
  cp -f o/$arch/third_party/libcxx/libcxx.a "$OUTDIR/$arch-linux-cosmo/lib/"
  for lib in c dl gcc_s m pthread resolv rt dl z stdc++; do
    printf '\041\074\141\162\143\150\076\012' >"$OUTDIR/$arch-linux-cosmo/lib/lib$lib.a"
  done
done
cp -f o/x86_64/ape/ape.o "$OUTDIR/x86_64-linux-cosmo/lib/"
cp -f o/x86_64/ape/ape.lds "$OUTDIR/x86_64-linux-cosmo/lib/"
cp -f o/aarch64/ape/aarch64.lds "$OUTDIR/aarch64-linux-cosmo/lib/"
cp -f o/x86_64/ape/ape-no-modify-self.o "$OUTDIR/x86_64-linux-cosmo/lib/"

cp -f ape/ape-m1.c "$OUTDIR/bin/"
cp -af tool/cosmocc/bin/* "$OUTDIR/bin/"
cp -f o/x86_64/ape/ape.elf "$OUTDIR/bin/ape-x86_64.elf"
cp -f o/x86_64/ape/ape.macho "$OUTDIR/bin/ape-x86_64.macho"
cp -f o/aarch64/ape/ape.elf "$OUTDIR/bin/ape-aarch64.elf"
for x in assimilate march-native mktemper fixupobj zipcopy apelink pecheck mkdeps zipobj; do
  o//tool/build/apelink.com \
    -l o/x86_64/ape/ape.elf \
    -l o/aarch64/ape/ape.elf \
    -M ape/ape-m1.c \
    -o "$OUTDIR/bin/$x" \
    o/x86_64/tool/build/$x.com.dbg \
    o/aarch64/tool/build/$x.com.dbg
done
for x in make ctags; do
  o//tool/build/apelink.com \
    -l o/x86_64/ape/ape.elf \
    -l o/aarch64/ape/ape.elf \
    -M ape/ape-m1.c \
    -o "$OUTDIR/bin/$x" \
    o/x86_64/third_party/$x/$x.com.dbg \
    o/aarch64/third_party/$x/$x.com.dbg
done
