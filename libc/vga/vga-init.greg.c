/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
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
#include "libc/intrin/kprintf.h"
#include "libc/mem/alloca.h"
#include "libc/runtime/pc.internal.h"
#include "libc/runtime/runtime.h"
#include "libc/runtime/stack.h"
#include "libc/str/str.h"
#include "libc/vga/vga.internal.h"

#ifdef __x86_64__

struct Tty _vga_tty;

void _vga_reinit(struct Tty *tty, unsigned short starty, unsigned short startx,
                 unsigned init_flags) {
  struct mman *mm = __get_mm();
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
  __invert_and_perm_ref_memory_area(mm, __get_pml4t(), vid_buf_phy, vid_buf_sz,
                                    PAGE_RW | PAGE_XD);
  /*
   * Initialize our tty structure from the current screen geometry, screen
   * contents, cursor position, & character dimensions.
   */
  _StartTty(tty, vid_type, height, width, stride, starty, startx, chr_ht,
            chr_wid, vid_buf, init_flags);
}

textstartup void _vga_init(void) {
  if (IsMetal()) {
    struct mman *mm = __get_mm();
    unsigned short starty = mm->pc_video_curs_info.y,
                   startx = mm->pc_video_curs_info.x;
    _vga_reinit(&_vga_tty, starty, startx, 0);
  }
}

#if !IsTiny()
/**
 * Non-emergency console vprintf(), useful for dumping debugging or
 * informational messages at program startup.
 *
 * @see uprintf()
 */
void uvprintf(const char *fmt, va_list v) {
  if (!IsMetal()) {
    kvprintf(fmt, v);
  } else {
    long size = __get_safe_size(8000, 3000);
    char *buf = alloca(size);
    CheckLargeStackAllocation(buf, size);
    size_t count = kvsnprintf(buf, size, fmt, v);
    if (count >= size) count = size - 1;
    _TtyWrite(&_vga_tty, buf, count);
    _klog_serial(buf, count);
  }
}

/**
 * Non-emergency console printf(), useful for dumping debugging or
 * informational messages at program startup.
 *
 * uprintf() is similar to kprintf(), but on bare metal with VGA support, it
 * uses the normal, fast graphical console, rather than initializing an
 * emergency console.  This makes uprintf() faster — on bare metal — at the
 * expense of being less crash-proof.
 *
 * (The uprintf() function name comes from the FreeBSD kernel.)
 *
 * @see kprintf()
 * @see https://man.freebsd.org/cgi/man.cgi?query=uprintf&sektion=9&n=1
 */
void uprintf(const char *fmt, ...) {
  va_list v;
  va_start(v, fmt);
  uvprintf(fmt, v);
  va_end(v);
}
#endif /* !IsTiny() */

#endif /* __x86_64__ */
