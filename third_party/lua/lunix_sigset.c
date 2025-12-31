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
#include "third_party/lua/lunix_internal.h"
#include "libc/limits.h"
#include "libc/mem/mem.h"
#include "libc/serialize.h"
#include "libc/stdio/append.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/sig.h"

////////////////////////////////////////////////////////////////////////////////
// unix.Sigset object

// unix.Sigset(sig:int, ...)
//     └─→ unix.Sigset
int LuaUnixSigset(lua_State *L) {
  int i, n;
  sigset_t set;
  sigemptyset(&set);
  n = lua_gettop(L);
  for (i = 1; i <= n; ++i) {
    sigaddset(&set, luaL_checkinteger(L, i));
  }
  LuaPushSigset(L, set);
  return 1;
}

// unix.Sigset:add(sig:int)
static int LuaUnixSigsetAdd(lua_State *L) {
  sigset_t *set;
  lua_Integer sig;
  set = luaL_checkudata(L, 1, "unix.Sigset");
  sig = luaL_checkinteger(L, 2);
  sigaddset(set, sig);
  return 0;
}

// unix.Sigset:remove(sig:int)
static int LuaUnixSigsetRemove(lua_State *L) {
  sigset_t *set;
  lua_Integer sig;
  set = luaL_checkudata(L, 1, "unix.Sigset");
  sig = luaL_checkinteger(L, 2);
  sigdelset(set, sig);
  return 0;
}

// unix.Sigset:fill()
static int LuaUnixSigsetFill(lua_State *L) {
  sigset_t *set;
  set = luaL_checkudata(L, 1, "unix.Sigset");
  sigfillset(set);
  return 0;
}

// unix.Sigset:clear()
static int LuaUnixSigsetClear(lua_State *L) {
  sigset_t *set;
  set = luaL_checkudata(L, 1, "unix.Sigset");
  sigemptyset(set);
  return 0;
}

// unix.Sigset:contains(sig:int)
//     └─→ bool
static int LuaUnixSigsetContains(lua_State *L) {
  sigset_t *set;
  lua_Integer sig;
  set = luaL_checkudata(L, 1, "unix.Sigset");
  sig = luaL_checkinteger(L, 2);
  return ReturnBoolean(L, sigismember(set, sig));
}

static int LuaUnixSigsetTostring(lua_State *L) {
  char *b = 0;
  sigset_t *ss;
  int sig, first;
  ss = luaL_checkudata(L, 1, "unix.Sigset");
  appends(&b, "unix.Sigset");
  appendw(&b, '(');
  for (sig = first = 1; sig <= NSIG; ++sig) {
    if (sigismember(ss, sig) == 1) {
      if (!first) {
        appendw(&b, READ16LE(", "));
      } else {
        first = 0;
      }
      appendw(&b, READ64LE("unix.\0\0"));
      appends(&b, strsignal(sig));
    }
  }
  appendw(&b, ')');
  lua_pushlstring(L, b, appendz(b).i);
  free(b);
  return 1;
}

static const luaL_Reg kLuaUnixSigsetMeth[] = {
    {"add", LuaUnixSigsetAdd},            //
    {"fill", LuaUnixSigsetFill},          //
    {"clear", LuaUnixSigsetClear},        //
    {"remove", LuaUnixSigsetRemove},      //
    {"contains", LuaUnixSigsetContains},  //
    {0},                                  //
};

static const luaL_Reg kLuaUnixSigsetMeta[] = {
    {"__tostring", LuaUnixSigsetTostring},  //
    {"__repr", LuaUnixSigsetTostring},      //
    {0},                                    //
};

void LuaUnixSigsetObj(lua_State *L) {
  luaL_newmetatable(L, "unix.Sigset");
  luaL_setfuncs(L, kLuaUnixSigsetMeta, 0);
  luaL_newlibtable(L, kLuaUnixSigsetMeth);
  luaL_setfuncs(L, kLuaUnixSigsetMeth, 0);
  lua_setfield(L, -2, "__index");
  lua_pop(L, 1);
}
