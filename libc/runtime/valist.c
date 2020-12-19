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
#include "libc/macros.h"
#include "libc/runtime/valist.h"

static void *__va_arg_mem(struct __va *ap, size_t sz, size_t align) {
  void *r = (void *)ROUNDUP((intptr_t)ap->overflow_arg_area, align);
  ap->overflow_arg_area = (void *)ROUNDUP((intptr_t)r + sz, 8);
  return r;
}

void *__va_arg(struct __va *ap, size_t sz, unsigned align, unsigned k) {
  void *r;
  switch (k) {
    case 0:
      if (ap->gp_offset < 48) {
        r = (char *)ap->reg_save_area + ap->gp_offset;
        ap->gp_offset += 8;
        return r;
      } else {
        return __va_arg_mem(ap, sz, align);
      }
    case 1:
      if (ap->fp_offset < 112) {
        r = (char *)ap->reg_save_area + ap->fp_offset;
        ap->fp_offset += 8;
        return r;
      } else {
        return __va_arg_mem(ap, sz, align);
      }
    default:
      return __va_arg_mem(ap, sz, align);
  }
}
