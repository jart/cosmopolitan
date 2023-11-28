#ifndef COSMOPOLITAN_TOOL_NET_LJSON_H_
#define COSMOPOLITAN_TOOL_NET_LJSON_H_
#include "third_party/lua/lauxlib.h"
COSMOPOLITAN_C_START_

struct DecodeJson {
  int rc;
  const char *p;
};

struct DecodeJson DecodeJson(struct lua_State *, const char *, size_t);

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_TOOL_NET_LJSON_H_ */
