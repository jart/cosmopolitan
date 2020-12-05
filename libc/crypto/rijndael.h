#ifndef COSMOPOLITAN_LIBC_CRYPTO_RIJNDAEL_H_
#define COSMOPOLITAN_LIBC_CRYPTO_RIJNDAEL_H_
#ifndef __STRICT_ANSI__
#include "libc/str/str.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_
/*───────────────────────────────────────────────────────────────────────────│─╗
│ cosmopolitan § cryptography » advanced encryption standard               ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│─┘
    AES-256 Latency                                  x86 2010+    SSE2
    ───────────────                                  ─────────   ──────
    rijndael(14, block, &ctx)                           23 ns    218 ns
    unrijndael(14, block, &ctx)                         23 ns    690 ns
    rijndaelinit(&ctx, 14, k1, k2)                     136 ns    135 ns
    unrijndaelinit(&ctx, 14, k1, k2)                   186 ns    639 ns

    Untrustworthy System Viability                   x86 2010+    SSE2
    ──────────────────────────────                   ─────────   ──────
    rijndael(14, block, &ctx)                             A        C
    unrijndael(14, block, &ctx)                           A        C
    rijndaelinit(&ctx, 14, k1, k2)                        B        B
    unrijndaelinit(&ctx, 14, k1, k2)                      B        C

    Comparison                                   Cosmo    Rijndael  Tiny-AES
    ──────────────────────────────              ───────   ────────  ────────
    Generalized Math                               Yes        Yes       No
    Footprint                                  1,782 b    9,258 b     903 b
    Performance (New Hardware)                   ~20 ns     ~40 ns   ~400 ns
    Performance (Old Hardware)                  ~400 ns     ~40 ns   ~400 ns */

typedef uint32_t aes_block_t _Vector_size(16) forcealign(16);

struct Rijndael {
  union {
    aes_block_t xmm;
    uint32_t u32[4];
    uint8_t u8[16];
  } rk[15];
};

void rijndaelinit(struct Rijndael *, uint32_t, aes_block_t, aes_block_t);
aes_block_t rijndael(uint32_t, aes_block_t, const struct Rijndael *);
void unrijndaelinit(struct Rijndael *, uint32_t, aes_block_t, aes_block_t);
aes_block_t unrijndael(uint32_t, aes_block_t, const struct Rijndael *);

/*───────────────────────────────────────────────────────────────────────────│─╗
│ cosmopolitan § cryptography » implementation details                     ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/

extern const uint8_t kAesSbox[256] forcealign(64);
extern const uint8_t kAesSboxInverse[256] forcealign(64);

aes_block_t InvMixColumns(aes_block_t) hidden;

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* !ANSI */
#endif /* COSMOPOLITAN_LIBC_CRYPTO_RIJNDAEL_H_ */
