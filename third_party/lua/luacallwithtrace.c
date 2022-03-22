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
#include "third_party/lua/cosmo.h"
#include "third_party/lua/lauxlib.h"

// calling convention for lua stack of L is:
//   -2 is function
//   -1 is is argument (assuming nargs == 1)
// L will have this after the call
//   -1 is error or result (assuming nres == 1)
// @param L is main Lua interpreter
// @note this needs to be reentrant
int LuaCallWithTrace(lua_State *L, int nargs, int nres, lua_State *C) {
  int nresults, status;
  bool canyield = !!C;           // allow yield if coroutine is provided
  if (!C) C = lua_newthread(L);  // create a new coroutine if not passed
  // move coroutine to the bottom of the stack (including one that is passed)
  lua_insert(L, 1);
  // make sure that there is enough stack space
  if (!lua_checkstack(C, 1 + nargs)) {
    lua_pushliteral(L, "too many arguments to resume");
    return LUA_ERRRUN; /* error flag */
  }
  // move the function (and arguments) to the top of the coro stack
  lua_xmove(L, C, 1 + nargs);
  // resume the coroutine thus executing the function
  status = lua_resume(C, L, nargs, &nresults);
  lua_remove(L, 1);  // remove coroutine (still) at the bottom
  if (status != LUA_OK && status != LUA_YIELD) {
    // move the error message
    lua_xmove(C, L, 1);
    // replace the error with the traceback on failure
    luaL_traceback(L, C, lua_tostring(L, -1), 0);
    lua_remove(L, -2);  // remove the error message
  } else {
    if (!lua_checkstack(L, MAX(nresults, nres))) {
      lua_pop(C, nresults); /* remove results anyway */
      lua_pushliteral(L, "too many results to resume");
      return LUA_ERRRUN; /* error flag */
    }
    lua_xmove(C, L, nresults);  // move results to the main stack
    // grow the stack in case returned fewer results
    // than the caller expects, as lua_resume
    // doesn't adjust the stack for needed results
    for (; nresults < nres; nresults++) lua_pushnil(L);
    if (!canyield) status = LUA_OK;  // treat LUA_YIELD the same as LUA_OK
  }
  return status;
}
