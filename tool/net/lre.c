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
#include "libc/macros.internal.h"
#include "libc/x/x.h"
#include "third_party/lua/lauxlib.h"
#include "third_party/regex/regex.h"

static const char kRegCode[][8] = {
    "OK",     "NOMATCH", "BADPAT", "COLLATE", "ECTYPE", "EESCAPE", "ESUBREG",
    "EBRACK", "EPAREN",  "EBRACE", "BADBR",   "ERANGE", "ESPACE",  "BADRPT",
};

static void LuaSetIntField(lua_State *L, const char *k, lua_Integer v) {
  lua_pushinteger(L, v);
  lua_setfield(L, -2, k);
}

static regex_t *LuaReCompileImpl(lua_State *L, const char *p, int f) {
  int e;
  regex_t *r;
  r = xmalloc(sizeof(regex_t));
  f &= REG_EXTENDED | REG_ICASE | REG_NEWLINE | REG_NOSUB;
  f ^= REG_EXTENDED;
  if ((e = regcomp(r, p, f)) != REG_OK) {
    free(r);
    luaL_error(L, "regcomp(%s) → REG_%s", p,
               kRegCode[MAX(0, MIN(ARRAYLEN(kRegCode) - 1, e))]);
    unreachable;
  }
  return r;
}

static int LuaReSearchImpl(lua_State *L, regex_t *r, const char *s, int f) {
  int i, n;
  regmatch_t *m;
  n = r->re_nsub + 1;
  m = xcalloc(n, sizeof(regmatch_t));
  if (regexec(r, s, n, m, f >> 8) == REG_OK) {
    for (i = 0; i < n; ++i) {
      lua_pushlstring(L, s + m[i].rm_so, m[i].rm_eo - m[i].rm_so);
    }
  } else {
    n = 0;
  }
  free(m);
  return n;
}

static int LuaReSearch(lua_State *L) {
  regex_t *r;
  int i, e, n, f;
  const char *p, *s;
  p = luaL_checkstring(L, 1);
  s = luaL_checkstring(L, 2);
  f = luaL_optinteger(L, 3, 0);
  if (f & ~(REG_EXTENDED | REG_ICASE | REG_NEWLINE | REG_NOSUB |
            REG_NOTBOL << 8 | REG_NOTEOL << 8)) {
    luaL_argerror(L, 3, "invalid flags");
    unreachable;
  }
  r = LuaReCompileImpl(L, p, f);
  n = LuaReSearchImpl(L, r, s, f);
  regfree(r);
  free(r);
  return n;
}

static int LuaReCompile(lua_State *L) {
  int f, e;
  const char *p;
  regex_t *r, **u;
  p = luaL_checkstring(L, 1);
  f = luaL_optinteger(L, 2, 0);
  if (f & ~(REG_EXTENDED | REG_ICASE | REG_NEWLINE | REG_NOSUB)) {
    luaL_argerror(L, 3, "invalid flags");
    unreachable;
  }
  r = LuaReCompileImpl(L, p, f);
  u = lua_newuserdata(L, sizeof(regex_t *));
  luaL_setmetatable(L, "regex_t*");
  *u = r;
  return 1;
}

static int LuaReRegexSearch(lua_State *L) {
  int f;
  regex_t **u;
  const char *s;
  u = luaL_checkudata(L, 1, "regex_t*");
  s = luaL_checkstring(L, 2);
  f = luaL_optinteger(L, 3, 0);
  if (!*u) {
    luaL_argerror(L, 1, "destroyed");
    unreachable;
  }
  if (f & ~(REG_NOTBOL << 8 | REG_NOTEOL << 8)) {
    luaL_argerror(L, 3, "invalid flags");
    unreachable;
  }
  return LuaReSearchImpl(L, *u, s, f);
}

static int LuaReRegexGc(lua_State *L) {
  regex_t **u;
  u = luaL_checkudata(L, 1, "regex_t*");
  if (*u) {
    regfree(*u);
    free(*u);
    *u = NULL;
  }
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

static void LuaReRegex(lua_State *L) {
  luaL_newmetatable(L, "regex_t*");
  luaL_setfuncs(L, kLuaReRegexMeta, 0);
  luaL_newlibtable(L, kLuaReRegexMeth);
  luaL_setfuncs(L, kLuaReRegexMeth, 0);
  lua_setfield(L, -2, "__index");
  lua_pop(L, 1);
}

int LuaRe(lua_State *L) {
  luaL_newlib(L, kLuaRe);
  LuaSetIntField(L, "BASIC", REG_EXTENDED);
  LuaSetIntField(L, "ICASE", REG_ICASE);
  LuaSetIntField(L, "NEWLINE", REG_NEWLINE);
  LuaSetIntField(L, "NOSUB", REG_NOSUB);
  LuaSetIntField(L, "NOTBOL", REG_NOTBOL << 8);
  LuaSetIntField(L, "NOTEOL", REG_NOTEOL << 8);
  LuaReRegex(L);
  return 1;
}
