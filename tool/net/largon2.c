/*-*- mode:c;indent-tabs-mode:t;c-basic-offset:8;tab-width:8;coding:utf-8   -*-│
│ vi: set et ft=c ts=8 sw=8 fenc=utf-8                                     :vi │
╚──────────────────────────────────────────────────────────────────────────────╝
│                                                                              │
│  largon2                                                                     │
│  Copyright © 2016 Thibault Charbonnier                                       │
│                                                                              │
│  Permission is hereby granted, free of charge, to any person obtaining       │
│  a copy of this software and associated documentation files (the             │
│  "Software"), to deal in the Software without restriction, including         │
│  without limitation the rights to use, copy, modify, merge, publish,         │
│  distribute, sublicense, and/or sell copies of the Software, and to          │
│  permit persons to whom the Software is furnished to do so, subject to       │
│  the following conditions:                                                   │
│                                                                              │
│  The above copyright notice and this permission notice shall be              │
│  included in all copies or substantial portions of the Software.             │
│                                                                              │
│  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,             │
│  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF          │
│  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.      │
│  IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY        │
│  CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,        │
│  TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE           │
│  SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                      │
│                                                                              │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "libc/isystem/stdio.h"
#include "libc/isystem/string.h"
#include "third_party/argon2/argon2.h"
#include "third_party/lua/lauxlib.h"
#include "third_party/lua/lua.h"
#include "third_party/lua/lualib.h"

asm(".ident\t\"\\n\\n\
largon2 (MIT License)\\n\
Copyright 2016 Thibault Charbonnier\"");
asm(".include \"libc/disclaimer.inc\"");

// clang-format off
/***
Lua C binding for the Argon2 password hashing function.
Compatible with Lua 5.x and LuaJIT.

See the [Argon2 documentation](https://github.com/P-H-C/phc-winner-argon2)
for in-depth instructions and details about Argon2.

This module's version is compatible with Argon2
[20161029](https://github.com/P-H-C/phc-winner-argon2/releases/tag/20161029)
and later.

Note: this document is also valid for the
[lua-argon2-ffi](https://github.com/thibaultcha/lua-argon2-ffi) module: an FFI
implementation of this binding for LuaJIT which uses the same API as this
original implementation.

@module argon2
@author Thibault Charbonnier
@license MIT
@release 3.0.1
*/


/***
Argon2 hashing variants. Those fields are `userdatums`, read-only values that
can be fed to the module's configuration or the `hash_encoded` function.
See the [Argon2 documentation](https://github.com/P-H-C/phc-winner-argon2)
for a description of those variants.
@field argon2_i
@field argon2_d
@field argon2_id
@table variants
*/


/***
Argon2 hashing options. Those options can be given to `hash_encoded` as a table.
If values are omitted, the default values of this module will be used.
Default values of this module can be overridden with `m_cost()`, `t_cost()`,
`parallelism()`, `hash_len()`, and `variant()`.
@field t_cost Number of iterations (`number`, default: `3`).
    argon2.hash_encoded("password", "salt", { t_cost = 4 })
Can be set to a new default in lua-argon2 (C binding only) by calling:
    argon2.t_cost(4)
@field m_cost Sets memory usage as KiB (`number`, default: `4096`).
    argon2.hash_encoded("password", "salt", {
      m_cost = math.pow(2, 16) -- 2^16 aka 65536 KiB
    })
Can be set to a new default in lua-argon2 (C binding only) by calling:
    argon2.m_cost(16)
@field parallelism Number of threads and compute lanes (`number`, default: `1`).
    argon2.hash_encoded("password", "salt", { parallelism = 2 })
Can be set to a new default in lua-argon2 (C binding only) by calling:
    argon2.parallelism(2)
@field hash_len Length of the hash output length (`number`, default: `32`).
    argon2.hash_encoded("password", "salt", { hash_len = 64 })
Can be set to a new default in lua-argon2 (C binding only) by calling:
    argon2.hash_len(64)
@field variant Choose the Argon2 variant to use (Argon2i, Argon2d, Argon2id)
from the `variants` table. (`userdata`, default: `argon2.variants.argon2_i`).
    argon2.hash_encoded("password", "salt", { variant = argon2.variants.argon2_d })
Can be set to a new default in lua-argon2 (C binding only) by calling:
    argon2.variant(argon2.variants.argon2_i)
    argon2.variant(argon2.variants.argon2_d)
    argon2.variant(argon2.variants.argon2_id)
@table options
*/
#define LUA_ARGON2_DEFAULT_T_COST 3
#define LUA_ARGON2_DEFAULT_M_COST 4096
#define LUA_ARGON2_DEFAULT_PARALLELISM 1
#define LUA_ARGON2_DEFAULT_HASH_LEN 32


typedef struct largon2_config_s largon2_config_t;
struct largon2_config_s {
    uint32_t         m_cost;
    uint32_t         t_cost;
    uint32_t         parallelism;
    uint32_t         hash_len;
    argon2_type      variant;
};


/* CONFIGURATION */


static void
largon2_create_config(lua_State *L)
{
    largon2_config_t *cfg;

    cfg              = lua_newuserdata(L, sizeof(*cfg));
    cfg->t_cost      = LUA_ARGON2_DEFAULT_T_COST;
    cfg->m_cost      = LUA_ARGON2_DEFAULT_M_COST;
    cfg->parallelism = LUA_ARGON2_DEFAULT_PARALLELISM;
    cfg->hash_len    = LUA_ARGON2_DEFAULT_HASH_LEN;
    cfg->variant     = Argon2_id;
}


static largon2_config_t *
largon2_fetch_config(lua_State *L)
{
    largon2_config_t *cfg;

    cfg = lua_touserdata(L, lua_upvalueindex(1));
    if (!cfg)
        luaL_error(L, "could not retrieve argon2 config");

    return cfg;
}


static largon2_config_t *
largon2_arg_init(lua_State *L, int nargs)
{
    if (lua_gettop(L) > nargs) {
        luaL_error(L, "expecting no more than %d arguments, but got %d",
                   nargs, lua_gettop(L));
    }

    lua_settop(L, nargs);

    return largon2_fetch_config(L);
}


static void
largon2_integer_opt(lua_State *L, uint32_t optidx, uint32_t argidx,
                    uint32_t *property, const char *key)
{
    uint32_t        value;
    char            errmsg[64];

    if (!lua_isnil(L, optidx)) {
        if (lua_isnumber(L, optidx)) {
            value = lua_tonumber(L, optidx);
            *property = value;

        } else {
            sprintf(errmsg, "expected %s to be a number, got %s",
                    key, luaL_typename(L, optidx));
            luaL_argerror(L, argidx, errmsg);
        }
    }
}


static int
largon2_cfg_t_cost(lua_State *L)
{
    largon2_config_t *cfg = largon2_arg_init(L, 1);

    largon2_integer_opt(L, 1, 1, &cfg->t_cost, "t_cost");
    lua_pushinteger(L, cfg->t_cost);

    return 1;
}


static int
largon2_cfg_m_cost(lua_State *L)
{
    largon2_config_t *cfg = largon2_arg_init(L, 1);

    largon2_integer_opt(L, 1, 1, &cfg->m_cost, "m_cost");
    lua_pushinteger(L, cfg->m_cost);

    return 1;
}


static int
largon2_cfg_parallelism(lua_State *L)
{
    largon2_config_t *cfg = largon2_arg_init(L, 1);

    largon2_integer_opt(L, 1, 1, &cfg->parallelism, "parallelism");
    lua_pushinteger(L, cfg->parallelism);

    return 1;
}


static int
largon2_cfg_hash_len(lua_State *L)
{
    largon2_config_t *cfg = largon2_arg_init(L, 1);

    largon2_integer_opt(L, 1, 1, &cfg->hash_len, "hash_len");
    lua_pushinteger(L, cfg->hash_len);

    return 1;
}


static int
largon2_cfg_variant(lua_State *L)
{
    largon2_config_t *cfg = largon2_arg_init(L, 1);

    luaL_checktype(L, 1, LUA_TLIGHTUSERDATA);

    cfg->variant = (argon2_type) lua_touserdata(L, 1);

    return 1;
}


/* BINDINGS */


/***
Hashes a password with Argon2i, Argon2d, or Argon2id, producing an encoded
hash.
@function hash_encoded
@param[type=string] plain Plain string to hash_encoded.
@param[type=string] salt Salt to use to hash the plain string.
@param[type=table] options Options with which to hash the plain string. See
`options`. This parameter is optional, if values are omitted the default ones
will be used.
@treturn string `encoded`: Encoded hash computed by Argon2, or `nil` if an
error occurred.
@treturn string `err`: `nil`, or a string describing the error if any.

@usage
local hash, err = argon2.hash_encoded("password", "somesalt")
if err then
  error("could not hash_encoded: " .. err)
end

-- with options and variant
local hash, err = argon2.hash_encoded("password", "somesalt", {
  t_cost = 4,
  m_cost = math.pow(2, 16), -- 65536 KiB
  variant = argon2.variants.argon2_d
})
*/
static int
largon2_hash_encoded(lua_State *L)
{
    const char             *plain, *salt;
    char                   *encoded, *err_msg;
    size_t                  plainlen, saltlen;
    size_t                  encoded_len;
    uint32_t                t_cost;
    uint32_t                m_cost;
    uint32_t                hash_len;
    uint32_t                parallelism;
    argon2_type             variant;
    argon2_error_codes      ret_code;
    largon2_config_t       *cfg;
    luaL_Buffer             buf;

    plain = luaL_checklstring(L, 1, &plainlen);
    salt  = luaL_checklstring(L, 2, &saltlen);

    cfg = largon2_arg_init(L, 3);

    t_cost      = cfg->t_cost;
    m_cost      = cfg->m_cost;
    parallelism = cfg->parallelism;
    hash_len    = cfg->hash_len;
    variant     = cfg->variant;

    if (!lua_isnil(L, 3)) {
        if (!lua_istable(L, 3)) {
            luaL_argerror(L, 3, "expected to be a table");
        }

        lua_getfield(L, 3, "t_cost");
        largon2_integer_opt(L, -1, 3, &t_cost, "t_cost");
        lua_pop(L, 1);

        lua_getfield(L, 3, "m_cost");
        largon2_integer_opt(L, -1, 3, &m_cost, "m_cost");
        lua_pop(L, 1);

        lua_getfield(L, 3, "parallelism");
        largon2_integer_opt(L, -1, 3, &parallelism, "parallelism");
        lua_pop(L, 1);

        lua_getfield(L, 3, "hash_len");
        largon2_integer_opt(L, -1, 3, &hash_len, "hash_len");
        lua_pop(L, 1);

        lua_getfield(L, 3, "variant");
        if (!lua_isnil(L, -1)) {
            if (!lua_islightuserdata(L, -1)) {
                char errmsg[64];
                sprintf(errmsg, "expected variant to be a number, got %s",
                        luaL_typename(L, -1));
                luaL_argerror(L, 3, errmsg);
            }

            variant = (argon2_type) lua_touserdata(L, -1);
        }

        lua_pop(L, 1);
    }

    encoded_len = argon2_encodedlen(t_cost, m_cost, parallelism, saltlen,
                                    hash_len, variant);

    encoded = luaL_buffinitsize(L, &buf, encoded_len);

    if (variant == Argon2_d) {
        ret_code =
          argon2d_hash_encoded(t_cost, m_cost, parallelism, plain, plainlen,
                               salt, saltlen, hash_len, encoded, encoded_len);

    } else if (variant == Argon2_id) {
        ret_code =
          argon2id_hash_encoded(t_cost, m_cost, parallelism, plain, plainlen,
                                salt, saltlen, hash_len, encoded, encoded_len);

    } else {
        ret_code =
          argon2i_hash_encoded(t_cost, m_cost, parallelism, plain, plainlen,
                               salt, saltlen, hash_len, encoded, encoded_len);
    }

    luaL_pushresultsize(&buf, encoded_len - 1);

    if (ret_code != ARGON2_OK) {
        err_msg = (char *) argon2_error_message(ret_code);
        lua_pushnil(L);
        lua_pushstring(L, err_msg);
        return 2;
    }

    return 1;
}


/***
Verifies a password against an encoded string.
@function verify
@param[type=string] encoded Encoded string to verify the plain password against.
@param[type=string] password Plain password to verify.
@treturn boolean `ok`: `true` if the password matches, `false` if it is a
mismatch. If an error occurs during the verification, will be `nil`.
@treturn string `err`: `nil`, or a string describing the error if any. A
password mismatch will not return an error, but will return `ok = false`
instead.

@usage
local ok, err = argon2.verify(argon2i_hash, "password")
if err then
  -- failure to verify (*not* a password mismatch)
  error("could not verify: " .. err)
end

if not ok then
  -- password mismatch
  error("The password does not match the supplied hash")
end

-- with a argon2d hash
local ok, err = argon2.verify(argon2d_hash, "password")
*/
static int
largon2_verify(lua_State *L)
{
    const char             *plain, *encoded;
    size_t                  plainlen;
    argon2_type             variant;
    argon2_error_codes      ret_code;
    char                   *err_msg;

    if (lua_gettop(L) != 2) {
        return luaL_error(L, "expecting 2 arguments, but got %d",
                          lua_gettop(L));
    }

    encoded = luaL_checkstring(L, 1);
    plain   = luaL_checklstring(L, 2, &plainlen);

    if (strstr(encoded, "argon2d")) {
        variant = Argon2_d;

    } else if (strstr(encoded, "argon2id")) {
        variant = Argon2_id;

    } else {
        variant = Argon2_i;
    }

    ret_code = argon2_verify(encoded, plain, plainlen, variant);
    if (ret_code == ARGON2_VERIFY_MISMATCH) {
        lua_pushboolean(L, 0);
        return 1;
    }

    if (ret_code != ARGON2_OK) {
        err_msg = (char *) argon2_error_message(ret_code);
        lua_pushnil(L);
        lua_pushstring(L, err_msg);
        return 2;
    }

    lua_pushboolean(L, 1);

    return 1;
}


/* MODULE */


static void
largon2_push_argon2_variants_table(lua_State *L)
{
    lua_newtable(L);

    lua_pushlightuserdata(L, (void *) Argon2_i);
    lua_setfield(L, -2, "argon2_i");

    lua_pushlightuserdata(L, (void *) Argon2_d);
    lua_setfield(L, -2, "argon2_d");

    lua_pushlightuserdata(L, (void *) Argon2_id);
    lua_setfield(L, -2, "argon2_id");

    return;
}


static const luaL_Reg largon2[] = { { "verify", largon2_verify },
                                    { "hash_encoded", largon2_hash_encoded },
                                    { "t_cost", largon2_cfg_t_cost },
                                    { "m_cost", largon2_cfg_m_cost },
                                    { "parallelism", largon2_cfg_parallelism },
                                    { "hash_len", largon2_cfg_hash_len },
                                    { "variant", largon2_cfg_variant },
                                    { NULL, NULL } };


int
luaopen_argon2(lua_State *L)
{
    lua_newtable(L);

    largon2_create_config(L);
    luaL_setfuncs(L, largon2, 1);

    /* push argon2.variants table */

    largon2_push_argon2_variants_table(L);
    lua_setfield(L, -2, "variants");

    lua_pushliteral(L, "3.0.1");
    lua_setfield(L, -2, "_VERSION");

    lua_pushliteral(L, "Thibault Charbonnier");
    lua_setfield(L, -2, "_AUTHOR");

    lua_pushliteral(L, "MIT");
    lua_setfield(L, -2, "_LICENSE");

    lua_pushliteral(L, "https://github.com/thibaultcha/lua-argon2");
    lua_setfield(L, -2, "_URL");

    return 1;
}
