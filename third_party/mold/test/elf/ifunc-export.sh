// clang-format off
#!/bin/bash
. $(dirname $0)/common.inc

supports_ifunc || skip

cat <<EOF | $CC -c -fPIC -o $t/a.o -xc -
#include "libc/calls/calls.h"
#include "libc/calls/weirdtypes.h"
#include "libc/fmt/fmt.h"
#include "libc/stdio/dprintf.h"
#include "libc/stdio/stdio.h"
#include "libc/stdio/temp.h"
#include "third_party/musl/tempnam.h"

__attribute__((ifunc("resolve_foobar")))
void foobar(void);

void real_foobar(void) {
  printf("Hello world\n");
}

typedef void Func();

Func *resolve_foobar(void) {
  return real_foobar;
}
EOF

$CC -B. -shared -o $t/b.so $t/a.o
readelf --dyn-syms $t/b.so | grep -Eq '(IFUNC|<OS specific>: 10)\s+GLOBAL DEFAULT.* foobar'
