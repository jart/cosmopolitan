// clang-format off
#!/bin/bash
. $(dirname $0)/common.inc

ldd --help 2>&1 | grep -q musl && skip

[ $MACHINE = x86_64 ] || skip

cat <<EOF | $CC -c -o $t/a.o -x assembler -
.globl preinit, init, fini

.section .preinit_array,"aw",@preinit_array
.p2align 3
.quad preinit

.section .init_array,"aw",@init_array
.p2align 3
.quad init

.section .fini_array,"aw",@fini_array
.p2align 3
.quad fini
EOF

cat <<EOF | $CC -c -o $t/b.o -xc -
#include "libc/calls/calls.h"
#include "libc/calls/weirdtypes.h"
#include "libc/fmt/fmt.h"
#include "libc/stdio/dprintf.h"
#include "libc/stdio/stdio.h"
#include "libc/stdio/temp.h"
#include "third_party/musl/tempnam.h"
#include "libc/calls/calls.h"
#include "libc/calls/weirdtypes.h"
#include "libc/runtime/pathconf.h"
#include "libc/runtime/runtime.h"
#include "libc/runtime/sysconf.h"
#include "libc/sysv/consts/f.h"
#include "libc/sysv/consts/fileno.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/ok.h"
#include "libc/time/time.h"
#include "third_party/getopt/getopt.h"
#include "third_party/musl/crypt.h"
#include "third_party/musl/lockf.h"

void preinit() { write(STDOUT_FILENO, "preinit ", 8); }
void init() { write(STDOUT_FILENO, "init ", 5); }
void fini() { write(STDOUT_FILENO, "fini\n", 5); }
int main() {}
EOF

$CC -B. -o $t/exe $t/a.o $t/b.o
$QEMU $t/exe | grep -q 'preinit init fini'
