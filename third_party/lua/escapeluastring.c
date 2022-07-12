/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2022 Justine Alexandra Roberts Tunney                              │
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
#include "libc/bits/bits.h"
#include "libc/log/rop.h"
#include "libc/stdio/append.internal.h"
#include "libc/str/str.h"
#include "third_party/lua/cosmo.h"
#include "third_party/lua/lua.h"

int EscapeLuaString(char *s, size_t len, char **buf) {
  int rc;
  size_t i;
  RETURN_ON_ERROR(appendw(buf, '"'));
  for (i = 0; i < len; i++) {
    if (s[i] == '\n') {
      RETURN_ON_ERROR(appendw(buf, '\\' | 'n' << 8));
    } else if (s[i] == '\\' || s[i] == '\'' || s[i] == '\"') {
      RETURN_ON_ERROR(appendw(buf, '\\' | s[i] << 8));
    } else if (' ' <= s[i] && s[i] <= 0x7e) {
      RETURN_ON_ERROR(appendw(buf, s[i]));
    } else {
      RETURN_ON_ERROR(
          appendw(buf, '\\' | 'x' << 010 |
                           "0123456789abcdef"[(s[i] & 0xF0) >> 4] << 020 |
                           "0123456789abcdef"[(s[i] & 0x0F) >> 0] << 030));
    }
  }
  RETURN_ON_ERROR(appendw(buf, '"'));
  return 0;
OnError:
  return -1;
}
