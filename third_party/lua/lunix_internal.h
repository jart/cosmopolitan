#ifndef COSMOPOLITAN_THIRD_PARTY_LUA_LUNIX_INTERNAL_H_
#define COSMOPOLITAN_THIRD_PARTY_LUA_LUNIX_INTERNAL_H_
#include "libc/calls/struct/rusage.h"
#include "libc/calls/struct/sigset.h"
#include "libc/calls/struct/stat.h"
#include "libc/calls/struct/statfs.h"
#include "third_party/lua/lauxlib.h"
#include "third_party/lua/lua.h"
#include "third_party/lua/lunix.h"
COSMOPOLITAN_C_START_

extern lua_State *GL;

static inline int ReturnInteger(lua_State *L, lua_Integer x) {
  lua_pushinteger(L, x);
  return 1;
}

static inline int ReturnBoolean(lua_State *L, int x) {
  lua_pushboolean(L, !!x);
  return 1;
}

static inline int ReturnString(lua_State *L, const char *x) {
  lua_pushstring(L, x);
  return 1;
}

void *LuaRealloc(lua_State *, void *, size_t);
void *LuaAlloc(lua_State *, size_t);
void FreeStringList(char **);
char **ConvertLuaArrayToStringList(lua_State *, int);

void LuaSetIntField(lua_State *, const char *, lua_Integer);
void LuaPushSigset(lua_State *, sigset_t);
void LuaPushStat(lua_State *, struct stat *);
void LuaPushStatfs(lua_State *, struct statfs *);
void LuaPushRusage(lua_State *, struct rusage *);

int SysretBool(lua_State *, const char *, int, int);
int SysretInteger(lua_State *, const char *, int, int64_t);

void LuaUnixStatObj(lua_State *);
void LuaUnixStatfsObj(lua_State *);
void LuaUnixRusageObj(lua_State *);
void LuaUnixErrnoObj(lua_State *);
void LuaUnixMemoryObj(lua_State *);
void LuaUnixSigsetObj(lua_State *);
void LuaUnixDirObj(lua_State *);

int LuaUnixSigset(lua_State *);
int LuaUnixOpendir(lua_State *);
int LuaUnixFdopendir(lua_State *);
int LuaUnixMapshared(lua_State *);

int LuaUnixExecvp(lua_State *);
int LuaUnixExecvpe(lua_State *);
int LuaUnixFexecve(lua_State *);
int LuaUnixSpawn(lua_State *);
int LuaUnixSpawnp(lua_State *);
int LuaUnixKillpg(lua_State *);
int LuaUnixDaemon(lua_State *);
int LuaUnixNice(lua_State *);
int LuaUnixGetpriority(lua_State *);
int LuaUnixSetpriority(lua_State *);
int LuaUnixMkdtemp(lua_State *);
int LuaUnixMkstemp(lua_State *);

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_THIRD_PARTY_LUA_LUNIX_INTERNAL_H_ */
