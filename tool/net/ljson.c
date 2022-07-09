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
#include "libc/errno.h"
#include "libc/limits.h"
#include "libc/math.h"
#include "libc/str/str.h"
#include "libc/str/tpenc.h"
#include "third_party/lua/lauxlib.h"
#include "third_party/lua/lua.h"

struct Rc {
  int t;
  const char *p;
};

static int Accumulate(int x, int c, int d) {
  if (!__builtin_mul_overflow(x, 10, &x) &&
      !__builtin_add_overflow(x, (c - '0') * d, &x)) {
    return x;
  } else {
    errno = ERANGE;
    if (d > 0) {
      return INT_MAX;
    } else {
      return INT_MIN;
    }
  }
}

static struct Rc Parse(struct lua_State *L, const char *p, const char *e) {
  uint64_t w;
  struct Rc r;
  luaL_Buffer b;
  int A, B, C, D;
  int c, d, i, j, x, y, z;
  for (d = +1; p < e;) {
    switch ((c = *p++ & 255)) {

      case '-':
        d = -1;
        break;

      case ']':
      case '}':
        return (struct Rc){0, p};

      case '[':
        lua_newtable(L);
        i = 0;
        do {
          r = Parse(L, p, e);
          p = r.p;
          if (r.t) {
            lua_rawseti(L, -2, i++ + 1);
          }
        } while (r.t);
        return (struct Rc){1, p};

      case '{':
        lua_newtable(L);
        i = 0;
        do {
          r = Parse(L, p, e);
          p = r.p;
          if (r.t) {
            r = Parse(L, p, e);
            p = r.p;
            if (!r.t) {
              lua_pushnil(L);
            }
            lua_settable(L, -3);
          }
        } while (r.t);
        return (struct Rc){1, p};

      case '"':
        luaL_buffinit(L, &b);
        while (p < e) {
          switch ((c = *p++ & 255)) {
            default:
            AddChar:
              luaL_addchar(&b, c);
              break;
            case '\\':
              if (p < e) {
                switch ((c = *p++ & 255)) {
                  case '"':
                  case '/':
                  case '\\':
                  default:
                    goto AddChar;
                  case 'b':
                    c = '\b';
                    goto AddChar;
                  case 'f':
                    c = '\f';
                    goto AddChar;
                  case 'n':
                    c = '\n';
                    goto AddChar;
                  case 'r':
                    c = '\r';
                    goto AddChar;
                  case 't':
                    c = '\t';
                    goto AddChar;
                  case 'u':
                    if (p + 4 <= e &&                         //
                        (A = kHexToInt[p[0] & 255]) != -1 &&  //
                        (B = kHexToInt[p[1] & 255]) != -1 &&  //
                        (C = kHexToInt[p[2] & 255]) != -1 &&  //
                        (D = kHexToInt[p[3] & 255]) != -1) {
                      p += 4;
                      c = A << 12 | B << 8 | C << 4 | D;
                      w = tpenc(c);
                      do {
                        luaL_addchar(&b, w & 255);
                      } while ((w >>= 8));
                      break;
                    } else {
                      goto AddChar;
                    }
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

      case '0' ... '9':
        for (x = (c - '0') * d; p < e; ++p) {
          c = *p & 255;
          if (isdigit(c)) {
            x = Accumulate(x, c, d);
          } else if (c == '.') {
            ++p;
            goto Fraction;
          } else if (c == 'e' || c == 'E') {
            ++p;
            j = 0;
            y = 0;
            goto Exponent;
          } else {
            break;
          }
        }
        lua_pushinteger(L, x);
        return (struct Rc){1, p};

      Fraction:
        for (j = y = 0; p < e; ++p) {
          c = *p & 255;
          if (isdigit(c)) {
            --j;
            y = Accumulate(y, c, d);
          } else if (c == 'e' || c == 'E') {
            ++p;
            goto Exponent;
          } else {
            break;
          }
        }
        lua_pushnumber(L, x + y * exp10(j));
        return (struct Rc){1, p};

      Exponent:
        d = +1;
        for (z = 0; p < e; ++p) {
          c = *p & 255;
          if (isdigit(c)) {
            z = Accumulate(z, c, d);
          } else if (c == '-') {
            d = -1;
          } else if (c == '+') {
            d = +1;
          } else {
            break;
          }
        }
        lua_pushnumber(L, (x + y * exp10(j)) * exp10(z));
        return (struct Rc){1, p};

      case ',':
      case ':':
      case ' ':
      case '\n':
      case '\r':
      case '\t':
      default:
        break;
    }
  }
  return (struct Rc){0, p};
}

/**
 * Parses JSON data structure string into a Lua data structure.
 */
int ParseJson(struct lua_State *L, const char *p, size_t n) {
  if (n == -1) n = p ? strlen(p) : 0;
  return Parse(L, p, p + n).t;
}
