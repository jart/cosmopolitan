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
#include "libc/str/str.h"
#include "libc/vga/vga.internal.h"

/**
 * @internal
 * @fileoverview Instantiation of routines for normal console output in
 * graphical video modes.
 *
 * @see libc/vga/tty-graph.inc
 */

#undef KLOGTTY
#define MAYUNROLLLOOPS unrollloops

#define COLOR    TtyCanvasColor
#define BPP      32
#define MAPCOLOR TtyGraphMapColor
#define DIRTY    TtyGraphDirty
#undef UPDATE
#define RESETDIRTY     TtyGraphResetDirty
#define DRAWBITMAP     TtyGraphDrawBitmap
#define FILLRECT       TtyGraphFillRect
#define MOVERECT       TtyGraphMoveRect
#define DRAWCHAR       _TtyGraphDrawChar
#define ERASELINECELLS _TtyGraphEraseLineCells
#define MOVELINECELLS  _TtyGraphMoveLineCells
#include "libc/vga/tty-graph.inc"
