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
#include "libc/dce.h"
#include "libc/vga/vga.internal.h"
#include "libc/runtime/pc.internal.h"
#include "libc/str/str.h"

struct Tty _vga_tty;

void _vga_reinit(struct Tty *tty, unsigned short starty, unsigned short startx,
                 unsigned init_flags) {
  struct mman *mm = (struct mman *)(BANE + 0x0500);
  unsigned char vid_type = mm->pc_video_type;
  unsigned short height = mm->pc_video_height, width = mm->pc_video_width,
                 stride = mm->pc_video_stride;
  uint64_t vid_buf_phy = mm->pc_video_framebuffer;
  void *vid_buf = (void *)(BANE + vid_buf_phy);
  size_t vid_buf_sz = mm->pc_video_framebuffer_size;
  uint8_t chr_ht, chr_wid;
  if (vid_type == PC_VIDEO_TEXT) {
    unsigned short chr_ht_val = mm->pc_video_char_height;
    if (chr_ht_val > 32 || chr_ht_val < 2)
      chr_ht = VGA_ASSUME_CHAR_HEIGHT_PX;
    else
      chr_ht = chr_ht_val;
  } else
    chr_ht = VGA_ASSUME_CHAR_HEIGHT_PX;
  chr_wid = VGA_ASSUME_CHAR_WIDTH_PX;
  /* Make sure the video buffer is mapped into virtual memory. */
  __invert_memory_area(mm, __get_pml4t(), vid_buf_phy, vid_buf_sz,
                       PAGE_RW | PAGE_XD);
  __ref_pages(mm, __get_pml4t(), vid_buf_phy, vid_buf_sz);
  /*
   * Initialize our tty structure from the current screen geometry, screen
   * contents, cursor position, & character dimensions.
   */
  _StartTty(tty, vid_type, height, width, stride, starty, startx,
            chr_ht, chr_wid, vid_buf, init_flags);
}

textstartup void _vga_init(void) {
  if (IsMetal()) {
    struct mman *mm = (struct mman *)(BANE + 0x0500);
    unsigned short starty = mm->pc_video_curs_info.y,
                   startx = mm->pc_video_curs_info.x;
    _vga_reinit(&_vga_tty, starty, startx, 0);
  }
}
