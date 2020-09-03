#if 0
/*─────────────────────────────────────────────────────────────────╗
│ To the extent possible under law, Justine Tunney has waived      │
│ all copyright and related or neighboring rights to this file,    │
│ as it is written in the following disclaimers:                   │
│   • http://unlicense.org/                                        │
│   • http://creativecommons.org/publicdomain/zero/1.0/            │
╚─────────────────────────────────────────────────────────────────*/
#endif
#include "libc/bits/bits.h"
#include "libc/calls/calls.h"
#include "libc/macros.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/stdio.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/prot.h"

/*
  OpenBSD
  00025ff61000 000000000000 65536
  00026cf1d000 00000cfbc000 65536
  000254d83000 ffffe7e66000 65536
  0002c7bde000 000072e5b000 65536
  000253ff9000 ffff8c41b000 65536
  0002e96f3000 0000956fa000 65536
  000236346000 ffff4cc53000 65536
  0002ce744000 0000983fe000 65536
  0002fe0b8000 00002f974000 65536
  000225cd7000 ffff27c1f000 65536
*/

struct Mapping {
  uint8_t *map;
  size_t mapsize;
};

int main(int argc, char *argv[]) {
  size_t i;
  struct Mapping m[10];
  for (i = 0; i < ARRAYLEN(m); ++i) {
    m[i].mapsize = FRAMESIZE;
    m[i].map = mmap(NULL, m[i].mapsize, PROT_READ | PROT_WRITE,
                    MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
  }
  for (i = 0; i < ARRAYLEN(m); ++i) {
    printf("%p %p %zu\n", m[i].map, i ? m[i].map - m[i - 1].map : 0,
           m[i].mapsize);
  }
  return 0;
}
