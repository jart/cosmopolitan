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
#define PARSER_MT "getopt.parser"

typedef struct {
  char **argv;
  struct option *longopts;
  int argc;
  int nlong;
  int refs_idx;        // Lua registry index for string references
  int unknown_idx;     // Lua registry index for unknown options table
  int unknown_count;   // Count of unknown options
  const char *optstring;
  int done;            // Flag indicating parsing is complete
} GetoptParser;

static int ParseHasArg(const char *s) {
  if (!strcmp(s, "none"))
    return no_argument;
  if (!strcmp(s, "required"))
    return required_argument;
  if (!strcmp(s, "optional"))
    return optional_argument;
  return -1;
}

// getopt.new(args, optstring, longopts) -> parser
//
// Creates a new getopt parser that can be iterated to parse options.
// NOTE: Uses global getopt state and is NOT thread-safe.
static int LuaGetoptNew(lua_State *L) {
  lua_Integer argc_raw, nlong_raw;
  int argc, nlong, has_arg;
  const char *optstring;
  char **argv = NULL;
  struct option *longopts = NULL;
  GetoptParser *parser;

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

  // Validate all longopts entries BEFORE allocating C memory
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

  // Allocate C memory
  argv = calloc(argc + 2, sizeof(char *));
  if (!argv)
    return luaL_error(L, "out of memory");

  longopts = calloc(nlong + 1, sizeof(struct option));
  if (!longopts) {
    free(argv);
    return luaL_error(L, "out of memory");
  }

  // Create parser userdata
  parser = lua_newuserdata(L, sizeof(GetoptParser));
  luaL_setmetatable(L, PARSER_MT);

  parser->argv = argv;
  parser->longopts = longopts;
  parser->argc = argc + 1;
  parser->nlong = nlong;
  parser->optstring = optstring;
  parser->done = 0;
  parser->unknown_count = 0;

  // Create a table to hold references to all Lua strings
  lua_newtable(L);
  parser->refs_idx = luaL_ref(L, LUA_REGISTRYINDEX);

  // Create unknown options table
  lua_newtable(L);
  parser->unknown_idx = luaL_ref(L, LUA_REGISTRYINDEX);

  int ref_count = 0;

  // Extract argv strings, keeping them rooted in refs table
  lua_rawgeti(L, LUA_REGISTRYINDEX, parser->refs_idx);
  int refs_stack = lua_gettop(L);

  argv[0] = "lua";
  for (int i = 1; i <= argc; i++) {
    lua_rawgeti(L, 1, i);
    argv[i] = (char *)lua_tostring(L, -1);
    lua_rawseti(L, refs_stack, ++ref_count);
  }
  argv[argc + 1] = NULL;

  lua_pop(L, 1);  // pop refs table

  // Extract longopts, keeping strings rooted
  if (nlong > 0) {
    lua_rawgeti(L, LUA_REGISTRYINDEX, parser->refs_idx);
    refs_stack = lua_gettop(L);

    for (int i = 0; i < nlong; i++) {
      lua_rawgeti(L, 3, i + 1);

      lua_rawgeti(L, -1, 1);
      longopts[i].name = lua_tostring(L, -1);
      lua_rawseti(L, refs_stack, ++ref_count);

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

    lua_pop(L, 1);  // pop refs table
  }

  // Reset getopt state
  optind = 1;
  opterr = 0;

  // Return parser (already on stack)
  return 1;
}

// parser:next() -> opt, arg
//
// Returns the next option and its argument (if any).
// Returns nil when no more options.
static int LuaGetoptNext(lua_State *L) {
  GetoptParser *parser = luaL_checkudata(L, 1, PARSER_MT);

  if (parser->done) {
    lua_pushnil(L);
    return 1;
  }

  int opt, longidx;
  char shortopt[2];
  shortopt[1] = '\0';

  opt = getopt_long(parser->argc, parser->argv, parser->optstring,
                    parser->longopts, &longidx);

  if (opt == -1) {
    parser->done = 1;
    lua_pushnil(L);
    return 1;
  }

  if (opt == '?') {
    // Unknown option - record it and return "?" with the option name
    lua_rawgeti(L, LUA_REGISTRYINDEX, parser->unknown_idx);
    if (optopt) {
      shortopt[0] = optopt;
      lua_pushstring(L, shortopt);
    } else if (parser->argv[optind - 1]) {
      lua_pushstring(L, parser->argv[optind - 1]);
    } else {
      lua_pop(L, 1);  // pop unknown table
      // Skip this unknown option and try next
      return LuaGetoptNext(L);
    }
    lua_rawseti(L, -2, ++parser->unknown_count);
    lua_pop(L, 1);  // pop unknown table

    // Return "?" and the unknown option name
    lua_pushstring(L, "?");
    if (optopt) {
      shortopt[0] = optopt;
      lua_pushstring(L, shortopt);
    } else if (parser->argv[optind - 1]) {
      lua_pushstring(L, parser->argv[optind - 1]);
    }
    return 2;
  }

  if (opt == 0) {
    // Long option with flag set
    const char *longname = parser->longopts[longidx].name;
    lua_pushstring(L, longname);
    if (optarg) {
      lua_pushstring(L, optarg);
      return 2;
    }
    return 1;
  }

  // Short option (or long option returning val)
  shortopt[0] = opt;
  lua_pushstring(L, shortopt);
  if (optarg) {
    lua_pushstring(L, optarg);
    return 2;
  }
  return 1;
}

// parser:remaining() -> table
//
// Returns a table of remaining non-option arguments.
static int LuaGetoptRemaining(lua_State *L) {
  GetoptParser *parser = luaL_checkudata(L, 1, PARSER_MT);

  lua_newtable(L);
  int j = 1;
  for (int i = optind; i < parser->argc; i++) {
    lua_pushstring(L, parser->argv[i]);
    lua_rawseti(L, -2, j++);
  }
  return 1;
}

// parser:unknown() -> table
//
// Returns a table of unknown options encountered.
static int LuaGetoptUnknown(lua_State *L) {
  GetoptParser *parser = luaL_checkudata(L, 1, PARSER_MT);
  lua_rawgeti(L, LUA_REGISTRYINDEX, parser->unknown_idx);
  return 1;
}

// Garbage collection for parser
static int LuaGetoptParserGC(lua_State *L) {
  GetoptParser *parser = luaL_checkudata(L, 1, PARSER_MT);

  if (parser->argv) {
    free(parser->argv);
    parser->argv = NULL;
  }
  if (parser->longopts) {
    free(parser->longopts);
    parser->longopts = NULL;
  }

  // Unref the tables
  luaL_unref(L, LUA_REGISTRYINDEX, parser->refs_idx);
  luaL_unref(L, LUA_REGISTRYINDEX, parser->unknown_idx);

  return 0;
}

static const luaL_Reg kLuaGetoptParserMethods[] = {
    {"next", LuaGetoptNext},
    {"remaining", LuaGetoptRemaining},
    {"unknown", LuaGetoptUnknown},
    {0},
};

static const luaL_Reg kLuaGetopt[] = {
    {"new", LuaGetoptNew},
    {0},
};

int LuaGetopt(lua_State *L) {
  // Create parser metatable
  luaL_newmetatable(L, PARSER_MT);
  lua_pushvalue(L, -1);
  lua_setfield(L, -2, "__index");
  lua_pushcfunction(L, LuaGetoptParserGC);
  lua_setfield(L, -2, "__gc");
  luaL_setfuncs(L, kLuaGetoptParserMethods, 0);
  lua_pop(L, 1);

  // Create getopt module table
  luaL_newlib(L, kLuaGetopt);
  return 1;
}
