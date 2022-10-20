#ifndef COSMOPOLITAN_LIBC_RUNTIME_MEMTRACK_H_
#define COSMOPOLITAN_LIBC_RUNTIME_MEMTRACK_H_
#include "ape/sections.internal.h"
#include "libc/dce.h"
#include "libc/intrin/nopl.internal.h"
#include "libc/macros.internal.h"
#include "libc/nt/version.h"
#include "libc/runtime/stack.h"
#include "libc/sysv/consts/ss.h"
#include "libc/thread/tls.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

#define kAutomapStart         0x100080040000
#define kAutomapSize          (kMemtrackStart - kAutomapStart)
#define kMemtrackStart        0x1fe7fffc0000
#define kMemtrackSize         (0x1ffffffc0000 - kMemtrackStart)
#define kFixedmapStart        0x300000040000
#define kFixedmapSize         (0x400000040000 - kFixedmapStart)
#define kMemtrackFdsStart     0x6fe000040000
#define kMemtrackFdsSize      (0x6feffffc0000 - kMemtrackFdsStart)
#define kMemtrackZiposStart   0x6fd000040000
#define kMemtrackZiposSize    (0x6fdffffc0000 - kMemtrackZiposStart)
#define kMemtrackKmallocStart 0x6fc000040000
#define kMemtrackKmallocSize  (0x6fcffffc0000 - kMemtrackKmallocStart)
#define kMemtrackGran         (!IsAsan() ? FRAMESIZE : FRAMESIZE * 8)

struct MemoryInterval {
  int x;
  int y;
  long h;
  long size;
  long offset;
  int flags;
  char prot;
  bool iscow;
  bool readonlyfile;
};

struct MemoryIntervals {
  size_t i, n;
  struct MemoryInterval *p;
  struct MemoryInterval s[OPEN_MAX];
};

extern struct MemoryIntervals _mmi;

void __mmi_lock(void);
void __mmi_unlock(void);
void __mmi_funlock(void);
bool IsMemtracked(int, int);
void PrintSystemMappings(int);
unsigned FindMemoryInterval(const struct MemoryIntervals *, int) nosideeffect;
bool AreMemoryIntervalsOk(const struct MemoryIntervals *) nosideeffect;
void PrintMemoryIntervals(int, const struct MemoryIntervals *);
int TrackMemoryInterval(struct MemoryIntervals *, int, int, long, int, int,
                        bool, bool, long, long);
int ReleaseMemoryIntervals(struct MemoryIntervals *, int, int,
                           void (*)(struct MemoryIntervals *, int, int));
void ReleaseMemoryNt(struct MemoryIntervals *, int, int);
int UntrackMemoryIntervals(void *, size_t);
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

forceinline pureconst bool IsKmallocFrame(int x) {
  return (int)(kMemtrackKmallocStart >> 16) <= x &&
         x <= (int)((kMemtrackKmallocStart + kMemtrackKmallocSize - 1) >> 16);
}

forceinline pureconst bool IsShadowFrame(int x) {
  return 0x7fff <= x && x < 0x10008000;
}

forceinline pureconst bool IsArenaFrame(int x) {
  return 0x5004 <= x && x <= 0x7ffb;
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
  const unsigned char *BegA, *EndA, *BegB, *EndB;
  if (n) {
    BegA = p;
    EndA = BegA + (n - 1);
    BegB = _base;
    EndB = _end - 1;
    return MAX(BegA, BegB) < MIN(EndA, EndB);
  } else {
    return 0;
  }
}

forceinline pureconst bool OverlapsArenaSpace(const void *p, size_t n) {
  intptr_t BegA, EndA, BegB, EndB;
  if (n) {
    BegA = (intptr_t)p;
    EndA = BegA + (n - 1);
    BegB = 0x50000000;
    EndB = 0x7ffdffff;
    return MAX(BegA, BegB) < MIN(EndA, EndB);
  } else {
    return 0;
  }
}

forceinline pureconst bool OverlapsShadowSpace(const void *p, size_t n) {
  intptr_t BegA, EndA, BegB, EndB;
  if (n) {
    BegA = (intptr_t)p;
    EndA = BegA + (n - 1);
    BegB = 0x7fff0000;
    EndB = 0x10007fffffff;
    return MAX(BegA, BegB) < MIN(EndA, EndB);
  } else {
    return 0;
  }
}

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_RUNTIME_MEMTRACK_H_ */
