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
#include "libc/runtime/runtime.h"
#include "libc/str/str.h"
#include "third_party/gdtoa/gdtoa.h"
#include "third_party/quickjs/internal.h"

asm(".ident\t\"\\n\\n\
QuickJS (MIT License)\\n\
Copyright (c) 2017-2021 Fabrice Bellard\\n\
Copyright (c) 2017-2021 Charlie Gordon\"");
asm(".include \"libc/disclaimer.inc\"");
/* clang-format off */

/* XXX: remove */
static double js_strtod(const char *p, int radix, BOOL is_float)
{
    double d;
    int c;
    if (!is_float || radix != 10) {
        uint64_t n_max, n;
        int int_exp, is_neg;
        is_neg = 0;
        if (*p == '-') {
            is_neg = 1;
            p++;
        }
        /* skip leading zeros */
        while (*p == '0')
            p++;
        n = 0;
        if (radix == 10)
            n_max = ((uint64_t)-1 - 9) / 10; /* most common case */
        else
            n_max = ((uint64_t)-1 - (radix - 1)) / radix;
        /* XXX: could be more precise */
        int_exp = 0;
        while (*p != '\0') {
            c = to_digit((uint8_t)*p);
            if (c >= radix)
                break;
            if (n <= n_max) {
                n = n * radix + c;
            } else {
                int_exp++;
            }
            p++;
        }
        d = n;
        if (int_exp != 0) {
            d *= pow(radix, int_exp);
        }
        if (is_neg)
            d = -d;
    } else {
        d = strtod(p, NULL);
    }
    return d;
}

/* return an exception in case of memory error. Return JS_NAN if
   invalid syntax */
#ifdef CONFIG_BIGNUM
JSValue js_atof2(JSContext *ctx, const char *str, const char **pp, int radix, int flags, slimb_t *pexponent)
#else
JSValue js_atof(JSContext *ctx, const char *str, const char **pp, int radix, int flags)
#endif
{
    const char *p, *p_start;
    int sep, is_neg;
    BOOL is_float, has_legacy_octal;
    int atod_type = flags & ATOD_TYPE_MASK;
    char buf1[64], *buf;
    int i, j, len;
    BOOL buf_allocated = FALSE;
    JSValue val;
    /* optional separator between digits */
    sep = (flags & ATOD_ACCEPT_UNDERSCORES) ? '_' : 256;
    has_legacy_octal = FALSE;
    p = str;
    p_start = p;
    is_neg = 0;
    if (p[0] == '+') {
        p++;
        p_start++;
        if (!(flags & ATOD_ACCEPT_PREFIX_AFTER_SIGN))
            goto no_radix_prefix;
    } else if (p[0] == '-') {
        p++;
        p_start++;
        is_neg = 1;
        if (!(flags & ATOD_ACCEPT_PREFIX_AFTER_SIGN))
            goto no_radix_prefix;
    }
    if (p[0] == '0') {
        if ((p[1] == 'x' || p[1] == 'X') &&
            (radix == 0 || radix == 16)) {
            p += 2;
            radix = 16;
        } else if ((p[1] == 'o' || p[1] == 'O') &&
                   radix == 0 && (flags & ATOD_ACCEPT_BIN_OCT)) {
            p += 2;
            radix = 8;
        } else if ((p[1] == 'b' || p[1] == 'B') &&
                   radix == 0 && (flags & ATOD_ACCEPT_BIN_OCT)) {
            p += 2;
            radix = 2;
        } else if ((p[1] >= '0' && p[1] <= '9') &&
                   radix == 0 && (flags & ATOD_ACCEPT_LEGACY_OCTAL)) {
            int i;
            has_legacy_octal = TRUE;
            sep = 256;
            for (i = 1; (p[i] >= '0' && p[i] <= '7'); i++)
                continue;
            if (p[i] == '8' || p[i] == '9')
                goto no_prefix;
            p += 1;
            radix = 8;
        } else {
            goto no_prefix;
        }
        /* there must be a digit after the prefix */
        if (to_digit((uint8_t)*p) >= radix)
            goto fail;
    no_prefix: ;
    } else {
 no_radix_prefix:
        if (!(flags & ATOD_INT_ONLY) &&
            (atod_type == ATOD_TYPE_FLOAT64 ||
             atod_type == ATOD_TYPE_BIG_FLOAT) &&
            strstart(p, "Infinity", &p)) {
#ifdef CONFIG_BIGNUM
            if (atod_type == ATOD_TYPE_BIG_FLOAT) {
                bf_t *a;
                val = JS_NewBigFloat(ctx);
                if (JS_IsException(val))
                    goto done;
                a = JS_GetBigFloat(val);
                bf_set_inf(a, is_neg);
            } else
#endif
            {
                double d = 1.0 / 0.0;
                if (is_neg)
                    d = -d;
                val = JS_NewFloat64(ctx, d);
            }
            goto done;
        }
    }
    if (radix == 0)
        radix = 10;
    is_float = FALSE;
    p_start = p;
    while (to_digit((uint8_t)*p) < radix
           ||  (*p == sep && (radix != 10 ||
                              p != p_start + 1 || p[-1] != '0') &&
                to_digit((uint8_t)p[1]) < radix)) {
        p++;
    }
    if (!(flags & ATOD_INT_ONLY)) {
        if (*p == '.' && (p > p_start || to_digit((uint8_t)p[1]) < radix)) {
            is_float = TRUE;
            p++;
            if (*p == sep)
                goto fail;
            while (to_digit((uint8_t)*p) < radix ||
                   (*p == sep && to_digit((uint8_t)p[1]) < radix))
                p++;
        }
        if (p > p_start &&
            (((*p == 'e' || *p == 'E') && radix == 10) ||
             ((*p == 'p' || *p == 'P') && (radix == 2 || radix == 8 || radix == 16)))) {
            const char *p1 = p + 1;
            is_float = TRUE;
            if (*p1 == '+') {
                p1++;
            } else if (*p1 == '-') {
                p1++;
            }
            if (isdigit((uint8_t)*p1)) {
                p = p1 + 1;
                while (isdigit((uint8_t)*p) || (*p == sep && isdigit((uint8_t)p[1])))
                    p++;
            }
        }
    }
    if (p == p_start)
        goto fail;
    buf = buf1;
    buf_allocated = FALSE;
    len = p - p_start;
    if (UNLIKELY((len + 2) > sizeof(buf1))) {
        buf = js_malloc_rt(ctx->rt, len + 2); /* no exception raised */
        if (!buf)
            goto mem_error;
        buf_allocated = TRUE;
    }
    /* remove the separators and the radix prefixes */
    j = 0;
    if (is_neg)
        buf[j++] = '-';
    for (i = 0; i < len; i++) {
        if (p_start[i] != '_')
            buf[j++] = p_start[i];
    }
    buf[j] = '\0';
#ifdef CONFIG_BIGNUM
    if (flags & ATOD_ACCEPT_SUFFIX) {
        if (*p == 'n') {
            p++;
            atod_type = ATOD_TYPE_BIG_INT;
        } else if (*p == 'l') {
            p++;
            atod_type = ATOD_TYPE_BIG_FLOAT;
        } else if (*p == 'm') {
            p++;
            atod_type = ATOD_TYPE_BIG_DECIMAL;
        } else {
            if (flags & ATOD_MODE_BIGINT) {
                if (!is_float)
                    atod_type = ATOD_TYPE_BIG_INT;
                if (has_legacy_octal)
                    goto fail;
            } else {
                if (is_float && radix != 10)
                    goto fail;
            }
        }
    } else {
        if (atod_type == ATOD_TYPE_FLOAT64) {
            if (flags & ATOD_MODE_BIGINT) {
                if (!is_float)
                    atod_type = ATOD_TYPE_BIG_INT;
                if (has_legacy_octal)
                    goto fail;
            } else {
                if (is_float && radix != 10)
                    goto fail;
            }
        }
    }
    switch(atod_type) {
    case ATOD_TYPE_FLOAT64:
        {
            double d;
            d = js_strtod(buf, radix, is_float);
            /* return int or float64 */
            val = JS_NewFloat64(ctx, d);
        }
        break;
    case ATOD_TYPE_BIG_INT:
        if (has_legacy_octal || is_float)
            goto fail;
        val = ctx->rt->bigint_ops.from_string(ctx, buf, radix, flags, NULL);
        break;
    case ATOD_TYPE_BIG_FLOAT:
        if (has_legacy_octal)
            goto fail;
        val = ctx->rt->bigfloat_ops.from_string(ctx, buf, radix, flags,
                                                pexponent);
        break;
    case ATOD_TYPE_BIG_DECIMAL:
        if (radix != 10)
            goto fail;
        val = ctx->rt->bigdecimal_ops.from_string(ctx, buf, radix, flags, NULL);
        break;
    default:
        abort();
    }
#else
    {
        double d;
        (void)has_legacy_octal;
        if (is_float && radix != 10)
            goto fail;
        d = js_strtod(buf, radix, is_float);
        val = JS_NewFloat64(ctx, d);
    }
#endif
done:
    if (buf_allocated)
        js_free_rt(ctx->rt, buf);
    if (pp)
        *pp = p;
    return val;
 fail:
    val = JS_NAN;
    goto done;
 mem_error:
    val = JS_ThrowOutOfMemory(ctx);
    goto done;
}

#ifdef CONFIG_BIGNUM
JSValue js_atof(JSContext *ctx, const char *str, const char **pp, int radix, int flags)
{
    return js_atof2(ctx, str, pp, radix, flags, NULL);
}
#endif
