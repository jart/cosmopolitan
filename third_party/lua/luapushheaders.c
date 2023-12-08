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
#include "libc/mem/mem.h"
#include "net/http/escape.h"
#include "net/http/http.h"
#include "third_party/lua/cosmo.h"
#include "third_party/lua/lua.h"

int LuaPushHeaders(lua_State *L, struct HttpMessage *m, const char *b) {
  char *s;
  size_t i, h;
  struct HttpHeader *x;
  lua_newtable(L);
  for (h = 0; h < kHttpHeadersMax; ++h) {
    if (m->headers[h].a) {
      LuaPushHeader(L, m, b, h);
      lua_setfield(L, -2, GetHttpHeaderName(h));
    }
  }
  for (i = 0; i < m->xheaders.n; ++i) {
    x = m->xheaders.p + i;
    if ((h = GetHttpHeader(b + x->v.a, x->v.b - x->v.a)) == -1) {
      LuaPushLatin1(L, b + x->v.a, x->v.b - x->v.a);
      lua_setfield(L, -2, (s = DecodeLatin1(b + x->k.a, x->k.b - x->k.a, 0)));
      free(s);
    }
  }
  return 1;
}
