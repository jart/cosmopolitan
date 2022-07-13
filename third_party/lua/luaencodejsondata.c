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
#include "libc/intrin/kprintf.h"
#include "libc/log/log.h"
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

struct Serializer {
  struct LuaVisited visited;
  const char *reason;
  char *strbuf;
  size_t strbuflen;
  bool sorted;
};

static int Serialize(lua_State *, char **, int, struct Serializer *, int);

static int SerializeNull(lua_State *L, char **buf) {
  RETURN_ON_ERROR(appendw(buf, READ32LE("null")));
  return 0;
OnError:
  return -1;
}

static int SerializeBoolean(lua_State *L, char **buf, int idx) {
  RETURN_ON_ERROR(appendw(
      buf, lua_toboolean(L, idx) ? READ32LE("true") : READ64LE("false\0\0")));
  return 0;
OnError:
  return -1;
}

static int SerializeNumber(lua_State *L, char **buf, int idx) {
  char ibuf[128];
  if (lua_isinteger(L, idx)) {
    RETURN_ON_ERROR(appendd(
        buf, ibuf, FormatInt64(ibuf, luaL_checkinteger(L, idx)) - ibuf));
  } else {
    RETURN_ON_ERROR(appends(buf, DoubleToJson(ibuf, lua_tonumber(L, idx))));
  }
  return 0;
OnError:
  return -1;
}

static int SerializeString(lua_State *L, char **buf, int idx,
                           struct Serializer *z) {
  char *s;
  size_t m;
  s = lua_tolstring(L, idx, &m);
  if (!(s = EscapeJsStringLiteral(&z->strbuf, &z->strbuflen, s, m, &m))) {
    goto OnError;
  }
  RETURN_ON_ERROR(appendw(buf, '"'));
  RETURN_ON_ERROR(appendd(buf, s, m));
  RETURN_ON_ERROR(appendw(buf, '"'));
  return 0;
OnError:
  return -1;
}

static int SerializeArray(lua_State *L, char **buf, struct Serializer *z,
                          int level, size_t tbllen) {
  size_t i;
  RETURN_ON_ERROR(appendw(buf, '['));
  for (i = 1; i <= tbllen; i++) {
    lua_rawgeti(L, -1, i);
    if (i > 1) RETURN_ON_ERROR(appendw(buf, ','));
    RETURN_ON_ERROR(Serialize(L, buf, -1, z, level - 1));
    lua_pop(L, 1);
  }
  RETURN_ON_ERROR(appendw(buf, ']'));
  return 0;
OnError:
  return -1;
}

static int SerializeObject(lua_State *L, char **buf, struct Serializer *z,
                           int level) {
  bool comma = false;
  RETURN_ON_ERROR(appendw(buf, '{'));
  lua_pushnil(L);
  while (lua_next(L, -2)) {
    if (lua_type(L, -2) == LUA_TSTRING) {
      if (comma) {
        RETURN_ON_ERROR(appendw(buf, ','));
      } else {
        comma = true;
      }
      RETURN_ON_ERROR(SerializeString(L, buf, -2, z));
      RETURN_ON_ERROR(appendw(buf, ':'));
      RETURN_ON_ERROR(Serialize(L, buf, -1, z, level - 1));
      lua_pop(L, 1);
    } else {
      z->reason = "json objects must only use string keys";
      goto OnError;
    }
  }
  RETURN_ON_ERROR(appendw(buf, '}'));
  return 0;
OnError:
  return -1;
}

static int SerializeSorted(lua_State *L, char **buf, struct Serializer *z,
                           int level) {
  int i;
  struct StrList sl = {0};
  lua_pushnil(L);
  while (lua_next(L, -2)) {
    if (lua_type(L, -2) == LUA_TSTRING) {
      RETURN_ON_ERROR(i = AppendStrList(&sl));
      RETURN_ON_ERROR(SerializeString(L, sl.p + i, -2, z));
      RETURN_ON_ERROR(appendw(sl.p + i, ':'));
      RETURN_ON_ERROR(Serialize(L, sl.p + i, -1, z, level - 1));
      lua_pop(L, 1);
    } else {
      z->reason = "json objects must only use string keys";
      goto OnError;
    }
  }
  SortStrList(&sl);
  RETURN_ON_ERROR(appendw(buf, '{'));
  RETURN_ON_ERROR(JoinStrList(&sl, buf, ','));
  RETURN_ON_ERROR(appendw(buf, '}'));
  FreeStrList(&sl);
  return 0;
OnError:
  FreeStrList(&sl);
  return -1;
}

static int SerializeTable(lua_State *L, char **buf, int idx,
                          struct Serializer *z, int level) {
  int rc;
  bool isarray;
  lua_Unsigned n;
  RETURN_ON_ERROR(rc = LuaPushVisit(&z->visited, lua_topointer(L, idx)));
  if (!rc) {
    lua_pushvalue(L, idx);
    if ((n = lua_rawlen(L, -1)) > 0) {
      isarray = true;
    } else {
      // the json parser inserts `[0]=false` in empty arrays
      // so we can tell them apart from empty objects, which
      // is needed in order to have `[]` roundtrip the parse
      isarray =
          (lua_rawgeti(L, -1, 0) == LUA_TBOOLEAN && !lua_toboolean(L, -1));
      lua_pop(L, 1);
    }
    if (isarray) {
      RETURN_ON_ERROR(SerializeArray(L, buf, z, level, n));
    } else if (z->sorted) {
      RETURN_ON_ERROR(SerializeSorted(L, buf, z, level));
    } else {
      RETURN_ON_ERROR(SerializeObject(L, buf, z, level));
    }
    LuaPopVisit(&z->visited);
    lua_pop(L, 1);  // table ref
    return 0;
  } else {
    z->reason = "won't serialize cyclic lua table";
    goto OnError;
  }
OnError:
  return -1;
}

static int Serialize(lua_State *L, char **buf, int idx, struct Serializer *z,
                     int level) {
  if (level > 0) {
    switch (lua_type(L, idx)) {
      case LUA_TNIL:
        return SerializeNull(L, buf);
      case LUA_TBOOLEAN:
        return SerializeBoolean(L, buf, idx);
      case LUA_TSTRING:
        return SerializeString(L, buf, idx, z);
      case LUA_TNUMBER:
        return SerializeNumber(L, buf, idx);
      case LUA_TTABLE:
        return SerializeTable(L, buf, idx, z, level);
      default:
        z->reason = "unsupported lua type";
        return -1;
    }
  } else {
    z->reason = "table has great depth";
    return -1;
  }
}

/**
 * Encodes Lua data structure as JSON.
 *
 * On success, the serialized value is returned in `buf` and the Lua
 * stack should be in the same state when this function was called. On
 * error, values *will* be returned on the Lua stack:
 *
 * - possible junk...
 * - nil (index -2)
 * - error string (index -1)
 *
 * The following error return conditions are implemented:
 *
 * - Value found that isn't nil/bool/number/string/table
 * - Object existed with non-string key
 * - Tables had too much depth
 * - Tables are cyclic
 * - Out of C memory
 *
 * Your `*buf` must initialized to null. If it's allocated, then it must
 * have been allocated by cosmo's append*() library. After this function
 * is called, `*buf` will need to be free()'d.
 *
 * @param L is Lua interpreter state
 * @param buf receives encoded output string
 * @param idx is index of item on Lua stack
 * @return 0 on success, or -1 on error
 */
int LuaEncodeJsonData(lua_State *L, char **buf, int idx, bool sorted) {
  int rc, depth = 64;
  struct Serializer z = {.reason = "out of memory", .sorted = sorted};
  if (lua_checkstack(L, depth * 4)) {
    rc = Serialize(L, buf, idx, &z, depth);
    free(z.visited.p);
    free(z.strbuf);
    if (rc == -1) {
      lua_pushnil(L);
      lua_pushstring(L, z.reason);
    }
    return rc;
  } else {
    luaL_error(L, "can't set stack depth");
    unreachable;
  }
}
