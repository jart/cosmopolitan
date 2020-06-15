#ifndef COSMOPOLITAN_LIBC_NEXGEN32E_NONTEMPORAL_H_
#define COSMOPOLITAN_LIBC_NEXGEN32E_NONTEMPORAL_H_
#include "libc/bits/emmintrin.h"
#include "libc/nexgen32e/x86feature.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

#define sfence() asm volatile("sfence" ::: "memory")
#define lfence() asm volatile("lfence" ::: "memory")

/**
 * Stores memory asynchronously, e.g.
 *
 *  for (i = 0; i < n; ++i)
 *    nontemporal_store(m[i], x);
 *  }
 *  sfence();
 *
 * @param MEM is an aligned xmm vector pointer
 * @param REG is an xmm vector
 * @return REG
 */
#define nontemporal_store(MEM, REG) \
  _Generic((REG), __m128i           \
           : __movntdq, __m128      \
           : __movntps, __m128d     \
           : __movntpd)(MEM, REG)

/**
 * Loads memory asynchronously, e.g.
 *
 *   x1 = nontemporal_load(m16[0]);
 *   x2 = nontemporal_load(m16[1]);
 *   x3 = nontemporal_load(m16[2]);
 *   x4 = nontemporal_load(m16[3]);
 *   lfence();
 *
 * @param REG is an xmm vector
 * @param MEM is an aligned xmm vector pointer
 * @return REG
 */
#define nontemporal_load(REG, MEM) __movntdqa(MEM)

#define __DECLARE_MOVNT(OS, TS)                                         \
  forceinline __m128##TS __movnt##OS(__m128##TS *mem, __m128##TS reg) { \
    asm("movnt" #OS "\t%1,%0" : "=m"(*mem) : "x"(reg));                 \
    return reg;                                                         \
  }

__DECLARE_MOVNT(ps, )
__DECLARE_MOVNT(dq, i)
__DECLARE_MOVNT(pd, d)

forceinline __m128i __movntdqa(const __m128i *mem) {
  __m128i reg;
  if (X86_HAVE(SSE4_1)) {
    asm("movntdqa\t%1,%0" : "=x"(reg) : "m"(*mem));
  } else {
    asm("movdqa\t%1,%0" : "=x"(reg) : "m"(*mem));
  }
  return reg;
}

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_NEXGEN32E_NONTEMPORAL_H_ */
