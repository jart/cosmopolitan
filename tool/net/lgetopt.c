/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2024 Justine Alexandra Roberts Tunney                              │
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
#include "tool/net/lgetopt.h"
#include "libc/mem/mem.h"
#include "libc/str/str.h"
#include "third_party/getopt/long1.h"
#include "third_party/getopt/long2.h"
#include "third_party/lua/lauxlib.h"
#include "third_party/lua/lua.h"

#define MAX_ARGC    10000
#define MAX_LONGOPTS 1000

static int ParseHasArg(const char *s) {
  if (!strcmp(s, "none"))
    return no_argument;
  if (!strcmp(s, "required"))
    return required_argument;
  if (!strcmp(s, "optional"))
    return optional_argument;
  return -1;
}

// getopt.parse(args, optstring, longopts) -> opts, remaining, unknown
//
// NOTE: This function uses global getopt state and is NOT thread-safe.
// Do not call from multiple threads or coroutines concurrently.
static int LuaGetoptParse(lua_State *L) {
  lua_Integer argc_raw, nlong_raw;
  int argc, nlong, opt, longidx, has_arg;
  const char *optstring;
  char **argv = NULL;
  struct option *longopts = NULL;
  const char *longname;
  char shortopt[2];
  int refs_idx, opts_idx, remaining_idx, unknown_idx;

  luaL_checktype(L, 1, LUA_TTABLE);
  optstring = luaL_checkstring(L, 2);
  if (!lua_isnoneornil(L, 3))
    luaL_checktype(L, 3, LUA_TTABLE);

  // Get sizes with bounds checking
  argc_raw = luaL_len(L, 1);
  if (argc_raw < 0 || argc_raw > MAX_ARGC)
    return luaL_error(L, "args table too large (max %d)", MAX_ARGC);
  argc = (int)argc_raw;

  nlong_raw = lua_isnoneornil(L, 3) ? 0 : luaL_len(L, 3);
  if (nlong_raw < 0 || nlong_raw > MAX_LONGOPTS)
    return luaL_error(L, "longopts table too large (max %d)", MAX_LONGOPTS);
  nlong = (int)nlong_raw;

  // Create a table to hold references to all Lua strings we extract.
  // This prevents the strings from being garbage collected while we
  // hold raw pointers to them in argv[] and longopts[].name.
  lua_newtable(L);
  refs_idx = lua_gettop(L);
  int ref_count = 0;

  // Validate all longopts entries BEFORE allocating C memory.
  // This ensures luaL_error won't leak memory.
  for (int i = 0; i < nlong; i++) {
    lua_rawgeti(L, 3, i + 1);
    if (!lua_istable(L, -1)) {
      return luaL_error(L, "longopt[%d] must be a table", i + 1);
    }
    // Check name is string
    lua_rawgeti(L, -1, 1);
    if (!lua_isstring(L, -1)) {
      return luaL_error(L, "longopt[%d][1] (name) must be a string", i + 1);
    }
    lua_pop(L, 1);
    // Check has_arg is valid
    lua_rawgeti(L, -1, 2);
    if (!lua_isstring(L, -1)) {
      return luaL_error(L, "longopt[%d][2] (has_arg) must be a string", i + 1);
    }
    has_arg = ParseHasArg(lua_tostring(L, -1));
    if (has_arg == -1) {
      return luaL_error(L,
          "longopt[%d][2] must be 'none', 'required', or 'optional'", i + 1);
    }
    lua_pop(L, 1);
    // Check short is string if present
    lua_rawgeti(L, -1, 3);
    if (!lua_isnoneornil(L, -1) && !lua_isstring(L, -1)) {
      return luaL_error(L, "longopt[%d][3] (short) must be a string or nil",
                        i + 1);
    }
    lua_pop(L, 1);
    lua_pop(L, 1);  // pop longopt entry
  }

  // Validate all args entries are strings
  for (int i = 1; i <= argc; i++) {
    lua_rawgeti(L, 1, i);
    if (!lua_isstring(L, -1)) {
      return luaL_error(L, "args[%d] must be a string", i);
    }
    lua_pop(L, 1);
  }

  // Now allocate C memory - all validation passed
  argv = calloc(argc + 2, sizeof(char *));
  if (!argv)
    return luaL_error(L, "out of memory");

  longopts = calloc(nlong + 1, sizeof(struct option));
  if (!longopts) {
    free(argv);
    return luaL_error(L, "out of memory");
  }

  // Extract argv strings, keeping them rooted in refs table
  argv[0] = "lua";
  for (int i = 1; i <= argc; i++) {
    lua_rawgeti(L, 1, i);
    argv[i] = (char *)lua_tostring(L, -1);
    // Store in refs table to prevent GC
    lua_rawseti(L, refs_idx, ++ref_count);
  }
  argv[argc + 1] = NULL;
  argc++;

  // Extract longopts, keeping strings rooted
  for (int i = 0; i < nlong; i++) {
    lua_rawgeti(L, 3, i + 1);

    lua_rawgeti(L, -1, 1);
    longopts[i].name = lua_tostring(L, -1);
    lua_rawseti(L, refs_idx, ++ref_count);  // keep rooted

    lua_rawgeti(L, -1, 2);
    longopts[i].has_arg = ParseHasArg(lua_tostring(L, -1));
    lua_pop(L, 1);

    lua_rawgeti(L, -1, 3);
    if (lua_isstring(L, -1)) {
      const char *s = lua_tostring(L, -1);
      longopts[i].val = s[0];
    } else {
      longopts[i].val = 0;
    }
    lua_pop(L, 1);

    longopts[i].flag = NULL;
    lua_pop(L, 1);  // pop longopt entry
  }

  // Reset getopt state
  optind = 1;
  opterr = 0;

  // Create result tables
  lua_newtable(L);  // opts
  opts_idx = lua_gettop(L);

  lua_newtable(L);  // unknown options
  unknown_idx = lua_gettop(L);
  int unknown_count = 0;

  // Parse options
  shortopt[1] = '\0';
  while ((opt = getopt_long(argc, argv, optstring, longopts, &longidx)) != -1) {
    if (opt == '?') {
      // Record unknown option
      if (optopt) {
        shortopt[0] = optopt;
        lua_pushstring(L, shortopt);
      } else if (argv[optind - 1]) {
        lua_pushstring(L, argv[optind - 1]);
      } else {
        continue;
      }
      lua_rawseti(L, unknown_idx, ++unknown_count);
      continue;
    }
    if (opt == 0) {
      // Long option with flag set
      longname = longopts[longidx].name;
      if (optarg) {
        lua_pushstring(L, optarg);
      } else {
        lua_pushboolean(L, 1);
      }
      lua_setfield(L, opts_idx, longname);
    } else {
      // Short option (or long option returning val)
      shortopt[0] = opt;
      if (optarg) {
        lua_pushstring(L, optarg);
      } else {
        lua_pushboolean(L, 1);
      }
      lua_setfield(L, opts_idx, shortopt);

      // Also set long name if this came from a long option
      for (int i = 0; i < nlong; i++) {
        if (longopts[i].val == opt) {
          if (optarg) {
            lua_pushstring(L, optarg);
          } else {
            lua_pushboolean(L, 1);
          }
          lua_setfield(L, opts_idx, longopts[i].name);
          break;
        }
      }
    }
  }

  // Create remaining args table
  lua_newtable(L);
  remaining_idx = lua_gettop(L);
  int j = 1;
  for (int i = optind; i < argc; i++) {
    lua_pushstring(L, argv[i]);
    lua_rawseti(L, remaining_idx, j++);
  }

  free(argv);
  free(longopts);

  // Return opts, remaining, unknown
  lua_pushvalue(L, opts_idx);
  lua_pushvalue(L, remaining_idx);
  lua_pushvalue(L, unknown_idx);

  return 3;
}

static const luaL_Reg kLuaGetopt[] = {
    {"parse", LuaGetoptParse},
    {0},
};

int LuaGetopt(lua_State *L) {
  luaL_newlib(L, kLuaGetopt);
  return 1;
}
