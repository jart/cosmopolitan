/*
** Lua arrays
** See Copyright Notice in lua.h
*/

#define larray_c
#define LUA_CORE

#include "lprefix.h"


#include <math.h>
#include <limits.h>

#include "lua.h"

#include "ldebug.h"
#include "ldo.h"
#include "lgc.h"
#include "lmem.h"
#include "lobject.h"
#include "lstate.h"
#include "lstring.h"
#include "ltable.h"
#include "larray.h"
#include "lvm.h"

Array *luaA_new (lua_State *L) {
  GCObject *o = luaC_newobj(L, LUA_TARRAY, sizeof(Array));
  Array *a = gco2a(o);
  a->array = NULL;
  a->alimit = 0;
  return a;
}


void luaA_resize (lua_State *L, Array *a, unsigned int newsize) {
  unsigned int i;
  unsigned int oldsize = a->alimit;
  TValue *newarray;

  /* allocate new array */
  newarray = luaM_reallocvector(L, a->array, oldsize, newsize, TValue);
  if (newarray == NULL && newsize > 0) {  /* allocation failed? */
    luaM_error(L);  /* raise error (with array unchanged) */
  }

  /* allocation ok; initialize new part of the array */
  a->array = newarray;  /* set new array part */
  a->alimit = newsize;
  for (i = oldsize; i < newsize; i++)  /* clear new slice of the array */
    setempty(&a->array[i]);
}


void luaA_free (lua_State *L, Array *a) {
  luaM_freearray(L, a->array, a->alimit);
  luaM_free(L, a);
}


const TValue *luaA_getint (lua_State *L, Array *a, lua_Integer key) {
  /* (1 <= key && key <= t->alimit) */
  if (l_castS2U(key) - 1u < a->alimit)
    return &a->array[key - 1];
  else
    return luaO_nilobject;
}

const TValue *luaA_get (lua_State *L, Array *a, const TValue *key) {
  if (ttypetag(key) == LUA_VNUMINT) {
    lua_Integer ikey = ivalue(key);
    if (l_castS2U(ikey) - 1u < a->alimit)
      return &a->array[ikey - 1];
    else
      return luaO_nilobject;
  } else {
    return luaO_nilobject;
  }
}

void luaA_setint (lua_State *L, Array *a, lua_Integer key, TValue *value) {
  if (l_castS2U(key) - 1u < a->alimit) {
    /* set value! */
    TValue* val = &a->array[key - 1];
    val->value_ = value->value_;
    val->tt_ = value->tt_;
    checkliveness(L,val);
  } else {
    /* TODO: this error message could be improved! */    
    luaG_runerror(L, "array index out of bounds");
  }
}

void luaA_set (lua_State *L, Array *a, const TValue* key, TValue *value) {
  if (ttypetag(key) == LUA_VNUMINT) {
    lua_Integer ikey = ivalue(key);
    luaA_setint(L, a, ikey, value);
  } else {
    /* TODO: the error message could be improved */
    luaG_runerror(L, "attempt to index array with a non-integer value");
  }
}
