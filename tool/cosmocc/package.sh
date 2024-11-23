#!/bin/sh
# cosmopolitan toolchain packager
#
#     tool/cosmocc/package.sh
#     cd cosmocc
#     zip -ry9 ../cosmocc.zip .
#

set -ex

mode() {
  case $(uname -m) in
    arm64|aarch64)  echo aarch64  ;;
    *)              echo          ;;
  esac
}

OUTDIR=${1:-cosmocc}
APELINK=o/$(mode)/tool/build/apelink
AMD64=${2:-x86_64}
ARM64=${3:-aarch64}
NPROC=$(($(nproc)/2))
GCCVER=14.1.0

make -j$NPROC m= \
  $APELINK

make -j$NPROC m=$AMD64 \
  o/cosmocc.h.txt \
  o/$AMD64/ape/ape.lds \
  o/$AMD64/libc/crt/crt.o \
  o/$AMD64/ape/ape.elf \
  o/$AMD64/ape/ape.macho \
  o/$AMD64/ape/ape.o \
  o/$AMD64/ape/ape-copy-self.o \
  o/$AMD64/ape/ape-no-modify-self.o \
  o/$AMD64/cosmopolitan.a \
  o/$AMD64/third_party/libcxx/libcxx.a \
  o/$AMD64/tool/build/assimilate.dbg \
  o/$AMD64/tool/build/march-native.dbg \
  o/$AMD64/tool/build/mktemper.dbg \
  o/$AMD64/tool/build/fixupobj.dbg \
  o/$AMD64/tool/build/zipcopy.dbg \
  o/$AMD64/tool/build/mkdeps.dbg \
  o/$AMD64/tool/build/zipobj.dbg \
  o/$AMD64/tool/build/apelink.dbg \
  o/$AMD64/tool/build/pecheck.dbg \
  o/$AMD64/tool/build/ar.dbg \
  o/$AMD64/tool/build/chmod.dbg \
  o/$AMD64/tool/build/cocmd.dbg \
  o/$AMD64/tool/build/compile.dbg \
  o/$AMD64/tool/build/mkdir.dbg \
  o/$AMD64/tool/build/cp.dbg \
  o/$AMD64/tool/build/echo.dbg \
  o/$AMD64/tool/build/gzip.dbg \
  o/$AMD64/tool/build/objbincopy.dbg \
  o/$AMD64/tool/build/package.dbg \
  o/$AMD64/tool/build/rm.dbg \
  o/$AMD64/tool/build/touch.dbg \
  o/$AMD64/tool/build/sha256sum.dbg \
  o/$AMD64/tool/build/resymbol.dbg \
  o/$AMD64/third_party/make/make.dbg \
  o/$AMD64/third_party/ctags/ctags.dbg

make -j$NPROC m=$AMD64-tiny \
  o/cosmocc.h.txt \
  o/$AMD64-tiny/ape/ape.lds \
  o/$AMD64-tiny/libc/crt/crt.o \
  o/$AMD64-tiny/ape/ape.elf \
  o/$AMD64-tiny/ape/ape.macho \
  o/$AMD64-tiny/ape/ape.o \
  o/$AMD64-tiny/ape/ape-copy-self.o \
  o/$AMD64-tiny/ape/ape-no-modify-self.o \
  o/$AMD64-tiny/cosmopolitan.a \
  o/$AMD64-tiny/third_party/libcxx/libcxx.a \

make -j$NPROC m=$AMD64-dbg \
  o/cosmocc.h.txt \
  o/$AMD64-dbg/ape/ape.lds \
  o/$AMD64-dbg/libc/crt/crt.o \
  o/$AMD64-dbg/ape/ape.elf \
  o/$AMD64-dbg/ape/ape.macho \
  o/$AMD64-dbg/ape/ape.o \
  o/$AMD64-dbg/ape/ape-copy-self.o \
  o/$AMD64-dbg/ape/ape-no-modify-self.o \
  o/$AMD64-dbg/cosmopolitan.a \
  o/$AMD64-dbg/third_party/libcxx/libcxx.a \

make CONFIG_TARGET_ARCH= -j$NPROC m=$AMD64-optlinux \
  o/cosmocc.h.txt \
  o/$AMD64-optlinux/ape/ape.lds \
  o/$AMD64-optlinux/libc/crt/crt.o \
  o/$AMD64-optlinux/ape/ape.elf \
  o/$AMD64-optlinux/ape/ape.macho \
  o/$AMD64-optlinux/ape/ape.o \
  o/$AMD64-optlinux/ape/ape-copy-self.o \
  o/$AMD64-optlinux/ape/ape-no-modify-self.o \
  o/$AMD64-optlinux/cosmopolitan.a \
  o/$AMD64-optlinux/third_party/libcxx/libcxx.a \

make -j$NPROC m=$ARM64 \
  o/$ARM64/ape/ape.elf \
  o/$ARM64/ape/aarch64.lds \
  o/$ARM64/libc/crt/crt.o \
  o/$ARM64/cosmopolitan.a \
  o/$ARM64/third_party/libcxx/libcxx.a \
  o/$ARM64/tool/build/assimilate.dbg \
  o/$ARM64/tool/build/march-native.dbg \
  o/$ARM64/tool/build/mktemper.dbg \
  o/$ARM64/tool/build/fixupobj.dbg \
  o/$ARM64/tool/build/zipcopy.dbg \
  o/$ARM64/tool/build/mkdeps.dbg \
  o/$ARM64/tool/build/zipobj.dbg \
  o/$ARM64/tool/build/apelink.dbg \
  o/$ARM64/tool/build/pecheck.dbg \
  o/$ARM64/tool/build/ar.dbg \
  o/$ARM64/tool/build/chmod.dbg \
  o/$ARM64/tool/build/cocmd.dbg \
  o/$ARM64/tool/build/compile.dbg \
  o/$ARM64/tool/build/cp.dbg \
  o/$ARM64/tool/build/echo.dbg \
  o/$ARM64/tool/build/gzip.dbg \
  o/$ARM64/tool/build/objbincopy.dbg \
  o/$ARM64/tool/build/package.dbg \
  o/$ARM64/tool/build/rm.dbg \
  o/$ARM64/tool/build/touch.dbg \
  o/$ARM64/tool/build/mkdir.dbg \
  o/$ARM64/tool/build/sha256sum.dbg \
  o/$ARM64/tool/build/resymbol.dbg \
  o/$ARM64/third_party/make/make.dbg \
  o/$ARM64/third_party/ctags/ctags.dbg

make -j$NPROC m=$ARM64-tiny \
  o/$ARM64-tiny/ape/ape.elf \
  o/$ARM64-tiny/ape/aarch64.lds \
  o/$ARM64-tiny/libc/crt/crt.o \
  o/$ARM64-tiny/cosmopolitan.a \
  o/$ARM64-tiny/third_party/libcxx/libcxx.a \

make -j$NPROC m=$ARM64-dbg \
  o/$ARM64-dbg/ape/ape.elf \
  o/$ARM64-dbg/ape/aarch64.lds \
  o/$ARM64-dbg/libc/crt/crt.o \
  o/$ARM64-dbg/cosmopolitan.a \
  o/$ARM64-dbg/third_party/libcxx/libcxx.a \

make -j$NPROC m=$ARM64-optlinux \
  o/$ARM64-optlinux/ape/ape.elf \
  o/$ARM64-optlinux/ape/aarch64.lds \
  o/$ARM64-optlinux/libc/crt/crt.o \
  o/$ARM64-optlinux/cosmopolitan.a \
  o/$ARM64-optlinux/third_party/libcxx/libcxx.a \

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

fetch() {
  if command -v wget >/dev/null; then
    wget $1
  else
    curl -LO $1
  fi
}

OLD=$PWD
cd "$OUTDIR/"
if [ ! -x bin/x86_64-linux-cosmo-gcc ]; then
  fetch https://github.com/ahgamut/superconfigure/releases/download/z0.0.60/aarch64-gcc.zip &
  fetch https://github.com/ahgamut/superconfigure/releases/download/z0.0.60/x86_64-gcc.zip &
  fetch https://github.com/ahgamut/superconfigure/releases/download/z0.0.60/llvm.zip &
  wait
  unzip aarch64-gcc.zip &
  unzip x86_64-gcc.zip &
  unzip llvm.zip bin/clang-19 bin/clang-format &
  wait
  rm -f aarch64-gcc.zip
  rm -f x86_64-gcc.zip
  rm -f llvm.zip
  mv bin/clang-19 libexec/clang  # use `cosmocc -mclang` instead
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
  # cmp -s libexec/gcc/$arch-linux-cosmo/$GCCVER/ld.bfd libexec/gcc/$arch-linux-cosmo/$GCCVER/ld
  # ln -sf ld.bfd libexec/gcc/$arch-linux-cosmo/$GCCVER/ld
  # cmp -s libexec/gcc/$arch-linux-cosmo/$GCCVER/ld.bfd bin/$arch-linux-cosmo-ld
  # ln -sf ../libexec/gcc/$arch-linux-cosmo/$GCCVER/ld.bfd bin/$arch-linux-cosmo-ld
  cmp -s libexec/gcc/$arch-linux-cosmo/$GCCVER/as bin/$arch-linux-cosmo-as
  ln -sf ../libexec/gcc/$arch-linux-cosmo/$GCCVER/as bin/$arch-linux-cosmo-as
  cmp -s libexec/gcc/$arch-linux-cosmo/$GCCVER/ld.bfd bin/$arch-linux-cosmo-ld.bfd
  ln -sf ../libexec/gcc/$arch-linux-cosmo/$GCCVER/ld.bfd bin/$arch-linux-cosmo-ld.bfd
done
cd "$OLD"

for arch in $AMD64 $ARM64; do
  mkdir -p "$OUTDIR/$arch-linux-cosmo/lib/"
  mkdir -p "$OUTDIR/$arch-linux-cosmo/lib/dbg"
  mkdir -p "$OUTDIR/$arch-linux-cosmo/lib/tiny"
  mkdir -p "$OUTDIR/$arch-linux-cosmo/lib/optlinux"

  cp -f o/$arch/libc/crt/crt.o "$OUTDIR/$arch-linux-cosmo/lib/"
  cp -f o/$arch-dbg/libc/crt/crt.o "$OUTDIR/$arch-linux-cosmo/lib/dbg/"
  cp -f o/$arch-tiny/libc/crt/crt.o "$OUTDIR/$arch-linux-cosmo/lib/tiny/"
  cp -f o/$arch-optlinux/libc/crt/crt.o "$OUTDIR/$arch-linux-cosmo/lib/optlinux/"

  cp -f o/$arch/cosmopolitan.a "$OUTDIR/$arch-linux-cosmo/lib/libcosmo.a"
  cp -f o/$arch-dbg/cosmopolitan.a "$OUTDIR/$arch-linux-cosmo/lib/dbg/libcosmo.a"
  cp -f o/$arch-tiny/cosmopolitan.a "$OUTDIR/$arch-linux-cosmo/lib/tiny/libcosmo.a"
  cp -f o/$arch-optlinux/cosmopolitan.a "$OUTDIR/$arch-linux-cosmo/lib/optlinux/libcosmo.a"

  cp -f o/$arch/third_party/libcxx/libcxx.a "$OUTDIR/$arch-linux-cosmo/lib/"
  cp -f o/$arch-dbg/third_party/libcxx/libcxx.a "$OUTDIR/$arch-linux-cosmo/lib/dbg/"
  cp -f o/$arch-tiny/third_party/libcxx/libcxx.a "$OUTDIR/$arch-linux-cosmo/lib/tiny/"
  cp -f o/$arch-optlinux/third_party/libcxx/libcxx.a "$OUTDIR/$arch-linux-cosmo/lib/optlinux/"

  for lib in c dl gcc_s m crypt pthread resolv rt dl unwind gomp stdc++; do
    printf '\041\074\141\162\143\150\076\012' >"$OUTDIR/$arch-linux-cosmo/lib/lib$lib.a"
  done
  mkdir -p "$OUTDIR/lib/gcc/"
  touch "$OUTDIR/lib/gcc/libgomp.spec"  # needed if user passes -fopenmp but not -lgomp
done

cp -f o/$AMD64/ape/ape.o "$OUTDIR/x86_64-linux-cosmo/lib/"
cp -f o/$AMD64-dbg/ape/ape.o "$OUTDIR/x86_64-linux-cosmo/lib/dbg/"
cp -f o/$AMD64-tiny/ape/ape.o "$OUTDIR/x86_64-linux-cosmo/lib/tiny/"
cp -f o/$AMD64-optlinux/ape/ape.o "$OUTDIR/x86_64-linux-cosmo/lib/optlinux/"

cp -f o/$AMD64/ape/ape.lds "$OUTDIR/x86_64-linux-cosmo/lib/"
cp -f o/$AMD64-dbg/ape/ape.lds "$OUTDIR/x86_64-linux-cosmo/lib/dbg/"
cp -f o/$AMD64-tiny/ape/ape.lds "$OUTDIR/x86_64-linux-cosmo/lib/tiny/"
cp -f o/$AMD64-optlinux/ape/ape.lds "$OUTDIR/x86_64-linux-cosmo/lib/optlinux/"

cp -f o/$ARM64/ape/aarch64.lds "$OUTDIR/aarch64-linux-cosmo/lib/"
cp -f o/$ARM64-dbg/ape/aarch64.lds "$OUTDIR/aarch64-linux-cosmo/lib/dbg/"
cp -f o/$ARM64-tiny/ape/aarch64.lds "$OUTDIR/aarch64-linux-cosmo/lib/tiny/"
cp -f o/$ARM64-optlinux/ape/aarch64.lds "$OUTDIR/aarch64-linux-cosmo/lib/optlinux/"

cp -f o/$AMD64/ape/ape-no-modify-self.o "$OUTDIR/x86_64-linux-cosmo/lib/"
cp -f o/$AMD64-dbg/ape/ape-no-modify-self.o "$OUTDIR/x86_64-linux-cosmo/lib/dbg/"
cp -f o/$AMD64-tiny/ape/ape-no-modify-self.o "$OUTDIR/x86_64-linux-cosmo/lib/tiny/"
cp -f o/$AMD64-optlinux/ape/ape-no-modify-self.o "$OUTDIR/x86_64-linux-cosmo/lib/optlinux/"

cp -f ape/ape-m1.c "$OUTDIR/bin/"
cp -af tool/cosmocc/bin/* "$OUTDIR/bin/"
cp -f o/$AMD64/ape/ape.elf "$OUTDIR/bin/ape-x86_64.elf"
cp -f o/$AMD64/ape/ape.macho "$OUTDIR/bin/ape-x86_64.macho"
cp -f o/$ARM64/ape/ape.elf "$OUTDIR/bin/ape-aarch64.elf"

for x in assimilate march-native mktemper fixupobj zipcopy apelink pecheck mkdeps zipobj \
         ar chmod cocmd cp echo gzip objbincopy package rm touch mkdir compile sha256sum \
         resymbol; do
  ape $APELINK \
    -l o/$AMD64/ape/ape.elf \
    -l o/$ARM64/ape/ape.elf \
    -M ape/ape-m1.c \
    -o "$OUTDIR/bin/$x" \
    o/$AMD64/tool/build/$x.dbg \
    o/$ARM64/tool/build/$x.dbg
done

for x in ar chmod cp echo gzip package rm touch mkdir compile sha256sum; do
  mv "$OUTDIR/bin/$x" "$OUTDIR/bin/$x.ape"
done

for x in make ctags; do
  ape $APELINK \
    -l o/$AMD64/ape/ape.elf \
    -l o/$ARM64/ape/ape.elf \
    -M ape/ape-m1.c \
    -o "$OUTDIR/bin/$x" \
    o/$AMD64/third_party/$x/$x.dbg \
    o/$ARM64/third_party/$x/$x.dbg
done
