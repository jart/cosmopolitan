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
#include "libc/log/log.h"
#include "libc/log/shadowargs.h"
#include "libc/runtime/internal.h"
#include "libc/runtime/symbols.h"

alignas(1) const
    char __shadowargs_slotnames[SHADOWARGS_SLOTS][SHADOWARGS_MAXNAME] = {
        "func", "%rsp", "%rdi", "%rsi", "%rdx", "%rcx", "%r8", "%r9",
};
alignas(64) struct ShadowArgs __shadowargs[SHADOWARGS_COUNT];
unsigned __shadowargs_index;

nocallersavedregisters void __fentry__(uint64_t rdi, uint64_t rsi, uint64_t rdx,
                                       uint64_t rcx, uint64_t r8, uint64_t r9) {
  void *addr = __builtin_return_address(0);
  intptr_t frame = (intptr_t)__builtin_frame_address(0);
  unsigned i = __shadowargs_index;
  intptr_t lastframe = (intptr_t)__shadowargs[i].frame;
  if (frame < lastframe) {
    i--;
    i &= SHADOWARGS_COUNT - 1;
  } else if (frame > lastframe) {
    unsigned j = i;
    do {
      j++;
      j &= SHADOWARGS_COUNT - 1;
    } while (frame > (intptr_t)__shadowargs[j].frame && j != i);
    i = j;
  }
  __shadowargs[i].addr = addr;
  __shadowargs[i].frame = (void *)frame;
  __shadowargs[i].arg1 = rdi;
  __shadowargs[i].arg2 = rsi;
  __shadowargs[i].arg3 = rdx;
  __shadowargs[i].arg4 = rcx;
  __shadowargs[i].arg5 = r8;
  __shadowargs[i].arg6 = r9;
  __shadowargs_index = i;
}
