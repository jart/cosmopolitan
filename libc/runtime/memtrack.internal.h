#ifndef COSMOPOLITAN_LIBC_RUNTIME_MEMTRACK_H_
#define COSMOPOLITAN_LIBC_RUNTIME_MEMTRACK_H_
#include "libc/dce.h"
#include "libc/macros.internal.h"
#include "libc/nt/enum/version.h"
#include "libc/runtime/runtime.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

#define _kAutomapStart  0x0000100080000000 /* asan can't spread its poison */
#define _kAutomapSize   0x00000fff80000000 /* beyond the above mem address */
#define _kFixedmapStart 0x0000200000000000

/*
 * TODO: Why can't we allocate addresses above 4GB on Windows 7 x64?
 * https://github.com/jart/cosmopolitan/issues/19
 */
#define MEMTRACK_ADDRESS(NORMAL, WIN7) \
  (!(IsWindows() && NtGetVersion() < kNtVersionWindows10) ? NORMAL : WIN7)
#define kAutomapStart  MEMTRACK_ADDRESS(_kAutomapStart, 0x10000000)
#define kAutomapSize   MEMTRACK_ADDRESS(_kAutomapSize, 0x40000000)
#define kFixedmapStart MEMTRACK_ADDRESS(_kFixedmapStart, 0x40000000)

struct MemoryInterval {
  int x;
  int y;
  long h;
  int prot;
  int flags;
};

struct MemoryIntervals {
  long i, n;
  struct MemoryInterval *p;
  struct MemoryInterval s[OPEN_MAX];
};

extern hidden struct MemoryIntervals _mmi;

bool AreMemoryIntervalsOk(const struct MemoryIntervals *) nosideeffect hidden;
void PrintMemoryIntervals(int, const struct MemoryIntervals *) hidden;
int TrackMemoryInterval(struct MemoryIntervals *, int, int, long, int,
                        int) hidden;
int ReleaseMemoryIntervals(struct MemoryIntervals *, int, int,
                           void (*)(struct MemoryIntervals *, int, int)) hidden;
void ReleaseMemoryNt(struct MemoryIntervals *, int, int) hidden;
int UntrackMemoryIntervals(void *, size_t) hidden;

static inline noasan unsigned FindMemoryInterval(
    const struct MemoryIntervals *mm, int x) {
  unsigned l, m, r;
  l = 0;
  r = mm->i;
  while (l < r) {
    m = (l + r) >> 1;
    if (mm->p[m].y < x) {
      l = m + 1;
    } else {
      r = m;
    }
  }
  return l;
}

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_RUNTIME_MEMTRACK_H_ */
