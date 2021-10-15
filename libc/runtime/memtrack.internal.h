#ifndef COSMOPOLITAN_LIBC_RUNTIME_MEMTRACK_H_
#define COSMOPOLITAN_LIBC_RUNTIME_MEMTRACK_H_
#include "libc/dce.h"
#include "libc/macros.internal.h"
#include "libc/nt/enum/version.h"
#include "libc/runtime/runtime.h"
#include "libc/runtime/stack.h"
#include "libc/sysv/consts/ss.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

#define kAutomapStart _kMem(0x100080000000, 0x000010000000)
#define kAutomapSize                                             \
  _kMem(0x200000000000 - 0x100080000000 - _kMmi(0x800000000000), \
        0x000040000000 - 0x000010000000 - _kMmi(0x000080000000))
#define kMemtrackStart                          \
  _kMem(0x200000000000 - _kMmi(0x800000000000), \
        0x000040000000 - _kMmi(0x000080000000))
#define kMemtrackSize  _kMem(_kMmi(0x800000000000), _kMmi(0x000080000000))
#define kMemtrackGran  (!IsAsan() ? FRAMESIZE : FRAMESIZE * 8)
#define kFixedmapStart _kMem(0x300000000000, 0x000040000000)
#define kFixedmapSize \
  _kMem(0x400000000000 - 0x300000000000, 0x000070000000 - 0x000040000000)
#define _kMmi(VSPACE) \
  ROUNDUP(VSPACE / FRAMESIZE * sizeof(struct MemoryInterval), FRAMESIZE)
#define _kMem(NORMAL, WIN7) \
  (!(IsWindows() && NtGetVersion() < kNtVersionWindows10) ? NORMAL : WIN7)

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

#define IsLegalPointer(p) \
  (-0x800000000000 <= (intptr_t)(p) && (intptr_t)(p) <= 0x7fffffffffff)

forceinline pureconst bool IsAutoFrame(int x) {
  return (kAutomapStart >> 16) <= x &&
         x <= ((kAutomapStart + (kAutomapSize - 1)) >> 16);
}

forceinline pureconst bool IsMemtrackFrame(int x) {
  return (kAutomapStart >> 16) <= x &&
         x <= ((kAutomapStart + (kAutomapSize - 1)) >> 16);
}

forceinline pureconst bool IsArenaFrame(int x) {
  return 0x5000 <= x && x < 0x7ffe;
}

forceinline pureconst bool IsShadowFrame(int x) {
  return 0x7fff <= x && x < 0x10008000;
}

forceinline pureconst bool IsStaticStackFrame(int x) {
  return (GetStaticStackAddr(0) >> 16) <= x &&
         x <= ((GetStaticStackAddr(0) + (GetStackSize() - FRAMESIZE)) >> 16);
}

forceinline pureconst bool IsSigAltStackFrame(int x) {
  return (GetStackAddr(0) >> 16) <= x &&
         x <= ((GetStackAddr(0) + (SIGSTKSZ - FRAMESIZE)) >> 16);
}

forceinline pureconst bool IsOldStackFrame(int x) {
  intptr_t old = ROUNDDOWN(__oldstack, STACKSIZE);
  return (old >> 16) <= x && x <= ((old + (STACKSIZE - FRAMESIZE)) >> 16);
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
