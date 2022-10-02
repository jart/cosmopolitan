/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ This is free and unencumbered software released into the public domain.      │
│                                                                              │
│ Anyone is free to copy, modify, publish, use, compile, sell, or              │
│ distribute this software, either in source code form or as a compiled        │
│ binary, for any purpose, commercial or non-commercial, and by any            │
│ means.                                                                       │
│                                                                              │
│ In jurisdictions that recognize copyright laws, the author or authors        │
│ of this software dedicate any and all copyright interest in the              │
│ software to the public domain. We make this dedication for the benefit       │
│ of the public at large and to the detriment of our heirs and                 │
│ successors. We intend this dedication to be an overt act of                  │
│ relinquishment in perpetuity of all present and future rights to this        │
│ software under copyright law.                                                │
│                                                                              │
│ THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,              │
│ EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF           │
│ MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.       │
│ IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR            │
│ OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,        │
│ ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR        │
│ OTHER DEALINGS IN THE SOFTWARE.                                              │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "libc/calls/struct/fd.internal.h"
#include "libc/calls/struct/iovec.h"
#include "libc/calls/struct/iovec.internal.h"
#include "libc/dce.h"
#include "libc/vga/vga.internal.h"
#include "libc/runtime/pc.internal.h"
#include "libc/str/str.h"

struct Tty _vga_tty;

ssize_t sys_writev_vga(struct Fd *fd, const struct iovec *iov, int iovlen) {
  size_t i, wrote = 0;
  ssize_t res = 0;
  for (i = 0; i < iovlen; ++i) {
    void *output = iov[i].iov_base;
    size_t len = iov[i].iov_len;
    res = _TtyWrite(&_vga_tty, output, len);
    if (res < 0)
      break;
    wrote += res;
    if (res != len)
      return wrote;
  }
  if (!wrote)
    return res;
  return wrote;
}

static void _vga_init_test(void *vid_buf, unsigned char vid_type,
                           size_t stride) {
  switch (vid_type) {
    case PC_VIDEO_TEXT:
      break;
    case PC_VIDEO_BGR565:
      {
        char *row_buf = (char *)vid_buf + stride * 100;
        uint16_t *row_pix = (uint16_t *)row_buf;
        unsigned i;
        row_pix[0] = 0x0000;
        row_pix[1] = 0x0000;
        row_pix[2] = 0x07fc;
        row_pix[3] = 0x07fc;
        row_pix[4] = 0x07fc;
        row_pix[5] = 0x0000;
        row_pix[6] = 0x0000;
        row_pix[7] = 0x0000;
        row_buf += stride;
        row_pix = (uint16_t *)row_buf;
        row_pix[0] = 0x0000;
        row_pix[1] = 0x07fc;
        row_pix[2] = 0x07fc;
        row_pix[3] = 0x07fc;
        row_pix[4] = 0x07fc;
        row_pix[5] = 0x07fc;
        row_pix[6] = 0x0000;
        row_pix[7] = 0x0000;
        row_buf += stride;
        row_pix = (uint16_t *)row_buf;
        row_pix[0] = 0x07fc;
        row_pix[1] = 0x07fc;
        row_pix[2] = 0x0000;
        row_pix[3] = 0x0000;
        row_pix[4] = 0x0000;
        row_pix[5] = 0x07fc;
        row_pix[6] = 0x07fc;
        row_pix[7] = 0x0000;
        row_buf += stride;
        row_pix = (uint16_t *)row_buf;
        row_pix[0] = 0x07fc;
        row_pix[1] = 0x07fc;
        row_pix[2] = 0x07fc;
        row_pix[3] = 0x07fc;
        row_pix[4] = 0x07fc;
        row_pix[5] = 0x07fc;
        row_pix[6] = 0x07fc;
        row_pix[7] = 0x0000;
        for (i = 0; i < 4; ++i) {
          row_buf += stride;
          row_pix = (uint16_t *)row_buf;
          row_pix[0] = 0xf81f;
          row_pix[1] = 0xf81f;
          row_pix[2] = 0x0000;
          row_pix[3] = 0x0000;
          row_pix[4] = 0x0000;
          row_pix[5] = 0xf81f;
          row_pix[6] = 0xf81f;
          row_pix[7] = 0x0000;
        }
      }
      break;
    case PC_VIDEO_BGR555:
      {
        char *row_buf = (char *)vid_buf + stride * 100;
        uint16_t *row_pix = (uint16_t *)row_buf;
        unsigned i;
        row_pix[0] = 0x8000;
        row_pix[1] = 0x8000;
        row_pix[2] = 0x83fc;
        row_pix[3] = 0x83fc;
        row_pix[4] = 0x83fc;
        row_pix[5] = 0x8000;
        row_pix[6] = 0x8000;
        row_pix[7] = 0x8000;
        row_buf += stride;
        row_pix = (uint16_t *)row_buf;
        row_pix[0] = 0x8000;
        row_pix[1] = 0x83fc;
        row_pix[2] = 0x83fc;
        row_pix[3] = 0x83fc;
        row_pix[4] = 0x83fc;
        row_pix[5] = 0x83fc;
        row_pix[6] = 0x8000;
        row_pix[7] = 0x8000;
        row_buf += stride;
        row_pix = (uint16_t *)row_buf;
        row_pix[0] = 0x83fc;
        row_pix[1] = 0x83fc;
        row_pix[2] = 0x8000;
        row_pix[3] = 0x8000;
        row_pix[4] = 0x8000;
        row_pix[5] = 0x83fc;
        row_pix[6] = 0x83fc;
        row_pix[7] = 0x8000;
        row_buf += stride;
        row_pix = (uint16_t *)row_buf;
        row_pix[0] = 0x83fc;
        row_pix[1] = 0x83fc;
        row_pix[2] = 0x83fc;
        row_pix[3] = 0x83fc;
        row_pix[4] = 0x83fc;
        row_pix[5] = 0x83fc;
        row_pix[6] = 0x83fc;
        row_pix[7] = 0x8000;
        for (i = 0; i < 4; ++i) {
          row_buf += stride;
          row_pix = (uint16_t *)row_buf;
          row_pix[0] = 0xfc1f;
          row_pix[1] = 0xfc1f;
          row_pix[2] = 0x8000;
          row_pix[3] = 0x8000;
          row_pix[4] = 0x8000;
          row_pix[5] = 0xfc1f;
          row_pix[6] = 0xfc1f;
          row_pix[7] = 0x8000;
        }
      }
      break;
    default:
      {
        char *row_buf = (char *)vid_buf + stride * 100;
        uint32_t *row_pix = (uint32_t *)row_buf;
        unsigned i;
        row_pix[0] = 0xff000000;
        row_pix[1] = 0xff000000;
        row_pix[2] = 0xff00ffe0;
        row_pix[3] = 0xff00ffe0;
        row_pix[4] = 0xff00ffe0;
        row_pix[5] = 0xff000000;
        row_pix[6] = 0xff000000;
        row_pix[7] = 0xff000000;
        row_buf += stride;
        row_pix = (uint32_t *)row_buf;
        row_pix[0] = 0xff000000;
        row_pix[1] = 0xff00ffe0;
        row_pix[2] = 0xff00ffe0;
        row_pix[3] = 0xff00ffe0;
        row_pix[4] = 0xff00ffe0;
        row_pix[5] = 0xff00ffe0;
        row_pix[6] = 0xff000000;
        row_pix[7] = 0xff000000;
        row_buf += stride;
        row_pix = (uint32_t *)row_buf;
        row_pix[0] = 0xff00ffe0;
        row_pix[1] = 0xff00ffe0;
        row_pix[2] = 0xff000000;
        row_pix[3] = 0xff000000;
        row_pix[4] = 0xff000000;
        row_pix[5] = 0xff00ffe0;
        row_pix[6] = 0xff00ffe0;
        row_pix[7] = 0xff000000;
        row_buf += stride;
        row_pix = (uint32_t *)row_buf;
        row_pix[0] = 0xff00ffe0;
        row_pix[1] = 0xff00ffe0;
        row_pix[2] = 0xff00ffe0;
        row_pix[3] = 0xff00ffe0;
        row_pix[4] = 0xff00ffe0;
        row_pix[5] = 0xff00ffe0;
        row_pix[6] = 0xff00ffe0;
        row_pix[7] = 0xff000000;
        for (i = 0; i < 4; ++i) {
          row_buf += stride;
          row_pix = (uint32_t *)row_buf;
          row_pix[0] = 0xffff00ff;
          row_pix[1] = 0xffff00ff;
          row_pix[2] = 0xff000000;
          row_pix[3] = 0xff000000;
          row_pix[4] = 0xff000000;
          row_pix[5] = 0xffff00ff;
          row_pix[6] = 0xffff00ff;
          row_pix[7] = 0xff000000;
        }
      }
      break;
  }
}

__attribute__((__constructor__)) static textstartup void _vga_init(void) {
  if (IsMetal()) {
    struct mman *mm = (struct mman *)(BANE + 0x0500);
    unsigned char vid_type = mm->pc_video_type;
    unsigned short height = mm->pc_video_height, width = mm->pc_video_width,
                   stride = mm->pc_video_stride;
    uint64_t vid_buf_phy = mm->pc_video_framebuffer;
    void *vid_buf = (void *)(BANE + vid_buf_phy);
    size_t vid_buf_sz = mm->pc_video_framebuffer_size;
    /*
     * Get the initial cursor position from the BIOS data area.  Also get
     * the height (in scan lines) of each character; this is used to set the
     * cursor shape.
     */
    typedef struct {
      unsigned char col, row;
    } bios_curs_pos_t;
    bios_curs_pos_t pos = *(bios_curs_pos_t *)(BANE + 0x0450ull);
    uint8_t chr_ht = *(uint8_t *)(BANE + 0x0485ull),
            chr_ht_hi = *(uint8_t *)(BANE + 0x0486ull);
    if (chr_ht_hi != 0 || chr_ht > 32)
      chr_ht = 32;
    /* Make sure the video buffer is mapped into virtual memory. */
    __invert_memory_area(mm, __get_pml4t(), vid_buf_phy, vid_buf_sz, PAGE_RW);
#if 1
    /* Test video frame buffer output. */
    _vga_init_test(vid_buf, vid_type, stride);
#endif
    /*
     * Initialize our tty structure from the current screen geometry,
     * screen contents, cursor position, & character height.
     */
    _StartTty(&_vga_tty, height, width, pos.row, pos.col, chr_ht,
              vid_buf, NULL);
  }
}
