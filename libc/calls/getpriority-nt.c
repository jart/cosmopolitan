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
#include "libc/bits/bits.h"
#include "libc/calls/calls.h"
#include "libc/calls/internal.h"
#include "libc/calls/kntprioritycombos.internal.h"
#include "libc/conv/conv.h"
#include "libc/nexgen32e/ffs.h"
#include "libc/nt/enum/processcreationflags.h"
#include "libc/nt/enum/threadpriority.h"
#include "libc/nt/process.h"
#include "libc/nt/runtime.h"
#include "libc/nt/thread.h"
#include "libc/runtime/runtime.h"
#include "libc/sysv/consts/prio.h"
#include "libc/sysv/errfuns.h"

textwindows int getpriority$nt(int ignored) {
  size_t i;
  uint32_t tier, lg2tier, wut;
  if ((tier = GetPriorityClass(GetCurrentProcess())) != 0 &&
      (wut = GetThreadPriority(GetCurrentThread())) != -1u) {
    lg2tier = ffs(tier);
    for (i = 0; i < kNtPriorityCombosLen; ++i) {
      if (kNtPriorityCombos[i].lg2tier == lg2tier &&
          kNtPriorityCombos[i].wut == wut) {
        return kNtPriorityCombos[i].nice;
      }
    }
    abort();
  } else {
    return __winerr();
  }
}
