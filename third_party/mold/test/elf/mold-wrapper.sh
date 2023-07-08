// clang-format off
#!/bin/bash
. $(dirname $0)/common.inc

[ "$CC" = cc ] || skip

ldd mold-wrapper.so | grep -q libasan && skip

nm mold | grep -q '__[at]san_init' && skip

cat <<'EOF' > $t/a.sh
#!/bin/bash
echo "$0" "$@" $FOO
EOF

chmod 755 $t/a.sh

cat <<'EOF' | $CC -xc -o $t/exe -
#define _GNU_SOURCE 1

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
#include "third_party/getopt/getopt.internal.h"
#include "third_party/musl/crypt.h"
#include "third_party/musl/lockf.h"

extern char **environ;

int main(int argc, char **argv) {
  if (!strcmp(argv[1], "execl")) {
    execl("/usr/bin/ld", "/usr/bin/ld", "execl", (char *)0);
    perror("execl");
    return 1;
  }

  if (!strcmp(argv[1], "execlp")) {
    execlp("/usr/bin/ld", "/usr/bin/ld", "execlp", (char *)0);
    perror("execl");
    return 1;
  }

  if (!strcmp(argv[1], "execle")) {
    execle("/usr/bin/ld", "/usr/bin/ld", "execle", (char *)0, environ);
    perror("execl");
    return 1;
  }

  if (!strcmp(argv[1], "execv")) {
    execv("/usr/bin/ld", (char *[]){"/usr/bin/ld", "execv", (char *)0});
    perror("execl");
    return 1;
  }

  if (!strcmp(argv[1], "execvp")) {
    execvp("/usr/bin/ld", (char *[]){"/usr/bin/ld", "execvp", (char *)0});
    perror("execl");
    return 1;
  }

  if (!strcmp(argv[1], "execvpe")) {
    char *env[] = {"FOO=bar", NULL};
    execvpe("/usr/bin/ld", (char *[]){"/usr/bin/ld", "execvpe", (char *)0}, env);
    perror("execl");
    return 1;
  }

  fprintf(stderr, "unreachable: %s\n", argv[1]);
  return 1;
}
EOF

LD_PRELOAD=`pwd`/mold-wrapper.so MOLD_PATH=$t/a.sh $t/exe execl | grep -q 'a.sh execl'
LD_PRELOAD=`pwd`/mold-wrapper.so MOLD_PATH=$t/a.sh $t/exe execlp | grep -q 'a.sh execlp'
LD_PRELOAD=`pwd`/mold-wrapper.so MOLD_PATH=$t/a.sh $t/exe execle | grep -q 'a.sh execle'
LD_PRELOAD=`pwd`/mold-wrapper.so MOLD_PATH=$t/a.sh $t/exe execv | grep -q 'a.sh execv'
LD_PRELOAD=`pwd`/mold-wrapper.so MOLD_PATH=$t/a.sh $t/exe execvp | grep -q 'a.sh execvp'
LD_PRELOAD=`pwd`/mold-wrapper.so MOLD_PATH=$t/a.sh $t/exe execvpe | grep -q 'a.sh execvpe bar'
