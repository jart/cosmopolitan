#if 0
/*─────────────────────────────────────────────────────────────────╗
│ To the extent possible under law, Justine Tunney has waived      │
│ all copyright and related or neighboring rights to this file,    │
│ as it is written in the following disclaimers:                   │
│   • http://unlicense.org/                                        │
│   • http://creativecommons.org/publicdomain/zero/1.0/            │
╚─────────────────────────────────────────────────────────────────*/
#endif
#include "libc/calls/internal.h"
#include "libc/fmt/conv.h"
#include "libc/fmt/itoa.h"
#include "libc/intrin/bits.h"
#include "libc/macros.internal.h"
#include "libc/runtime/memtrack.internal.h"
#include "libc/stdio/stdio.h"
// clang-format off

struct WinArgs {
  char *argv[4096];
  char *envp[4092];
  intptr_t auxv[2][2];
  char argblock[ARG_MAX / 2];
  char envblock[ARG_MAX / 2];
};

//#define INCREMENT _roundup2pow(kAutomapSize/16)
#define INCREMENT (64L*1024*1024*1024)

uint64_t last;

void plan2(uint64_t addr, uint64_t end, const char *name) {
  char sz[32];
  sizefmt(sz, end-addr+1, 1024);
  printf("%08x-%08x %-6s %s\n", addr>>16, end>>16, sz, name);
}

void plan(uint64_t addr, uint64_t end, const char *name) {
  uint64_t incr, next;
  while (addr > last) {
    if ((incr = last % INCREMENT)) {
      incr = INCREMENT - incr;
    } else {
      incr = INCREMENT;
    }
    plan2(last,MIN(last+incr,addr)-1,"free");
    last = MIN(last+incr,addr);
  }
  while (addr <= end) {
    if (end-addr+1 <= INCREMENT) {
      plan2(addr,end,name);
      last = end + 1;
      break;
    }
    if (!(addr % INCREMENT)) {
      plan2(addr, addr + INCREMENT - 1, name);
      last = addr + INCREMENT;
      addr += INCREMENT;
    } else {
      next = INCREMENT - addr % INCREMENT + addr;
      plan2(addr, next - 1, name);
      last = next;
      addr = next;
    }
  }
}

int main(int argc, char *argv[]) {
  uint64_t x, y;

  plan(0x00000000, 0x00200000-1, "null");
  plan(0x00200000, 0x00400000-1, "loader");
  if (!IsWindows() || IsAtLeastWindows10()) {
    plan(0x00400000, 0x50000000-1, "image");
    plan(0x50000000, 0x7ffdffff, "arena");
    plan(0x7fff0000, 0x10007fffffff, "asan");
  } else {
    plan(0x00400000, 0x01000000-1, "image");
  }
  plan(kAutomapStart, kAutomapStart + kAutomapSize - 1, "automap");
  plan(kMemtrackStart, kMemtrackStart + kMemtrackSize - 1, "memtrack");
  plan(kFixedmapStart, kFixedmapStart + kFixedmapSize - 1, "fixedmap");
  if (IsWindows() && !IsAtLeastWindows10()) {
    plan(0x50000000, 0x7ffdffff, "arena");
  }
  x = GetStaticStackAddr(0);
  y = ROUNDUP(sizeof(struct WinArgs), FRAMESIZE);
  plan(x - y, x - 1, "winargs");
  plan(x, x + GetStackSize() - 1, "stack");
  if (!IsWindows() || IsAtLeastWindows10()) {
    plan(0x7f0000000000, 0x800000000000 - 1, "kernel");
  }

  return 0;
}
