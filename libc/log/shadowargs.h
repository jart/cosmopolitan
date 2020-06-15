/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=8 sts=2 sw=2 fenc=utf-8                                :vi│
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
#ifndef COSMOPOLITAN_LIBC_SHADOWARGS_H_
#define COSMOPOLITAN_LIBC_SHADOWARGS_H_

#define SHADOWARGS_COUNT 32
#define SHADOWARGS_SLOTS 8
#define SHADOWARGS_MAXNAME 5

#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

struct ShadowArgs {
  union {
    unsigned long slots[SHADOWARGS_SLOTS];
    struct {
      void *addr;
      void *frame;
      unsigned long arg1;
      unsigned long arg2;
      unsigned long arg3;
      unsigned long arg4;
      unsigned long arg5;
      unsigned long arg6;
    };
  };
};

hidden extern unsigned __shadowargs_index;
hidden extern struct ShadowArgs __shadowargs[SHADOWARGS_COUNT];
hidden extern const char __shadowargs_slotnames[SHADOWARGS_SLOTS]
                                               [SHADOWARGS_MAXNAME];

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_SHADOWARGS_H_ */
