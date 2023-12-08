/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
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
#include "libc/log/rop.internal.h"
#include "libc/stdio/append.h"
#include "third_party/lua/cosmo.h"
#include "third_party/lua/lua.h"

bool LuaHasMultipleItems(lua_State *L) {
  int i;
  lua_pushnil(L);
  for (i = 0; lua_next(L, -2); ++i) {
    if (i > 0) {
      lua_pop(L, 2);
      return true;
    }
    lua_pop(L, 1);
  }
  return false;
}

int SerializeObjectIndent(char **buf, struct Serializer *z, int depth) {
  int i;
  RETURN_ON_ERROR(appendw(buf, '\n'));
  for (i = 0; i < depth; ++i) {
    RETURN_ON_ERROR(appends(buf, z->conf.indent));
  }
  return 0;
OnError:
  return -1;
}

int SerializeObjectStart(char **buf, struct Serializer *z, int depth,
                         bool multi) {
  RETURN_ON_ERROR(appendw(buf, '{'));
  if (multi) {
    RETURN_ON_ERROR(SerializeObjectIndent(buf, z, depth + 1));
  }
  return 0;
OnError:
  return -1;
}

int SerializeObjectEnd(char **buf, struct Serializer *z, int depth,
                       bool multi) {
  if (multi) {
    RETURN_ON_ERROR(SerializeObjectIndent(buf, z, depth));
  }
  RETURN_ON_ERROR(appendw(buf, '}'));
  return 0;
OnError:
  return -1;
}
