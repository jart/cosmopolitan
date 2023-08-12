// clang-format off
#!/bin/bash
. $(dirname $0)/common.inc

cat <<EOF | $CC -c -o $t/a.o -xc -
__attribute__((section("foo"))) char bar[] = "section foo";
EOF

# Test synthetic symbols

cat <<EOF | $CC -c -o $t/b.o -xc -
#include "libc/assert.h"
#include "libc/calls/calls.h"
#include "libc/calls/weirdtypes.h"
#include "libc/fmt/fmt.h"
#include "libc/stdio/dprintf.h"
#include "libc/stdio/stdio.h"
#include "libc/stdio/temp.h"
#include "third_party/musl/tempnam.h"
#include "libc/mem/alg.h"
#include "libc/mem/mem.h"
#include "libc/str/str.h"

extern char __ehdr_start[];
extern char __executable_start[];
extern char __dso_handle[];
extern char _end[];
extern char end[];
extern char _etext[];
extern char etext[];
extern char _edata[];
extern char edata[];
extern char __start_foo[];
extern char __stop_foo[];

int main() {
  assert(_end);
  assert(_end == end);
  assert(_etext);
  assert(_etext == etext);
  assert(_edata);
  assert(_edata == edata);

  printf("__ehdr_start=%p\n", &__ehdr_start);
  printf("__executable_start=%p\n", &__executable_start);
  printf("__dso_handle=%p\n", &__dso_handle);
  printf("%.*s\n", (int)(__stop_foo - __start_foo), __start_foo);
}
EOF

$CC -B. -no-pie -Wl,--image-base=0x40000 -o $t/exe $t/a.o $t/b.o
$QEMU $t/exe > $t/log

grep -q '^__ehdr_start=0x40000$' $t/log
grep -q '^__executable_start=0x40000$' $t/log
grep -q '^__dso_handle=' $t/log
grep -q '^section foo$' $t/log

# Make sure that synthetic symbols overwrite existing ones

cat <<EOF | $CC -c -o $t/c.o -xc -
#include "libc/assert.h"
#include "libc/calls/calls.h"
#include "libc/calls/weirdtypes.h"
#include "libc/fmt/fmt.h"
#include "libc/stdio/dprintf.h"
#include "libc/stdio/stdio.h"
#include "libc/stdio/temp.h"
#include "third_party/musl/tempnam.h"
#include "libc/mem/alg.h"
#include "libc/mem/mem.h"
#include "libc/str/str.h"

char __ehdr_start[] = "foo";
char __executable_start[] = "foo";
char _end[] = "foo";
char end[] = "foo";
char _etext[] = "foo";
char etext[] = "foo";
char _edata[] = "foo";
char edata[] = "foo";
char __start_foo[] = "foo";
char __stop_foo[] = "foo";

int main() {
  assert(_end);
  assert(_end != end);
  assert(_etext);
  assert(_etext != etext);
  assert(_edata);
  assert(_edata != edata);

  printf("end=%s\n", end);
  printf("etext=%s\n", etext);
  printf("edata=%s\n", edata);

  printf("__ehdr_start=%p\n", &__ehdr_start);
  printf("__executable_start=%p\n", &__executable_start);
  printf("%.*s\n", (int)(__stop_foo - __start_foo), __start_foo);
}
EOF

$CC -B. -no-pie -Wl,--image-base=0x40000 -o $t/exe $t/a.o $t/c.o
$QEMU $t/exe > $t/log

grep -q '^end=foo$' $t/log
grep -q '^etext=foo$' $t/log
grep -q '^edata=foo$' $t/log
grep -q '^__ehdr_start=0x40000$' $t/log
grep -q '^__executable_start=0x40000$' $t/log
grep -q '^section foo$' $t/log
