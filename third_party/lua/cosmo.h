#ifndef COSMOPOLITAN_THIRD_PARTY_LUA_COSMO_H_
#define COSMOPOLITAN_THIRD_PARTY_LUA_COSMO_H_
#include "net/http/http.h"
#include "net/http/url.h"
#include "third_party/lua/lauxlib.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

char *LuaFormatStack(lua_State *) dontdiscard;
int LuaCallWithTrace(lua_State *, int, int, lua_State *);
int LuaEncodeJsonData(lua_State *, char **, int, char *, int);
int LuaEncodeLuaData(lua_State *, char **, int, char *, int);
int LuaEncodeUrl(lua_State *);
int LuaParseUrl(lua_State *);
int LuaPushHeader(lua_State *, struct HttpMessage *, char *, int);
int LuaPushHeaders(lua_State *, struct HttpMessage *, const char *);
void EscapeLuaString(char *, size_t, char **);
void LuaPushLatin1(lua_State *, const char *, size_t);
void LuaPushUrlParams(lua_State *, struct UrlParams *);
void LuaPrintStack(lua_State *);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_THIRD_PARTY_LUA_COSMO_H_ */
