#include "lua.h"


int luaopen_lib2 (lua_State *L);

LUAMOD_API int luaopen_lib21 (lua_State *L) {
  return luaopen_lib2(L);
}


