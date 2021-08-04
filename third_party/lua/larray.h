/*
** Lua arrays
** See Copyright Notice in lua.h
*/

#ifndef larray_h
#define larray_h

#include "lobject.h"

#define luaH_emptyobject	(&luaH_emptyobject_)
#define luaO_nilobject (&G(L)->nilvalue)

LUAI_FUNC Array *luaA_new (lua_State *L);
LUAI_FUNC void luaA_resize (lua_State *L, Array *a, unsigned int nsize);
LUAI_FUNC void luaA_free (lua_State *L, Array *a);

LUAI_FUNC const TValue *luaA_getint (lua_State *L, Array *a, lua_Integer key);
LUAI_FUNC void luaA_setint (lua_State *L, Array *a, lua_Integer key, TValue *value);
LUAI_FUNC const TValue *luaA_get (lua_State *L, Array *a, const TValue *key);
LUAI_FUNC void luaA_set (lua_State *L, Array *a, const TValue *key, TValue *value);

#endif
