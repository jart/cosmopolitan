#ifndef COSMOPOLITAN_LIBC_RUNTIME_MEMTRACK_H_
#define COSMOPOLITAN_LIBC_RUNTIME_MEMTRACK_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

#define kAutomapStart  0x0000100000000000
#define kAutomapSize   0x0000100000000000
#define kFixedmapStart 0x0000200000000000

struct MemoryIntervals {
  int i;
  struct MemoryInterval {
    int x;
    int y;
  } p[64];
  long h[64];
};

extern struct MemoryIntervals _mmi;

unsigned FindMemoryInterval(const struct MemoryIntervals *, int) nosideeffect;
bool AreMemoryIntervalsOk(const struct MemoryIntervals *) nosideeffect;
void PrintMemoryIntervals(int, const struct MemoryIntervals *);
int TrackMemoryInterval(struct MemoryIntervals *, int, int, long);
int ReleaseMemoryIntervals(struct MemoryIntervals *, int, int,
                           void (*)(struct MemoryIntervals *, int, int));
void ReleaseMemoryNt(struct MemoryIntervals *, int, int);
int UntrackMemoryIntervals(void *, size_t);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_RUNTIME_MEMTRACK_H_ */
