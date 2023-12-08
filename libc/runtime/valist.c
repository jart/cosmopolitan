/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2020 Justine Alexandra Roberts Tunney                              │
│                                                                              │
│ Permission to use, copy, modify, and/or distribute this software for         │
│ any purpose with or without fee is hereby granted, provided that the         │
│ above copyright notice and this permission notice appear in all copies.      │
│                                                                              │
│ THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL                │
│ WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED                │
│ WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE             │
│ AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL         │
│ DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR        │
│ PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER               │
│ TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR             │
│ PERFORMANCE OF THIS SOFTWARE.                                                │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "libc/macros.internal.h"

/* <sync libc/integral/lp64arg.inc> */
struct __va_list {
  uint32_t gp_offset;
  uint32_t fp_offset;
  void *overflow_arg_area;
  void *reg_save_area;
};
/* </sync libc/integral/lp64arg.inc> */

static void *__va_arg_mem(struct __va_list *ap, size_t sz, size_t align) {
  void *r = (void *)ROUNDUP((intptr_t)ap->overflow_arg_area, align);
  ap->overflow_arg_area = (void *)ROUNDUP((intptr_t)r + sz, 8);
  return r;
}

void *__va_arg(struct __va_list *ap, size_t sz, unsigned align, unsigned k) {
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
