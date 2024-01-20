/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2022 Justine Alexandra Roberts Tunney                              │
│                                                                              │
│ Permission to use, copy, modify, and/or distribute this software for         │
│ any purpose with or without fee is hereby granted, provided that the         │
│ above copyright notice and this permission notice appear in all copies.      │
│                                                                              │
│ THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL                │
│ WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED                │
│ WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE             │
│ AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL         │
│ DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR        │
│ PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER               │
│ TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR             │
│ PERFORMANCE OF THIS SOFTWARE.                                                │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "tool/net/lfuncs.h"
#include "dsp/scale/cdecimate2xuint8x8.h"
#include "libc/calls/calls.h"
#include "libc/calls/struct/rusage.h"
#include "libc/calls/struct/stat.h"
#include "libc/calls/struct/timespec.h"
#include "libc/errno.h"
#include "libc/fmt/itoa.h"
#include "libc/fmt/leb128.h"
#include "libc/intrin/bsf.h"
#include "libc/intrin/bsr.h"
#include "libc/intrin/popcnt.h"
#include "libc/log/check.h"
#include "libc/log/log.h"
#include "libc/macros.internal.h"
#include "libc/math.h"
#include "libc/mem/gc.h"
#include "libc/mem/mem.h"
#include "libc/nexgen32e/bench.h"
#include "libc/nexgen32e/crc32.h"
#include "libc/nexgen32e/rdtsc.h"
#include "libc/nexgen32e/rdtscp.h"
#include "libc/runtime/runtime.h"
#include "libc/serialize.h"
#include "libc/sock/sock.h"
#include "libc/stdio/rand.h"
#include "libc/str/highwayhash64.h"
#include "libc/str/str.h"
#include "libc/str/strwidth.h"
#include "libc/str/tab.internal.h"
#include "libc/sysv/consts/af.h"
#include "libc/sysv/consts/ipproto.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/rusage.h"
#include "libc/sysv/consts/sock.h"
#include "libc/thread/thread.h"
#include "libc/time/time.h"
#include "libc/x/x.h"
#include "net/http/escape.h"
#include "net/http/http.h"
#include "net/http/ip.h"
#include "net/http/url.h"
#include "third_party/lua/cosmo.h"
#include "third_party/lua/lauxlib.h"
#include "third_party/lua/lua.h"
#include "third_party/lua/luaconf.h"
#include "third_party/lua/lunix.h"
#include "third_party/mbedtls/everest.h"
#include "third_party/mbedtls/md.h"
#include "third_party/mbedtls/md5.h"
#include "third_party/mbedtls/platform.h"
#include "third_party/mbedtls/sha1.h"
#include "third_party/mbedtls/sha256.h"
#include "third_party/mbedtls/sha512.h"
#include "third_party/musl/netdb.h"
#include "third_party/zlib/zlib.h"

static int Rdpid(void) {
#ifdef __x86_64__
  return rdpid();
#else
  return -1;
#endif
}

int LuaHex(lua_State *L) {
  char b[19];
  uint64_t x;
  x = luaL_checkinteger(L, 1);
  FormatHex64(b, x, true);
  lua_pushstring(L, b);
  return 1;
}

int LuaOct(lua_State *L) {
  char b[24];
  uint64_t x;
  x = luaL_checkinteger(L, 1);
  FormatOctal64(b, x, true);
  lua_pushstring(L, b);
  return 1;
}

int LuaBin(lua_State *L) {
  char b[67];
  uint64_t x;
  x = luaL_checkinteger(L, 1);
  FormatBinary64(b, x, 2);
  lua_pushstring(L, b);
  return 1;
}

int LuaGetTime(lua_State *L) {
  struct timespec now = timespec_real();
  lua_pushnumber(L, now.tv_sec + now.tv_nsec * 1e-9);
  return 1;
}

int LuaSleep(lua_State *L) {
  usleep(1e6 * luaL_checknumber(L, 1));
  return 0;
}

int LuaRdtsc(lua_State *L) {
  lua_pushinteger(L, rdtsc());
  return 1;
}

int LuaGetCpuNode(lua_State *L) {
  lua_pushinteger(L, TSC_AUX_NODE(Rdpid()));
  return 1;
}

int LuaGetCpuCore(lua_State *L) {
  lua_pushinteger(L, TSC_AUX_CORE(Rdpid()));
  return 1;
}

int LuaGetCpuCount(lua_State *L) {
  lua_pushinteger(L, __get_cpu_count());
  return 1;
}

int LuaGetLogLevel(lua_State *L) {
  lua_pushinteger(L, __log_level);
  return 1;
}

int LuaSetLogLevel(lua_State *L) {
  __log_level = luaL_checkinteger(L, 1);
  return 0;
}

static int LuaRand(lua_State *L, uint64_t impl(void)) {
  lua_pushinteger(L, impl());
  return 1;
}

int LuaLemur64(lua_State *L) {
  return LuaRand(L, lemur64);
}

int LuaRand64(lua_State *L) {
  return LuaRand(L, _rand64);
}

int LuaRdrand(lua_State *L) {
  return LuaRand(L, rdrand);
}

int LuaRdseed(lua_State *L) {
  return LuaRand(L, rdseed);
}

int LuaDecimate(lua_State *L) {
  char *p;
  size_t n, m;
  const char *s;
  luaL_Buffer buf;
  s = luaL_checklstring(L, 1, &n);
  m = ROUNDUP(n, 16);
  p = luaL_buffinitsize(L, &buf, m);
  memcpy(p, s, n);
  bzero(p + n, m - n);
  cDecimate2xUint8x8(m, (unsigned char *)p,
                     (signed char[8]){-1, -3, 3, 17, 17, 3, -3, -1});
  luaL_pushresultsize(&buf, (n + 1) >> 1);
  return 1;
}

int LuaMeasureEntropy(lua_State *L) {
  size_t n;
  const char *s;
  s = luaL_checklstring(L, 1, &n);
  lua_pushnumber(L, MeasureEntropy(s, n));
  return 1;
}

int LuaGetHostOs(lua_State *L) {
  const char *s = NULL;
  if (IsLinux()) {
    s = "LINUX";
  } else if (IsMetal()) {
    s = "METAL";
  } else if (IsWindows()) {
    s = "WINDOWS";
  } else if (IsXnu()) {
    s = "XNU";
  } else if (IsOpenbsd()) {
    s = "OPENBSD";
  } else if (IsFreebsd()) {
    s = "FREEBSD";
  } else if (IsNetbsd()) {
    s = "NETBSD";
  }
  if (s) {
    lua_pushstring(L, s);
  } else {
    lua_pushnil(L);
  }
  return 1;
}

int LuaGetHostIsa(lua_State *L) {
  const char *s;
#ifdef __x86_64__
  s = "X86_64";
#elif defined(__aarch64__)
  s = "AARCH64";
#elif defined(__powerpc64__)
  s = "POWERPC64";
#elif defined(__s390x__)
  s = "S390X";
#else
#error "unsupported architecture"
#endif
  lua_pushstring(L, s);
  return 1;
}

int LuaFormatIp(lua_State *L) {
  char b[16];
  uint32_t ip;
  ip = htonl(luaL_checkinteger(L, 1));
  inet_ntop(AF_INET, &ip, b, sizeof(b));
  lua_pushstring(L, b);
  return 1;
}

int LuaParseIp(lua_State *L) {
  size_t n;
  const char *s;
  s = luaL_checklstring(L, 1, &n);
  lua_pushinteger(L, ParseIp(s, n));
  return 1;
}

static int LuaIsIp(lua_State *L, bool IsIp(uint32_t)) {
  lua_pushboolean(L, IsIp(luaL_checkinteger(L, 1)));
  return 1;
}

int LuaIsPublicIp(lua_State *L) {
  return LuaIsIp(L, IsPublicIp);
}

int LuaIsPrivateIp(lua_State *L) {
  return LuaIsIp(L, IsPrivateIp);
}

int LuaIsLoopbackIp(lua_State *L) {
  return LuaIsIp(L, IsLoopbackIp);
}

int LuaCategorizeIp(lua_State *L) {
  lua_pushstring(L, GetIpCategoryName(CategorizeIp(luaL_checkinteger(L, 1))));
  return 1;
}

int LuaFormatHttpDateTime(lua_State *L) {
  char buf[30];
  lua_pushstring(L, FormatUnixHttpDateTime(buf, luaL_checkinteger(L, 1)));
  return 1;
}

int LuaParseHttpDateTime(lua_State *L) {
  size_t n;
  const char *s;
  s = luaL_checklstring(L, 1, &n);
  lua_pushinteger(L, ParseHttpDateTime(s, n));
  return 1;
}

int LuaParseParams(lua_State *L) {
  void *m;
  size_t size;
  const char *data;
  struct UrlParams h;
  if (!lua_isnoneornil(L, 1)) {
    data = luaL_checklstring(L, 1, &size);
    bzero(&h, sizeof(h));
    if ((m = ParseParams(data, size, &h))) {
      LuaPushUrlParams(L, &h);
      free(h.p);
      free(m);
      return 1;
    } else {
      luaL_error(L, "out of memory");
      __builtin_unreachable();
    }
  } else {
    return lua_gettop(L);
  }
}

int LuaParseHost(lua_State *L) {
  void *m;
  size_t n;
  struct Url h;
  const char *p;
  if (!lua_isnoneornil(L, 1)) {
    bzero(&h, sizeof(h));
    p = luaL_checklstring(L, 1, &n);
    if ((m = ParseHost(p, n, &h))) {
      lua_newtable(L);
      LuaPushUrlView(L, &h.host);
      LuaPushUrlView(L, &h.port);
      free(m);
      return 1;
    } else {
      luaL_error(L, "out of memory");
      __builtin_unreachable();
    }
  } else {
    return lua_gettop(L);
  }
}

int LuaPopcnt(lua_State *L) {
  lua_pushinteger(L, popcnt(luaL_checkinteger(L, 1)));
  return 1;
}

int LuaBsr(lua_State *L) {
  long x;
  if ((x = luaL_checkinteger(L, 1))) {
    lua_pushinteger(L, _bsrl(x));
    return 1;
  } else {
    luaL_argerror(L, 1, "zero");
    __builtin_unreachable();
  }
}

int LuaBsf(lua_State *L) {
  long x;
  if ((x = luaL_checkinteger(L, 1))) {
    lua_pushinteger(L, _bsfl(x));
    return 1;
  } else {
    luaL_argerror(L, 1, "zero");
    __builtin_unreachable();
  }
}

int LuaHighwayHash64(lua_State *L) {
  size_t n;
  uint64_t k[4];
  const char *p;
  p = luaL_checklstring(L, 1, &n);
  k[0] = luaL_optinteger(L, 2, 0);
  k[1] = luaL_optinteger(L, 3, 0);
  k[2] = luaL_optinteger(L, 4, 0);
  k[3] = luaL_optinteger(L, 5, 0);
  lua_pushinteger(L, HighwayHash64(p, n, k));
  return 1;
}

static int LuaHash(lua_State *L, uint32_t H(uint32_t, const void *, size_t)) {
  long i;
  size_t n;
  const char *p;
  i = luaL_checkinteger(L, 1);
  p = luaL_checklstring(L, 2, &n);
  lua_pushinteger(L, H(i, p, n));
  return 1;
}

int LuaCrc32(lua_State *L) {
  return LuaHash(L, crc32_z);
}

int LuaCrc32c(lua_State *L) {
  return LuaHash(L, crc32c);
}

int LuaIndentLines(lua_State *L) {
  size_t n, j;
  const void *p;
  if (!lua_isnoneornil(L, 1)) {
    p = luaL_checklstring(L, 1, &n);
    j = luaL_optinteger(L, 2, 1);
    if (!(0 <= j && j <= 65535)) {
      luaL_argerror(L, 2, "not in range 0..65535");
      __builtin_unreachable();
    }
    char *q = IndentLines(p, n, &n, j);
    lua_pushlstring(L, q, n);
    free(q);
    return 1;
  } else {
    return lua_gettop(L);
  }
}

int LuaGetMonospaceWidth(lua_State *L) {
  int w;
  if (lua_isnumber(L, 1)) {
    w = wcwidth(lua_tointeger(L, 1));
  } else if (lua_isstring(L, 1)) {
    w = strwidth(luaL_checkstring(L, 1), luaL_optinteger(L, 2, 0) & 7);
  } else {
    luaL_argerror(L, 1, "not integer or string");
    __builtin_unreachable();
  }
  lua_pushinteger(L, w);
  return 1;
}

// Slurp(path:str[, i:int[, j:int]])
//     ├─→ data:str
//     └─→ nil, unix.Errno
int LuaSlurp(lua_State *L) {
  ssize_t rc;
  char tb[2048];
  luaL_Buffer b;
  struct stat st;
  int fd, olderr;
  bool shouldpread;
  lua_Integer i, j, got;
  olderr = errno;
  if (lua_isnoneornil(L, 2)) {
    i = 1;
  } else {
    i = luaL_checkinteger(L, 2);
  }
  if (lua_isnoneornil(L, 3)) {
    j = LUA_MAXINTEGER;
  } else {
    j = luaL_checkinteger(L, 3);
  }
  luaL_buffinit(L, &b);
  if ((fd = open(luaL_checkstring(L, 1), O_RDONLY | O_SEQUENTIAL)) == -1) {
    return LuaUnixSysretErrno(L, "open", olderr);
  }
  if (i < 0 || j < 0) {
    if (fstat(fd, &st) == -1) {
      close(fd);
      return LuaUnixSysretErrno(L, "fstat", olderr);
    }
    if (i < 0) {
      i = st.st_size + (i + 1);
    }
    if (j < 0) {
      j = st.st_size + (j + 1);
    }
  }
  if (i < 1) {
    i = 1;
  }
  shouldpread = i > 1;
  for (; i <= j; i += got) {
    if (shouldpread) {
      rc = pread(fd, tb, MIN(j - i + 1, sizeof(tb)), i - 1);
    } else {
      rc = read(fd, tb, MIN(j - i + 1, sizeof(tb)));
    }
    if (rc != -1) {
      got = rc;
      if (!got) break;
      luaL_addlstring(&b, tb, got);
    } else if (errno == EINTR) {
      errno = olderr;
      got = 0;
    } else {
      close(fd);
      return LuaUnixSysretErrno(L, "read", olderr);
    }
  }
  if (close(fd) == -1) {
    return LuaUnixSysretErrno(L, "close", olderr);
  }
  luaL_pushresult(&b);
  return 1;
}

// Barf(path:str, data:str[, mode:int[, flags:int[, offset:int]]])
//     ├─→ true
//     └─→ nil, unix.Errno
int LuaBarf(lua_State *L) {
  ssize_t rc;
  const char *data;
  lua_Number offset;
  size_t i, n, wrote;
  int fd, mode, flags, olderr;
  olderr = errno;
  data = luaL_checklstring(L, 2, &n);
  if (lua_isnoneornil(L, 5)) {
    offset = 0;
  } else {
    offset = luaL_checkinteger(L, 5);
    if (offset < 1) {
      luaL_error(L, "offset must be >= 1");
      __builtin_unreachable();
    }
    --offset;
  }
  mode = luaL_optinteger(L, 3, 0644);
  flags = O_WRONLY | O_SEQUENTIAL | luaL_optinteger(L, 4, O_TRUNC | O_CREAT);
  if (flags & O_NONBLOCK) {
    luaL_error(L, "O_NONBLOCK not allowed");
    __builtin_unreachable();
  }
  if ((flags & O_APPEND) && offset) {
    luaL_error(L, "O_APPEND with offset not possible");
    __builtin_unreachable();
  }
  if ((fd = open(luaL_checkstring(L, 1), flags, mode)) == -1) {
    return LuaUnixSysretErrno(L, "open", olderr);
  }
  for (i = 0; i < n; i += wrote) {
    if (offset) {
      rc = pwrite(fd, data + i, n - i, offset + i);
    } else {
      rc = write(fd, data + i, n - i);
    }
    if (rc != -1) {
      wrote = rc;
    } else if (errno == EINTR) {
      errno = olderr;
      wrote = 0;
    } else {
      close(fd);
      return LuaUnixSysretErrno(L, "write", olderr);
    }
  }
  if (close(fd) == -1) {
    return LuaUnixSysretErrno(L, "close", olderr);
  }
  lua_pushboolean(L, true);
  return 1;
}

int LuaResolveIp(lua_State *L) {
  ssize_t rc;
  int64_t ip;
  const char *host;
  struct addrinfo *ai = NULL;
  struct addrinfo hint = {AI_NUMERICSERV, AF_INET, SOCK_STREAM, IPPROTO_TCP};
  host = luaL_checkstring(L, 1);
  if ((ip = ParseIp(host, -1)) != -1) {
    lua_pushinteger(L, ip);
    return 1;
  } else if ((rc = getaddrinfo(host, "0", &hint, &ai)) == 0) {
    lua_pushinteger(
        L, ntohl(((struct sockaddr_in *)ai->ai_addr)->sin_addr.s_addr));
    freeaddrinfo(ai);
    return 1;
  } else {
    lua_pushnil(L);
    lua_pushfstring(L, "%s: DNS lookup failed: EAI_%s", host, gai_strerror(rc));
    return 2;
  }
}

static int LuaCheckControlFlags(lua_State *L, int idx) {
  int f = luaL_optinteger(L, idx, 0);
  if (f & ~(kControlWs | kControlC0 | kControlC1)) {
    luaL_argerror(L, idx, "invalid control flags");
    __builtin_unreachable();
  }
  return f;
}

int LuaHasControlCodes(lua_State *L) {
  int f;
  size_t n;
  const char *p;
  p = luaL_checklstring(L, 1, &n);
  f = LuaCheckControlFlags(L, 2);
  lua_pushboolean(L, HasControlCodes(p, n, f) != -1);
  return 1;
}

int LuaEncodeLatin1(lua_State *L) {
  int f;
  char *q;
  size_t n;
  const char *p;
  p = luaL_checklstring(L, 1, &n);
  f = LuaCheckControlFlags(L, 2);
  if ((q = EncodeLatin1(p, n, &n, f))) {
    lua_pushlstring(L, q, n);
    free(q);
    return 1;
  } else {
    luaL_error(L, "out of memory");
    __builtin_unreachable();
  }
}

dontinline int LuaBase32Impl(lua_State *L,
                             char *B32(const char *, size_t, const char *,
                                       size_t, size_t *)) {
  char *p;
  size_t sl, al;  // source/output and alphabet lengths
  const char *s = luaL_checklstring(L, 1, &sl);
  // use an empty string, as EncodeBase32 provides a default value
  const char *a = luaL_optlstring(L, 2, "", &al);
  if (!IS2POW(al) || al > 128 || al == 1)
    return luaL_error(L, "alphabet length is not a power of 2 in range 2..128");
  if (!(p = B32(s, sl, a, al, &sl))) return luaL_error(L, "out of memory");
  lua_pushlstring(L, p, sl);
  free(p);
  return 1;
}

int LuaEncodeBase32(lua_State *L) {
  return LuaBase32Impl(L, EncodeBase32);
}

int LuaDecodeBase32(lua_State *L) {
  return LuaBase32Impl(L, DecodeBase32);
}

int LuaEncodeHex(lua_State *L) {
  char *p;
  size_t n;
  const char *s;
  luaL_Buffer buf;
  s = luaL_checklstring(L, 1, &n);
  p = luaL_buffinitsize(L, &buf, n * 2 + 1);
  hexpcpy(p, s, n);
  luaL_pushresultsize(&buf, n * 2);
  return 1;
}

int LuaDecodeHex(lua_State *L) {
  char *p;
  int x, y;
  size_t i, n;
  const char *s;
  luaL_Buffer buf;
  s = luaL_checklstring(L, 1, &n);
  if (n & 1) {
    luaL_argerror(L, 1, "hex string length uneven");
    __builtin_unreachable();
  }
  p = luaL_buffinitsize(L, &buf, n >> 1);
  for (i = 0; i < n; i += 2) {
    if ((x = kHexToInt[s[i + 0] & 255]) == -1 ||
        (y = kHexToInt[s[i + 1] & 255]) == -1) {
      luaL_argerror(L, 1, "hex string has non-hex character");
      __builtin_unreachable();
    }
    p[i >> 1] = x << 4 | y;
  }
  luaL_pushresultsize(&buf, n >> 1);
  return 1;
}

int LuaGetRandomBytes(lua_State *L) {
  size_t n;
  luaL_Buffer buf;
  n = luaL_optinteger(L, 1, 16);
  if (!(n > 0 && n <= 256)) {
    luaL_argerror(L, 1, "not in range 1..256");
    __builtin_unreachable();
  }
  CHECK_EQ(n, getrandom(luaL_buffinitsize(L, &buf, n), n, 0));
  luaL_pushresultsize(&buf, n);
  return 1;
}

int LuaGetHttpReason(lua_State *L) {
  lua_pushstring(L, GetHttpReason(luaL_checkinteger(L, 1)));
  return 1;
}

int LuaGetCryptoHash(lua_State *L) {
  size_t hl, pl, kl;
  uint8_t d[64];
  // get hash name, payload, and key
  const void *h = luaL_checklstring(L, 1, &hl);
  const void *p = luaL_checklstring(L, 2, &pl);
  const void *k = luaL_optlstring(L, 3, "", &kl);
  const mbedtls_md_info_t *digest = mbedtls_md_info_from_string(h);
  if (!digest) return luaL_argerror(L, 1, "unknown hash type");
  if (kl == 0) {
    // no key provided, run generic hash function
    if ((digest->f_md)(p, pl, d)) return luaL_error(L, "bad input data");
  } else if (mbedtls_md_hmac(digest, k, kl, p, pl, d)) {
    return luaL_error(L, "bad input data");
  }
  lua_pushlstring(L, (void *)d, digest->size);
  mbedtls_platform_zeroize(d, sizeof(d));
  return 1;
}

static dontinline int LuaIsValid(lua_State *L, bool V(const char *, size_t)) {
  size_t size;
  const char *data;
  data = luaL_checklstring(L, 1, &size);
  lua_pushboolean(L, V(data, size));
  return 1;
}

int LuaIsValidHttpToken(lua_State *L) {
  return LuaIsValid(L, IsValidHttpToken);
}

int LuaIsAcceptablePath(lua_State *L) {
  return LuaIsValid(L, IsAcceptablePath);
}

int LuaIsReasonablePath(lua_State *L) {
  return LuaIsValid(L, IsReasonablePath);
}

int LuaIsAcceptableHost(lua_State *L) {
  return LuaIsValid(L, IsAcceptableHost);
}

int LuaIsAcceptablePort(lua_State *L) {
  return LuaIsValid(L, IsAcceptablePort);
}

static dontinline int LuaCoderImpl(lua_State *L,
                                   char *C(const char *, size_t, size_t *)) {
  void *q;
  size_t n;
  const void *p;
  if (!lua_isnoneornil(L, 1)) {
    p = luaL_checklstring(L, 1, &n);
    if ((q = C(p, n, &n))) {
      lua_pushlstring(L, q, n);
      free(q);
    } else {
      luaL_error(L, "out of memory");
      __builtin_unreachable();
    }
    return 1;
  } else {
    return lua_gettop(L);
  }
}

static dontinline int LuaCoder(lua_State *L,
                               char *C(const char *, size_t, size_t *)) {
  return LuaCoderImpl(L, C);
}

int LuaUnderlong(lua_State *L) {
  return LuaCoder(L, Underlong);
}

int LuaEncodeBase64(lua_State *L) {
  return LuaCoder(L, EncodeBase64);
}

int LuaDecodeBase64(lua_State *L) {
  return LuaCoder(L, DecodeBase64);
}

int LuaDecodeLatin1(lua_State *L) {
  return LuaCoder(L, DecodeLatin1);
}

int LuaEscapeHtml(lua_State *L) {
  return LuaCoder(L, EscapeHtml);
}

int LuaEscapeParam(lua_State *L) {
  return LuaCoder(L, EscapeParam);
}

int LuaEscapePath(lua_State *L) {
  return LuaCoder(L, EscapePath);
}

int LuaEscapeHost(lua_State *L) {
  return LuaCoder(L, EscapeHost);
}

int LuaEscapeIp(lua_State *L) {
  return LuaCoder(L, EscapeIp);
}

int LuaEscapeUser(lua_State *L) {
  return LuaCoder(L, EscapeUser);
}

int LuaEscapePass(lua_State *L) {
  return LuaCoder(L, EscapePass);
}

int LuaEscapeSegment(lua_State *L) {
  return LuaCoder(L, EscapeSegment);
}

int LuaEscapeFragment(lua_State *L) {
  return LuaCoder(L, EscapeFragment);
}

int LuaEscapeLiteral(lua_State *L) {
  const char *p;
  char *z, *q = 0;
  size_t n, y = 0;
  p = luaL_checklstring(L, 1, &n);
  if ((z = EscapeJsStringLiteral(&q, &y, p, n, &n))) {
    lua_pushlstring(L, z, n);
    free(z);
    return 1;
  } else {
    luaL_error(L, "out of memory");
    __builtin_unreachable();
  }
}

int LuaVisualizeControlCodes(lua_State *L) {
  return LuaCoder(L, VisualizeControlCodes);
}

static dontinline int LuaHasherImpl(lua_State *L, size_t k,
                                    int H(const void *, size_t, uint8_t *)) {
  size_t n;
  uint8_t d[64];
  const void *p;
  if (!lua_isnoneornil(L, 1)) {
    p = luaL_checklstring(L, 1, &n);
    H(p, n, d);
    lua_pushlstring(L, (void *)d, k);
    mbedtls_platform_zeroize(d, sizeof(d));
    return 1;
  } else {
    return lua_gettop(L);
  }
}

static dontinline int LuaHasher(lua_State *L, size_t k,
                                int H(const void *, size_t, uint8_t *)) {
  return LuaHasherImpl(L, k, H);
}

int LuaMd5(lua_State *L) {
  return LuaHasher(L, 16, mbedtls_md5_ret);
}

int LuaSha1(lua_State *L) {
  return LuaHasher(L, 20, mbedtls_sha1_ret);
}

int LuaSha224(lua_State *L) {
  return LuaHasher(L, 28, mbedtls_sha256_ret_224);
}

int LuaSha256(lua_State *L) {
  return LuaHasher(L, 32, mbedtls_sha256_ret_256);
}

int LuaSha384(lua_State *L) {
  return LuaHasher(L, 48, mbedtls_sha512_ret_384);
}

int LuaSha512(lua_State *L) {
  return LuaHasher(L, 64, mbedtls_sha512_ret_512);
}

int LuaIsHeaderRepeatable(lua_State *L) {
  int h;
  bool r;
  size_t n;
  const char *s;
  s = luaL_checklstring(L, 1, &n);
  if ((h = GetHttpHeader(s, n)) != -1) {
    r = kHttpRepeatable[h];
  } else {
    r = false;
  }
  lua_pushboolean(L, r);
  return 1;
}

void LuaPushUrlView(lua_State *L, struct UrlView *v) {
  if (v->p) {
    lua_pushlstring(L, v->p, v->n);
  } else {
    lua_pushnil(L);
  }
}

static int64_t GetInterrupts(void) {
  struct rusage ru;
  if (!getrusage(RUSAGE_SELF, &ru)) {
    return ru.ru_nivcsw;
  } else {
    return 0;
  }
}

static int DoNothing(lua_State *L) {
  return 0;
}

int LuaBenchmark(lua_State *L) {
  uint64_t t1, t2;
  int64_t interrupts;
  double avgticks, overhead;
  int core, iter, count, attempts, maxattempts;
  luaL_checktype(L, 1, LUA_TFUNCTION);
  count = luaL_optinteger(L, 2, 100);
  maxattempts = luaL_optinteger(L, 3, 10);
  __warn_if_powersave();
  lua_gc(L, LUA_GCSTOP);

  for (attempts = 0;;) {
    lua_gc(L, LUA_GCCOLLECT);
    pthread_yield();
    core = TSC_AUX_CORE(Rdpid());
    interrupts = GetInterrupts();
    for (avgticks = iter = 1; iter < count; ++iter) {
      lua_pushcfunction(L, DoNothing);
      t1 = __startbench_m();
      lua_call(L, 0, 0);
      t2 = __endbench_m();
      avgticks += 1. / iter * ((int)(t2 - t1) - avgticks);
    }
    ++attempts;
    if (TSC_AUX_CORE(Rdpid()) == core && GetInterrupts() == interrupts) {
      break;
    } else if (attempts >= maxattempts) {
      luaL_error(L, "system is under too much load to run benchmark");
      __builtin_unreachable();
    }
  }
  overhead = avgticks;

  for (attempts = 0;;) {
    lua_gc(L, LUA_GCCOLLECT);
    pthread_yield();
    core = TSC_AUX_CORE(Rdpid());
    interrupts = GetInterrupts();
    for (avgticks = iter = 1; iter < count; ++iter) {
      lua_pushvalue(L, 1);
      t1 = __startbench_m();
      lua_call(L, 0, 0);
      t2 = __endbench_m();
      avgticks += 1. / iter * ((int)(t2 - t1) - avgticks);
    }
    ++attempts;
    if (TSC_AUX_CORE(Rdpid()) == core && GetInterrupts() == interrupts) {
      break;
    } else if (attempts >= maxattempts) {
      luaL_error(L, "system is under too much load to run benchmark");
      __builtin_unreachable();
    }
  }
  avgticks = MAX(avgticks - overhead, 0);

  lua_gc(L, LUA_GCRESTART);
  lua_pushinteger(L, avgticks / 3);
  lua_pushinteger(L, round(avgticks));
  lua_pushinteger(L, round(overhead));
  lua_pushinteger(L, attempts);
  return 4;
}

static void LuaCompress2(lua_State *L, void *dest, size_t *destLen,
                         const void *source, size_t sourceLen, int level) {
  switch (compress2(dest, destLen, source, sourceLen, level)) {
    case Z_OK:
      break;
    case Z_BUF_ERROR:
      luaL_error(L, "out of memory");
      __builtin_unreachable();
    case Z_STREAM_ERROR:
      luaL_error(L, "invalid level");
      __builtin_unreachable();
    default:
      __builtin_unreachable();
  }
}

// VERY DEPRECATED - PLEASE DO NOT USE
int LuaCompress(lua_State *L) {
  size_t n, m;
  char *q, *e;
  uint32_t crc;
  const char *p;
  luaL_Buffer buf;
  int level, hdrlen;
  p = luaL_checklstring(L, 1, &n);
  level = luaL_optinteger(L, 2, Z_DEFAULT_COMPRESSION);
  m = compressBound(n);
  if (lua_toboolean(L, 3)) {
    // raw mode
    q = luaL_buffinitsize(L, &buf, m);
    LuaCompress2(L, q, &m, p, n, level);
  } else {
    // easy mode
    q = luaL_buffinitsize(L, &buf, 10 + 4 + m);
    crc = crc32_z(0, p, n);
    e = uleb64(q, n);
    e = WRITE32LE(e, crc);
    hdrlen = e - q;
    LuaCompress2(L, q + hdrlen, &m, p, n, level);
    m += hdrlen;
  }
  luaL_pushresultsize(&buf, m);
  return 1;
}

// VERY DEPRECATED - PLEASE DO NOT USE
int LuaUncompress(lua_State *L) {
  int rc;
  char *q;
  uint32_t crc;
  const char *p;
  luaL_Buffer buf;
  size_t n, m, len;
  p = luaL_checklstring(L, 1, &n);
  if (lua_isnoneornil(L, 2)) {
    if ((rc = unuleb64(p, n, &m)) == -1 || n < rc + 4) {
      luaL_error(L, "compressed value too short to be valid");
      __builtin_unreachable();
    }
    len = m;
    crc = READ32LE(p + rc);
    q = luaL_buffinitsize(L, &buf, m);
    if (uncompress((void *)q, &m, (unsigned char *)p + rc + 4, n) != Z_OK ||
        m != len || crc32_z(0, q, m) != crc) {
      luaL_error(L, "compressed value is corrupted");
      __builtin_unreachable();
    }
  } else {
    len = m = luaL_checkinteger(L, 2);
    q = luaL_buffinitsize(L, &buf, m);
    if (uncompress((void *)q, &m, (void *)p, n) != Z_OK || m != len) {
      luaL_error(L, "compressed value is corrupted");
      __builtin_unreachable();
    }
  }
  luaL_pushresultsize(&buf, m);
  return 1;
}

// unix.deflate(uncompressed:str[, level:int])
//     ├─→ compressed:str
//     └─→ nil, error:str
int LuaDeflate(lua_State *L) {
  char *out;
  z_stream zs;
  int rc, level;
  const char *in;
  luaL_Buffer buf;
  size_t insize, outsize, actualoutsize;
  in = luaL_checklstring(L, 1, &insize);
  level = luaL_optinteger(L, 2, Z_DEFAULT_COMPRESSION);
  outsize = compressBound(insize);
  out = luaL_buffinitsize(L, &buf, outsize);

  zs.next_in = (const uint8_t *)in;
  zs.avail_in = insize;
  zs.next_out = (uint8_t *)out;
  zs.avail_out = outsize;
  zs.zalloc = Z_NULL;
  zs.zfree = Z_NULL;

  if ((rc = deflateInit2(&zs, level, Z_DEFLATED, -MAX_WBITS, MAX_MEM_LEVEL,
                         Z_DEFAULT_STRATEGY)) != Z_OK) {
    lua_pushnil(L);
    lua_pushfstring(L, "%s() failed: %d", "deflateInit2", rc);
    return 2;
  }

  rc = deflate(&zs, Z_FINISH);
  actualoutsize = outsize - zs.avail_out;
  deflateEnd(&zs);

  if (rc != Z_STREAM_END) {
    lua_pushnil(L);
    lua_pushfstring(L, "%s() failed: %d", "deflate", rc);
    return 2;
  }

  luaL_pushresultsize(&buf, actualoutsize);
  return 1;
}

// unix.inflate(compressed:str, maxoutsize:int)
//     ├─→ uncompressed:str
//     └─→ nil, error:str
int LuaInflate(lua_State *L) {
  int rc;
  char *out;
  z_stream zs;
  const char *in;
  luaL_Buffer buf;
  size_t insize, outsize, actualoutsize;
  in = luaL_checklstring(L, 1, &insize);
  outsize = luaL_checkinteger(L, 2);
  out = luaL_buffinitsize(L, &buf, outsize);

  zs.next_in = (const uint8_t *)in;
  zs.avail_in = insize;
  zs.total_in = insize;
  zs.next_out = (uint8_t *)out;
  zs.avail_out = outsize;
  zs.total_out = outsize;
  zs.zalloc = Z_NULL;
  zs.zfree = Z_NULL;

  if ((rc = inflateInit2(&zs, -MAX_WBITS)) != Z_OK) {
    lua_pushnil(L);
    lua_pushfstring(L, "%s() failed: %d", "inflateInit2", rc);
    return 2;
  }

  rc = inflate(&zs, Z_FINISH);
  actualoutsize = outsize - zs.avail_out;
  inflateEnd(&zs);

  if (rc != Z_STREAM_END) {
    lua_pushnil(L);
    lua_pushfstring(L, "%s() failed: %d", "inflate", rc);
    return 2;
  }

  luaL_pushresultsize(&buf, actualoutsize);
  return 1;
}

static void GetCurve25519Arg(lua_State *L, int arg, uint8_t buf[static 32]) {
  size_t len;
  const char *val;
  val = luaL_checklstring(L, arg, &len);
  bzero(buf, 32);
  if (len) {
    if (len > 32) {
      len = 32;
    }
    memcpy(buf, val, len);
  }
}

/*
 * Example usage:
 *
 *     >: secret1 = "\1"
 *     >: secret2 = "\2"
 *     >: public1 = Curve25519(secret1, "\9")
 *     >: public2 = Curve25519(secret2, "\9")
 *     >: Curve25519(secret1, public2)
 *     "\x93\xfe\xa2\xa7\xc1\xae\xb6,\xfddR\xff...
 *     >: Curve25519(secret2, public1)
 *     "\x93\xfe\xa2\xa7\xc1\xae\xb6,\xfddR\xff...
 *
 */
int LuaCurve25519(lua_State *L) {
  uint8_t mypublic[32], secret[32], basepoint[32];
  GetCurve25519Arg(L, 1, secret);
  GetCurve25519Arg(L, 2, basepoint);
  curve25519(mypublic, secret, basepoint);
  lua_pushlstring(L, (const char *)mypublic, 32);
  return 1;
}
