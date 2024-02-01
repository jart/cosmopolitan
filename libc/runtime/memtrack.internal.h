#ifndef COSMOPOLITAN_LIBC_RUNTIME_MEMTRACK_H_
#define COSMOPOLITAN_LIBC_RUNTIME_MEMTRACK_H_
#include "ape/sections.internal.h"
#include "libc/dce.h"
#include "libc/macros.internal.h"
#include "libc/nt/version.h"
#include "libc/runtime/runtime.h"
#include "libc/runtime/stack.h"
#include "libc/sysv/consts/ss.h"
#include "libc/thread/tls.h"
COSMOPOLITAN_C_START_

#define kAutomapStart       0x100080040000
#define kAutomapSize        (kMemtrackStart - kAutomapStart)
#define kMemtrackStart      0x1fe7fffc0000
#define kMemtrackSize       (0x1ffffffc0000 - kMemtrackStart)
#define kFixedmapStart      0x300000040000
#define kFixedmapSize       (0x400000040000 - kFixedmapStart)
#define kMemtrackNsyncStart 0x6fc000040000
#define kMemtrackNsyncSize  (0x6fcffffc0000 - kMemtrackNsyncStart)
#define kMemtrackFdsStart   0x6fe000040000
#define kMemtrackFdsSize    (0x6feffffc0000 - kMemtrackFdsStart)
#define kMemtrackZiposStart 0x6fd000040000
#define kMemtrackZiposSize  (0x6fdffffc0000 - kMemtrackZiposStart)
#define kMemtrackGran       (!IsAsan() ? FRAMESIZE : FRAMESIZE * 8)

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
  struct MemoryInterval s[16];
};

extern struct MemoryIntervals _mmi;

void __mmi_lock(void);
void __mmi_unlock(void);
bool IsMemtracked(int, int);
void PrintSystemMappings(int);
unsigned __find_memory(const struct MemoryIntervals *, int) nosideeffect;
bool __check_memtrack(const struct MemoryIntervals *) nosideeffect;
void PrintMemoryIntervals(int, const struct MemoryIntervals *);
int __track_memory(struct MemoryIntervals *, int, int, long, int, int, bool,
                   bool, long, long);
int __untrack_memory(struct MemoryIntervals *, int, int,
                     void (*)(struct MemoryIntervals *, int, int));
void __release_memory_nt(struct MemoryIntervals *, int, int);
int __untrack_memories(void *, size_t);
size_t __get_memtrack_size(struct MemoryIntervals *) nosideeffect;

#ifdef __x86_64__
/*
 * AMD64 has 48-bit signed pointers (PML4T)
 * AMD64 is trying to go bigger, i.e. 57-bit (PML5T)
 * LINUX forbids userspace from leveraging negative pointers
 * Q-EMU may impose smaller vaspaces emulating AMD on non-AMD
 *
 * Having "signed pointers" means these top sixteen bits
 *
 *     0x0000000000000000
 *       ^^^^
 *
 * must be
 *
 *   - 0000 for positive pointers
 *   - FFFF for negative pointers
 *
 * otherwise the instruction using the faulty pointer will fault.
 */
#define IsLegalPointer(p) \
  (-0x800000000000 <= (intptr_t)(p) && (intptr_t)(p) <= 0x7fffffffffff)
#define ADDR_32_TO_48(x) (intptr_t)((uint64_t)(int)(x) << 16)
#elif defined(__aarch64__)
/*
 * ARM64 has 48-bit unsigned pointers (Armv8.0-A)
 * ARM64 can possibly go bigger, i.e. 52-bit (Armv8.2-A)
 * ARM64 can impose arbitrarily smaller vaspaces, e.g. 40/44-bit
 * APPLE in their limitless authoritarianism forbids 32-bit pointers
 */
#define IsLegalPointer(p) ((uintptr_t)(p) <= 0xffffffffffff)
#define ADDR_32_TO_48(x)  (uintptr_t)((uint64_t)(uint32_t)(x) << 16)
#else
/* RISC-V Sipeed Nezha has 39-bit vaspace */
#error "unsupported architecture"
#endif

forceinline pureconst bool IsLegalSize(uint64_t n) {
  /* subtract frame size so roundup is safe */
  return n <= 0x800000000000 - FRAMESIZE;
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

forceinline pureconst bool IsNsyncFrame(int x) {
  return (int)(kMemtrackNsyncStart >> 16) <= x &&
         x <= (int)((kMemtrackNsyncStart + kMemtrackNsyncSize - 1) >> 16);
}

forceinline pureconst bool IsZiposFrame(int x) {
  return (int)(kMemtrackZiposStart >> 16) <= x &&
         x <= (int)((kMemtrackZiposStart + kMemtrackZiposSize - 1) >> 16);
}

forceinline pureconst bool IsShadowFrame(int x) {
  return 0x7fff <= x && x < 0x10008000;
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

forceinline pureconst bool IsOldStack(const void *x) {
  size_t foss_stack_size = 8ul * 1024 * 1024;
  uintptr_t top = __oldstack + foss_stack_size;
  uintptr_t bot = __oldstack - foss_stack_size;
  return bot <= (uintptr_t)x && (uintptr_t)x < top;
}

forceinline pureconst bool IsOldStackFrame(int x) {
  size_t foss_stack_size = 8ul * 1024 * 1024;
  uintptr_t top = __oldstack + foss_stack_size;
  uintptr_t bot = __oldstack - foss_stack_size;
  return (int)(bot >> 16) <= x && x <= (int)((top >> 16) - 1);
}

forceinline pureconst bool IsFixedFrame(int x) {
  return (kFixedmapStart >> 16) <= x &&
         x <= ((kFixedmapStart + (kFixedmapSize - 1)) >> 16);
}

forceinline pureconst bool OverlapsImageSpace(const void *p, size_t n) {
  const unsigned char *BegA, *EndA, *BegB, *EndB;
  if (n) {
    BegA = p;
    EndA = BegA + n;
    BegB = __executable_start;
    EndB = _end;
    return MAX(BegA, BegB) < MIN(EndA, EndB);
  } else {
    return 0;
  }
}

forceinline pureconst bool OverlapsShadowSpace(const void *p, size_t n) {
  intptr_t BegA, EndA, BegB, EndB;
  if (n) {
    BegA = (intptr_t)p;
    EndA = BegA + n;
    BegB = 0x7fff0000;
    EndB = 0x100080000000;
    return MAX(BegA, BegB) < MIN(EndA, EndB);
  } else {
    return 0;
  }
}

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_RUNTIME_MEMTRACK_H_ */
