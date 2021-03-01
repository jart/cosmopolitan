#if 0
/*─────────────────────────────────────────────────────────────────╗
│ To the extent possible under law, Justine Tunney has waived      │
│ all copyright and related or neighboring rights to this file,    │
│ as it is written in the following disclaimers:                   │
│   • http://unlicense.org/                                        │
│   • http://creativecommons.org/publicdomain/zero/1.0/            │
╚─────────────────────────────────────────────────────────────────*/
#endif
#include "libc/fmt/itoa.h"
#include "libc/macros.internal.h"
#include "libc/runtime/e820.internal.h"
#include "libc/runtime/pc.internal.h"
#include "libc/stdio/stdio.h"

const char *DescribeMemoryType(int type) {
  switch (type) {
    case kMemoryUsable:
      return "kMemoryUsable";
    case kMemoryUnusable:
      return "kMemoryUnusable";
    case kMemoryAcpiReclaimable:
      return "kMemoryAcpiReclaimable";
    case kMemoryAcpiNvs:
      return "kMemoryAcpiNvs";
    case kMemoryBad:
      return "kMemoryBad";
    default:
      return "UNKNOWN";
  }
}

int main(int argc, char *argv[]) {
  int i;
  const char *ts;
  struct mman *mm;
  mm = (struct mman *)(BANE + 0x0500);
  for (i = 0; i < mm->e820n; ++i) {
    printf("%p-%p %s\n", mm->e820[i].addr, mm->e820[i].size,
           DescribeMemoryType(mm->e820[i].type));
  }
  for (;;) asm("pause");
  return 0;
}
