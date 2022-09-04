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
#include "libc/vga/vga.internal.h"
#include "libc/runtime/pc.internal.h"
#include "libc/str/str.h"

#define CRTPORT 0x3d4
#define WIDTH 80

typedef struct {
  char ch;
  uint8_t attr;
} char_cell_t;

typedef char_cell_t char_row_t[WIDTH];

static unsigned short height = 25, curr_row, curr_col;
static uint8_t curr_attr = 0x07;

static void scroll(void) {
  unsigned j;
  uint8_t attr = curr_attr;
  char_row_t * const vid_buf = (char_row_t *)(BANE + 0xb8000ull);
  memmove(vid_buf, vid_buf + 1, (height - 1) * sizeof(char_row_t));
  for (j = 0; j < WIDTH; ++j)
    vid_buf[height - 1][j] = (char_cell_t){ ' ', attr };
}

static void may_scroll(void) {
  if (curr_col >= WIDTH) {
    curr_col = 0;
    scroll();
  }
}

static void updatexy_vga(void) {
  unsigned short pos = curr_row * WIDTH + curr_col;
  outb(CRTPORT, 0x0e);
  outb(CRTPORT + 1, (unsigned char)(pos >> 8));
  outb(CRTPORT, 0x0f);
  outb(CRTPORT + 1, (unsigned char)pos);
}

static void writec_vga(char c) {
  /* TODO: handle UTF-8 multi-bytes */
  /* TODO: handle VT102 escape sequences */
  /* TODO: maybe make BEL (\a) character code emit an alarm of some sort */
  char_row_t * const vid_buf = (char_row_t *)(BANE + 0xb8000ull);
  uint8_t attr = curr_attr;
  unsigned short col;
  switch (c) {
    case '\b':
      if (curr_col) {
        col = curr_col - 1;
        vid_buf[curr_row][col] = (char_cell_t){ ' ', attr };
        curr_col = col;
      }
      break;
    case '\t':
      col = curr_col;
      do {
        vid_buf[curr_row][col] = (char_cell_t){ ' ', attr };
        ++col;
      } while (col % 8 != 0);
      curr_col = col;
      may_scroll();
      break;
    case '\r':
      curr_col = 0;
      break;
    case '\n':
      curr_col = 0;
      if (curr_row < height - 1)
        ++curr_row;
      else
        scroll();
      break;
    default:
      col = curr_col;
      vid_buf[curr_row][col] = (char_cell_t){ c, attr };
      curr_col = col + 1;
      may_scroll();
  }
}

ssize_t sys_writev_vga(struct Fd *fd, const struct iovec *iov, int iovlen) {
  char_row_t * const vid_buf = (char_row_t *)(BANE + 0xb8000ull);
  size_t i, j, wrote = 0;
  for (i = 0; i < iovlen; ++i) {
    const char *input = (const char *)iov[i].iov_base;
    for (j = 0; j < iov[i].iov_len; ++j) {
      writec_vga(input[j]);
      ++wrote;
    }
  }
  updatexy_vga();
  return wrote;
}

__attribute__((__constructor__)) static textstartup void _vga_init(void) {
  /* Get the initial cursor position from the BIOS data area. */
  typedef struct {
    unsigned char col, row;
  } bios_curs_pos_t;
  bios_curs_pos_t pos = *(bios_curs_pos_t *)(BANE + 0x0450ull);
  curr_row = pos.row;
  curr_col = pos.col;
}
