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
#include "libc/macros.h"
#include "libc/str/str.h"
#include "third_party/lua/lauxlib.h"
#include "third_party/regex/regex.h"

struct ReErrno {
  int err;
  char doc[64];
};

static void LuaSetIntField(lua_State *L, const char *k, lua_Integer v) {
  lua_pushinteger(L, v);
  lua_setfield(L, -2, k);
}

static int LuaReReturnError(lua_State *L, regex_t *r, int rc) {
  struct ReErrno *e;
  lua_pushnil(L);
  e = lua_newuserdatauv(L, sizeof(struct ReErrno), 0);
  luaL_setmetatable(L, "re.Errno");
  e->err = rc;
  regerror(rc, r, e->doc, sizeof(e->doc));
  return 2;
}

static regex_t *LuaReCompileImpl(lua_State *L, const char *p, int f) {
  int rc;
  regex_t *r;
  r = lua_newuserdatauv(L, sizeof(regex_t), 0);
  luaL_setmetatable(L, "re.Regex");
  f &= REG_EXTENDED | REG_ICASE | REG_NEWLINE | REG_NOSUB;
  f ^= REG_EXTENDED;
  if ((rc = regcomp(r, p, f)) == REG_OK) {
    return r;
  } else {
    LuaReReturnError(L, r, rc);
    return NULL;
  }
}

static int LuaReSearchImpl(lua_State *L, regex_t *r, const char *s, int f) {
  int rc, i, n;
  regmatch_t *m;
  luaL_Buffer tmp;
  n = 1 + r->re_nsub;
  m = (regmatch_t *)luaL_buffinitsize(L, &tmp, n * sizeof(regmatch_t));
  m->rm_so = 0;
  m->rm_eo = 0;
  if ((rc = regexec(r, s, n, m, f >> 8)) == REG_OK) {
    for (i = 0; i < n; ++i) {
      lua_pushlstring(L, s + m[i].rm_so, m[i].rm_eo - m[i].rm_so);
    }
    return n;
  } else {
    return LuaReReturnError(L, r, rc);
  }
}

////////////////////////////////////////////////////////////////////////////////
// re

static int LuaReSearch(lua_State *L) {
  int f;
  regex_t *r;
  const char *p, *s;
  p = luaL_checkstring(L, 1);
  s = luaL_checkstring(L, 2);
  f = luaL_optinteger(L, 3, 0);
  if (f & ~(REG_EXTENDED | REG_ICASE | REG_NEWLINE | REG_NOSUB |
            REG_NOTBOL << 8 | REG_NOTEOL << 8)) {
    luaL_argerror(L, 3, "invalid flags");
    __builtin_unreachable();
  }
  if ((r = LuaReCompileImpl(L, p, f))) {
    return LuaReSearchImpl(L, r, s, f);
  } else {
    return 2;
  }
}

static int LuaReCompile(lua_State *L) {
  int f;
  regex_t *r;
  const char *p;
  p = luaL_checkstring(L, 1);
  f = luaL_optinteger(L, 2, 0);
  if (f & ~(REG_EXTENDED | REG_ICASE | REG_NEWLINE | REG_NOSUB)) {
    luaL_argerror(L, 2, "invalid flags");
    __builtin_unreachable();
  }
  if ((r = LuaReCompileImpl(L, p, f))) {
    return 1;
  } else {
    return 2;
  }
}

////////////////////////////////////////////////////////////////////////////////
// re.Regex

static int LuaReRegexSearch(lua_State *L) {
  int f;
  regex_t *r;
  const char *s;
  r = luaL_checkudata(L, 1, "re.Regex");
  s = luaL_checkstring(L, 2);
  f = luaL_optinteger(L, 3, 0);
  if (f & ~(REG_NOTBOL << 8 | REG_NOTEOL << 8)) {
    luaL_argerror(L, 3, "invalid flags");
    __builtin_unreachable();
  }
  return LuaReSearchImpl(L, r, s, f);
}

static int LuaReRegexGc(lua_State *L) {
  regex_t *r;
  r = luaL_checkudata(L, 1, "re.Regex");
  regfree(r);
  return 0;
}

static const luaL_Reg kLuaRe[] = {
    {"compile", LuaReCompile},  //
    {"search", LuaReSearch},    //
    {NULL, NULL},               //
};

static const luaL_Reg kLuaReRegexMeth[] = {
    {"search", LuaReRegexSearch},  //
    {NULL, NULL},                  //
};

static const luaL_Reg kLuaReRegexMeta[] = {
    {"__gc", LuaReRegexGc},  //
    {NULL, NULL},            //
};

static void LuaReRegexObj(lua_State *L) {
  luaL_newmetatable(L, "re.Regex");
  luaL_setfuncs(L, kLuaReRegexMeta, 0);
  luaL_newlibtable(L, kLuaReRegexMeth);
  luaL_setfuncs(L, kLuaReRegexMeth, 0);
  lua_setfield(L, -2, "__index");
  lua_pop(L, 1);
}

////////////////////////////////////////////////////////////////////////////////
// re.Errno

static struct ReErrno *GetReErrno(lua_State *L) {
  return luaL_checkudata(L, 1, "re.Errno");
}

// re.Errno:errno()
//     └─→ errno:int
static int LuaReErrnoErrno(lua_State *L) {
  lua_pushinteger(L, GetReErrno(L)->err);
  return 1;
}

// re.Errno:doc()
//     └─→ description:str
static int LuaReErrnoDoc(lua_State *L) {
  lua_pushstring(L, GetReErrno(L)->doc);
  return 1;
}

static const luaL_Reg kLuaReErrnoMeth[] = {
    {"errno", LuaReErrnoErrno},  //
    {"doc", LuaReErrnoDoc},      //
    {0},                         //
};

static const luaL_Reg kLuaReErrnoMeta[] = {
    {"__tostring", LuaReErrnoDoc},  //
    {0},                            //
};

static void LuaReErrnoObj(lua_State *L) {
  luaL_newmetatable(L, "re.Errno");
  luaL_setfuncs(L, kLuaReErrnoMeta, 0);
  luaL_newlibtable(L, kLuaReErrnoMeth);
  luaL_setfuncs(L, kLuaReErrnoMeth, 0);
  lua_setfield(L, -2, "__index");
  lua_pop(L, 1);
}

////////////////////////////////////////////////////////////////////////////////

_Alignas(1) static const struct thatispacked {
  const char s[8];
  char x;
} kReMagnums[] = {
    {"BASIC", REG_EXTENDED},     // compile flag
    {"ICASE", REG_ICASE},        // compile flag
    {"NEWLINE", REG_NEWLINE},    // compile flag
    {"NOSUB", REG_NOSUB},        // compile flag
    {"NOMATCH", REG_NOMATCH},    // error
    {"BADPAT", REG_BADPAT},      // error
    {"ECOLLATE", REG_ECOLLATE},  // error
    {"ECTYPE", REG_ECTYPE},      // error
    {"EESCAPE", REG_EESCAPE},    // error
    {"ESUBREG", REG_ESUBREG},    // error
    {"EBRACK", REG_EBRACK},      // error
    {"EPAREN", REG_EPAREN},      // error
    {"EBRACE", REG_EBRACE},      // error
    {"BADBR", REG_BADBR},        // error
    {"ERANGE", REG_ERANGE},      // error
    {"ESPACE", REG_ESPACE},      // error
    {"BADRPT", REG_BADRPT},      // error
};

int LuaRe(lua_State *L) {
  int i;
  char buf[9];
  luaL_newlib(L, kLuaRe);
  LuaSetIntField(L, "NOTBOL", REG_NOTBOL << 8);  // search flag
  LuaSetIntField(L, "NOTEOL", REG_NOTEOL << 8);  // search flag
  for (i = 0; i < ARRAYLEN(kReMagnums); ++i) {
    memcpy(buf, kReMagnums[i].s, 8);
    buf[8] = 0;
    LuaSetIntField(L, buf, kReMagnums[i].x);
  }
  LuaReRegexObj(L);
  LuaReErrnoObj(L);
  return 1;
}
