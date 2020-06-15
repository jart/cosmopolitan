/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2020 Justine Alexandra Roberts Tunney                              │
│                                                                              │
│ This program is free software; you can redistribute it and/or modify         │
│ it under the terms of the GNU General Public License as published by         │
│ the Free Software Foundation; version 2 of the License.                      │
│                                                                              │
│ This program is distributed in the hope that it will be useful, but          │
│ WITHOUT ANY WARRANTY; without even the implied warranty of                   │
│ MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU             │
│ General Public License for more details.                                     │
│                                                                              │
│ You should have received a copy of the GNU General Public License            │
│ along with this program; if not, write to the Free Software                  │
│ Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA                │
│ 02110-1301 USA                                                               │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "libc/nexgen32e/rdtsc.h"
#include "libc/nt/dll.h"
#include "libc/nt/events.h"
#include "libc/nt/struct/point.h"
#include "libc/nt/struct/teb.h"
#include "libc/rand/rand.h"

/**
 * Returns somewhat randomish number on Windows.
 */
textwindows int64_t winrandish(void) {
  int64_t res;
  struct NtPoint point;
  res = ((int64_t)NtGetPid() << 17) ^ NtGetTid() ^ rdtsc();
  /*
   * This function is intended for older CPUs built before 2012, so
   * let's avoid having our CUI apps yoink USER32.DLL until we're
   * certain we need it, thus avoiding a hundred lines of noise in
   * NtTrace.exe output.
   */
  typeof(GetCursorPos) *GetCursorPos_ =
      GetProcAddress(GetModuleHandle("user32.dll"), "GetCursorPos");
  if (GetCursorPos_ && GetCursorPos_(&point)) res ^= point.x * point.y;
  return res;
}
