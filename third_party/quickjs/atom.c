/*
 * QuickJS Javascript Engine
 *
 * Copyright (c) 2017-2021 Fabrice Bellard
 * Copyright (c) 2017-2021 Charlie Gordon
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
#include "libc/assert.h"
#include "libc/fmt/fmt.h"
#include "libc/str/str.h"
#include "third_party/quickjs/internal.h"
#include "third_party/quickjs/libregexp.h"
#include "third_party/quickjs/quickjs.h"

asm(".ident\t\"\\n\\n\
QuickJS (MIT License)\\n\
Copyright (c) 2017-2021 Fabrice Bellard\\n\
Copyright (c) 2017-2021 Charlie Gordon\"");
asm(".include \"libc/disclaimer.inc\"");
/* clang-format off */

/* Should only be used for debug. */
const char *JS_AtomGetStrRT(JSRuntime *rt, char *buf, int buf_size, JSAtom atom)
{
    if (__JS_AtomIsTaggedInt(atom)) {
        snprintf(buf, buf_size, "%u", __JS_AtomToUInt32(atom));
    } else {
        JSAtomStruct *p;
        assert(atom < rt->atom_size);
        if (atom == JS_ATOM_NULL) {
            snprintf(buf, buf_size, "<null>");
        } else {
            int i, c;
            char *q;
            JSString *str;
            q = buf;
            p = rt->atom_array[atom];
            assert(!atom_is_free(p));
            str = p;
            if (str) {
                if (!str->is_wide_char) {
                    /* special case ASCII strings */
                    c = 0;
                    for(i = 0; i < str->len; i++) {
                        c |= str->u.str8[i];
                    }
                    if (c < 0x80)
                        return (const char *)str->u.str8;
                }
                for(i = 0; i < str->len; i++) {
                    if (str->is_wide_char)
                        c = str->u.str16[i];
                    else
                        c = str->u.str8[i];
                    if ((q - buf) >= buf_size - UTF8_CHAR_LEN_MAX)
                        break;
                    if (c < 128) {
                        *q++ = c;
                    } else {
                        q += unicode_to_utf8((uint8_t *)q, c);
                    }
                }
            }
            *q = '\0';
        }
    }
    return buf;
}

const char *JS_AtomGetStr(JSContext *ctx, char *buf, int buf_size, JSAtom atom)
{
    return JS_AtomGetStrRT(ctx->rt, buf, buf_size, atom);
}

static JSValue __JS_AtomToValue(JSContext *ctx, JSAtom atom, BOOL force_string)
{
    char buf[ATOM_GET_STR_BUF_SIZE];
    if (__JS_AtomIsTaggedInt(atom)) {
        snprintf(buf, sizeof(buf), "%u", __JS_AtomToUInt32(atom));
        return JS_NewString(ctx, buf);
    } else {
        JSRuntime *rt = ctx->rt;
        JSAtomStruct *p;
        assert(atom < rt->atom_size);
        p = rt->atom_array[atom];
        if (p->atom_type == JS_ATOM_TYPE_STRING) {
            goto ret_string;
        } else if (force_string) {
            if (p->len == 0 && p->is_wide_char != 0) {
                /* no description string */
                p = rt->atom_array[JS_ATOM_empty_string];
            }
        ret_string:
            return JS_DupValue(ctx, JS_MKPTR(JS_TAG_STRING, p));
        } else {
            return JS_DupValue(ctx, JS_MKPTR(JS_TAG_SYMBOL, p));
        }
    }
}

JSValue JS_AtomToValue(JSContext *ctx, JSAtom atom)
{
    return __JS_AtomToValue(ctx, atom, FALSE);
}

JSValue JS_AtomToString(JSContext *ctx, JSAtom atom)
{
    return __JS_AtomToValue(ctx, atom, TRUE);
}

/* return TRUE if the atom is an array index (i.e. 0 <= index <=
   2^32-2 and return its value */
BOOL JS_AtomIsArrayIndex(JSContext *ctx, uint32_t *pval, JSAtom atom)
{
    if (__JS_AtomIsTaggedInt(atom)) {
        *pval = __JS_AtomToUInt32(atom);
        return TRUE;
    } else {
        JSRuntime *rt = ctx->rt;
        JSAtomStruct *p;
        uint32_t val;
        assert(atom < rt->atom_size);
        p = rt->atom_array[atom];
        if (p->atom_type == JS_ATOM_TYPE_STRING &&
            is_num_string(&val, p) && val != -1) {
            *pval = val;
            return TRUE;
        } else {
            *pval = 0;
            return FALSE;
        }
    }
}

/* return TRUE if the string is a number n with 0 <= n <= 2^32-1 */
BOOL is_num_string(uint32_t *pval, const JSString *p)
{
    uint32_t n;
    uint64_t n64;
    int c, i, len;
    len = p->len;
    if (len == 0 || len > 10)
        return FALSE;
    if (p->is_wide_char)
        c = p->u.str16[0];
    else
        c = p->u.str8[0];
    if (isdigit(c)) {
        if (c == '0') {
            if (len != 1)
                return FALSE;
            n = 0;
        } else {
            n = c - '0';
            for(i = 1; i < len; i++) {
                if (p->is_wide_char)
                    c = p->u.str16[i];
                else
                    c = p->u.str8[i];
                if (!isdigit(c))
                    return FALSE;
                n64 = (uint64_t)n * 10 + (c - '0');
                if ((n64 >> 32) != 0)
                    return FALSE;
                n = n64;
            }
        }
        *pval = n;
        return TRUE;
    } else {
        return FALSE;
    }
}

JSAtom js_get_atom_index(JSRuntime *rt, JSAtomStruct *p)
{
    uint32_t i = p->hash_next;  /* atom_index */
    if (p->atom_type != JS_ATOM_TYPE_SYMBOL) {
        JSAtomStruct *p1;
        i = rt->atom_hash[p->hash & (rt->atom_hash_size - 1)];
        p1 = rt->atom_array[i];
        while (p1 != p) {
            assert(i != 0);
            i = p1->hash_next;
            p1 = rt->atom_array[i];
        }
    }
    return i;
}

/* val must be a symbol */
JSAtom js_symbol_to_atom(JSContext *ctx, JSValue val)
{
    JSAtomStruct *p = JS_VALUE_GET_PTR(val);
    return js_get_atom_index(ctx->rt, p);
}

/* return a string atom containing name concatenated with str1 */
JSAtom js_atom_concat_str(JSContext *ctx, JSAtom name, const char *str1)
{
    JSValue str;
    JSAtom atom;
    const char *cstr;
    char *cstr2;
    size_t len, len1;
    str = JS_AtomToString(ctx, name);
    if (JS_IsException(str))
        return JS_ATOM_NULL;
    cstr = JS_ToCStringLen(ctx, &len, str);
    if (!cstr)
        goto fail;
    len1 = strlen(str1);
    cstr2 = js_malloc(ctx, len + len1 + 1);
    if (!cstr2)
        goto fail;
    memcpy(cstr2, cstr, len);
    memcpy(cstr2 + len, str1, len1);
    cstr2[len + len1] = '\0';
    atom = JS_NewAtomLen(ctx, cstr2, len + len1);
    js_free(ctx, cstr2);
    JS_FreeCString(ctx, cstr);
    JS_FreeValue(ctx, str);
    return atom;
 fail:
    JS_FreeCString(ctx, cstr);
    JS_FreeValue(ctx, str);
    return JS_ATOM_NULL;
}
