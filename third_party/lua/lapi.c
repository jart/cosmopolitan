/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╚──────────────────────────────────────────────────────────────────────────────╝
│                                                                              │
│  Lua                                                                         │
│  Copyright © 2004-2021 Lua.org, PUC-Rio.                                     │
│                                                                              │
│  Permission is hereby granted, free of charge, to any person obtaining       │
│  a copy of this software and associated documentation files (the             │
│  "Software"), to deal in the Software without restriction, including         │
│  without limitation the rights to use, copy, modify, merge, publish,         │
│  distribute, sublicense, and/or sell copies of the Software, and to          │
│  permit persons to whom the Software is furnished to do so, subject to       │
│  the following conditions:                                                   │
│                                                                              │
│  The above copyright notice and this permission notice shall be              │
│  included in all copies or substantial portions of the Software.             │
│                                                                              │
│  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,             │
│  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF          │
│  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.      │
│  IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY        │
│  CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,        │
│  TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE           │
│  SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                      │
│                                                                              │
╚─────────────────────────────────────────────────────────────────────────────*/
#define lapi_c
#define LUA_CORE
#include "third_party/lua/lapi.h"
#include "third_party/lua/ldebug.h"
#include "third_party/lua/ldo.h"
#include "third_party/lua/lfunc.h"
#include "third_party/lua/lgc.h"
#include "third_party/lua/lmem.h"
#include "third_party/lua/lobject.h"
#include "third_party/lua/lprefix.h"
#include "third_party/lua/lstate.h"
#include "third_party/lua/lstring.h"
#include "third_party/lua/ltable.h"
#include "third_party/lua/ltm.h"
#include "third_party/lua/lua.h"
#include "third_party/lua/lundump.h"
#include "third_party/lua/lvm.h"

asm(".ident\t\"\\n\\n\
Lua 5.4.3 (MIT License)\\n\
Copyright 1994–2021 Lua.org, PUC-Rio.\"");
asm(".include \"libc/disclaimer.inc\"");


const char lua_ident[] =
  "$LuaVersion: " LUA_COPYRIGHT " $"
  "$LuaAuthors: " LUA_AUTHORS " $";



/*
** Test for a valid index (one that is not the 'nilvalue').
** '!ttisnil(o)' implies 'o != &G(L)->nilvalue', so it is not needed.
** However, it covers the most common cases in a faster way.
*/
#define isvalid(L, o)	(!ttisnil(o) || o != &G(L)->nilvalue)


/* test for pseudo index */
#define ispseudo(i)		((i) <= LUA_REGISTRYINDEX)

/* test for upvalue */
#define isupvalue(i)		((i) < LUA_REGISTRYINDEX)


static TValue *index2value (lua_State *L, int idx) {
  CallInfo *ci = L->ci;
  if (idx > 0) {
    StkId o = ci->func + idx;
    api_check(L, idx <= L->ci->top - (ci->func + 1), "unacceptable index");
    if (o >= L->top) return &G(L)->nilvalue;
    else return s2v(o);
  }
  else if (!ispseudo(idx)) {  /* negative index */
    api_check(L, idx != 0 && -idx <= L->top - (ci->func + 1), "invalid index");
    return s2v(L->top + idx);
  }
  else if (idx == LUA_REGISTRYINDEX)
    return &G(L)->l_registry;
  else {  /* upvalues */
    idx = LUA_REGISTRYINDEX - idx;
    api_check(L, idx <= MAXUPVAL + 1, "upvalue index too large");
    if (ttislcf(s2v(ci->func)))  /* light C function? */
      return &G(L)->nilvalue;  /* it has no upvalues */
    else {
      CClosure *func = clCvalue(s2v(ci->func));
      return (idx <= func->nupvalues) ? &func->upvalue[idx-1]
                                      : &G(L)->nilvalue;
    }
  }
}


static StkId index2stack (lua_State *L, int idx) {
  CallInfo *ci = L->ci;
  if (idx > 0) {
    StkId o = ci->func + idx;
    api_check(L, o < L->top, "unacceptable index");
    return o;
  }
  else {    /* non-positive index */
    api_check(L, idx != 0 && -idx <= L->top - (ci->func + 1), "invalid index");
    api_check(L, !ispseudo(idx), "invalid index");
    return L->top + idx;
  }
}


LUA_API int lua_checkstack (lua_State *L, int n) {
  int res;
  CallInfo *ci;
  lua_lock(L);
  ci = L->ci;
  api_check(L, n >= 0, "negative 'n'");
  if (L->stack_last - L->top > n)  /* stack large enough? */
    res = 1;  /* yes; check is OK */
  else {  /* no; need to grow stack */
    int inuse = cast_int(L->top - L->stack) + EXTRA_STACK;
    if (inuse > LUAI_MAXSTACK - n)  /* can grow without overflow? */
      res = 0;  /* no */
    else  /* try to grow stack */
      res = luaD_growstack(L, n, 0);
  }
  if (res && ci->top < L->top + n)
    ci->top = L->top + n;  /* adjust frame top */
  lua_unlock(L);
  return res;
}

/**
 * Exchanges values between different threads of the same state.
 *
 * This funcetion pops n values from the stack from, and pushes them onto
 * the stack to.
 */
LUA_API void lua_xmove (lua_State *from, lua_State *to, int n) {
  int i;
  if (from == to) return;
  lua_lock(to);
  api_checknelems(from, n);
  api_check(from, G(from) == G(to), "moving among independent states");
  api_check(from, to->ci->top - to->top >= n, "stack overflow");
  from->top -= n;
  for (i = 0; i < n; i++) {
    setobjs2s(to, to->top, from->top + i);
    to->top++;  /* stack already checked by previous 'api_check' */
  }
  lua_unlock(to);
}

/**
 * lua_atpanic
 *
 * [-0, +0, –]
 *
 * lua_CFunction lua_atpanic (lua_State *L, lua_CFunction panicf);
 *
 * Sets a new panic function and returns the old one (see §4.4).
 */
LUA_API lua_CFunction lua_atpanic (lua_State *L, lua_CFunction panicf) {
  lua_CFunction old;
  lua_lock(L);
  old = G(L)->panic;
  G(L)->panic = panicf;
  lua_unlock(L);
  return old;
}

/**
 * lua_version
 *
 * [-0, +0, –]
 *
 * lua_Number lua_version (lua_State *L);
 *
 * Returns the version number of this core.
 */
LUA_API lua_Number lua_version (lua_State *L) {
  UNUSED(L);
  return LUA_VERSION_NUM;
}



/*
** basic stack manipulation
*/


/**
 * lua_absindex [-0, +0, –]
 *
 * Converts the acceptable index idx into an equivalent absolute index (that
 * is, one that does not depend on the stack size).
 */
LUA_API int lua_absindex (lua_State *L, int idx) {
  return (idx > 0 || ispseudo(idx))
         ? idx
         : cast_int(L->top - L->ci->func) + idx;
}

/**
 * lua_gettop [-0, +0, –]
 *
 * Returns the index of the top element in the stack. Because indices start
 * at 1, this result is equal to the number of elements in the stack; in
 * particular, 0 means an empty stack.
 */
LUA_API int lua_gettop (lua_State *L) {
  return cast_int(L->top - (L->ci->func + 1));
}

/**
 * lua_settop [-?, +?, e]
 *
 * Accepts any index, or 0, and sets the stack top to this index. If the new
 * top is greater than the old one, then the new elements are filled with
 * nil. If index is 0, then all stack elements are removed.
 *
 * This function can run arbitrary code when removing an index marked as
 * to-be-closed from the stack.
 */
LUA_API void lua_settop (lua_State *L, int idx) {
  CallInfo *ci;
  StkId func, newtop;
  ptrdiff_t diff;  /* difference for new top */
  lua_lock(L);
  ci = L->ci;
  func = ci->func;
  if (idx >= 0) {
    api_check(L, idx <= ci->top - (func + 1), "new top too large");
    diff = ((func + 1) + idx) - L->top;
    for (; diff > 0; diff--)
      setnilvalue(s2v(L->top++));  /* clear new slots */
  }
  else {
    api_check(L, -(idx+1) <= (L->top - (func + 1)), "invalid new top");
    diff = idx + 1;  /* will "subtract" index (as it is negative) */
  }
  api_check(L, L->tbclist < L->top, "previous pop of an unclosed slot");
  newtop = L->top + diff;
  if (diff < 0 && L->tbclist >= newtop) {
    lua_assert(hastocloseCfunc(ci->nresults));
    luaF_close(L, newtop, CLOSEKTOP, 0);
  }
  L->top = newtop;  /* correct top only after closing any upvalue */
  lua_unlock(L);
}


LUA_API void lua_closeslot (lua_State *L, int idx) {
  StkId level;
  lua_lock(L);
  level = index2stack(L, idx);
  api_check(L, hastocloseCfunc(L->ci->nresults) && L->tbclist == level,
     "no variable to close at given level");
  luaF_close(L, level, CLOSEKTOP, 0);
  level = index2stack(L, idx);  /* stack may be moved */
  setnilvalue(s2v(level));
  lua_unlock(L);
}


/*
** Reverse the stack segment from 'from' to 'to'
** (auxiliary to 'lua_rotate')
** Note that we move(copy) only the value inside the stack.
** (We do not move additional fields that may exist.)
*/
static void reverse (lua_State *L, StkId from, StkId to) {
  for (; from < to; from++, to--) {
    TValue temp;
    setobj(L, &temp, s2v(from));
    setobjs2s(L, from, to);
    setobj2s(L, to, &temp);
  }
}


/*
** Let x = AB, where A is a prefix of length 'n'. Then,
** rotate x n == BA. But BA == (A^r . B^r)^r.
*/
LUA_API void lua_rotate (lua_State *L, int idx, int n) {
  StkId p, t, m;
  lua_lock(L);
  t = L->top - 1;  /* end of stack segment being rotated */
  p = index2stack(L, idx);  /* start of segment */
  api_check(L, (n >= 0 ? n : -n) <= (t - p + 1), "invalid 'n'");
  m = (n >= 0 ? t - n : p - n - 1);  /* end of prefix */
  reverse(L, p, m);  /* reverse the prefix with length 'n' */
  reverse(L, m + 1, t);  /* reverse the suffix */
  reverse(L, p, t);  /* reverse the entire segment */
  lua_unlock(L);
}


LUA_API void lua_copy (lua_State *L, int fromidx, int toidx) {
  TValue *fr, *to;
  lua_lock(L);
  fr = index2value(L, fromidx);
  to = index2value(L, toidx);
  api_check(L, isvalid(L, to), "invalid index");
  setobj(L, to, fr);
  if (isupvalue(toidx))  /* function upvalue? */
    luaC_barrier(L, clCvalue(s2v(L->ci->func)), fr);
  /* LUA_REGISTRYINDEX does not need gc barrier
     (collector revisits it before finishing collection) */
  lua_unlock(L);
}


LUA_API void lua_pushvalue (lua_State *L, int idx) {
  lua_lock(L);
  setobj2s(L, L->top, index2value(L, idx));
  api_incr_top(L);
  lua_unlock(L);
}



/*
** access functions (stack -> C)
*/


LUA_API int lua_type (lua_State *L, int idx) {
  const TValue *o = index2value(L, idx);
  return (isvalid(L, o) ? ttype(o) : LUA_TNONE);
}


LUA_API const char *lua_typename (lua_State *L, int t) {
  UNUSED(L);
  api_check(L, LUA_TNONE <= t && t < LUA_NUMTYPES, "invalid type");
  return ttypename(t);
}


LUA_API int lua_iscfunction (lua_State *L, int idx) {
  const TValue *o = index2value(L, idx);
  return (ttislcf(o) || (ttisCclosure(o)));
}


LUA_API int lua_isinteger (lua_State *L, int idx) {
  const TValue *o = index2value(L, idx);
  return ttisinteger(o);
}


LUA_API int lua_isnumber (lua_State *L, int idx) {
  lua_Number n;
  const TValue *o = index2value(L, idx);
  return tonumber(o, &n);
}


LUA_API int lua_isstring (lua_State *L, int idx) {
  const TValue *o = index2value(L, idx);
  return (ttisstring(o) || cvt2str(o));
}


LUA_API int lua_isuserdata (lua_State *L, int idx) {
  const TValue *o = index2value(L, idx);
  return (ttisfulluserdata(o) || ttislightuserdata(o));
}


LUA_API int lua_rawequal (lua_State *L, int index1, int index2) {
  const TValue *o1 = index2value(L, index1);
  const TValue *o2 = index2value(L, index2);
  return (isvalid(L, o1) && isvalid(L, o2)) ? luaV_rawequalobj(o1, o2) : 0;
}


LUA_API void lua_arith (lua_State *L, int op) {
  lua_lock(L);
  if (op != LUA_OPUNM && op != LUA_OPBNOT)
    api_checknelems(L, 2);  /* all other operations expect two operands */
  else {  /* for unary operations, add fake 2nd operand */
    api_checknelems(L, 1);
    setobjs2s(L, L->top, L->top - 1);
    api_incr_top(L);
  }
  /* first operand at top - 2, second at top - 1; result go to top - 2 */
  luaO_arith(L, op, s2v(L->top - 2), s2v(L->top - 1), L->top - 2);
  L->top--;  /* remove second operand */
  lua_unlock(L);
}


LUA_API int lua_compare (lua_State *L, int index1, int index2, int op) {
  const TValue *o1;
  const TValue *o2;
  int i = 0;
  lua_lock(L);  /* may call tag method */
  o1 = index2value(L, index1);
  o2 = index2value(L, index2);
  if (isvalid(L, o1) && isvalid(L, o2)) {
    switch (op) {
      case LUA_OPEQ: i = luaV_equalobj(L, o1, o2); break;
      case LUA_OPLT: i = luaV_lessthan(L, o1, o2); break;
      case LUA_OPLE: i = luaV_lessequal(L, o1, o2); break;
      default: api_check(L, 0, "invalid option");
    }
  }
  lua_unlock(L);
  return i;
}


LUA_API size_t lua_stringtonumber (lua_State *L, const char *s) {
  size_t sz = luaO_str2num(s, s2v(L->top));
  if (sz != 0)
    api_incr_top(L);
  return sz;
}


LUA_API lua_Number lua_tonumberx (lua_State *L, int idx, int *pisnum) {
  lua_Number n = 0;
  const TValue *o = index2value(L, idx);
  int isnum = tonumber(o, &n);
  if (pisnum)
    *pisnum = isnum;
  return n;
}


LUA_API lua_Integer lua_tointegerx (lua_State *L, int idx, int *pisnum) {
  lua_Integer res = 0;
  const TValue *o = index2value(L, idx);
  int isnum = tointeger(o, &res);
  if (pisnum)
    *pisnum = isnum;
  return res;
}


LUA_API int lua_toboolean (lua_State *L, int idx) {
  const TValue *o = index2value(L, idx);
  return !l_isfalse(o);
}


LUA_API const char *lua_tolstring (lua_State *L, int idx, size_t *len) {
  TValue *o;
  lua_lock(L);
  o = index2value(L, idx);
  if (!ttisstring(o)) {
    if (!cvt2str(o)) {  /* not convertible? */
      if (len != NULL) *len = 0;
      lua_unlock(L);
      return NULL;
    }
    luaO_tostring(L, o);
    luaC_checkGC(L);
    o = index2value(L, idx);  /* previous call may reallocate the stack */
  }
  if (len != NULL)
    *len = vslen(o);
  lua_unlock(L);
  return svalue(o);
}


LUA_API lua_Unsigned lua_rawlen (lua_State *L, int idx) {
  const TValue *o = index2value(L, idx);
  switch (ttypetag(o)) {
    case LUA_VSHRSTR: return tsvalue(o)->shrlen;
    case LUA_VLNGSTR: return tsvalue(o)->u.lnglen;
    case LUA_VUSERDATA: return uvalue(o)->len;
    case LUA_VTABLE: return luaH_getn(hvalue(o));
    default: return 0;
  }
}


LUA_API lua_CFunction lua_tocfunction (lua_State *L, int idx) {
  const TValue *o = index2value(L, idx);
  if (ttislcf(o)) return fvalue(o);
  else if (ttisCclosure(o))
    return clCvalue(o)->f;
  else return NULL;  /* not a C function */
}


static void *touserdata (const TValue *o) {
  switch (ttype(o)) {
    case LUA_TUSERDATA: return getudatamem(uvalue(o));
    case LUA_TLIGHTUSERDATA: return pvalue(o);
    default: return NULL;
  }
}


/**
 * lua_touserdata [-0, +0, –]
 *
 * If the value at the given index is a full userdata, returns its
 * memory-block address. If the value is a light userdata, returns its value
 * (a pointer). Otherwise, returns NULL.
 */
LUA_API void *lua_touserdata (lua_State *L, int idx) {
  const TValue *o = index2value(L, idx);
  return touserdata(o);
}


/**
 * lua_tothread [-0, +0, –]
 *
 * Converts the value at the given index to a Lua thread (represented as
 * lua_State*). This value must be a thread; otherwise, the function returns
 * NULL.
 */
LUA_API lua_State *lua_tothread (lua_State *L, int idx) {
  const TValue *o = index2value(L, idx);
  return (!ttisthread(o)) ? NULL : thvalue(o);
}


/**
 * lua_topointer [-0, +0, –]
 *
 * Converts the value at the given index to a generic C pointer (void*). The
 * value can be a userdata, a table, a thread, a string, or a function;
 * otherwise, lua_topointer returns NULL. Different objects will give
 * different pointers. There is no way to convert the pointer back to its
 * original value.
 *
 * Typically this function is used only for hashing and debug information.
 */
LUA_API const void *lua_topointer (lua_State *L, int idx) {
  /*
  ** Returns a pointer to the internal representation of an object.
  ** Note that ANSI C does not allow the conversion of a pointer to
  ** function to a 'void*', so the conversion here goes through
  ** a 'size_t'. (As the returned pointer is only informative, this
  ** conversion should not be a problem.)
  */
  const TValue *o = index2value(L, idx);
  switch (ttypetag(o)) {
    case LUA_VLCF: return cast_voidp(cast_sizet(fvalue(o)));
    case LUA_VUSERDATA: case LUA_VLIGHTUSERDATA:
      return touserdata(o);
    default: {
      if (iscollectable(o))
        return gcvalue(o);
      else
        return NULL;
    }
  }
}



/*
** push functions (C -> stack)
*/


/**
 * lua_pushnil [-0, +1, –]
 *
 * Pushes a nil value onto the stack.
 */
LUA_API void lua_pushnil (lua_State *L) {
  lua_lock(L);
  setnilvalue(s2v(L->top));
  api_incr_top(L);
  lua_unlock(L);
}


/**
 * lua_pushnumber [-0, +1, –]
 *
 * Pushes a float with value n onto the stack.
 */
LUA_API void lua_pushnumber (lua_State *L, lua_Number n) {
  lua_lock(L);
  setfltvalue(s2v(L->top), n);
  api_incr_top(L);
  lua_unlock(L);
}


/**
 * lua_pushinteger [-0, +1, –]
 *
 * void lua_pushinteger (lua_State *L, lua_Integer n);
 *
 * Pushes an integer with value n onto the stack.
 */
LUA_API void lua_pushinteger (lua_State *L, lua_Integer n) {
  lua_lock(L);
  setivalue(s2v(L->top), n);
  api_incr_top(L);
  lua_unlock(L);
}


/*
** Pushes on the stack a string with given length. Avoid using 's' when
** 'len' == 0 (as 's' can be NULL in that case), due to later use of
** 'memcmp' and 'memcpy'.
*/
LUA_API const char *lua_pushlstring (lua_State *L, const char *s, size_t len) {
  TString *ts;
  lua_lock(L);
  ts = (len == 0) ? luaS_new(L, "") : luaS_newlstr(L, s, len);
  setsvalue2s(L, L->top, ts);
  api_incr_top(L);
  luaC_checkGC(L);
  lua_unlock(L);
  return getstr(ts);
}


/**
 * lua_pushstring [-0, +1, m]
 *
 * Pushes the zero-terminated string pointed to by s onto the stack. Lua will
 * make or reuse an internal copy of the given string, so the memory at s can
 * be freed or reused immediately after the function returns.
 *
 * Returns a pointer to the internal copy of the string (see §4.1.3).
 *
 * If s is NULL, pushes nil and returns NULL.
 */
LUA_API const char *lua_pushstring (lua_State *L, const char *s) {
  lua_lock(L);
  if (s == NULL)
    setnilvalue(s2v(L->top));
  else {
    TString *ts;
    ts = luaS_new(L, s);
    setsvalue2s(L, L->top, ts);
    s = getstr(ts);  /* internal copy's address */
  }
  api_incr_top(L);
  luaC_checkGC(L);
  lua_unlock(L);
  return s;
}


/**
 * lua_pushvfstring [-0, +1, v]
 *
 * Equivalent to lua_pushfstring, except that it receives a va_list instead
 * of a variable number of arguments.
 */
LUA_API const char *lua_pushvfstring (lua_State *L, const char *fmt,
                                      va_list argp) {
  const char *ret;
  lua_lock(L);
  ret = luaO_pushvfstring(L, fmt, argp);
  luaC_checkGC(L);
  lua_unlock(L);
  return ret;
}


/**
 * lua_pushfstring [-0, +1, v]
 *
 * Pushes onto the stack a formatted string and returns a pointer to this
 * string (see §4.1.3). It is similar to the ISO C function sprintf, but has
 * two important differences. First, you do not have to allocate space for
 * the result; the result is a Lua string and Lua takes care of memory
 * allocation (and deallocation, through garbage collection). Second, the
 * conversion specifiers are quite restricted. There are no flags, widths, or
 * precisions. The conversion specifiers can only be '%%' (inserts the
 * character '%'), '%s' (inserts a zero-terminated string, with no size
 * restrictions), '%f' (inserts a lua_Number), '%I' (inserts a lua_Integer),
 * '%p' (inserts a pointer), '%d' (inserts an int), '%c' (inserts an int as a
 * one-byte character), and '%U' (inserts a long int as a UTF-8 byte
 * sequence).
 *
 * This function may raise errors due to memory overflow or an invalid
 * conversion specifier.
 */
LUA_API const char *lua_pushfstring (lua_State *L, const char *fmt, ...) {
  const char *ret;
  va_list argp;
  lua_lock(L);
  va_start(argp, fmt);
  ret = luaO_pushvfstring(L, fmt, argp);
  va_end(argp);
  luaC_checkGC(L);
  lua_unlock(L);
  return ret;
}


/**
 * lua_pushcclosure [-n, +1, m]
 *
 * Pushes a new C closure onto the stack. This function receives a pointer to
 * a C function and pushes onto the stack a Lua value of type function that,
 * when called, invokes the corresponding C function. The parameter n tells
 * how many upvalues this function will have (see §4.2).
 *
 * Any function to be callable by Lua must follow the correct protocol to
 * receive its parameters and return its results (see lua_CFunction).
 *
 * When a C function is created, it is possible to associate some values with
 * it, the so called upvalues; these upvalues are then accessible to the
 * function whenever it is called. This association is called a C closure
 * (see §4.2). To create a C closure, first the initial values for its
 * upvalues must be pushed onto the stack. (When there are multiple upvalues,
 * the first value is pushed first.) Then lua_pushcclosure is called to
 * create and push the C function onto the stack, with the argument n telling
 * how many values will be associated with the function. lua_pushcclosure
 * also pops these values from the stack.
 *
 * The maximum value for n is 255.
 *
 * When n is zero, this function creates a light C function, which is just a
 * pointer to the C function. In that case, it never raises a memory error.
 */
LUA_API void lua_pushcclosure (lua_State *L, lua_CFunction fn, int n) {
  lua_lock(L);
  if (n == 0) {
    setfvalue(s2v(L->top), fn);
    api_incr_top(L);
  }
  else {
    CClosure *cl;
    api_checknelems(L, n);
    api_check(L, n <= MAXUPVAL, "upvalue index too large");
    cl = luaF_newCclosure(L, n);
    cl->f = fn;
    L->top -= n;
    while (n--) {
      setobj2n(L, &cl->upvalue[n], s2v(L->top + n));
      /* does not need barrier because closure is white */
      lua_assert(iswhite(cl));
    }
    setclCvalue(L, s2v(L->top), cl);
    api_incr_top(L);
    luaC_checkGC(L);
  }
  lua_unlock(L);
}


/**
 * lua_pushboolean [-0, +1, –]
 *
 * Pushes a boolean value with value b onto the stack.
 */
LUA_API void lua_pushboolean (lua_State *L, int b) {
  lua_lock(L);
  /* a.k.a. L->top->val.tt_ = b ? LUA_VTRUE : LUA_VFALSE; */
  if (b)
    setbtvalue(s2v(L->top));
  else
    setbfvalue(s2v(L->top));
  api_incr_top(L);
  lua_unlock(L);
}


/**
 * lua_pushlightuserdata [-0, +1, –]
 *
 * Pushes a light userdata onto the stack.
 *
 * Userdata represent C values in Lua. A light userdata represents a pointer,
 * a void*. It is a value (like a number): you do not create it, it has no
 * individual metatable, and it is not collected (as it was never created). A
 * light userdata is equal to "any" light userdata with the same C address.
 */
LUA_API void lua_pushlightuserdata (lua_State *L, void *p) {
  lua_lock(L);
  setpvalue(s2v(L->top), p);
  api_incr_top(L);
  lua_unlock(L);
}


/**
 * lua_pushthread [-0, +1, –]
 *
 * Pushes the thread represented by L onto the stack. Returns 1 if this
 * thread is the main thread of its state.
 */
LUA_API int lua_pushthread (lua_State *L) {
  lua_lock(L);
  setthvalue(L, s2v(L->top), L);
  api_incr_top(L);
  lua_unlock(L);
  return (G(L)->mainthread == L);
}



/*
** get functions (Lua -> stack)
*/


static int auxgetstr (lua_State *L, const TValue *t, const char *k) {
  const TValue *slot;
  TString *str = luaS_new(L, k);
  if (luaV_fastget(L, t, str, slot, luaH_getstr)) {
    setobj2s(L, L->top, slot);
    api_incr_top(L);
  }
  else {
    setsvalue2s(L, L->top, str);
    api_incr_top(L);
    luaV_finishget(L, t, s2v(L->top - 1), L->top - 1, slot);
  }
  lua_unlock(L);
  return ttype(s2v(L->top - 1));
}


/*
** Get the global table in the registry. Since all predefined
** indices in the registry were inserted right when the registry
** was created and never removed, they must always be in the array
** part of the registry.
*/
#define getGtable(L)  \
	(&hvalue(&G(L)->l_registry)->array[LUA_RIDX_GLOBALS - 1])


LUA_API int lua_getglobal (lua_State *L, const char *name) {
  const TValue *G;
  lua_lock(L);
  G = getGtable(L);
  return auxgetstr(L, G, name);
}


LUA_API int lua_gettable (lua_State *L, int idx) {
  const TValue *slot;
  TValue *t;
  lua_lock(L);
  t = index2value(L, idx);
  if (luaV_fastget(L, t, s2v(L->top - 1), slot, luaH_get)) {
    setobj2s(L, L->top - 1, slot);
  }
  else
    luaV_finishget(L, t, s2v(L->top - 1), L->top - 1, slot);
  lua_unlock(L);
  return ttype(s2v(L->top - 1));
}


LUA_API int lua_getfield (lua_State *L, int idx, const char *k) {
  lua_lock(L);
  return auxgetstr(L, index2value(L, idx), k);
}


LUA_API int lua_geti (lua_State *L, int idx, lua_Integer n) {
  TValue *t;
  const TValue *slot;
  lua_lock(L);
  t = index2value(L, idx);
  if (luaV_fastgeti(L, t, n, slot)) {
    setobj2s(L, L->top, slot);
  }
  else {
    TValue aux;
    setivalue(&aux, n);
    luaV_finishget(L, t, &aux, L->top, slot);
  }
  api_incr_top(L);
  lua_unlock(L);
  return ttype(s2v(L->top - 1));
}


static int finishrawget (lua_State *L, const TValue *val) {
  if (isempty(val))  /* avoid copying empty items to the stack */
    setnilvalue(s2v(L->top));
  else
    setobj2s(L, L->top, val);
  api_incr_top(L);
  lua_unlock(L);
  return ttype(s2v(L->top - 1));
}


static Table *gettable (lua_State *L, int idx) {
  TValue *t = index2value(L, idx);
  api_check(L, ttistable(t), "table expected");
  return hvalue(t);
}


/**
 * lua_rawget [-1, +1, –]
 *
 * Similar to lua_gettable, but does a raw access (i.e., without
 * metamethods).
 */
LUA_API int lua_rawget (lua_State *L, int idx) {
  Table *t;
  const TValue *val;
  lua_lock(L);
  api_checknelems(L, 1);
  t = gettable(L, idx);
  val = luaH_get(t, s2v(L->top - 1));
  L->top--;  /* remove key */
  return finishrawget(L, val);
}


LUA_API int lua_rawgeti (lua_State *L, int idx, lua_Integer n) {
  Table *t;
  lua_lock(L);
  t = gettable(L, idx);
  return finishrawget(L, luaH_getint(t, n));
}


/**
 * lua_rawgetp [-0, +1, –]
 *
 * Pushes onto the stack the value t[k], where t is the table at the given
 * index and k is the pointer p represented as a light userdata. The access
 * is raw; that is, it does not use the __index metavalue.
 *
 * Returns the type of the pushed value.
 */
LUA_API int lua_rawgetp (lua_State *L, int idx, const void *p) {
  Table *t;
  TValue k;
  lua_lock(L);
  t = gettable(L, idx);
  setpvalue(&k, cast_voidp(p));
  return finishrawget(L, luaH_get(t, &k));
}


/**
 * lua_createtable [-0, +1, m]
 *
 * Creates a new empty table and pushes it onto the stack. Parameter narr is
 * a hint for how many elements the table will have as a sequence; parameter
 * nrec is a hint for how many other elements the table will have. Lua may
 * use these hints to preallocate memory for the new table. This
 * preallocation may help performance when you know in advance how many
 * elements the table will have. Otherwise you can use the function
 * lua_newtable.
 */
LUA_API void lua_createtable (lua_State *L, int narray, int nrec) {
  Table *t;
  lua_lock(L);
  t = luaH_new(L);
  sethvalue2s(L, L->top, t);
  api_incr_top(L);
  if (narray > 0 || nrec > 0)
    luaH_resize(L, t, narray, nrec);
  luaC_checkGC(L);
  lua_unlock(L);
}


/**
 * lua_getmetatable [-0, +(0|1), –]
 *
 * int lua_getmetatable (lua_State *L, int index);
 *
 * If the value at the given index has a metatable, the function pushes that
 * metatable onto the stack and returns 1. Otherwise, the function returns 0
 * and pushes nothing on the stack.
 */
LUA_API int lua_getmetatable (lua_State *L, int objindex) {
  const TValue *obj;
  Table *mt;
  int res = 0;
  lua_lock(L);
  obj = index2value(L, objindex);
  switch (ttype(obj)) {
    case LUA_TTABLE:
      mt = hvalue(obj)->metatable;
      break;
    case LUA_TUSERDATA:
      mt = uvalue(obj)->metatable;
      break;
    default:
      mt = G(L)->mt[ttype(obj)];
      break;
  }
  if (mt != NULL) {
    sethvalue2s(L, L->top, mt);
    api_incr_top(L);
    res = 1;
  }
  lua_unlock(L);
  return res;
}


/**
 * lua_getiuservalue [-0, +1, –]
 *
 * int lua_getiuservalue (lua_State *L, int index, int n);
 *
 * Pushes onto the stack the n-th user value associated with the full
 * userdata at the given index and returns the type of the pushed value.
 *
 * If the userdata does not have that value, pushes nil and returns
 * LUA_TNONE.
 */
LUA_API int lua_getiuservalue (lua_State *L, int idx, int n) {
  TValue *o;
  int t;
  lua_lock(L);
  o = index2value(L, idx);
  api_check(L, ttisfulluserdata(o), "full userdata expected");
  if (n <= 0 || n > uvalue(o)->nuvalue) {
    setnilvalue(s2v(L->top));
    t = LUA_TNONE;
  }
  else {
    setobj2s(L, L->top, &uvalue(o)->uv[n - 1].uv);
    t = ttype(s2v(L->top));
  }
  api_incr_top(L);
  lua_unlock(L);
  return t;
}


/*
** set functions (stack -> Lua)
*/

/*
** t[k] = value at the top of the stack (where 'k' is a string)
*/
static void auxsetstr (lua_State *L, const TValue *t, const char *k) {
  const TValue *slot;
  TString *str = luaS_new(L, k);
  api_checknelems(L, 1);
  if (luaV_fastget(L, t, str, slot, luaH_getstr)) {
    luaV_finishfastset(L, t, slot, s2v(L->top - 1));
    L->top--;  /* pop value */
  }
  else {
    setsvalue2s(L, L->top, str);  /* push 'str' (to make it a TValue) */
    api_incr_top(L);
    luaV_finishset(L, t, s2v(L->top - 1), s2v(L->top - 2), slot);
    L->top -= 2;  /* pop value and key */
  }
  lua_unlock(L);  /* lock done by caller */
}


LUA_API void lua_setglobal (lua_State *L, const char *name) {
  const TValue *G;
  lua_lock(L);  /* unlock done in 'auxsetstr' */
  G = getGtable(L);
  auxsetstr(L, G, name);
}


LUA_API void lua_settable (lua_State *L, int idx) {
  TValue *t;
  const TValue *slot;
  lua_lock(L);
  api_checknelems(L, 2);
  t = index2value(L, idx);
  if (luaV_fastget(L, t, s2v(L->top - 2), slot, luaH_get)) {
    luaV_finishfastset(L, t, slot, s2v(L->top - 1));
  }
  else
    luaV_finishset(L, t, s2v(L->top - 2), s2v(L->top - 1), slot);
  L->top -= 2;  /* pop index and value */
  lua_unlock(L);
}


LUA_API void lua_setfield (lua_State *L, int idx, const char *k) {
  lua_lock(L);  /* unlock done in 'auxsetstr' */
  auxsetstr(L, index2value(L, idx), k);
}


/**
 * lua_seti [-1, +0, e]
 *
 * Does the equivalent to t[n] = v, where t is the value at the given index
 * and v is the value on the top of the stack.
 *
 * This function pops the value from the stack. As in Lua, this function may
 * trigger a metamethod for the "newindex" event (see §2.4).
 */
LUA_API void lua_seti (lua_State *L, int idx, lua_Integer n) {
  TValue *t;
  const TValue *slot;
  lua_lock(L);
  api_checknelems(L, 1);
  t = index2value(L, idx);
  if (luaV_fastgeti(L, t, n, slot)) {
    luaV_finishfastset(L, t, slot, s2v(L->top - 1));
  }
  else {
    TValue aux;
    setivalue(&aux, n);
    luaV_finishset(L, t, &aux, s2v(L->top - 1), slot);
  }
  L->top--;  /* pop value */
  lua_unlock(L);
}


static void aux_rawset (lua_State *L, int idx, TValue *key, int n) {
  Table *t;
  lua_lock(L);
  api_checknelems(L, n);
  t = gettable(L, idx);
  luaH_set(L, t, key, s2v(L->top - 1));
  invalidateTMcache(t);
  luaC_barrierback(L, obj2gco(t), s2v(L->top - 1));
  L->top -= n;
  lua_unlock(L);
}


/**
 * lua_rawset [-2, +0, m]
 *
 * Similar to lua_settable, but does a raw assignment (i.e., without
 * metamethods).
 */
LUA_API void lua_rawset (lua_State *L, int idx) {
  aux_rawset(L, idx, s2v(L->top - 2), 2);
}


/**
 * lua_rawsetp [-1, +0, m]
 *
 * Does the equivalent of t[p] = v, where t is the table at the given index,
 * p is encoded as a light userdata, and v is the value on the top of the
 * stack.
 *
 * This function pops the value from the stack. The assignment is raw, that
 * is, it does not use the __newindex metavalue.
 */
LUA_API void lua_rawsetp (lua_State *L, int idx, const void *p) {
  TValue k;
  setpvalue(&k, cast_voidp(p));
  aux_rawset(L, idx, &k, 1);
}


/**
 * lua_rawseti [-1, +0, m]
 *
 * Does the equivalent of t[i] = v, where t is the table at the given index
 * and v is the value on the top of the stack.
 *
 * This function pops the value from the stack. The assignment is raw, that
 * is, it does not use the __newindex metavalue.
 */
LUA_API void lua_rawseti (lua_State *L, int idx, lua_Integer n) {
  Table *t;
  lua_lock(L);
  api_checknelems(L, 1);
  t = gettable(L, idx);
  luaH_setint(L, t, n, s2v(L->top - 1));
  luaC_barrierback(L, obj2gco(t), s2v(L->top - 1));
  L->top--;
  lua_unlock(L);
}


/**
 * lua_setmetatable [-1, +0, –]
 *
 * Pops a table or nil from the stack and sets that value as the new
 * metatable for the value at the given index. (nil means no metatable.)
 *
 * (For historical reasons, this function returns an int, which now is always
 * 1.)
 */
LUA_API int lua_setmetatable (lua_State *L, int objindex) {
  TValue *obj;
  Table *mt;
  lua_lock(L);
  api_checknelems(L, 1);
  obj = index2value(L, objindex);
  if (ttisnil(s2v(L->top - 1)))
    mt = NULL;
  else {
    api_check(L, ttistable(s2v(L->top - 1)), "table expected");
    mt = hvalue(s2v(L->top - 1));
  }
  switch (ttype(obj)) {
    case LUA_TTABLE: {
      hvalue(obj)->metatable = mt;
      if (mt) {
        luaC_objbarrier(L, gcvalue(obj), mt);
        luaC_checkfinalizer(L, gcvalue(obj), mt);
      }
      break;
    }
    case LUA_TUSERDATA: {
      uvalue(obj)->metatable = mt;
      if (mt) {
        luaC_objbarrier(L, uvalue(obj), mt);
        luaC_checkfinalizer(L, gcvalue(obj), mt);
      }
      break;
    }
    default: {
      G(L)->mt[ttype(obj)] = mt;
      break;
    }
  }
  L->top--;
  lua_unlock(L);
  return 1;
}


/**
 * lua_setiuservalue [-1, +0, –]
 *
 * Pops a value from the stack and sets it as the new n-th user value
 * associated to the full userdata at the given index. Returns 0 if the
 * userdata does not have that value.
 */
LUA_API int lua_setiuservalue (lua_State *L, int idx, int n) {
  TValue *o;
  int res;
  lua_lock(L);
  api_checknelems(L, 1);
  o = index2value(L, idx);
  api_check(L, ttisfulluserdata(o), "full userdata expected");
  if (!(cast_uint(n) - 1u < cast_uint(uvalue(o)->nuvalue)))
    res = 0;  /* 'n' not in [1, uvalue(o)->nuvalue] */
  else {
    setobj(L, &uvalue(o)->uv[n - 1].uv, s2v(L->top - 1));
    luaC_barrierback(L, gcvalue(o), s2v(L->top - 1));
    res = 1;
  }
  L->top--;
  lua_unlock(L);
  return res;
}


/*
** 'load' and 'call' functions (run Lua code)
*/


#define checkresults(L,na,nr) \
     api_check(L, (nr) == LUA_MULTRET || (L->ci->top - L->top >= (nr) - (na)), \
	"results from function overflow current stack size")


LUA_API void lua_callk (lua_State *L, int nargs, int nresults,
                        lua_KContext ctx, lua_KFunction k) {
  StkId func;
  lua_lock(L);
  api_check(L, k == NULL || !isLua(L->ci),
    "cannot use continuations inside hooks");
  api_checknelems(L, nargs+1);
  api_check(L, L->status == LUA_OK, "cannot do calls on non-normal thread");
  checkresults(L, nargs, nresults);
  func = L->top - (nargs+1);
  if (k != NULL && yieldable(L)) {  /* need to prepare continuation? */
    L->ci->u.c.k = k;  /* save continuation */
    L->ci->u.c.ctx = ctx;  /* save context */
    luaD_call(L, func, nresults);  /* do the call */
  }
  else  /* no continuation or no yieldable */
    luaD_callnoyield(L, func, nresults);  /* just do the call */
  adjustresults(L, nresults);
  lua_unlock(L);
}



/*
** Execute a protected call.
*/
struct CallS {  /* data to 'f_call' */
  StkId func;
  int nresults;
};


static void f_call (lua_State *L, void *ud) {
  struct CallS *c = cast(struct CallS *, ud);
  luaD_callnoyield(L, c->func, c->nresults);
}



LUA_API int lua_pcallk (lua_State *L, int nargs, int nresults, int errfunc,
                        lua_KContext ctx, lua_KFunction k) {
  struct CallS c;
  int status;
  ptrdiff_t func;
  lua_lock(L);
  api_check(L, k == NULL || !isLua(L->ci),
    "cannot use continuations inside hooks");
  api_checknelems(L, nargs+1);
  api_check(L, L->status == LUA_OK, "cannot do calls on non-normal thread");
  checkresults(L, nargs, nresults);
  if (errfunc == 0)
    func = 0;
  else {
    StkId o = index2stack(L, errfunc);
    api_check(L, ttisfunction(s2v(o)), "error handler must be a function");
    func = savestack(L, o);
  }
  c.func = L->top - (nargs+1);  /* function to be called */
  if (k == NULL || !yieldable(L)) {  /* no continuation or no yieldable? */
    c.nresults = nresults;  /* do a 'conventional' protected call */
    status = luaD_pcall(L, f_call, &c, savestack(L, c.func), func);
  }
  else {  /* prepare continuation (call is already protected by 'resume') */
    CallInfo *ci = L->ci;
    ci->u.c.k = k;  /* save continuation */
    ci->u.c.ctx = ctx;  /* save context */
    /* save information for error recovery */
    ci->u2.funcidx = cast_int(savestack(L, c.func));
    ci->u.c.old_errfunc = L->errfunc;
    L->errfunc = func;
    setoah(ci->callstatus, L->allowhook);  /* save value of 'allowhook' */
    ci->callstatus |= CIST_YPCALL;  /* function can do error recovery */
    luaD_call(L, c.func, nresults);  /* do the call */
    ci->callstatus &= ~CIST_YPCALL;
    L->errfunc = ci->u.c.old_errfunc;
    status = LUA_OK;  /* if it is here, there were no errors */
  }
  adjustresults(L, nresults);
  lua_unlock(L);
  return status;
}


LUA_API int lua_load (lua_State *L, lua_Reader reader, void *data,
                      const char *chunkname, const char *mode) {
  ZIO z;
  int status;
  lua_lock(L);
  if (!chunkname) chunkname = "?";
  luaZ_init(L, &z, reader, data);
  status = luaD_protectedparser(L, &z, chunkname, mode);
  if (status == LUA_OK) {  /* no errors? */
    LClosure *f = clLvalue(s2v(L->top - 1));  /* get newly created function */
    if (f->nupvalues >= 1) {  /* does it have an upvalue? */
      /* get global table from registry */
      const TValue *gt = getGtable(L);
      /* set global table as 1st upvalue of 'f' (may be LUA_ENV) */
      setobj(L, f->upvals[0]->v, gt);
      luaC_barrier(L, f->upvals[0], gt);
    }
  }
  lua_unlock(L);
  return status;
}


LUA_API int lua_dump (lua_State *L, lua_Writer writer, void *data, int strip) {
  int status;
  TValue *o;
  lua_lock(L);
  api_checknelems(L, 1);
  o = s2v(L->top - 1);
  if (isLfunction(o))
    status = luaU_dump(L, getproto(o), writer, data, strip);
  else
    status = 1;
  lua_unlock(L);
  return status;
}


LUA_API int lua_status (lua_State *L) {
  return L->status;
}


/*
** Garbage-collection function
*/
LUA_API int lua_gc (lua_State *L, int what, ...) {
  va_list argp;
  int res = 0;
  global_State *g;
  lua_lock(L);
  g = G(L);
  va_start(argp, what);
  switch (what) {
    case LUA_GCSTOP: {
      g->gcrunning = 0;
      break;
    }
    case LUA_GCRESTART: {
      luaE_setdebt(g, 0);
      g->gcrunning = 1;
      break;
    }
    case LUA_GCCOLLECT: {
      luaC_fullgc(L, 0);
      break;
    }
    case LUA_GCCOUNT: {
      /* GC values are expressed in Kbytes: #bytes/2^10 */
      res = cast_int(gettotalbytes(g) >> 10);
      break;
    }
    case LUA_GCCOUNTB: {
      res = cast_int(gettotalbytes(g) & 0x3ff);
      break;
    }
    case LUA_GCSTEP: {
      int data = va_arg(argp, int);
      l_mem debt = 1;  /* =1 to signal that it did an actual step */
      lu_byte oldrunning = g->gcrunning;
      g->gcrunning = 1;  /* allow GC to run */
      if (data == 0) {
        luaE_setdebt(g, 0);  /* do a basic step */
        luaC_step(L);
      }
      else {  /* add 'data' to total debt */
        debt = cast(l_mem, data) * 1024 + g->GCdebt;
        luaE_setdebt(g, debt);
        luaC_checkGC(L);
      }
      g->gcrunning = oldrunning;  /* restore previous state */
      if (debt > 0 && g->gcstate == GCSpause)  /* end of cycle? */
        res = 1;  /* signal it */
      break;
    }
    case LUA_GCSETPAUSE: {
      int data = va_arg(argp, int);
      res = getgcparam(g->gcpause);
      setgcparam(g->gcpause, data);
      break;
    }
    case LUA_GCSETSTEPMUL: {
      int data = va_arg(argp, int);
      res = getgcparam(g->gcstepmul);
      setgcparam(g->gcstepmul, data);
      break;
    }
    case LUA_GCISRUNNING: {
      res = g->gcrunning;
      break;
    }
    case LUA_GCGEN: {
      int minormul = va_arg(argp, int);
      int majormul = va_arg(argp, int);
      res = isdecGCmodegen(g) ? LUA_GCGEN : LUA_GCINC;
      if (minormul != 0)
        g->genminormul = minormul;
      if (majormul != 0)
        setgcparam(g->genmajormul, majormul);
      luaC_changemode(L, KGC_GEN);
      break;
    }
    case LUA_GCINC: {
      int pause = va_arg(argp, int);
      int stepmul = va_arg(argp, int);
      int stepsize = va_arg(argp, int);
      res = isdecGCmodegen(g) ? LUA_GCGEN : LUA_GCINC;
      if (pause != 0)
        setgcparam(g->gcpause, pause);
      if (stepmul != 0)
        setgcparam(g->gcstepmul, stepmul);
      if (stepsize != 0)
        g->gcstepsize = stepsize;
      luaC_changemode(L, KGC_INC);
      break;
    }
    default: res = -1;  /* invalid option */
  }
  va_end(argp);
  lua_unlock(L);
  return res;
}



/*
** miscellaneous functions
*/


/**
 * lua_error [-1, +0, v]
 *
 * Raises a Lua error, using the value on the top of the stack as the error
 * object. This function does a long jump, and therefore never returns (see
 * luaL_error).
 */
LUA_API int lua_error (lua_State *L) {
  TValue *errobj;
  lua_lock(L);
  errobj = s2v(L->top - 1);
  api_checknelems(L, 1);
  /* error object is the memory error message? */
  if (ttisshrstring(errobj) && eqshrstr(tsvalue(errobj), G(L)->memerrmsg))
    luaM_error(L);  /* raise a memory error */
  else
    luaG_errormsg(L);  /* raise a regular error */
  /* code unreachable; will unlock when control actually leaves the kernel */
  __builtin_unreachable();
}


/**
 * lua_next [-1, +(2|0), v]
 *
 * Pops a key from the stack, and pushes a key–value pair from the table at
 * the given index, the "next" pair after the given key. If there are no more
 * elements in the table, then lua_next returns 0 and pushes nothing.
 *
 * A typical table traversal looks like this:
 *
 *     // table is in the stack at index 't'
 *     lua_pushnil(L);  // first key
 *     while (lua_next(L, t) != 0) {
 *       // uses 'key' (at index -2) and 'value' (at index -1)
 *       printf("%s - %s\n",
 *       lua_typename(L, lua_type(L, -2)),
 *       lua_typename(L, lua_type(L, -1)));
 *       // removes 'value'; keeps 'key' for next iteration
 *       lua_pop(L, 1);
 *     }
 *
 * While traversing a table, avoid calling lua_tolstring directly on a key,
 * unless you know that the key is actually a string. Recall that
 * lua_tolstring may change the value at the given index; this confuses the
 * next call to lua_next.
 *
 * This function may raise an error if the given key is neither nil nor
 * present in the table. See function next for the caveats of modifying the
 * table during its traversal.
 */
LUA_API int lua_next (lua_State *L, int idx) {
  Table *t;
  int more;
  lua_lock(L);
  api_checknelems(L, 1);
  t = gettable(L, idx);
  more = luaH_next(L, t, L->top - 1);
  if (more) {
    api_incr_top(L);
  }
  else  /* no more elements */
    L->top -= 1;  /* remove key */
  lua_unlock(L);
  return more;
}


/**
 * lua_toclose [-0, +0, m]
 *
 * Marks the given index in the stack as a to-be-closed slot (see §3.3.8).
 * Like a to-be-closed variable in Lua, the value at that slot in the stack
 * will be closed when it goes out of scope. Here, in the context of a C
 * function, to go out of scope means that the running function returns to
 * Lua, or there is an error, or the slot is removed from the stack through
 * lua_settop or lua_pop, or there is a call to lua_closeslot. A slot marked
 * as to-be-closed should not be removed from the stack by any other function
 * in the API except lua_settop or lua_pop, unless previously deactivated by
 * lua_closeslot.
 *
 * This function should not be called for an index that is equal to or below
 * an active to-be-closed slot.
 *
 * Note that, both in case of errors and of a regular return, by the time the
 * __close metamethod runs, the C stack was already unwound, so that any
 * automatic C variable declared in the calling function (e.g., a buffer)
 * will be out of scope.
 */
LUA_API void lua_toclose (lua_State *L, int idx) {
  int nresults;
  StkId o;
  lua_lock(L);
  o = index2stack(L, idx);
  nresults = L->ci->nresults;
  api_check(L, L->tbclist < o, "given index below or equal a marked one");
  luaF_newtbcupval(L, o);  /* create new to-be-closed upvalue */
  if (!hastocloseCfunc(nresults))  /* function not marked yet? */
    L->ci->nresults = codeNresults(nresults);  /* mark it */
  lua_assert(hastocloseCfunc(L->ci->nresults));
  lua_unlock(L);
}

/**
 * lua_concat [-n, +1, e]
 *
 * Concatenates the n values at the top of the stack, pops them, and leaves
 * the result on the top. If n is 1, the result is the single value on the
 * stack (that is, the function does nothing); if n is 0, the result is the
 * empty string. Concatenation is performed following the usual semantics of
 * Lua (see §3.4.6).
 */
LUA_API void lua_concat (lua_State *L, int n) {
  lua_lock(L);
  api_checknelems(L, n);
  if (n > 0)
    luaV_concat(L, n);
  else {  /* nothing to concatenate */
    setsvalue2s(L, L->top, luaS_newlstr(L, "", 0));  /* push empty string */
    api_incr_top(L);
  }
  luaC_checkGC(L);
  lua_unlock(L);
}


/**
 * lua_len [-0, +1, e]
 *
 * Returns the length of the value at the given index. It is equivalent to
 * the '#' operator in Lua (see §3.4.7) and may trigger a metamethod for the
 * "length" event (see §2.4). The result is pushed on the stack.
 */
LUA_API void lua_len (lua_State *L, int idx) {
  TValue *t;
  lua_lock(L);
  t = index2value(L, idx);
  luaV_objlen(L, L->top, t);
  api_incr_top(L);
  lua_unlock(L);
}


/**
 * lua_getallocf [-0, +0, –]
 *
 * Returns the memory-allocation function of a given state. If ud is not
 * NULL, Lua stores in *ud the opaque pointer given when the memory-allocator
 * function was set.
 */
LUA_API lua_Alloc lua_getallocf (lua_State *L, void **ud) {
  lua_Alloc f;
  lua_lock(L);
  if (ud) *ud = G(L)->ud;
  f = G(L)->frealloc;
  lua_unlock(L);
  return f;
}


/**
 * lua_setallocf [-0, +0, –]
 *
 * void lua_setallocf (lua_State *L, lua_Alloc f, void *ud);
 *
 * Changes the allocator function of a given state to f with user data ud.
 */
LUA_API void lua_setallocf (lua_State *L, lua_Alloc f, void *ud) {
  lua_lock(L);
  G(L)->ud = ud;
  G(L)->frealloc = f;
  lua_unlock(L);
}


/**
 * lua_setwarnf [-0, +0, –]
 *
 * Sets the warning function to be used by Lua to emit warnings (see
 * lua_WarnFunction). The ud parameter sets the value ud passed to the
 * warning function.
 */
void lua_setwarnf (lua_State *L, lua_WarnFunction f, void *ud) {
  lua_lock(L);
  G(L)->ud_warn = ud;
  G(L)->warnf = f;
  lua_unlock(L);
}


/**
 * lua_warning [-0, +0, –]
 *
 * Emits a warning with the given message. A message in a call with tocont
 * true should be continued in another call to this function.
 *
 * See warn for more details about warnings.
 */
void lua_warning (lua_State *L, const char *msg, int tocont) {
  lua_lock(L);
  luaE_warning(L, msg, tocont);
  lua_unlock(L);
}



/**
 * lua_newuserdatauv [-0, +1, m]
 *
 * This function creates and pushes on the stack a new full userdata, with
 * nuvalue associated Lua values, called user values, plus an associated
 * block of raw memory with size bytes. (The user values can be set and read
 * with the functions lua_setiuservalue and lua_getiuservalue.)
 *
 * The function returns the address of the block of memory. Lua ensures that
 * this address is valid as long as the corresponding userdata is alive (see
 * §2.5). Moreover, if the userdata is marked for finalization (see §2.5.3),
 * its address is valid at least until the call to its finalizer.
 */
LUA_API void *lua_newuserdatauv (lua_State *L, size_t size, int nuvalue) {
  Udata *u;
  lua_lock(L);
  api_check(L, 0 <= nuvalue && nuvalue < USHRT_MAX, "invalid value");
  u = luaS_newudata(L, size, nuvalue);
  setuvalue(L, s2v(L->top), u);
  api_incr_top(L);
  luaC_checkGC(L);
  lua_unlock(L);
  return getudatamem(u);
}


static const char *aux_upvalue (TValue *fi, int n, TValue **val,
                                GCObject **owner) {
  switch (ttypetag(fi)) {
    case LUA_VCCL: {  /* C closure */
      CClosure *f = clCvalue(fi);
      if (!(cast_uint(n) - 1u < cast_uint(f->nupvalues)))
        return NULL;  /* 'n' not in [1, f->nupvalues] */
      *val = &f->upvalue[n-1];
      if (owner) *owner = obj2gco(f);
      return "";
    }
    case LUA_VLCL: {  /* Lua closure */
      LClosure *f = clLvalue(fi);
      TString *name;
      Proto *p = f->p;
      if (!(cast_uint(n) - 1u  < cast_uint(p->sizeupvalues)))
        return NULL;  /* 'n' not in [1, p->sizeupvalues] */
      *val = f->upvals[n-1]->v;
      if (owner) *owner = obj2gco(f->upvals[n - 1]);
      name = p->upvalues[n-1].name;
      return (name == NULL) ? "(no name)" : getstr(name);
    }
    default: return NULL;  /* not a closure */
  }
}


/**
 * lua_getupvalue [-0, +(0|1), –]
 *
 * Gets information about the n-th upvalue of the closure at index funcindex.
 * It pushes the upvalue's value onto the stack and returns its name. Returns
 * NULL (and pushes nothing) when the index n is greater than the number of
 * upvalues.
 *
 * See debug.getupvalue for more information about upvalues.
 */
LUA_API const char *lua_getupvalue (lua_State *L, int funcindex, int n) {
  const char *name;
  TValue *val = NULL;  /* to avoid warnings */
  lua_lock(L);
  name = aux_upvalue(index2value(L, funcindex), n, &val, NULL);
  if (name) {
    setobj2s(L, L->top, val);
    api_incr_top(L);
  }
  lua_unlock(L);
  return name;
}


/**
 * lua_setupvalue [-(0|1), +0, –]
 *
 * Sets the value of a closure's upvalue. It assigns the value on the top of
 * the stack to the upvalue and returns its name. It also pops the value from
 * the stack.
 *
 * Returns NULL (and pops nothing) when the index n is greater than the
 * number of upvalues.
 *
 * Parameters funcindex and n are as in the function lua_getupvalue.
 */
LUA_API const char *lua_setupvalue (lua_State *L, int funcindex, int n) {
  const char *name;
  TValue *val = NULL;  /* to avoid warnings */
  GCObject *owner = NULL;  /* to avoid warnings */
  TValue *fi;
  lua_lock(L);
  fi = index2value(L, funcindex);
  api_checknelems(L, 1);
  name = aux_upvalue(fi, n, &val, &owner);
  if (name) {
    L->top--;
    setobj(L, val, s2v(L->top));
    luaC_barrier(L, owner, val);
  }
  lua_unlock(L);
  return name;
}


static UpVal **getupvalref (lua_State *L, int fidx, int n, LClosure **pf) {
  static const UpVal *const nullup = NULL;
  LClosure *f;
  TValue *fi = index2value(L, fidx);
  api_check(L, ttisLclosure(fi), "Lua function expected");
  f = clLvalue(fi);
  if (pf) *pf = f;
  if (1 <= n && n <= f->p->sizeupvalues)
    return &f->upvals[n - 1];  /* get its upvalue pointer */
  else
    return (UpVal**)&nullup;
}


/**
 * lua_upvalueid [-0, +0, –]
 *
 * Returns a unique identifier for the upvalue numbered n from the closure at
 * index funcindex.
 *
 * These unique identifiers allow a program to check whether different
 * closures share upvalues. Lua closures that share an upvalue (that is, that
 * access a same external local variable) will return identical ids for those
 * upvalue indices.
 *
 * Parameters funcindex and n are as in the function lua_getupvalue, but n
 * cannot be greater than the number of upvalues.
 */
LUA_API void *lua_upvalueid (lua_State *L, int fidx, int n) {
  TValue *fi = index2value(L, fidx);
  switch (ttypetag(fi)) {
    case LUA_VLCL: {  /* lua closure */
      return *getupvalref(L, fidx, n, NULL);
    }
    case LUA_VCCL: {  /* C closure */
      CClosure *f = clCvalue(fi);
      if (1 <= n && n <= f->nupvalues)
        return &f->upvalue[n - 1];
      /* else */
    }  /* FALLTHROUGH */
    case LUA_VLCF:
      return NULL;  /* light C functions have no upvalues */
    default: {
      api_check(L, 0, "function expected");
      return NULL;
    }
  }
}


/**
 * lua_upvaluejoin [-0, +0, –]
 *
 * Make the n1-th upvalue of the Lua closure at index funcindex1 refer to the
 * n2-th upvalue of the Lua closure at index funcindex2.
 */
LUA_API void lua_upvaluejoin (lua_State *L, int fidx1, int n1,
                                            int fidx2, int n2) {
  LClosure *f1;
  UpVal **up1 = getupvalref(L, fidx1, n1, &f1);
  UpVal **up2 = getupvalref(L, fidx2, n2, NULL);
  api_check(L, *up1 != NULL && *up2 != NULL, "invalid upvalue index");
  *up1 = *up2;
  luaC_objbarrier(L, f1, *up1);
}


