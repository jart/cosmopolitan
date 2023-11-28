#ifndef COSMOPOLITAN_THIRD_PARTY_LUA_COSMO_H_
#define COSMOPOLITAN_THIRD_PARTY_LUA_COSMO_H_
#include "net/http/http.h"
#include "net/http/url.h"
#include "third_party/lua/lauxlib.h"
#include "third_party/lua/visitor.h"
COSMOPOLITAN_C_START_

struct EncoderConfig {
  short maxdepth;
  bool sorted;
  bool pretty;
  const char *indent;
};

struct Serializer {
  struct LuaVisited visited;
  struct EncoderConfig conf;
  const char *reason;
  char *strbuf;
  size_t strbuflen;
};

bool LuaHasMultipleItems(lua_State *);
char *LuaFormatStack(lua_State *) __wur;
int LuaCallWithTrace(lua_State *, int, int, lua_State *);
int LuaEncodeJsonData(lua_State *, char **, int, struct EncoderConfig);
int LuaEncodeLuaData(lua_State *, char **, int, struct EncoderConfig);
int LuaEncodeUrl(lua_State *);
int LuaParseUrl(lua_State *);
int LuaPushHeader(lua_State *, struct HttpMessage *, const char *, int);
int LuaPushHeaders(lua_State *, struct HttpMessage *, const char *);
void LuaPrintStack(lua_State *);
void LuaPushLatin1(lua_State *, const char *, size_t);
void LuaPushUrlParams(lua_State *, struct UrlParams *);
int SerializeObjectStart(char **, struct Serializer *, int, bool);
int SerializeObjectEnd(char **, struct Serializer *, int, bool);
int SerializeObjectIndent(char **, struct Serializer *, int);

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_THIRD_PARTY_LUA_COSMO_H_ */
