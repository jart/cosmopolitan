// clang-format off
#!/bin/bash
. $(dirname $0)/common.inc

[ $MACHINE = sh4 ] && skip

static=
test_cflags -static && static=-static

# I don't know why, but we need -pthread on m68k
static="$static -pthread"

cat <<EOF > $t/a.cc
int main() {
  try {
    throw 0;
  } catch (int x) {
    return x;
  }
  return 1;
}
EOF

$CXX -c -o $t/b.o $t/a.cc -fPIC
$CXX -c -o $t/c.o $t/a.cc -fno-PIC

$CXX -B. -o $t/exe1 $t/b.o $static
$QEMU $t/exe1

$CXX -B. -o $t/exe2 $t/c.o $static
$QEMU $t/exe2

$CXX -B. -o $t/exe3 $t/b.o -pie
$QEMU $t/exe3

$CXX -B. -o $t/exe4 $t/c.o -no-pie
$QEMU $t/exe4

$CXX -B. -o $t/exe5 $t/b.o -pie -Wl,--gc-sections
$QEMU $t/exe5

$CXX -B. -o $t/exe6 $t/c.o $static -Wl,--gc-sections
$QEMU $t/exe6

if [ $MACHINE = x86_64 ]; then
  $CXX -c -o $t/d.o $t/a.cc -mcmodel=large -fPIC

  $CXX -B. -o $t/exe7 $t/d.o $static
  $QEMU $t/exe7

  $CXX -B. -o $t/exe8 $t/d.o -pie
  $QEMU $t/exe8
fi

if [ $MACHINE = x86_64 -o $MACHINE = aarch64 ]; then
  $CXX -c -o $t/e.o $t/a.cc -mcmodel=large -fno-PIC

  $CXX -B. -o $t/exe9 $t/e.o $static
  $QEMU $t/exe9

  $CXX -B. -o $t/exe10 $t/e.o -no-pie
  $QEMU $t/exe10
fi

# riscv64-linux-gnu-strip crashes for some reason
if [ $MACHINE != riscv32 ]; then
  $CXX -B. -o $t/exe11 $t/b.o -pie
  $STRIP $t/exe11
  $QEMU $t/exe11

  $CXX -B. -o $t/exe12 $t/c.o -no-pie
  $STRIP $t/exe12
  $QEMU $t/exe12
fi
