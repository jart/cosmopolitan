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
#include "tool/decode/lib/flagger.h"
#include "libc/mem/arraylist2.internal.h"
#include "libc/mem/mem.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"

/**
 * Formats numeric flags integer as symbolic code.
 *
 * @param names maps individual flags to string names in no order
 * @param id is the flags
 * @return NUL-terminated string that needs free()
 */
char *RecreateFlags(const struct IdName *names, unsigned long id) {
  bool first;
  size_t bufi, bufn;
  char *bufp, extrabuf[20];
  bufi = 0;
  bufn = 0;
  bufp = NULL;
  first = true;
  for (; names->name; names++) {
    if ((id == 0 && names->id == 0) ||
        (id != 0 && names->id != 0 && (id & names->id) == names->id)) {
      id &= ~names->id;
      if (!first) {
        APPEND(&bufp, &bufi, &bufn, "|");
      } else {
        first = false;
      }
      CONCAT(&bufp, &bufi, &bufn, (char *)names->name, strlen(names->name));
    }
  }
  if (id) {
    if (bufi) APPEND(&bufp, &bufi, &bufn, "|");
    CONCAT(&bufp, &bufi, &bufn, extrabuf,
           snprintf(extrabuf, sizeof(extrabuf), "%#x", id));
  } else if (!bufi) {
    APPEND(&bufp, &bufi, &bufn, "0");
  }
  return bufp;
}
