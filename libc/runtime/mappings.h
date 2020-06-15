#ifndef COSMOPOLITAN_LIBC_RUNTIME_MAPPINGS_H_
#define COSMOPOLITAN_LIBC_RUNTIME_MAPPINGS_H_
#include "libc/dce.h"
#include "libc/macros.h"
#include "libc/runtime/runtime.h"

#define MMAP_MAX 300 /* TODO: crunch */

#define kStackCeiling 0x0000700000000000L
#define kStackBottom  0x0000600000000000L

#define kFixedMappingsStart 0x0000100000000000L /* cosmo won't auto-assign */
#define kFixedMappingsSize  0x0000100000000000L /* 16TB */

#define kMappingsStart 0x0000200000000000L /* cosmo auto-assigns here */
#define kMappingsSize  0x0000100000000000L /* 16TB */

#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

#define ISOVERLAPPING(C1, C2)                \
  (((C1).x >= (C2).x && (C1).x <= (C2).y) || \
   ((C1).y >= (C2).x && (C1).y <= (C2).y))

#define ADDR_TO_COORD(ADDR) \
  (int)(((intptr_t)(ADDR) & ~(FRAMESIZE - 1)) / FRAMESIZE)

#define COORD_TO_ADDR(COORD) (void *)((intptr_t)(COORD)*FRAMESIZE)
#define COORD_TO_SIZE(COORD) (void *)((intptr_t)(COORD)*FRAMESIZE)

#define ADDRSIZE_TO_COORD(ADDR, SIZE) \
  ((struct MemoryCoord){              \
      .x = ADDR_TO_COORD(ADDR),       \
      .y = ADDR_TO_COORD(ADDR) +      \
           ((unsigned)(ROUNDUP((SIZE), FRAMESIZE) / FRAMESIZE) - 1)})

#define COORD_TO_ADDRSIZE(COORD)        \
  ((struct AddrSize){                   \
      .addr = COORD_TO_ADDR((COORD).x), \
      .size = ((size_t)((COORD).y - (COORD).x + 1) * FRAMESIZE)})

#define GRANULATE_ADDRSIZE(ADDR, SIZE)           \
  do {                                           \
    struct AddrSize AdSiz;                       \
    struct MemoryCoord MemCo;                    \
    MemCo = ADDRSIZE_TO_COORD(*(ADDR), *(SIZE)); \
    AdSiz = COORD_TO_ADDRSIZE(MemCo);            \
    *(ADDR) = AdSiz.addr;                        \
    *(SIZE) = AdSiz.size;                        \
  } while (0)

struct AddrSize {
  void *addr;
  size_t size;
};

/**
 * Ordered inclusive 64kb-granular ranges on NexGen32e w/o PML5.
 * c.𝑥 ≤ c.𝑦      so say c all.
 * cₙ.𝑥 ≤ cₙ₊₁.𝑥  so say c all.
 */
struct Mappings {
  size_t i;
  struct MemoryCoord {
    int32_t x, y;
  } p[MMAP_MAX];
  int64_t h[MMAP_MAX];
};

extern struct Mappings _mm;

bool isheap(void *);
size_t findmapping(int32_t);
size_t findmapping_(int32_t, const struct MemoryCoord *, size_t);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_RUNTIME_MAPPINGS_H_ */
