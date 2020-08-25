#ifndef COSMOPOLITAN_DSP_MPEG_YCBCRIO_H_
#define COSMOPOLITAN_DSP_MPEG_YCBCRIO_H_
#include "dsp/mpeg/mpeg.h"
#include "libc/bits/bswap.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

#define YCBCRIO_MAGIC bswap_32(0xBCCBCCBCu)

/**
 * Mappable persistable MPEG-2 video frame in Y-Cr-Cb colorspace.
 */
struct Ycbcrio {
  uint32_t magic;
  int32_t fd;
  uint64_t size;
  plm_frame_t frame;
};

struct Ycbcrio *YcbcrioOpen(const char *, const struct plm_frame_t *)
    paramsnonnull((1)) vallocesque returnsnonnull;

void YcbcrioClose(struct Ycbcrio **) paramsnonnull();

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_DSP_MPEG_YCBCRIO_H_ */
