/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2021 Justine Alexandra Roberts Tunney                              │
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
#include "third_party/lua/lunix_internal.h"
#include "libc/assert.h"
#include "libc/atomic.h"
#include "libc/calls/cp.internal.h"
#include "libc/cosmo.h"
#include "libc/errno.h"
#include "libc/intrin/atomic.h"
#include "libc/limits.h"
#include "libc/macros.h"
#include "libc/runtime/sysconf.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/clock.h"
#include "libc/sysv/consts/map.h"
#include "libc/thread/thread.h"
#include "libc/x/x.h"

////////////////////////////////////////////////////////////////////////////////
// unix.Memory object

struct Memory {
  union {
    char *bytes;
    atomic_long *words;
  } u;
  size_t size;
  void *map;
  size_t mapsize;
  pthread_mutex_t *lock;
};

// unix.Memory:read([offset:int[, bytes:int]])
//     └─→ str
static int LuaUnixMemoryRead(lua_State *L) {
  size_t i, n;
  struct Memory *m;
  m = luaL_checkudata(L, 1, "unix.Memory");
  i = luaL_optinteger(L, 2, 0);
  if (lua_isnoneornil(L, 3)) {
    // unix.Memory:read([offset:int])
    // extracts nul-terminated string
    if (i > m->size) {
      luaL_error(L, "out of range");
      __builtin_unreachable();
    }
    n = strnlen(m->u.bytes + i, m->size - i);
  } else {
    // unix.Memory:read(offset:int, bytes:int)
    // read binary data with boundary checking
    n = luaL_checkinteger(L, 3);
    if (i > m->size || n >= m->size || i + n > m->size) {
      luaL_error(L, "out of range");
      __builtin_unreachable();
    }
  }
  pthread_mutex_lock(m->lock);
  lua_pushlstring(L, m->u.bytes + i, n);
  pthread_mutex_unlock(m->lock);
  return 1;
}

// unix.Memory:write([offset:int,] data:str[, bytes:int])
static int LuaUnixMemoryWrite(lua_State *L) {
  int b;
  const char *s;
  size_t i, n, j;
  struct Memory *m;
  m = luaL_checkudata(L, 1, "unix.Memory");
  if (!lua_isnumber(L, 2)) {
    // unix.Memory:write(data:str[, bytes:int])
    i = 0;
    s = luaL_checklstring(L, 2, &n);
    b = 3;
  } else {
    // unix.Memory:write(offset:int, data:str[, bytes:int])
    i = luaL_checkinteger(L, 2);
    s = luaL_checklstring(L, 3, &n);
    b = 4;
  }
  if (i > m->size) {
    luaL_error(L, "out of range");
    __builtin_unreachable();
  }
  if (lua_isnoneornil(L, b)) {
    // unix.Memory:write(data:str[, offset:int])
    // writes binary data, plus a nul terminator
    if (i < n && n < m->size) {
      // include lua string's implicit nul so this round trips with
      // unix.Memory:read(offset:int) even when we're overwriting a
      // larger string that was previously inserted
      n += 1;
    } else {
      // there's no room to include the implicit nul-terminator so
      // leave it out which is safe b/c Memory:read() uses strnlen
    }
  } else {
    // unix.Memory:write(data:str, offset:int, bytes:int])
    // writes binary data without including nul-terminator
    j = luaL_checkinteger(L, b);
    if (j > n) {
      luaL_argerror(L, b, "bytes is more than what's in data");
      __builtin_unreachable();
    }
    n = j;
  }
  if (i + n > m->size) {
    luaL_error(L, "out of range");
    __builtin_unreachable();
  }
  pthread_mutex_lock(m->lock);
  memcpy(m->u.bytes + i, s, n);
  pthread_mutex_unlock(m->lock);
  return 0;
}

static atomic_long *GetWord(lua_State *L) {
  size_t i;
  struct Memory *m;
  m = luaL_checkudata(L, 1, "unix.Memory");
  i = luaL_checkinteger(L, 2);
  if (i >= m->size / sizeof(*m->u.words)) {
    luaL_error(L, "out of range");
    __builtin_unreachable();
  }
  return m->u.words + i;
}

// unix.Memory:load(word_index:int)
//     └─→ int
static int LuaUnixMemoryLoad(lua_State *L) {
  lua_pushinteger(L, atomic_load_explicit(GetWord(L), memory_order_relaxed));
  return 1;
}

// unix.Memory:store(word_index:int, value:int)
static int LuaUnixMemoryStore(lua_State *L) {
  atomic_store_explicit(GetWord(L), luaL_checkinteger(L, 3),
                        memory_order_relaxed);
  return 0;
}

// unix.Memory:xchg(word_index:int, value:int)
//     └─→ int
static int LuaUnixMemoryXchg(lua_State *L) {
  lua_pushinteger(L, atomic_exchange(GetWord(L), luaL_checkinteger(L, 3)));
  return 1;
}

// unix.Memory:cmpxchg(word_index:int, old:int, new:int)
//     └─→ success:bool, old:int
static int LuaUnixMemoryCmpxchg(lua_State *L) {
  long old = luaL_checkinteger(L, 3);
  lua_pushboolean(L, atomic_compare_exchange_strong(GetWord(L), &old,
                                                    luaL_checkinteger(L, 4)));
  lua_pushinteger(L, old);
  return 2;
}

// unix.Memory:add(word_index:int, value:int)
//     └─→ old:int
static int LuaUnixMemoryAdd(lua_State *L) {
  lua_pushinteger(L, atomic_fetch_add(GetWord(L), luaL_checkinteger(L, 3)));
  return 1;
}

// unix.Memory:and(word_index:int, value:int)
//     └─→ old:int
static int LuaUnixMemoryAnd(lua_State *L) {
  lua_pushinteger(L, atomic_fetch_and(GetWord(L), luaL_checkinteger(L, 3)));
  return 1;
}

// unix.Memory:or(word_index:int, value:int)
//     └─→ old:int
static int LuaUnixMemoryOr(lua_State *L) {
  lua_pushinteger(L, atomic_fetch_or(GetWord(L), luaL_checkinteger(L, 3)));
  return 1;
}

// unix.Memory:xor(word_index:int, value:int)
//     └─→ old:int
static int LuaUnixMemoryXor(lua_State *L) {
  lua_pushinteger(L, atomic_fetch_xor(GetWord(L), luaL_checkinteger(L, 3)));
  return 1;
}

// unix.Memory:wait(word_index:int, expect:int[, abs_deadline:int[, nanos:int]])
//     ├─→ 0
//     ├─→ nil, unix.Errno(unix.EINTR)
//     ├─→ nil, unix.Errno(unix.EAGAIN)
//     └─→ nil, unix.Errno(unix.ETIMEDOUT)
static int LuaUnixMemoryWait(lua_State *L) {
  lua_Integer expect;
  int rc, olderr = errno;
  struct timespec ts, *deadline;
  expect = luaL_checkinteger(L, 3);
  if (!(INT32_MIN <= expect && expect <= INT32_MAX)) {
    luaL_argerror(L, 3, "must be an int32_t");
    __builtin_unreachable();
  }
  if (lua_isnoneornil(L, 4)) {
    deadline = 0;  // wait forever
  } else {
    ts.tv_sec = luaL_checkinteger(L, 4);
    ts.tv_nsec = luaL_optinteger(L, 5, 0);
    deadline = &ts;
  }
  BEGIN_CANCELATION_POINT;
  rc = cosmo_futex_wait((atomic_int *)GetWord(L), expect,
                         PTHREAD_PROCESS_SHARED, CLOCK_REALTIME, deadline);
  END_CANCELATION_POINT;
  if (rc < 0) errno = -rc, rc = -1;
  return SysretInteger(L, "futex_wait", olderr, rc);
}

// unix.Memory:wake(index:int[, count:int])
//     └─→ woken:int
static int LuaUnixMemoryWake(lua_State *L) {
  int count, woken;
  count = luaL_optinteger(L, 3, INT_MAX);
  woken = cosmo_futex_wake((atomic_int *)GetWord(L), count,
                            PTHREAD_PROCESS_SHARED);
  npassert(woken >= 0);
  return ReturnInteger(L, woken);
}

static int LuaUnixMemoryTostring(lua_State *L) {
  char s[128];
  struct Memory *m;
  m = luaL_checkudata(L, 1, "unix.Memory");
  snprintf(s, sizeof(s), "unix.Memory(%zu)", m->size);
  lua_pushstring(L, s);
  return 1;
}

static int LuaUnixMemoryGc(lua_State *L) {
  struct Memory *m;
  m = luaL_checkudata(L, 1, "unix.Memory");
  if (m->u.bytes) {
    npassert(!munmap(m->map, m->mapsize));
    m->u.bytes = 0;
  }
  return 0;
}

static const luaL_Reg kLuaUnixMemoryMeth[] = {
    {"read", LuaUnixMemoryRead},        //
    {"write", LuaUnixMemoryWrite},      //
    {"load", LuaUnixMemoryLoad},        //
    {"store", LuaUnixMemoryStore},      //
    {"xchg", LuaUnixMemoryXchg},        //
    {"cmpxchg", LuaUnixMemoryCmpxchg},  //
    {"fetch_add", LuaUnixMemoryAdd},    //
    {"fetch_and", LuaUnixMemoryAnd},    //
    {"fetch_or", LuaUnixMemoryOr},      //
    {"fetch_xor", LuaUnixMemoryXor},    //
    {"wait", LuaUnixMemoryWait},        //
    {"wake", LuaUnixMemoryWake},        //
    {0},                                //
};

static const luaL_Reg kLuaUnixMemoryMeta[] = {
    {"__tostring", LuaUnixMemoryTostring},  //
    {"__repr", LuaUnixMemoryTostring},      //
    {"__gc", LuaUnixMemoryGc},              //
    {0},                                    //
};

void LuaUnixMemoryObj(lua_State *L) {
  luaL_newmetatable(L, "unix.Memory");
  luaL_setfuncs(L, kLuaUnixMemoryMeta, 0);
  luaL_newlibtable(L, kLuaUnixMemoryMeth);
  luaL_setfuncs(L, kLuaUnixMemoryMeth, 0);
  lua_setfield(L, -2, "__index");
  lua_pop(L, 1);
}

int LuaUnixMapshared(lua_State *L) {
  char *p;
  size_t n, c, g;
  struct Memory *m;
  pthread_mutexattr_t mattr;
  n = luaL_checkinteger(L, 1);
  if (!n) {
    luaL_error(L, "can't map empty region");
    __builtin_unreachable();
  }
  if (n % sizeof(long)) {
    luaL_error(L, "size must be multiple of word size");
    __builtin_unreachable();
  }
  if (n >= 0x100000000000) {
    luaL_error(L, "map size too big");
    __builtin_unreachable();
  }
  c = n;
  c += sizeof(*m->lock);
  g = sysconf(_SC_PAGESIZE);
  c = ROUNDUP(c, g);
  if (!(p = _mapshared(c))) {
    luaL_error(L, "out of memory");
    __builtin_unreachable();
  }
  m = lua_newuserdatauv(L, sizeof(*m), 1);
  luaL_setmetatable(L, "unix.Memory");
  m->u.bytes = p + sizeof(*m->lock);
  m->size = n;
  m->map = p;
  m->mapsize = c;
  m->lock = (pthread_mutex_t *)p;
  pthread_mutexattr_init(&mattr);
  pthread_mutexattr_settype(&mattr, PTHREAD_MUTEX_DEFAULT);
  pthread_mutexattr_setpshared(&mattr, PTHREAD_PROCESS_SHARED);
  pthread_mutex_init(m->lock, &mattr);
  pthread_mutexattr_destroy(&mattr);
  return 1;
}
