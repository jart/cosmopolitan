#ifndef COSMOPOLITAN_LIBC_RUNTIME_MEMTRACK_H_
#define COSMOPOLITAN_LIBC_RUNTIME_MEMTRACK_H_
#include "libc/dce.h"
#include "libc/macros.internal.h"
#include "libc/nt/enum/version.h"
#include "libc/runtime/runtime.h"
#include "libc/runtime/stack.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

#define _kAutomapStart  0x0000100080000000
#define _kAutomapSize   0x00000fff80000000
#define _kFixedmapStart 0x0000300000000000
#define _kFixedmapSize  0x00000fff80000000

/*
 * TODO: Why can't we allocate addresses above 4GB on Windows 7 x64?
 * https://github.com/jart/cosmopolitan/issues/19
 */
#define MEMTRACK_ADDRESS(NORMAL, WIN7) \
  (!(IsWindows() && NtGetVersion() < kNtVersionWindows10) ? NORMAL : WIN7)
#define kAutomapStart  MEMTRACK_ADDRESS(_kAutomapStart, 0x10000000)
#define kAutomapSize   MEMTRACK_ADDRESS(_kAutomapSize, 0x30000000)
#define kFixedmapStart MEMTRACK_ADDRESS(_kFixedmapStart, 0x40000000)
#define kFixedmapSize  MEMTRACK_ADDRESS(_kFixedmapSize, 0x30000000)

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

const char *DescribeFrame(int);
void PrintSystemMappings(int) hidden;
char *DescribeMapping(int, int, char[hasatleast 8]) hidden;
bool AreMemoryIntervalsOk(const struct MemoryIntervals *) nosideeffect hidden;
void PrintMemoryIntervals(int, const struct MemoryIntervals *) hidden;
int TrackMemoryInterval(struct MemoryIntervals *, int, int, long, int,
                        int) hidden;
int ReleaseMemoryIntervals(struct MemoryIntervals *, int, int,
                           void (*)(struct MemoryIntervals *, int, int)) hidden;
void ReleaseMemoryNt(struct MemoryIntervals *, int, int) hidden;
int UntrackMemoryIntervals(void *, size_t) hidden;

forceinline pureconst bool IsAutoFrame(int x) {
  return (kAutomapStart >> 16) <= x &&
         x <= ((kAutomapStart + (kAutomapSize - 1)) >> 16);
}

forceinline pureconst bool IsArenaFrame(int x) {
  return 0x5000 <= x && x < 0x7ffe;
}

forceinline pureconst bool IsShadowFrame(int x) {
  return 0x7fff <= x && x < 0x10008000;
}

forceinline pureconst bool IsStackFrame(int x) {
  return (GetStaticStackAddr(0) >> 16) <= x &&
         x <= ((GetStaticStackAddr(0) + (GetStackSize() - FRAMESIZE)) >> 16);
}

forceinline pureconst bool IsFixedFrame(int x) {
  return (kFixedmapStart >> 16) <= x &&
         x <= ((kFixedmapStart + (kFixedmapSize - 1)) >> 16);
}

forceinline pureconst bool OverlapsImageSpace(const void *p, size_t n) {
  const unsigned char *start, *ender;
  if (n) {
    start = p;
    ender = start + (n - 1);
    return ((_base <= start && start < _end) ||
            (_base <= ender && ender < _end) ||
            (start < _base && _end <= ender));
  } else {
    return 0;
  }
}

forceinline pureconst bool OverlapsArenaSpace(const void *p, size_t n) {
  intptr_t x, y;
  if (n) {
    x = (intptr_t)p;
    y = x + (n - 1);
    return ((0x50000000 <= x && x <= 0x7ffdffff) ||
            (0x50000000 <= y && y <= 0x7ffdffff) ||
            (x < 0x50000000 && 0x7ffdffff < y));
  } else {
    return 0;
  }
}

forceinline pureconst bool OverlapsShadowSpace(const void *p, size_t n) {
  intptr_t x, y;
  if (n) {
    x = (intptr_t)p;
    y = x + (n - 1);
    return ((0x7fff0000 <= x && x <= 0x10007fffffff) ||
            (0x7fff0000 <= y && y <= 0x10007fffffff) ||
            (x < 0x7fff0000 && 0x10007fffffff < y));
  } else {
    return 0;
  }
}

forceinline unsigned FindMemoryInterval(const struct MemoryIntervals *mm,
                                        int x) {
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

forceinline bool IsMemtracked(int x, int y) {
  unsigned i;
  i = FindMemoryInterval(&_mmi, x);
  if (i == _mmi.i) return false;
  if (!(_mmi.p[i].x <= x && x <= _mmi.p[i].y)) return false;
  for (;;) {
    if (y <= _mmi.p[i].y) return true;
    if (++i == _mmi.i) return false;
    if (_mmi.p[i].x != _mmi.p[i - 1].y + 1) return false;
  }
}

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_RUNTIME_MEMTRACK_H_ */
