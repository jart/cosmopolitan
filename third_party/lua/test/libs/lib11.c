#include "lua.h"

/* function from lib1.c */
int lib1_export (lua_State *L);

LUAMOD_API int luaopen_lib11 (lua_State *L) {
  return lib1_export(L);
}


