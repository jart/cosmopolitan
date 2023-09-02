#if 0
/*─────────────────────────────────────────────────────────────────╗
│ To the extent possible under law, Justine Tunney has waived      │
│ all copyright and related or neighboring rights to this file,    │
│ as it is written in the following disclaimers:                   │
│   • http://unlicense.org/                                        │
│   • http://creativecommons.org/publicdomain/zero/1.0/            │
╚─────────────────────────────────────────────────────────────────*/
#endif
#include "third_party/lua/lauxlib.h"
#include "third_party/lua/lualib.h"

int NativeAdd(lua_State *L) {
  lua_Number x, y;
  x = luaL_checknumber(L, 1);
  y = luaL_checknumber(L, 2);
  lua_pushnumber(L, x + y);
  return 1; /* number of results */
}

int main(int argc, char *argv[]) {
  lua_State *L;
  L = luaL_newstate();
  luaL_openlibs(L);
  lua_pushcfunction(L, NativeAdd);
  lua_setglobal(L, "NativeAdd");
  (void)luaL_dofile(L, "/zip/examples/hellolua.lua");
  lua_close(L);
  return 0;
}
