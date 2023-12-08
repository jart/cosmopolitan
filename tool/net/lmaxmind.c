/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
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
#include "libc/mem/mem.h"
#include "libc/x/x.h"
#include "third_party/lua/lauxlib.h"
#include "third_party/lua/lua.h"
#include "third_party/lua/luaconf.h"
#include "third_party/maxmind/maxminddb.h"

struct MaxmindDb {
  int refs;
  MMDB_s mmdb;
};

struct MaxmindResult {
  uint32_t ip;
  struct MaxmindDb *db;
  MMDB_lookup_result_s mmlr;
};

static const char *GetMmdbError(int err) {
  switch (err) {
    case MMDB_FILE_OPEN_ERROR:
      return "FILE_OPEN_ERROR";
    case MMDB_CORRUPT_SEARCH_TREE_ERROR:
      return "CORRUPT_SEARCH_TREE_ERROR";
    case MMDB_INVALID_METADATA_ERROR:
      return "INVALID_METADATA_ERROR";
    case MMDB_IO_ERROR:
      return "IO_ERROR";
    case MMDB_OUT_OF_MEMORY_ERROR:
      return "OUT_OF_MEMORY_ERROR";
    case MMDB_UNKNOWN_DATABASE_FORMAT_ERROR:
      return "UNKNOWN_DATABASE_FORMAT_ERROR";
    case MMDB_INVALID_DATA_ERROR:
      return "INVALID_DATA_ERROR";
    case MMDB_INVALID_LOOKUP_PATH_ERROR:
      return "INVALID_LOOKUP_PATH_ERROR";
    case MMDB_LOOKUP_PATH_DOES_NOT_MATCH_DATA_ERROR:
      return "LOOKUP_PATH_DOES_NOT_MATCH_DATA_ERROR";
    case MMDB_INVALID_NODE_NUMBER_ERROR:
      return "INVALID_NODE_NUMBER_ERROR";
    case MMDB_IPV6_LOOKUP_IN_IPV4_DATABASE_ERROR:
      return "IPV6_LOOKUP_IN_IPV4_DATABASE_ERROR";
    default:
      return "UNKNOWN";
  }
};

static int LuaMaxmindOpen(lua_State *L) {
  int err;
  const char *p;
  struct MaxmindDb **udb, *db;
  p = luaL_checklstring(L, 1, 0);
  db = xmalloc(sizeof(struct MaxmindDb));
  if ((err = MMDB_open(p, 0, &db->mmdb)) != MMDB_SUCCESS) {
    free(db);
    luaL_error(L, "MMDB_open(%s) → MMDB_%s", p, GetMmdbError(err));
    __builtin_unreachable();
  }
  db->refs = 1;
  udb = lua_newuserdatauv(L, sizeof(db), 1);
  luaL_setmetatable(L, "MaxmindDb*");
  *udb = db;
  return 1;
}

static wontreturn void LuaThrowMaxmindIpError(lua_State *L,
                                              const char *function_name,
                                              uint32_t ip, int err) {
  luaL_error(L, "%s(%d.%d.%d.%d) → MMDB_%s", function_name,
             (ip & 0xff000000) >> 030, (ip & 0x00ff0000) >> 020,
             (ip & 0x0000ff00) >> 010, (ip & 0x000000ff) >> 000,
             GetMmdbError(err));
  __builtin_unreachable();
}

static int LuaMaxmindDbLookup(lua_State *L) {
  int err;
  lua_Integer ip;
  struct MaxmindDb **udb, *db;
  struct MaxmindResult **ur, *r;
  udb = luaL_checkudata(L, 1, "MaxmindDb*");
  ip = luaL_checkinteger(L, 2);
  if (ip < 0 || ip > 0xffffffff) {
    lua_pushnil(L);
    return 1;
  }
  db = *udb;
  r = xmalloc(sizeof(struct MaxmindResult));
  r->mmlr = MMDB_lookup(&db->mmdb, ip, &err);
  if (err) {
    free(r);
    LuaThrowMaxmindIpError(L, "MMDB_lookup", ip, err);
  }
  if (!r->mmlr.found_entry) {
    free(r);
    lua_pushnil(L);
    return 1;
  }
  r->ip = ip;
  r->db = db;
  r->db->refs++;
  ur = lua_newuserdatauv(L, sizeof(r), 1);
  luaL_setmetatable(L, "MaxmindResult*");
  *ur = r;
  return 1;
}

static int LuaMaxmindResultNetmask(lua_State *L) {
  struct MaxmindResult **ur;
  ur = luaL_checkudata(L, 1, "MaxmindResult*");
  lua_pushinteger(L, (*ur)->mmlr.netmask - (128 - 32));
  return 1;
}

static MMDB_entry_data_list_s *LuaMaxmindDump(lua_State *L,
                                              MMDB_entry_data_list_s *dl) {
  size_t i, n;
  char ibuf[64];
  switch (dl->entry_data.type) {
    case MMDB_DATA_TYPE_UTF8_STRING:
      lua_pushlstring(L, dl->entry_data.utf8_string, dl->entry_data.data_size);
      return dl->next;
    case MMDB_DATA_TYPE_BYTES:
      lua_pushlstring(L, (void *)dl->entry_data.bytes,
                      dl->entry_data.data_size);
      return dl->next;
    case MMDB_DATA_TYPE_INT32:
      lua_pushinteger(L, dl->entry_data.int32);
      return dl->next;
    case MMDB_DATA_TYPE_UINT16:
      lua_pushinteger(L, dl->entry_data.uint16);
      return dl->next;
    case MMDB_DATA_TYPE_UINT32:
      lua_pushinteger(L, dl->entry_data.uint32);
      return dl->next;
    case MMDB_DATA_TYPE_BOOLEAN:
      lua_pushboolean(L, dl->entry_data.boolean);
      return dl->next;
    case MMDB_DATA_TYPE_UINT64:
      lua_pushinteger(L, dl->entry_data.uint64);
      return dl->next;
    case MMDB_DATA_TYPE_UINT128:
      sprintf(ibuf, "%#jjx", dl->entry_data.uint128);
      lua_pushstring(L, ibuf);
      return dl->next;
    case MMDB_DATA_TYPE_DOUBLE:
      lua_pushnumber(L, dl->entry_data.double_value);
      return dl->next;
    case MMDB_DATA_TYPE_FLOAT:
      lua_pushnumber(L, dl->entry_data.float_value);
      return dl->next;
    case MMDB_DATA_TYPE_ARRAY:
      lua_newtable(L);
      n = dl->entry_data.data_size;
      for (dl = dl->next, i = 0; dl && i < n; ++i) {
        dl = LuaMaxmindDump(L, dl);
        lua_seti(L, -2, i + 1);
      }
      return dl;
    case MMDB_DATA_TYPE_MAP:
      lua_newtable(L);
      n = dl->entry_data.data_size;
      for (dl = dl->next; dl && n; n--) {
        dl = LuaMaxmindDump(L, dl);
        dl = LuaMaxmindDump(L, dl);
        lua_settable(L, -3);
      }
      return dl;
    default:
      lua_pushnil(L);
      return dl->next;
  }
}

static int LuaMaxmindResultGet(lua_State *L) {
  int i, n, err;
  const char **path;
  MMDB_entry_s entry, *ep;
  MMDB_entry_data_s edata;
  struct MaxmindResult **ur;
  MMDB_entry_data_list_s *dl;
  n = lua_gettop(L) - 1;
  ur = luaL_checkudata(L, 1, "MaxmindResult*");
  if (n <= 0) {
    ep = &(*ur)->mmlr.entry;
  } else {
    path = xcalloc(n + 1, sizeof(const char *));
    for (i = 0; i < n; ++i) path[i] = lua_tostring(L, 2 + i);
    err = MMDB_aget_value(&(*ur)->mmlr.entry, &edata, path);
    free(path);
    if (err) {
      if (err == MMDB_LOOKUP_PATH_DOES_NOT_MATCH_DATA_ERROR) {
        lua_pushnil(L);
        return 1;
      } else {
        LuaThrowMaxmindIpError(L, "getpath", (*ur)->ip, err);
      }
    }
    if (!edata.offset) {
      lua_pushnil(L);
      return 1;
    }
    entry.mmdb = (*ur)->mmlr.entry.mmdb;
    entry.offset = edata.offset;
    ep = &entry;
  }
  err = MMDB_get_entry_data_list(ep, &dl);
  if (err) LuaThrowMaxmindIpError(L, "getlist", (*ur)->ip, err);
  LuaMaxmindDump(L, dl);
  MMDB_free_entry_data_list(dl);
  return 1;
}

static void FreeMaxmindDb(struct MaxmindDb *db) {
  if (!--db->refs) {
    MMDB_close(&db->mmdb);
    free(db);
  }
}

static int LuaMaxmindDbGc(lua_State *L) {
  struct MaxmindDb **udb;
  udb = luaL_checkudata(L, 1, "MaxmindDb*");
  if (*udb) {
    FreeMaxmindDb(*udb);
    *udb = 0;
  }
  return 0;
}

static int LuaMaxmindResultGc(lua_State *L) {
  struct MaxmindResult **ur;
  ur = luaL_checkudata(L, 1, "MaxmindResult*");
  if (*ur) {
    FreeMaxmindDb((*ur)->db);
    free(*ur);
    *ur = 0;
  }
  return 0;
}

static const luaL_Reg kLuaMaxmind[] = {
    {"open", LuaMaxmindOpen},  //
    {0},                       //
};

static const luaL_Reg kLuaMaxmindDbMeth[] = {
    {"lookup", LuaMaxmindDbLookup},  //
    {0},                             //
};

static const luaL_Reg kLuaMaxmindDbMeta[] = {
    {"__gc", LuaMaxmindDbGc},  //
    {0},                       //
};

static const luaL_Reg kLuaMaxmindResultMeth[] = {
    {"get", LuaMaxmindResultGet},          //
    {"netmask", LuaMaxmindResultNetmask},  //
    {0},                                   //
};

static const luaL_Reg kLuaMaxmindResultMeta[] = {
    {"__gc", LuaMaxmindResultGc},  //
    {0},                           //
};

static void LuaMaxmindDb(lua_State *L) {
  luaL_newmetatable(L, "MaxmindDb*");
  luaL_setfuncs(L, kLuaMaxmindDbMeta, 0);
  luaL_newlibtable(L, kLuaMaxmindDbMeth);
  luaL_setfuncs(L, kLuaMaxmindDbMeth, 0);
  lua_setfield(L, -2, "__index");
  lua_pop(L, 1);
}

static void LuaMaxmindResult(lua_State *L) {
  luaL_newmetatable(L, "MaxmindResult*");
  luaL_setfuncs(L, kLuaMaxmindResultMeta, 0);
  luaL_newlibtable(L, kLuaMaxmindResultMeth);
  luaL_setfuncs(L, kLuaMaxmindResultMeth, 0);
  lua_setfield(L, -2, "__index");
  lua_pop(L, 1);
}

int LuaMaxmind(lua_State *L) {
  luaL_newlib(L, kLuaMaxmind);
  LuaMaxmindResult(L);
  LuaMaxmindDb(L);
  return 1;
}
