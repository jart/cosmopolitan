#ifndef COSMOPOLITAN_LIBC_RUNTIME_MEMTRACK_H_
#define COSMOPOLITAN_LIBC_RUNTIME_MEMTRACK_H_
#include "libc/assert.h"
#include "libc/dce.h"
#include "libc/intrin/midpoint.h"
#include "libc/intrin/nopl.internal.h"
#include "libc/macros.internal.h"
#include "libc/nt/version.h"
#include "libc/runtime/stack.h"
#include "libc/sysv/consts/ss.h"
#include "libc/thread/tls.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

#define kAutomapStart       0x100080040000
#define kAutomapSize        (kMemtrackStart - kAutomapStart)
#define kMemtrackStart      0x1fe7fffc0000
#define kMemtrackSize       (0x1ffffffc0000 - kMemtrackStart)
#define kFixedmapStart      0x300000040000
#define kFixedmapSize       (0x400000040000 - kFixedmapStart)
#define kMemtrackFdsStart   _kMemVista(0x6fe000040000, 0x5e000040000)
#define kMemtrackFdsSize    \
  (_kMemVista(0x6feffffc0000, 0x5effffc0000) - kMemtrackFdsStart)
#define kMemtrackZiposStart _kMemVista(0x6fd000040000, 0x5d000040000)
#define kMemtrackZiposSize  \
  (_kMemVista(0x6fdffffc0000, 0x5dffffc0000) - kMemtrackZiposStart)
#define kMemtrackNsyncStart _kMemVista(0x6fc000040000, 0x5c000040000)
#define kMemtrackNsyncSize  \
  (_kMemVista(0x6fcffffc0000, 0x5cffffc0000 - kMemtrackNsyncStart)
#define kMemtrackGran       (!IsAsan() ? FRAMESIZE : FRAMESIZE * 8)
#define _kMemVista(NORMAL, WINVISTA) \
  (!IsWindows() || IsAtleastWindows8p1() ? NORMAL : WINVISTA)

struct MemoryInterval {
  int x;
  int y;
  long h;
  long size;
  int prot;
  int flags;
  long offset;
  bool iscow;
  bool readonlyfile;
};

struct MemoryIntervals {
  size_t i, n;
  struct MemoryInterval *p;
  struct MemoryInterval s[OPEN_MAX];
};

extern hidden struct MemoryIntervals _mmi;

void __mmi_lock(void) hidden;
void __mmi_unlock(void) hidden;
bool IsMemtracked(int, int) hidden;
void PrintSystemMappings(int) hidden;
bool AreMemoryIntervalsOk(const struct MemoryIntervals *) nosideeffect hidden;
void PrintMemoryIntervals(int, const struct MemoryIntervals *) hidden;
int TrackMemoryInterval(struct MemoryIntervals *, int, int, long, int, int,
                        bool, bool, long, long) hidden;
int ReleaseMemoryIntervals(struct MemoryIntervals *, int, int,
                           void (*)(struct MemoryIntervals *, int, int)) hidden;
void ReleaseMemoryNt(struct MemoryIntervals *, int, int) hidden;
int UntrackMemoryIntervals(void *, size_t) hidden;
size_t GetMemtrackSize(struct MemoryIntervals *);

#ifdef _NOPL0
#define __mmi_lock()   _NOPL0("__threadcalls", __mmi_lock)
#define __mmi_unlock() _NOPL0("__threadcalls", __mmi_unlock)
#else
#define __mmi_lock()   (__threaded ? __mmi_lock() : 0)
#define __mmi_unlock() (__threaded ? __mmi_unlock() : 0)
#endif

#define IsLegalPointer(p) \
  (-0x800000000000 <= (intptr_t)(p) && (intptr_t)(p) <= 0x7fffffffffff)

forceinline pureconst bool IsLegalSize(size_t n) {
  return n <= 0x7fffffffffff;
}

forceinline pureconst bool IsAutoFrame(int x) {
  return (int)(kAutomapStart >> 16) <= x &&
         x <= (int)((kAutomapStart + kAutomapSize - 1) >> 16);
}

forceinline pureconst bool IsMemtrackFrame(int x) {
  return (int)(kAutomapStart >> 16) <= x &&
         x <= (int)((kAutomapStart + kAutomapSize - 1) >> 16);
}

forceinline pureconst bool IsGfdsFrame(int x) {
  return (int)(kMemtrackFdsStart >> 16) <= x &&
         x <= (int)((kMemtrackFdsStart + kMemtrackFdsSize - 1) >> 16);
}

forceinline pureconst bool IsZiposFrame(int x) {
  return (int)(kMemtrackZiposStart >> 16) <= x &&
         x <= (int)((kMemtrackZiposStart + kMemtrackZiposSize - 1) >> 16);
}

forceinline pureconst bool IsNsyncFrame(int x) {
  return (int)(kMemtrackNsyncStart >> 16) <= x &&
         x <= (int)((kMemtrackNsyncStart + kMemtrackNsyncSize - 1) >> 16);
}

forceinline pureconst bool IsShadowFrame(int x) {
  return 0x7fff <= x && x < 0x10008000;
}

forceinline pureconst bool IsArenaFrame(int x) {
  return 0x5004 <= x && x <= 0x7ffb;
}

forceinline pureconst bool IsKernelFrame(int x) {
  intptr_t stack = GetStaticStackAddr(0);
  return (int)(stack >> 16) <= x &&
         x <= (int)((stack + (GetStackSize() - FRAMESIZE)) >> 16);
}

forceinline pureconst bool IsStaticStackFrame(int x) {
  intptr_t stack = GetStaticStackAddr(0);
  return (int)(stack >> 16) <= x &&
         x <= (int)((stack + (GetStackSize() - FRAMESIZE)) >> 16);
}

forceinline pureconst bool IsStackFrame(int x) {
  intptr_t stack = GetStackAddr();
  return (int)(stack >> 16) <= x &&
         x <= (int)((stack + (GetStackSize() - FRAMESIZE)) >> 16);
}

forceinline pureconst bool IsOldStackFrame(int x) {
  /* openbsd uses 4mb stack by default */
  /* freebsd uses 512mb stack by default */
  /* most systems use 8mb stack by default */
  intptr_t old = ROUNDDOWN(__oldstack, GetStackSize());
  return (old >> 16) <= x && x <= ((old + (GetStackSize() - FRAMESIZE)) >> 16);
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
    m = _midpoint(l, r);
    if (mm->p[m].y < x) {
      l = m + 1;
    } else {
      r = m;
    }
  }
  assert(l == mm->i || x <= mm->p[l].y);
  return l;
}

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_RUNTIME_MEMTRACK_H_ */
