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
#include "libc/log/check.h"
#include "libc/log/log.h"
#include "libc/runtime/stack.h"
#include "libc/str/str.h"
#include "libc/str/tpenc.h"
#include "libc/str/utf16.h"
#include "third_party/double-conversion/wrapper.h"
#include "third_party/lua/cosmo.h"
#include "third_party/lua/lauxlib.h"
#include "third_party/lua/ltests.h"
#include "third_party/lua/lua.h"
#include "tool/net/ljson.h"

#define KEY    1
#define COMMA  2
#define COLON  4
#define ARRAY  8
#define OBJECT 16
#define DEPTH  64

static struct DecodeJson Parse(struct lua_State *L, const char *p,
                               const char *e, int context, int depth) {
  long x;
  char w[4];
  luaL_Buffer b;
  struct DecodeJson r;
  const char *a, *reason;
  int A, B, C, D, c, d, i, u;
  if (UNLIKELY(!depth)) {
    return (struct DecodeJson){-1, "maximum depth exceeded"};
  }
  if (UNLIKELY((intptr_t)__builtin_frame_address(0) <
               GetStackAddr() + PAGESIZE * 2)) {
    return (struct DecodeJson){-1, "out of stack"};
  }
  for (a = p, d = +1; p < e;) {
    switch ((c = *p++ & 255)) {
      case ' ':  // spaces
      case '\n':
      case '\r':
      case '\t':
        a = p;
        break;

      case ',':  // present in list and object
        if (context & COMMA) {
          context = 0;
          a = p;
          break;
        } else {
          return (struct DecodeJson){-1, "unexpected ','"};
        }

      case ':':  // present only in object after key
        if (context & COLON) {
          context = 0;
          a = p;
          break;
        } else {
          return (struct DecodeJson){-1, "unexpected ':'"};
        }

      case 'n':  // null
        if (context & (KEY | COLON | COMMA)) goto OnColonCommaKey;
        if (p + 3 <= e && READ32LE(p - 1) == READ32LE("null")) {
          lua_pushnil(L);
          return (struct DecodeJson){1, p + 3};
        } else {
          goto IllegalCharacter;
        }

      case 'f':  // false
        if (context & (KEY | COLON | COMMA)) goto OnColonCommaKey;
        if (p + 4 <= e && READ32LE(p) == READ32LE("alse")) {
          lua_pushboolean(L, false);
          return (struct DecodeJson){1, p + 4};
        } else {
          goto IllegalCharacter;
        }

      case 't':  // true
        if (context & (KEY | COLON | COMMA)) goto OnColonCommaKey;
        if (p + 3 <= e && READ32LE(p - 1) == READ32LE("true")) {
          lua_pushboolean(L, true);
          return (struct DecodeJson){1, p + 3};
        } else {
          goto IllegalCharacter;
        }

      default:
      IllegalCharacter:
        return (struct DecodeJson){-1, "illegal character"};
      OnColonCommaKey:
        if (context & KEY) goto BadObjectKey;
      OnColonComma:
        if (context & COLON) goto MissingColon;
        return (struct DecodeJson){-1, "missing ','"};
      MissingColon:
        return (struct DecodeJson){-1, "missing ':'"};
      BadObjectKey:
        return (struct DecodeJson){-1, "object key must be string"};

      case '-':  // negative
        if (context & (COLON | COMMA | KEY)) goto OnColonCommaKey;
        if (p < e && isdigit(*p)) {
          d = -1;
          break;
        } else {
          return (struct DecodeJson){-1, "bad negative"};
        }

      case '0':  // zero or number
        if (context & (COLON | COMMA | KEY)) goto OnColonCommaKey;
        if (p < e) {
          if (*p == '.') {
            if (p + 1 == e || !isdigit(p[1])) {
              return (struct DecodeJson){-1, "bad double"};
            }
            goto UseDubble;
          } else if (*p == 'e' || *p == 'E') {
            goto UseDubble;
          } else if (isdigit(*p)) {
            return (struct DecodeJson){-1, "unexpected octal"};
          }
        }
        lua_pushinteger(L, 0);
        return (struct DecodeJson){1, p};

      case '1' ... '9':  // integer
        if (context & (COLON | COMMA | KEY)) goto OnColonCommaKey;
        for (x = (c - '0') * d; p < e; ++p) {
          c = *p & 255;
          if (isdigit(c)) {
            if (__builtin_mul_overflow(x, 10, &x) ||
                __builtin_add_overflow(x, (c - '0') * d, &x)) {
              goto UseDubble;
            }
          } else if (c == '.') {
            if (p + 1 == e || !isdigit(p[1])) {
              return (struct DecodeJson){-1, "bad double"};
            }
            goto UseDubble;
          } else if (c == 'e' || c == 'E') {
            goto UseDubble;
          } else {
            break;
          }
        }
        lua_pushinteger(L, x);
        return (struct DecodeJson){1, p};

      UseDubble:  // number
        lua_pushnumber(L, StringToDouble(a, e - a, &c));
        DCHECK(c > 0, "paranoid avoiding infinite loop");
        if (a + c < e && (a[c] == 'e' || a[c] == 'E')) {
          return (struct DecodeJson){-1, "bad exponent"};
        }
        return (struct DecodeJson){1, a + c};

      case '[':  // Array
        if (context & (COLON | COMMA | KEY)) goto OnColonCommaKey;
        lua_newtable(L);  // +1
        for (context = ARRAY, i = 0;;) {
          r = Parse(L, p, e, context, depth - 1);  // +2
          if (UNLIKELY(r.rc == -1)) {
            lua_pop(L, 1);
            return r;
          }
          p = r.p;
          if (!r.rc) {
            break;
          }
          lua_rawseti(L, -2, i++ + 1);
          context = ARRAY | COMMA;
        }
        if (!i) {
          // we need this kludge so `[]` won't round-trip as `{}`
          lua_pushboolean(L, false);  // +2
          lua_rawseti(L, -2, 0);
        }
        return (struct DecodeJson){1, p};

      case ']':
        if (context & ARRAY) {
          return (struct DecodeJson){0, p};
        } else {
          return (struct DecodeJson){-1, "unexpected ']'"};
        }

      case '}':
        if (context & OBJECT) {
          return (struct DecodeJson){0, p};
        } else {
          return (struct DecodeJson){-1, "unexpected '}'"};
        }

      case '{':  // Object
        if (context & (COLON | COMMA | KEY)) goto OnColonCommaKey;
        lua_newtable(L);  // +1
        context = KEY | OBJECT;
        for (;;) {
          r = Parse(L, p, e, context, depth - 1);  // +2
          if (r.rc == -1) {
            lua_pop(L, 1);
            return r;
          }
          p = r.p;
          if (!r.rc) {
            return (struct DecodeJson){1, p};
          }
          r = Parse(L, p, e, COLON, depth - 1);  // +3
          if (r.rc == -1) {
            lua_pop(L, 2);
            return r;
          }
          if (!r.rc) {
            lua_pop(L, 2);
            return (struct DecodeJson){-1, "unexpected eof in object"};
          }
          p = r.p;
          lua_settable(L, -3);
          context = KEY | COMMA | OBJECT;
        }

      case '"':  // string
        if (context & (COLON | COMMA)) goto OnColonComma;
        luaL_buffinit(L, &b);
        for (;;) {
          if (UNLIKELY(p >= e)) {
          UnexpectedEofString:
            reason = "unexpected eof in string";
            goto StringFailureWithReason;
          }
          c = *p++ & 255;
          if (c == '"') {
            luaL_pushresult(&b);
            return (struct DecodeJson){1, p};
          } else if (c == '\\') {
            goto HandleEscape;
          } else if (UNLIKELY(c <= 0x1F)) {
            reason = "non-del c0 in string";
            goto StringFailureWithReason;
          } else {
            luaL_addchar(&b, c);
          }
          continue;
        HandleEscape:
          if (p >= e) {
            goto UnexpectedEofString;
          }
          switch ((c = *p++ & 255)) {
            case '"':
            case '/':
            case '\\':
              luaL_addchar(&b, c);
              break;
            case 'b':
              luaL_addchar(&b, '\b');
              break;
            case 'f':
              luaL_addchar(&b, '\f');
              break;
            case 'n':
              luaL_addchar(&b, '\n');
              break;
            case 'r':
              luaL_addchar(&b, '\r');
              break;
            case 't':
              luaL_addchar(&b, '\t');
              break;
            case 'x':
              if (p + 2 <= e &&                         //
                  (A = kHexToInt[p[0] & 255]) != -1 &&  // HEX
                  (B = kHexToInt[p[1] & 255]) != -1) {  //
                c = A << 4 | B;
                if (!(0x20 <= c && c <= 0x7E)) {
                  reason = "hex escape not printable";
                  goto StringFailureWithReason;
                }
                p += 2;
                luaL_addchar(&b, c);
                break;
              } else {
                reason = "invalid hex escape";
                goto StringFailureWithReason;
              }
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
                if (c <= 0x7f) {
                  w[0] = c;
                  i = 1;
                } else if (c <= 0x7ff) {
                  w[0] = 0300 | (c >> 6);
                  w[1] = 0200 | (c & 077);
                  i = 2;
                } else if (c <= 0xffff) {
                  if (IsSurrogate(c)) {
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
                reason = "invalid unicode escape";
                goto StringFailureWithReason;
              BadUnicode:
                // Echo invalid \uXXXX sequences
                // Rather than corrupting UTF-8!
                luaL_addstring(&b, "\\u");
              }
              break;
            default:
              reason = "invalid escape character";
              goto StringFailureWithReason;
          }
        }
        unreachable;
      StringFailureWithReason:
        luaL_pushresultsize(&b, 0);
        lua_pop(L, 1);
        return (struct DecodeJson){-1, reason};
    }
  }
  if (depth == DEPTH) {
    return (struct DecodeJson){0, 0};
  } else {
    return (struct DecodeJson){-1, "unexpected eof"};
  }
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
 * @param L is Lua interpreter state
 * @param p is input string
 * @param n is byte length of `p` or -1 for automatic strlen()
 * @return r.rc is 1 if value is pushed on lua stack
 * @return r.rc is 0 on eof
 * @return r.rc is -1 on error
 * @return r.p is is advanced `p` pointer if `rc ≥ 0`
 * @return r.p is string describing error if `rc < 0`
 */
struct DecodeJson DecodeJson(struct lua_State *L, const char *p, size_t n) {
  if (n == -1) n = p ? strlen(p) : 0;
  if (lua_checkstack(L, DEPTH * 3 + LUA_MINSTACK)) {
    return Parse(L, p, p + n, 0, DEPTH);
  } else {
    return (struct DecodeJson){-1, "can't set stack depth"};
  }
}
