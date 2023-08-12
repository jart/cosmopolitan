// clang-format off
#!/bin/bash
. $(dirname $0)/common.inc

# Test a tricky case of TLS alignment requirement where not only the virtual
# address of a symbol but also its offset against the TLS base address has to
# be aligned.
#
# On glibc, this issue requires a TLS model equivalent to global-dynamic in
# order to be triggered.

cat <<EOF | $CC -fPIC -c -o $t/a.o -xc -
#include "libc/assert.h"
#include "libc/calls/calls.h"
#include "libc/calls/termios.h"
#include "libc/fmt/conv.h"
#include "libc/limits.h"
#include "libc/mem/alg.h"
#include "libc/mem/alloca.h"
#include "libc/mem/mem.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/dprintf.h"
#include "libc/stdio/rand.h"
#include "libc/stdio/temp.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/exit.h"
#include "third_party/getopt/getopt.internal.h"
#include "third_party/musl/crypt.h"
#include "third_party/musl/rand48.h"

// .tdata
_Thread_local int x = 42;
// .tbss
__attribute__ ((aligned(64)))
_Thread_local int y = 0;

void *verify(void *unused) {
  assert((unsigned long)(&y) % 64 == 0);
  return NULL;
}
EOF

cat <<EOF | $CC -fPIC -c -o $t/b.o -xc -
#include "libc/calls/weirdtypes.h"
#include "libc/sysv/consts/clock.h"
#include "libc/thread/thread.h"
#include "libc/thread/thread2.h"
#include "libc/runtime/dlfcn.h"
#include "libc/assert.h"
void *(*verify)(void *);

int main() {
  void *handle = dlopen("a.so", RTLD_NOW);
  assert(handle);
  *(void**)(&verify) = dlsym(handle, "verify");
  assert(verify);

  pthread_t thread;

  verify(NULL);

  pthread_create(&thread, NULL, verify, NULL);
  pthread_join(thread, NULL);
}
EOF

$CC -B. -shared -o $t/a.so $t/a.o
$CC -B. -ldl -pthread -o $t/exe $t/b.o -Wl,-rpath,$t
$QEMU $t/exe