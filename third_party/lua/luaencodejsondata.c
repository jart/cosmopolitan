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
#include "libc/log/rop.h"
#include "libc/mem/mem.h"
#include "libc/runtime/gc.internal.h"
#include "libc/stdio/append.internal.h"
#include "libc/stdio/strlist.internal.h"
#include "libc/str/str.h"
#include "net/http/escape.h"
#include "third_party/double-conversion/wrapper.h"
#include "third_party/lua/cosmo.h"
#include "third_party/lua/lauxlib.h"
#include "third_party/lua/lua.h"
#include "third_party/lua/visitor.h"

static int LuaEncodeJsonDataImpl(lua_State *L, char **buf, int level,
                                 char *numformat, int idx,
                                 struct LuaVisited *visited) {
  char *s;
  int sli, rc;
  bool isarray;
  size_t tbllen, i, z;
  struct StrList sl = {0};
  char ibuf[128], fmt[] = "%.14g";
  if (level > 0) {
    switch (lua_type(L, idx)) {

      case LUA_TNIL:
        RETURN_ON_ERROR(appendw(buf, READ32LE("null")));
        return 0;

      case LUA_TBOOLEAN:
        RETURN_ON_ERROR(appendw(buf, lua_toboolean(L, idx)
                                         ? READ32LE("true")
                                         : READ64LE("false\0\0")));
        return 0;

      case LUA_TSTRING:
        s = lua_tolstring(L, idx, &z);
        if (!(s = EscapeJsStringLiteral(s, z, &z))) goto OnError;
        RETURN_ON_ERROR(appendw(buf, '"'));
        RETURN_ON_ERROR(appendd(buf, s, z));
        RETURN_ON_ERROR(appendw(buf, '"'));
        free(s);
        return 0;

      case LUA_TNUMBER:
        if (lua_isinteger(L, idx)) {
          RETURN_ON_ERROR(appendd(
              buf, ibuf, FormatInt64(ibuf, luaL_checkinteger(L, idx)) - ibuf));
        } else {
          RETURN_ON_ERROR(
              appends(buf, DoubleToEcmascript(ibuf, lua_tonumber(L, idx))));
        }
        return 0;

      case LUA_TTABLE:
        RETURN_ON_ERROR(rc = LuaPushVisit(visited, lua_topointer(L, idx)));
        if (!rc) {
          lua_pushvalue(L, idx);  // table ref
          tbllen = lua_rawlen(L, -1);
          // encode tables with numeric indices and empty tables as arrays
          isarray =
              tbllen > 0 ||                          // integer keys present
              (lua_pushnil(L), !lua_next(L, -2)) ||  // no non-integer keys
              (lua_pop(L, 2), false);  // pop key/value pushed by lua_next
          if (isarray) {
            for (i = 1; i <= tbllen; i++) {
              RETURN_ON_ERROR(sli = AppendStrList(&sl));
              lua_rawgeti(L, -1, i);  // table/-2, value/-1
              RETURN_ON_ERROR(LuaEncodeJsonDataImpl(L, &sl.p[sli], level - 1,
                                                    numformat, -1, visited));
              lua_pop(L, 1);
            }
          } else {
            i = 1;
            lua_pushnil(L);  // push the first key
            while (lua_next(L, -2)) {
              if (lua_type(L, -2) != LUA_TSTRING) {
                // json tables must be arrays or use string keys
                goto OnError;
              }
              // the json parser inserts a `__json_object__` into empty
              // objects, so we don't serialize `{}` as `[]` by mistake
              // and as such, we should ignore it here, for readability
              if (strcmp(luaL_checkstring(L, -2), "__json_object__")) {
                RETURN_ON_ERROR(sli = AppendStrList(&sl));
                RETURN_ON_ERROR(LuaEncodeJsonDataImpl(L, &sl.p[sli], level - 1,
                                                      numformat, -2, visited));
                RETURN_ON_ERROR(appendw(&sl.p[sli], ':'));
                RETURN_ON_ERROR(LuaEncodeJsonDataImpl(L, &sl.p[sli], level - 1,
                                                      numformat, -1, visited));
              }
              lua_pop(L, 1);  // table/-2, key/-1
            }
            // stack: table/-1, as the key was popped by lua_next
            SortStrList(&sl);
          }
          RETURN_ON_ERROR(appendw(buf, isarray ? '[' : '{'));
          RETURN_ON_ERROR(JoinStrList(&sl, buf, ','));
          FreeStrList(&sl);
          RETURN_ON_ERROR(appendw(buf, isarray ? ']' : '}'));
          LuaPopVisit(visited);
          lua_pop(L, 1);  // table ref
          return 0;
        } else {
          // cyclic data structure
          goto OnError;
        }

      default:
        // unsupported lua type
        goto OnError;
    }
  } else {
    // too much depth
    goto OnError;
  }
OnError:
  FreeStrList(&sl);
  return -1;
}

/**
 * Encodes Lua data structure as JSON.
 *
 * @param L is Lua interpreter state
 * @param buf receives encoded output string
 * @param numformat controls double formatting
 * @param idx is index of item on Lua stack
 * @return 0 on success, or -1 on error
 */
int LuaEncodeJsonData(lua_State *L, char **buf, char *numformat, int idx) {
  int rc;
  struct LuaVisited visited = {0};
  rc = LuaEncodeJsonDataImpl(L, buf, 64, numformat, idx, &visited);
  free(visited.p);
  return rc;
}
