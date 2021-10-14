/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2021 Justine Alexandra Roberts Tunney                              │
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
#include "libc/fmt/fmt.h"
#include "libc/log/libfatal.internal.h"
#include "libc/log/log.h"
#include "libc/nexgen32e/gc.internal.h"
#include "libc/stdio/stdio.h"
/* clang-format off */

/**
 * Prints list of deferred operations on shadow stack.
 */
void PrintGarbage(void) {
  size_t i;
  char name[19];
  const char *symbol;
  __printf("\n");
  __printf("                            SHADOW STACK @ 0x%p\n", __builtin_frame_address(0));
  __printf("garbage entry   parent frame     original ret        callback              arg        \n");
  __printf("-------------- -------------- ------------------ ------------------ ------------------\n");
  if (__garbage.i) {
    for (i = __garbage.i; i--;) {
      symbol = __get_symbol_by_addr(__garbage.p[i].ret);
      if (symbol) {
        snprintf(name, sizeof(name), "%s", symbol);
      } else {
        snprintf(name, sizeof(name), "0x%012lx", __garbage.p[i].ret);
      }
      __printf("0x%p 0x%p %18s %18s 0x%016lx\n",
               __garbage.p + i,
               __garbage.p[i].frame,
               name,
               __get_symbol_by_addr(__garbage.p[i].fn),
               __garbage.p[i].arg);
    }
  } else {
    __printf("%14s %14s %18s %18s %18s\n","empty","-","-","-","-");
  }
  __printf("\n");
}
