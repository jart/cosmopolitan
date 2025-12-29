/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2024 Will Maier                                                    │
│                                                                              │
│ Permission to use, copy, modify, and/or distribute this software for        │
│ any purpose with or without fee is hereby granted, provided that the        │
│ above copyright notice and this permission notice appear in all copies.     │
│                                                                              │
│ THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL               │
│ WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED               │
│ WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE            │
│ AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL        │
│ DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR       │
│ PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER              │
│ TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR            │
│ PERFORMANCE OF THIS SOFTWARE.                                               │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "third_party/lua/lauxlib.h"
#include "third_party/lua/lcosmo.h"
#include "third_party/lua/lunix.h"
#include "third_party/lua/cosmo.h"
#include "tool/net/lfuncs.h"
#include "tool/net/lpath.h"
#include "tool/net/ljson.h"
#include "tool/net/lfetch.h"
#include "tool/net/lgetopt.h"
#include "net/http/http.h"
#include <stdlib.h>
#include <limits.h>
#include <time.h>

char *FormatUnixHttpDateTime(char *s, int64_t t) {
  struct tm tm;
  gmtime_r(&t, &tm);
  FormatHttpDateTime(s, &tm);
  return s;
}

static int LuaDecodeJson(lua_State *L) {
  size_t n;
  const char *p;
  struct DecodeJson r;
  p = luaL_checklstring(L, 1, &n);
  r = DecodeJson(L, p, n);
  if (!r.rc) {
    lua_pushnil(L);
    lua_pushstring(L, "unexpected eof");
    return 2;
  }
  if (r.rc == -1) {
    lua_pushnil(L);
    lua_pushstring(L, r.p);
    return 2;
  }
  r = DecodeJson(L, r.p, n - (r.p - p));
  if (r.rc) {
    lua_pushnil(L);
    lua_pushstring(L, "junk after expression");
    return 2;
  }
  return 1;
}

static int LuaEncodeSmth(lua_State *L, int Encoder(lua_State *, char **, int,
                                                   struct EncoderConfig)) {
  char *p = 0;
  struct EncoderConfig conf = {
      .maxdepth = 64,
      .sorted = 1,
      .pretty = 0,
      .indent = "  ",
  };
  if (lua_istable(L, 2)) {
    lua_settop(L, 2);
    lua_getfield(L, 2, "maxdepth");
    if (!lua_isnoneornil(L, -1)) {
      lua_Integer n = lua_tointeger(L, -1);
      n = n < 0 ? 0 : (n > SHRT_MAX ? SHRT_MAX : n);
      conf.maxdepth = n;
    }
    lua_getfield(L, 2, "sorted");
    if (!lua_isnoneornil(L, -1)) {
      conf.sorted = lua_toboolean(L, -1);
    }
    lua_getfield(L, 2, "pretty");
    if (!lua_isnoneornil(L, -1)) {
      conf.pretty = lua_toboolean(L, -1);
      lua_getfield(L, 2, "indent");
      if (!lua_isnoneornil(L, -1)) {
        conf.indent = luaL_checkstring(L, -1);
      }
    }
  }
  lua_settop(L, 1);
  if (Encoder(L, &p, -1, conf) == -1) {
    free(p);
    return 2;
  }
  lua_pushstring(L, p);
  free(p);
  return 1;
}

static int LuaEncodeJson(lua_State *L) {
  return LuaEncodeSmth(L, LuaEncodeJsonData);
}

static int LuaEncodeLua(lua_State *L) {
  return LuaEncodeSmth(L, LuaEncodeLuaData);
}

// clang-format off
static const luaL_Reg kCosmoFuncs[] = {
    {"Barf", LuaBarf},
    {"Bsf", LuaBsf},
    {"Bsr", LuaBsr},
    {"bin", LuaBin},
    {"CategorizeIp", LuaCategorizeIp},
    {"Compress", LuaCompress},
    {"Crc32", LuaCrc32},
    {"Crc32c", LuaCrc32c},
    {"DecodeBase32", LuaDecodeBase32},
    {"DecodeBase64", LuaDecodeBase64},
    {"DecodeHex", LuaDecodeHex},
    {"DecodeJson", LuaDecodeJson},
    {"DecodeLatin1", LuaDecodeLatin1},
    {"Deflate", LuaDeflate},
    {"EncodeBase32", LuaEncodeBase32},
    {"EncodeBase64", LuaEncodeBase64},
    {"EncodeHex", LuaEncodeHex},
    {"EncodeJson", LuaEncodeJson},
    {"EncodeLatin1", LuaEncodeLatin1},
    {"EncodeLua", LuaEncodeLua},
    {"EncodeUrl", LuaEncodeUrl},
    {"EscapeFragment", LuaEscapeFragment},
    {"EscapeHost", LuaEscapeHost},
    {"EscapeHtml", LuaEscapeHtml},
    {"EscapeIp", LuaEscapeIp},
    {"EscapeLiteral", LuaEscapeLiteral},
    {"EscapeParam", LuaEscapeParam},
    {"EscapePass", LuaEscapePass},
    {"EscapePath", LuaEscapePath},
    {"EscapeSegment", LuaEscapeSegment},
    {"EscapeUser", LuaEscapeUser},
    {"FormatIp", LuaFormatIp},
    {"GetCpuCore", LuaGetCpuCore},
    {"GetCpuCount", LuaGetCpuCount},
    {"GetCpuNode", LuaGetCpuNode},
    {"GetHostIsa", LuaGetHostIsa},
    {"GetHostOs", LuaGetHostOs},
    {"GetHttpReason", LuaGetHttpReason},
    {"GetMonospaceWidth", LuaGetMonospaceWidth},
    {"GetRandomBytes", LuaGetRandomBytes},
    {"GetTime", LuaGetTime},
    {"HasControlCodes", LuaHasControlCodes},
    {"HighwayHash64", LuaHighwayHash64},
    {"hex", LuaHex},
    {"IndentLines", LuaIndentLines},
    {"Inflate", LuaInflate},
    {"IsAcceptableHost", LuaIsAcceptableHost},
    {"IsAcceptablePath", LuaIsAcceptablePath},
    {"IsAcceptablePort", LuaIsAcceptablePort},
    {"IsHeaderRepeatable", LuaIsHeaderRepeatable},
    {"IsLoopbackIp", LuaIsLoopbackIp},
    {"IsPrivateIp", LuaIsPrivateIp},
    {"IsPublicIp", LuaIsPublicIp},
    {"IsReasonablePath", LuaIsReasonablePath},
    {"IsValidHttpToken", LuaIsValidHttpToken},
    {"Lemur64", LuaLemur64},
    {"MeasureEntropy", LuaMeasureEntropy},
    {"oct", LuaOct},
    {"ParseHost", LuaParseHost},
    {"ParseHttpDateTime", LuaParseHttpDateTime},
    {"ParseIp", LuaParseIp},
    {"ParseParams", LuaParseParams},
    {"ParseUrl", LuaParseUrl},
    {"Popcnt", LuaPopcnt},
    {"Rand64", LuaRand64},
    {"ResolveIp", LuaResolveIp},
    {"Sleep", LuaSleep},
    {"Slurp", LuaSlurp},
    {"Uncompress", LuaUncompress},
    {"Underlong", LuaUnderlong},
    {"UuidV4", LuaUuidV4},
    {"UuidV7", LuaUuidV7},
    {"VisualizeControlCodes", LuaVisualizeControlCodes},
    {"Decimate", LuaDecimate},
    {"FormatHttpDateTime", LuaFormatHttpDateTime},
    {"GetCryptoHash", LuaGetCryptoHash},
    {"Md5", LuaMd5},
    {"Sha1", LuaSha1},
    {"Sha224", LuaSha224},
    {"Sha256", LuaSha256},
    {"Sha384", LuaSha384},
    {"Sha512", LuaSha512},
    {"Curve25519", LuaCurve25519},
    {"Fetch", LuaFetch},
    {NULL, NULL}
};
// clang-format on

/* Register submodule in package.loaded for direct require() support */
static void register_submodule(lua_State *L, const char *name) {
  /* stack: cosmo, submodule */
  lua_getfield(L, LUA_REGISTRYINDEX, LUA_LOADED_TABLE);
  lua_pushvalue(L, -2);              /* copy submodule */
  lua_setfield(L, -2, name);         /* package.loaded[name] = submodule */
  lua_pop(L, 1);                     /* pop package.loaded */
}

int luaopen_cosmo(lua_State *L) {
  /* initialize fetch SSL state */
  LuaInitFetch();

  luaL_newlib(L, kCosmoFuncs);

  /* register submodules for direct require("cosmo.xxx") */
  LuaUnix(L);
  register_submodule(L, "cosmo.unix");
  lua_pop(L, 1);

  LuaPath(L);
  register_submodule(L, "cosmo.path");
  lua_pop(L, 1);

  LuaRe(L);
  register_submodule(L, "cosmo.re");
  lua_pop(L, 1);

  luaopen_argon2(L);
  register_submodule(L, "cosmo.argon2");
  lua_pop(L, 1);

  luaopen_lsqlite3(L);
  register_submodule(L, "cosmo.lsqlite3");
  lua_pop(L, 1);

  LuaGetopt(L);
  register_submodule(L, "cosmo.getopt");
  lua_pop(L, 1);

  /* make help() global for convenience */
  (void)luaL_dostring(L, "_G.help = require('cosmo.help')");

  return 1;
}
