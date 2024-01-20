#ifndef COSMOPOLITAN_TOOL_NET_LFUNCS_H_
#define COSMOPOLITAN_TOOL_NET_LFUNCS_H_
#include "net/http/url.h"
#include "third_party/lua/lua.h"
COSMOPOLITAN_C_START_

int LuaMaxmind(lua_State *);
int LuaRe(lua_State *);
int luaopen_argon2(lua_State *);
int luaopen_lsqlite3(lua_State *);

int LuaBarf(lua_State *);
int LuaBenchmark(lua_State *);
int LuaBin(lua_State *);
int LuaBsf(lua_State *);
int LuaBsr(lua_State *);
int LuaCategorizeIp(lua_State *);
int LuaCompress(lua_State *);
int LuaCrc32(lua_State *);
int LuaCrc32c(lua_State *);
int LuaCurve25519(lua_State *);
int LuaDecimate(lua_State *);
int LuaDecodeBase32(lua_State *);
int LuaDecodeBase64(lua_State *);
int LuaDecodeHex(lua_State *);
int LuaDecodeLatin1(lua_State *);
int LuaDeflate(lua_State *);
int LuaEncodeBase32(lua_State *);
int LuaEncodeBase64(lua_State *);
int LuaEncodeHex(lua_State *);
int LuaEncodeLatin1(lua_State *);
int LuaEscapeFragment(lua_State *);
int LuaEscapeHost(lua_State *);
int LuaEscapeHtml(lua_State *);
int LuaEscapeIp(lua_State *);
int LuaEscapeLiteral(lua_State *);
int LuaEscapeParam(lua_State *);
int LuaEscapePass(lua_State *);
int LuaEscapePath(lua_State *);
int LuaEscapeSegment(lua_State *);
int LuaEscapeUser(lua_State *);
int LuaFormatHttpDateTime(lua_State *);
int LuaFormatIp(lua_State *);
int LuaGetCpuCore(lua_State *);
int LuaGetCpuCount(lua_State *);
int LuaGetCpuNode(lua_State *);
int LuaGetCryptoHash(lua_State *);
int LuaGetHostIsa(lua_State *);
int LuaGetHostOs(lua_State *);
int LuaGetHttpReason(lua_State *);
int LuaGetLogLevel(lua_State *);
int LuaGetMonospaceWidth(lua_State *);
int LuaGetRandomBytes(lua_State *);
int LuaGetTime(lua_State *);
int LuaHasControlCodes(lua_State *);
int LuaHex(lua_State *);
int LuaHighwayHash64(lua_State *);
int LuaIndentLines(lua_State *);
int LuaInflate(lua_State *);
int LuaIsAcceptableHost(lua_State *);
int LuaIsAcceptablePath(lua_State *);
int LuaIsAcceptablePort(lua_State *);
int LuaIsHeaderRepeatable(lua_State *);
int LuaIsLoopbackIp(lua_State *);
int LuaIsPrivateIp(lua_State *);
int LuaIsPublicIp(lua_State *);
int LuaIsReasonablePath(lua_State *);
int LuaIsValidHttpToken(lua_State *);
int LuaLemur64(lua_State *);
int LuaMd5(lua_State *);
int LuaMeasureEntropy(lua_State *);
int LuaOct(lua_State *);
int LuaParseHost(lua_State *);
int LuaParseHttpDateTime(lua_State *);
int LuaParseIp(lua_State *);
int LuaParseParams(lua_State *);
int LuaPopcnt(lua_State *);
int LuaRand64(lua_State *);
int LuaRdrand(lua_State *);
int LuaRdseed(lua_State *);
int LuaRdtsc(lua_State *);
int LuaResolveIp(lua_State *);
int LuaSetLogLevel(lua_State *);
int LuaSha1(lua_State *);
int LuaSha224(lua_State *);
int LuaSha256(lua_State *);
int LuaSha384(lua_State *);
int LuaSha512(lua_State *);
int LuaSleep(lua_State *);
int LuaSlurp(lua_State *);
int LuaUncompress(lua_State *);
int LuaUnderlong(lua_State *);
int LuaVisualizeControlCodes(lua_State *);

void LuaPushUrlView(lua_State *, struct UrlView *);
char *FormatUnixHttpDateTime(char *, int64_t);

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_TOOL_NET_LFUNCS_H_ */
