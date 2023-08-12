// clang-format off
#!/bin/bash
. $(dirname $0)/common.inc

supports_ifunc || skip

cat <<EOF | $CC -fno-PIE -o $t/a.o -c -xc -
#include "libc/runtime/dlfcn.h"
#include "libc/calls/calls.h"
#include "libc/calls/weirdtypes.h"
#include "libc/fmt/fmt.h"
#include "libc/stdio/dprintf.h"
#include "libc/stdio/stdio.h"
#include "libc/stdio/temp.h"
#include "third_party/musl/tempnam.h"

typedef void Func();
void foo(void);

int main() {
  void *handle = dlopen(NULL, RTLD_NOW);
  Func *p = dlsym(handle, "foo");

  foo();
  p();
  printf("%p %p\n", foo, p);
}
EOF

cat <<EOF | $CC -fPIC -o $t/b.o -c -xc -
#include "libc/calls/calls.h"
#include "libc/calls/weirdtypes.h"
#include "libc/fmt/fmt.h"
#include "libc/stdio/dprintf.h"
#include "libc/stdio/stdio.h"
#include "libc/stdio/temp.h"
#include "third_party/musl/tempnam.h"

__attribute__((ifunc("resolve_foo")))
void foo(void);

static void real_foo(void) {
  printf("foo ");
}

typedef void Func();

static Func *resolve_foo(void) {
  return real_foo;
}
EOF

$CC -B. -o $t/c.so $t/b.o -shared
$CC -B. -o $t/exe $t/a.o $t/c.so -no-pie -ldl
$QEMU $t/exe | grep -q 'foo foo'
