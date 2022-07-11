/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
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
#include "libc/bits/bits.h"
#include "libc/bits/likely.h"
#include "libc/intrin/kprintf.h"
#include "libc/log/log.h"
#include "libc/str/tpenc.h"
#include "libc/str/utf16.h"
#include "third_party/double-conversion/wrapper.h"
#include "third_party/lua/lauxlib.h"
#include "third_party/lua/ltests.h"
#include "third_party/lua/lua.h"

#define MAX_JSON_DEPTH 1024

struct Rc {
  int t;
  const char *p;
};

static struct Rc Parse(struct lua_State *L, const char *p, const char *e) {
  long x;
  char w[4];
  struct Rc r;
  const char *a;
  luaL_Buffer b;
  int A, B, C, D, c, d, i, u;
  if (lua_gettop(L) >= MAX_JSON_DEPTH) {
     luaL_error(L, "maximum depth exceeded\n");
     return (struct Rc){-1, p};
  }
  for (a = p, d = +1; p < e;) {
    switch ((c = *p++ & 255)) {
      default:
        luaL_error(L, "illegal character\n");
        return (struct Rc){-1, p};

      case ' ':  // spaces
      case '\n':
      case '\r':
      case '\t':
        a = p;
        break;

      case ',':  // present in list and object
        a = p;
        break;

      case ':':  // present only in object after key
        if (LUA_TSTRING != lua_type(L, -1)) {
          luaL_error(L, "unexpected ':'\n");
          return (struct Rc){-1, p};
        }
        a = p;
        break;

      case 'n':  // null
        if (p + 3 <= e && READ32LE(p - 1) == READ32LE("null")) {
          lua_pushnil(L);
          return (struct Rc){1, p + 3};
        }
        luaL_error(L, "expecting null\n");
        return (struct Rc){-1, p};

      case 't':  // true
        if (p + 3 <= e && READ32LE(p - 1) == READ32LE("true")) {
          lua_pushboolean(L, true);
          return (struct Rc){1, p + 3};
        }
        luaL_error(L, "expecting true\n");
        return (struct Rc){-1, p};

      case 'f':  // false
        if (p + 4 <= e && READ32LE(p) == READ32LE("alse")) {
          lua_pushboolean(L, false);
          return (struct Rc){1, p + 4};
        }
        luaL_error(L, "expecting false\n");
        return (struct Rc){-1, p};

      case '-':  // negative
        d = -1;
        break;

      case '0':  // zero or number
        if (p < e && *p == '.') {
          goto UseDubble;
        }
        lua_pushinteger(L, 0);
        return (struct Rc){1, p};

      case '1' ... '9':  // integer
        for (x = (c - '0') * d; p < e; ++p) {
          c = *p & 255;
          if (isdigit(c)) {
            if (__builtin_mul_overflow(x, 10, &x) ||
                __builtin_add_overflow(x, (c - '0') * d, &x)) {
              goto UseDubble;
            }
          } else if (c == '.' || c == 'e' || c == 'E') {
            goto UseDubble;
          } else {
            break;
          }
        }
        lua_pushinteger(L, x);
        return (struct Rc){1, p};

      UseDubble:  // number
        lua_pushnumber(L, StringToDouble(a, e - a, &c));
        return (struct Rc){1, a + c};

      case '[':  // Array
        lua_newtable(L);
        i = 0;
        do {
          r = Parse(L, p, e);
          p = r.p;
          if (r.t) {
            lua_rawseti(L, -2, i++ + 1);
          }
        } while (r.t);
        if (*(p - 1) != ']') {
          luaL_error(L, "invalid list\n");
          return (struct Rc){-1, p};
        }
        return (struct Rc){1, p};

      case ']':
      case '}':
        return (struct Rc){0, p};

      case '{':  // Object
        lua_newtable(L);
        i = 0;
        do {
          r = Parse(L, p, e);
          p = r.p;
          if (r.t) {
            if (LUA_TSTRING != lua_type(L, -1)) {
              /* json keys can only be strings */
              lua_settop(L, -2);
              break;
            }
            r = Parse(L, p, e);
            p = r.p;
            if (!r.t) {
              /* key provided but no value */
              lua_settop(L, -2);
              luaL_error(L, "key provided but no value\n");
              return (struct Rc){-1, p};
            }
            lua_settable(L, -3);
            ++i;
          }
        } while (r.t);
        if (!i) {
          // we need this kludge so `{}` won't round-trip as `[]`
          lua_pushstring(L, "__json_object__");
          lua_pushboolean(L, true);
          lua_settable(L, -3);
        }
        if (*(p - 1) != '}') {
          luaL_error(L, "invalid object\n");
          return (struct Rc){-1, p};
        }
        return (struct Rc){1, p};

      case '"':  // string
        luaL_buffinit(L, &b);
        while (p < e) {
          switch ((c = *p++ & 255)) {
            default:
            AddByte:
              luaL_addchar(&b, c);
              break;
            case '\\':
              if (p < e) {
                switch ((c = *p++ & 255)) {
                  case '0':
                  case 'x':
                    luaL_error(L, "invalid escaped character\n");
                    return (struct Rc){-1, p};

                  case '"':
                  case '/':
                  case '\\':
                  default:
                    goto AddByte;
                  case 'b':
                    c = '\b';
                    goto AddByte;
                  case 'f':
                    c = '\f';
                    goto AddByte;
                  case 'n':
                    c = '\n';
                    goto AddByte;
                  case 'r':
                    c = '\r';
                    goto AddByte;
                  case 't':
                    c = '\t';
                    goto AddByte;
                  case 'u':
                    if (p + 4 <= e &&                         //
                        (A = kHexToInt[p[0] & 255]) != -1 &&  //
                        (B = kHexToInt[p[1] & 255]) != -1 &&  // UCS-2
                        (C = kHexToInt[p[2] & 255]) != -1 &&  //
                        (D = kHexToInt[p[3] & 255]) != -1) {  //
                      c = A << 12 | B << 8 | C << 4 | D;
                      if (!IsSurrogate(c)) {
                        p += 4;
                      } else if (IsHighSurrogate(c)) {
                        if (p + 4 + 6 <= e &&                     //
                            p[4] == '\\' &&                       //
                            p[5] == 'u' &&                        //
                            (A = kHexToInt[p[6] & 255]) != -1 &&  // UTF-16
                            (B = kHexToInt[p[7] & 255]) != -1 &&  //
                            (C = kHexToInt[p[8] & 255]) != -1 &&  //
                            (D = kHexToInt[p[9] & 255]) != -1) {  //
                          u = A << 12 | B << 8 | C << 4 | D;
                          if (IsLowSurrogate(u)) {
                            p += 4 + 6;
                            c = MergeUtf16(c, u);
                          } else {
                            goto BadUnicode;
                          }
                        } else {
                          goto BadUnicode;
                        }
                      } else {
                        goto BadUnicode;
                      }
                      // UTF-8
                      if (c < 0x7f) {
                        w[0] = c;
                        i = 1;
                      } else if (c <= 0x7ff) {
                        w[0] = 0300 | (c >> 6);
                        w[1] = 0200 | (c & 077);
                        i = 2;
                      } else if (c <= 0xffff) {
                        if (UNLIKELY(IsSurrogate(c))) {
                        ReplacementCharacter:
                          c = 0xfffd;
                        }
                        w[0] = 0340 | (c >> 12);
                        w[1] = 0200 | ((c >> 6) & 077);
                        w[2] = 0200 | (c & 077);
                        i = 3;
                      } else if (~(c >> 18) & 007) {
                        w[0] = 0360 | (c >> 18);
                        w[1] = 0200 | ((c >> 12) & 077);
                        w[2] = 0200 | ((c >> 6) & 077);
                        w[3] = 0200 | (c & 077);
                        i = 4;
                      } else {
                        goto ReplacementCharacter;
                      }
                      luaL_addlstring(&b, w, i);
                    } else {
                    BadUnicode:
                      // Echo invalid \uXXXX sequences
                      // Rather than corrupting UTF-8!
                      luaL_addstring(&b, "\\u");
                    }
                    break;
                }
              }
              break;
            case '"':
              goto FinishString;
          }
        }
      FinishString:
        luaL_pushresult(&b);
        return (struct Rc){1, p};
    }
  }
  return (struct Rc){0, p};
}

/**
 * Parses JSON data structure string into Lua data structure.
 *
 * This function returns the number of items pushed to the Lua stack,
 * which should be 1, unless no parseable JSON content was found, in
 * which case this will return 0. On error -1 is returned. There's
 * currently no error return condition. This function doesn't do JSON
 * validity enforcement.
 *
 * JSON UTF-16 strings are re-encoded as valid UTF-8. 64-bit integers
 * are supported. If an integer overflows during parsing, it'll be
 * converted to a floating-point number instead. Invalid surrogate
 * escape sequences in strings won't be decoded.
 *
 * A weird case exists when parsing empty objects. In order to let Lua
 * tell them apart from empty arrays, we insert a special key that's
 * ignored by our JSON serializer, called `[__json_object__]=true`.
 *
 * @param L is Lua interpreter state
 * @param p is input string
 * @param n is byte length of `p` or -1 for automatic strlen()
 * @return 1 if value was pushed, 0 on end, or -1 on error
 */
int DecodeJson(struct lua_State *L, const char *p, size_t n) {
  if (n == -1) n = p ? strlen(p) : 0;
  if(!lua_checkstack(L, MAX_JSON_DEPTH + MAX_JSON_DEPTH/2)) {
      luaL_error(L, "unable to set stack depth of %d\n", MAX_JSON_DEPTH + MAX_JSON_DEPTH/2);
      return -1;
  }
  return Parse(L, p, p + n).t;
}
