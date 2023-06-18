#!/bin/sh

ARCH=${1:-x86_64}
IMPORT=${2:-/opt/cross11portcosmo}
PREFIX=third_party/gcc
OLDVERSION=11.2.0
NEWVERSION=11.2.0

rm -rf o/third_party/gcc
if [ $OLDVERSION != $NEWVERSION ]; then
  mv $PREFIX/libexec/gcc/$ARCH-linux-musl/$OLDVERSION $PREFIX/libexec/gcc/$ARCH-linux-musl/$NEWVERSION
  mv $PREFIX/lib/gcc/$ARCH-linux-musl/$OLDVERSION $PREFIX/lib/gcc/$ARCH-linux-musl/$NEWVERSION
  sed -i -e "s/$OLDVERSION/$NEWVERSION/g" $(find $PREFIX -name \*.sym | grep $ARCH)
fi

FILES="
$ARCH-linux-musl/bin/ld.bfd
libexec/gcc/$ARCH-linux-musl/$NEWVERSION/collect2
libexec/gcc/$ARCH-linux-musl/$NEWVERSION/cc1
libexec/gcc/$ARCH-linux-musl/$NEWVERSION/cc1plus
bin/$ARCH-linux-musl-as
bin/$ARCH-linux-musl-ar
bin/$ARCH-linux-musl-gcc
bin/$ARCH-linux-musl-g++
bin/$ARCH-linux-musl-nm
bin/$ARCH-linux-musl-objcopy
bin/$ARCH-linux-musl-c++filt
bin/$ARCH-linux-musl-addr2line
bin/$ARCH-linux-musl-objdump
bin/$ARCH-linux-musl-gcc-nm
bin/$ARCH-linux-musl-strip
"

for f in $FILES; do
  cp -f $IMPORT/$f $PREFIX/$f || exit
  gzip -f9 $PREFIX/$f || exit
done
