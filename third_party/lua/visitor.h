#ifndef COSMOPOLITAN_THIRD_PARTY_LUA_VISITOR_H_
#define COSMOPOLITAN_THIRD_PARTY_LUA_VISITOR_H_
COSMOPOLITAN_C_START_

struct LuaVisited {
  int i, n;
  const void **p;
};

int LuaPushVisit(struct LuaVisited *, const void *);
void LuaPopVisit(struct LuaVisited *);

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_THIRD_PARTY_LUA_VISITOR_H_ */
