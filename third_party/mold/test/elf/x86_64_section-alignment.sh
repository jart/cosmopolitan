// clang-format off
#!/bin/bash
. $(dirname $0)/common.inc

[ $MACHINE = x86_64 ] || skip

cat <<'EOF' | $CC -c -o $t/a.o -xc -
#include "libc/inttypes.h"
#include "libc/limits.h"
#include "libc/literal.h"
#include "libc/calls/calls.h"
#include "libc/calls/weirdtypes.h"
#include "libc/fmt/fmt.h"
#include "libc/stdio/dprintf.h"
#include "libc/stdio/stdio.h"
#include "libc/stdio/temp.h"
#include "third_party/musl/tempnam.h"

__attribute__((aligned(8192))) int foo = 1;

typedef struct {
  uint8_t e_ident[16];
  uint16_t e_type;
  uint16_t e_machine;
  uint32_t e_version;
  uint64_t e_entry;
  uint64_t e_phoff;
  uint64_t e_shoff;
  uint32_t e_flags;
  uint16_t e_ehsize;
  uint16_t e_phentsize;
  uint16_t e_phnum;
  uint16_t e_shentsize;
  uint16_t e_shnum;
  uint16_t e_shstrndx;
} Ehdr;

char __ehdr_start;

int main() {
  Ehdr *e = (Ehdr *)&__ehdr_start;
  printf("%lu %lu %lu\n", e->e_phoff % 8, e->e_shoff % 8, (uint64_t)&foo % 8192);
}
EOF

$CC -B. -o $t/exe $t/a.o
$QEMU $t/exe | grep -q '^0 0 0$'
