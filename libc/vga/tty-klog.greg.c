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
#include "libc/intrin/newbie.h"
#include "libc/macros.internal.h"
#include "libc/runtime/mman.internal.h"
#include "libc/str/str.h"
#include "libc/vga/vga.internal.h"

#ifdef __x86_64__

/*
 * @fileoverview Instantiation of routines for emergency console output in
 * graphical video modes.
 *
 * @see libc/vga/tty-graph.inc
 */

#ifndef __llvm__
#pragma GCC optimize("s")
#endif

#define KLOGTTY
#define MAYUNROLLLOOPS /* do not unroll loops; keep the code small! */

/* Instantiate output routines for 16-bit pixel formats. */
#define COLOR          uint16_t
#define BPP            16
#define MAPCOLOR       TtyKlog16MapColor
#define DIRTY          TtyKlog16Dirty
#define UPDATE         _TtyKlog16Update
#define RESETDIRTY     TtyKlog16ResetDirty
#define DRAWBITMAP     TtyKlog16DrawBitmap
#define FILLRECT       TtyKlog16FillRect
#define MOVERECT       TtyKlog16MoveRect
#define DRAWCHAR       _TtyKlog16DrawChar
#define ERASELINECELLS _TtyKlog16EraseLineCells
#define MOVELINECELLS  _TtyKlog16MoveLineCells
#include "libc/vga/tty-graph.inc"

#undef COLOR
#undef BPP
#undef MAPCOLOR
#undef BG
#undef DIRTY
#undef UPDATE
#undef RESETDIRTY
#undef DRAWBITMAP
#undef FILLRECT
#undef MOVERECT
#undef DRAWCHAR
#undef ERASELINECELLS
#undef MOVELINECELLS

/* Instantiate output routines for 32-bit pixel formats. */
#define COLOR          uint32_t
#define BPP            32
#define MAPCOLOR       TtyKlog32MapColor
#define DIRTY          TtyKlog32Dirty
#define UPDATE         _TtyKlog32Update
#define RESETDIRTY     TtyKlog32ResetDirty
#define DRAWBITMAP     TtyKlog32DrawBitmap
#define FILLRECT       TtyKlog32FillRect
#define MOVERECT       TtyKlog32MoveRect
#define DRAWCHAR       _TtyKlog32DrawChar
#define ERASELINECELLS _TtyKlog32EraseLineCells
#define MOVELINECELLS  _TtyKlog32MoveLineCells
#include "libc/vga/tty-graph.inc"

static unsigned short klog_y = 0, klog_x = 0;

void _klog_vga(const char *b, size_t n) {
  struct Tty tty;
  _vga_reinit(&tty, klog_y, klog_x, kTtyKlog);
  _TtyWrite(&tty, b, n);
  klog_y = _TtyGetY(&tty);
  klog_x = _TtyGetX(&tty);
}

#endif /* __x86_64__ */
