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
#include "libc/assert.h"
#include "libc/bits/bits.h"
#include "libc/fmt/itoa.h"
#include "libc/mem/mem.h"
#include "libc/runtime/gc.internal.h"
#include "libc/stdio/append.internal.h"
#include "net/http/escape.h"
#include "third_party/lua/cosmo.h"
#include "third_party/lua/lauxlib.h"
#include "third_party/lua/lua.h"
#include "third_party/lua/visitor.h"

static int LuaEncodeJsonDataImpl(lua_State *L, char **buf, int level,
                                 char *numformat, int idx,
                                 struct LuaVisited *visited) {
  char *s;
  bool isarray;
  size_t tbllen, i, z;
  char ibuf[21], fmt[] = "%.14g";
  if (level > 0) {
    switch (lua_type(L, idx)) {

      case LUA_TNIL:
        appendw(buf, READ32LE("null"));
        return 0;

      case LUA_TBOOLEAN:
        appendw(buf, lua_toboolean(L, idx) ? READ32LE("true")
                                           : READ64LE("false\0\0"));
        return 0;

      case LUA_TSTRING:
        s = lua_tolstring(L, idx, &z);
        s = EscapeJsStringLiteral(s, z, &z);
        appendw(buf, '"');
        appendd(buf, s, z);
        appendw(buf, '"');
        free(s);
        return 0;

      case LUA_TNUMBER:
        if (lua_isinteger(L, idx)) {
          appendd(buf, ibuf,
                  FormatInt64(ibuf, luaL_checkinteger(L, idx)) - ibuf);
        } else {
          // TODO(jart): replace this api
          while (*numformat == '%' || *numformat == '.' ||
                 isdigit(*numformat)) {
            ++numformat;
          }
          switch (*numformat) {
            case 'a':
            case 'g':
            case 'f':
              fmt[4] = *numformat;
              break;
            default:
              return luaL_error(L, "numformat string not allowed");
          }
          appendf(buf, fmt, lua_tonumber(L, idx));
        }
        return 0;

      case LUA_TTABLE:
        if (LuaPushVisit(visited, lua_topointer(L, idx))) {
          tbllen = lua_rawlen(L, idx);
          // encode tables with numeric indices and empty tables as arrays
          isarray =
              tbllen > 0 ||                          // integer keys present
              (lua_pushnil(L), !lua_next(L, -2)) ||  // no non-integer keys
              (lua_pop(L, 2), false);  // pop key/value pushed by lua_next
          appendw(buf, isarray ? '[' : '{');
          if (isarray) {
            for (i = 1; i <= tbllen; i++) {
              if (i > 1) appendw(buf, ',');
              lua_rawgeti(L, -1, i);  // table/-2, value/-1
              LuaEncodeJsonDataImpl(L, buf, level - 1, numformat, -1, visited);
              lua_pop(L, 1);
            }
          } else {
            i = 1;
            lua_pushnil(L);  // push the first key
            while (lua_next(L, -2)) {
              if (!lua_isstring(L, -2)) {
                luaL_error(L, "expected number or string as key value");
                unreachable;
              }
              if (i++ > 1) appendw(buf, ',');
              appendw(buf, '"');
              if (lua_type(L, -2) == LUA_TSTRING) {
                s = lua_tolstring(L, -2, &z);
                s = EscapeJsStringLiteral(s, z, &z);
                appendd(buf, s, z);
                free(s);
              } else {
                // we'd still prefer to use lua_tostring on a numeric index, but
                // can't use it in-place, as it breaks lua_next (changes numeric
                // key to a string)
                lua_pushvalue(L, -2);  // table/-4, key/-3, value/-2, key/-1
                s = lua_tolstring(L, idx, &z);
                appendd(buf, s, z);  // use the copy
                lua_remove(L, -1);  // remove copied key: tab/-3, key/-2, val/-1
              }
              appendw(buf, '"' | ':' << 010);
              LuaEncodeJsonDataImpl(L, buf, level - 1, numformat, -1, visited);
              lua_pop(L, 1);  // table/-2, key/-1
            }
            // stack: table/-1, as the key was popped by lua_next
          }
          appendw(buf, isarray ? ']' : '}');
          LuaPopVisit(visited);
          return 0;
        } else {
          // TODO(jart): don't leak memory with longjmp
          luaL_error(L, "can't serialize cyclic data structure to json");
          unreachable;
        }

      default:
        luaL_error(L, "won't serialize %s to json", luaL_typename(L, idx));
        unreachable;
    }
  } else {
    luaL_error(L, "too many nested tables");
    unreachable;
  }
}

int LuaEncodeJsonData(lua_State *L, char **buf, char *numformat, int idx) {
  int rc;
  struct LuaVisited visited = {0};
  rc = LuaEncodeJsonDataImpl(L, buf, 64, numformat, idx, &visited);
  assert(!visited.n);
  free(visited.p);
  return rc;
}
