#if 0
/*─────────────────────────────────────────────────────────────────╗
│ To the extent possible under law, Justine Tunney has waived      │
│ all copyright and related or neighboring rights to this file,    │
│ as it is written in the following disclaimers:                   │
│   • http://unlicense.org/                                        │
│   • http://creativecommons.org/publicdomain/zero/1.0/            │
╚─────────────────────────────────────────────────────────────────*/
#endif
#include "libc/calls/calls.h"
#include "libc/calls/hefty/spawn.h"
#include "libc/calls/internal.h"
#include "libc/conv/conv.h"
#include "libc/log/check.h"
#include "libc/mem/mem.h"
#include "libc/nt/enum/filemapflags.h"
#include "libc/nt/enum/pageflags.h"
#include "libc/nt/memory.h"
#include "libc/runtime/gc.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/stdio.h"
#include "libc/sysv/consts/fileno.h"
#include "libc/x/x.h"

int main(int argc, char *argv[]) {
  int pid;
  long *addr;
  int64_t fh;
  if (argc == 1) {
    fh = CreateFileMappingNuma(-1, &kNtIsInheritable, kNtPageReadwrite, 0,
                               FRAMESIZE, NULL, kNtNumaNoPreferredNode);
    addr = MapViewOfFileExNuma(fh, kNtFileMapRead | kNtFileMapWrite, 0, 0,
                               FRAMESIZE, NULL, kNtNumaNoPreferredNode);
    *addr = 0x31337;
    CHECK_NE(-1, (pid = spawnve(
                      0, (int[3]){STDIN_FILENO, STDOUT_FILENO, STDERR_FILENO},
                      "o/examples/forky.com",
                      (char *const[]){"o/examples/forky.com",
                                      gc(xasprintf("%#lx", (intptr_t)addr)),
                                      gc(xasprintf("%#lx", fh)), NULL},
                      environ)));
    CHECK_NE(-1, waitpid(pid, NULL, 0));
  } else {
    addr = (long *)(intptr_t)strtoul(argv[1], NULL, 0);
    fh = strtoul(argv[2], NULL, 0);
    addr = MapViewOfFileExNuma(fh, kNtFileMapRead | kNtFileMapWrite, 0, 0,
                               FRAMESIZE, addr, kNtNumaNoPreferredNode);
    printf("%#lx\n", *addr);
  }
  return 0;
}
