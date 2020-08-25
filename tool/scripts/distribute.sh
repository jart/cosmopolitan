#!/bin/sh
set -ex

rm -f o/ansi/.cosmopolitan.h o/ansi/cosmopolitan.h
make -j12 o/ansi/cosmopolitan.h MODE=ansi
make -j12 o//libc/crt/crt.o o//ape/ape.o o//ape/ape.lds
make -j12 o//cosmopolitan.a

rm -rf o/cosmo
mkdir -p o/cosmo
cp o//ape/ape.o          \
   o//ape/ape.lds        \
   o//libc/crt/crt.o     \
   o//cosmopolitan.a     \
   o/cosmo
{
  echo '#ifndef COSMOPOLITAN_H_'
  echo '#define COSMOPOLITAN_H_'
  echo '#ifdef __cplusplus'
  echo 'extern "C" {'
  echo '#endif'
  cat o/ansi/cosmopolitan.h
  echo '#ifdef __cplusplus'
  echo '} /* extern C */'
  echo '#endif'
  echo '#endif /* COSMOPOLITAN_H_ */'
} >o/cosmo/cosmopolitan.h
cd o/cosmo

cat <<EOF >HELLO.C
int main(int argc, char *argv[]) {
  printf("hello world\n");
  return 0;
}
EOF

gcc -O -s -static -nostdlib -nostdinc         \
    -Wl,-T,ape.lds -include cosmopolitan.h \
    -o HELLO.COM.DBG HELLO.C crt.o ape.o cosmopolitan.a

objcopy -SO binary HELLO.COM.DBG HELLO.COM
ls -hal ./HELLO.COM
./HELLO.COM
