#ifndef COSMOPOLITAN_LIBC_LZ4_H_
#define COSMOPOLITAN_LIBC_LZ4_H_
#include "libc/bits/bits.h"
/*───────────────────────────────────────────────────────────────────────────│─╗
│ cosmopolitan § lz4                                                       ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│─╝
  LZ4 is a framing format for REP MOVSB designed by Yann Collet.

  @see https://github.com/lz4/lz4/blob/master/doc/lz4_Frame_format.md
  @see https://github.com/lz4/lz4/blob/master/doc/lz4_Block_format.md
  @see http://ticki.github.io/blog/how-lz4-works/ */

#define LZ4_EOF                0
#define LZ4_VERSION            1
#define LZ4_MAGICNUMBER        0x184D2204
#define LZ4_SKIPPABLE0         0x184D2A50
#define LZ4_SKIPPABLEMASK      0xFFFFFFF0
#define LZ4_MAXHEADERSIZE      (MAGICNUMBER_SIZE + 2 + 8 + 4 + 1)
#define LZ4_BLOCKMAXSIZE_64KB  4
#define LZ4_BLOCKMAXSIZE_256KB 5
#define LZ4_BLOCKMAXSIZE_1MB   6
#define LZ4_BLOCKMAXSIZE_4MB   7

#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_
/*───────────────────────────────────────────────────────────────────────────│─╗
│ cosmopolitan § lz4 » frames                                              ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/

#define LZ4_MAGIC(FRAME)                      read32le(FRAME)
#define LZ4_FRAME_VERSION(FRAME)              ((_LZ4_FRAME_FLG(FRAME) >> 6) & 0b11)
#define LZ4_FRAME_BLOCKINDEPENDENCE(FRAME)    ((_LZ4_FRAME_FLG(FRAME) >> 5) & 1)
#define LZ4_FRAME_BLOCKCHECKSUMFLAG(FRAME)    ((_LZ4_FRAME_FLG(FRAME) >> 4) & 1)
#define LZ4_FRAME_BLOCKCONTENTSIZEFLAG(FRAME) ((_LZ4_FRAME_FLG(FRAME) >> 3) & 1)
#define LZ4_FRAME_BLOCKCONTENTCHECKSUMFLAG(FRAME) \
  ((_LZ4_FRAME_FLG(FRAME) >> 2) & 1)
#define LZ4_FRAME_DICTIONARYIDFLAG(FRAME) ((_LZ4_FRAME_FLG(FRAME) >> 0) & 1)
#define LZ4_FRAME_BLOCKMAXSIZE(FRAME)     ((_LZ4_FRAME_BD(FRAME) >> 4) & 0b111)
#define LZ4_FRAME_RESERVED1(FRAME)        ((_LZ4_FRAME_FLG(FRAME) >> 1) & 1)
#define LZ4_FRAME_RESERVED2(FRAME)        ((_LZ4_FRAME_BD(FRAME) >> 7) & 1)
#define LZ4_FRAME_RESERVED3(FRAME)        ((_LZ4_FRAME_BD(FRAME) >> 0) & 0b1111)
#define LZ4_FRAME_BLOCKCONTENTSIZE(FRAME) \
  (LZ4_FRAME_BLOCKCONTENTSIZEFLAG(FRAME) ? read64le((FRAME) + 4 + 1 + 1) : 0)
#define LZ4_FRAME_DICTIONARYID(FRAME)                          \
  (LZ4_FRAME_DICTIONARYIDFLAG(FRAME)                           \
       ? read32le(((FRAME) + 4 + 1 + 1 +                       \
                   8 * LZ4_FRAME_BLOCKCONTENTSIZEFLAG(FRAME))) \
       : 0)
#define LZ4_FRAME_HEADERCHECKSUM(FRAME)                                \
  (*((FRAME) + 4 + 1 + 1 + 8 * LZ4_FRAME_BLOCKCONTENTSIZEFLAG(FRAME) + \
     4 * LZ4_FRAME_DICTIONARYIDFLAG(FRAME)))
#define LZ4_FRAME_HEADERSIZE(FRAME)                        \
  (4 + 1 + 1 + 8 * LZ4_FRAME_BLOCKCONTENTSIZEFLAG(FRAME) + \
   4 * LZ4_FRAME_DICTIONARYIDFLAG(FRAME) + 1)
#define _LZ4_FRAME_FLG(FRAME) (*((FRAME) + 4))
#define _LZ4_FRAME_BD(FRAME)  (*((FRAME) + 5))

/*───────────────────────────────────────────────────────────────────────────│─╗
│ cosmopolitan § lz4 » blocks                                              ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/

#define LZ4_BLOCK_DATA(block)         (block + sizeof(uint32_t))
#define LZ4_BLOCK_DATASIZE(block)     (read32le(block) & 0x7fffffff)
#define LZ4_BLOCK_ISEOF(block)        (read32le(block) == LZ4_EOF)
#define LZ4_BLOCK_ISCOMPRESSED(block) ((read32le(block) & 0x80000000) == 0)
#define LZ4_BLOCK_SIZE(frame, block)              \
  (sizeof(uint32_t) + LZ4_BLOCK_DATASIZE(block) + \
   (LZ4_FRAME_BLOCKCHECKSUMFLAG(frame) ? sizeof(uint8_t) : 0))

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_LZ4_H_ */
