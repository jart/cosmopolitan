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
#include "libc/runtime/runtime.h"
#include "third_party/quickjs/internal.h"

asm(".ident\t\"\\n\\n\
QuickJS (MIT License)\\n\
Copyright (c) 2017-2021 Fabrice Bellard\\n\
Copyright (c) 2017-2021 Charlie Gordon\"");
asm(".include \"libc/disclaimer.inc\"");
/* clang-format off */

JSValue JS_NewBigFloat(JSContext *ctx)
{
    JSBigFloat *p;
    p = js_malloc(ctx, sizeof(*p));
    if (!p)
        return JS_EXCEPTION;
    p->header.ref_count = 1;
    bf_init(ctx->bf_ctx, &p->num);
    return JS_MKPTR(JS_TAG_BIG_FLOAT, p);
}

static JSValue js_float_env_constructor(JSContext *ctx,
                                        JSValueConst new_target,
                                        int argc, JSValueConst *argv)
{
    JSValue obj;
    JSFloatEnv *fe;
    int64_t prec;
    int flags, rndmode;
    prec = ctx->fp_env.prec;
    flags = ctx->fp_env.flags;
    if (!JS_IsUndefined(argv[0])) {
        if (JS_ToInt64Sat(ctx, &prec, argv[0]))
            return JS_EXCEPTION;
        if (prec < BF_PREC_MIN || prec > BF_PREC_MAX)
            return JS_ThrowRangeError(ctx, "invalid precision");
        flags = BF_RNDN; /* RNDN, max exponent size, no subnormal */
        if (argc > 1 && !JS_IsUndefined(argv[1])) {
            if (JS_ToInt32Sat(ctx, &rndmode, argv[1]))
                return JS_EXCEPTION;
            if (rndmode < BF_RNDN || rndmode > BF_RNDF)
                return JS_ThrowRangeError(ctx, "invalid rounding mode");
            flags = rndmode;
        }
    }
    obj = JS_NewObjectClass(ctx, JS_CLASS_FLOAT_ENV);
    if (JS_IsException(obj))
        return JS_EXCEPTION;
    fe = js_malloc(ctx, sizeof(*fe));
    if (!fe)
        return JS_EXCEPTION;
    fe->prec = prec;
    fe->flags = flags;
    fe->status = 0;
    JS_SetOpaque(obj, fe);
    return obj;
}

/* if the returned bigfloat is allocated it is equal to
   'buf'. Otherwise it is a pointer to the bigfloat in 'val'. Return
   NULL in case of error. */
bf_t *JS_ToBigFloat(JSContext *ctx, bf_t *buf, JSValueConst val)
{
    uint32_t tag;
    bf_t *r;
    JSBigFloat *p;
    tag = JS_VALUE_GET_NORM_TAG(val);
    switch(tag) {
    case JS_TAG_INT:
    case JS_TAG_BOOL:
    case JS_TAG_NULL:
        r = buf;
        bf_init(ctx->bf_ctx, r);
        if (bf_set_si(r, JS_VALUE_GET_INT(val)))
            goto fail;
        break;
    case JS_TAG_FLOAT64:
        r = buf;
        bf_init(ctx->bf_ctx, r);
        if (bf_set_float64(r, JS_VALUE_GET_FLOAT64(val))) {
        fail:
            bf_delete(r);
            return NULL;
        }
        break;
    case JS_TAG_BIG_INT:
    case JS_TAG_BIG_FLOAT:
        p = JS_VALUE_GET_PTR(val);
        r = &p->num;
        break;
    case JS_TAG_UNDEFINED:
    default:
        r = buf;
        bf_init(ctx->bf_ctx, r);
        bf_set_nan(r);
        break;
    }
    return r;
}

static JSValue js_float_env_get_prec(JSContext *ctx, JSValueConst this_val)
{
    return JS_NewInt64(ctx, ctx->fp_env.prec);
}

static JSValue js_float_env_get_expBits(JSContext *ctx, JSValueConst this_val)
{
    return JS_NewInt32(ctx, bf_get_exp_bits(ctx->fp_env.flags));
}

static JSValue js_float_env_setPrec(JSContext *ctx,
                                    JSValueConst this_val,
                                    int argc, JSValueConst *argv)
{
    JSValueConst func;
    int exp_bits, flags, saved_flags;
    JSValue ret;
    limb_t saved_prec;
    int64_t prec;
    func = argv[0];
    if (JS_ToInt64Sat(ctx, &prec, argv[1]))
        return JS_EXCEPTION;
    if (prec < BF_PREC_MIN || prec > BF_PREC_MAX)
        return JS_ThrowRangeError(ctx, "invalid precision");
    exp_bits = BF_EXP_BITS_MAX;
    if (argc > 2 && !JS_IsUndefined(argv[2])) {
        if (JS_ToInt32Sat(ctx, &exp_bits, argv[2]))
            return JS_EXCEPTION;
        if (exp_bits < BF_EXP_BITS_MIN || exp_bits > BF_EXP_BITS_MAX)
            return JS_ThrowRangeError(ctx, "invalid number of exponent bits");
    }
    flags = BF_RNDN | BF_FLAG_SUBNORMAL | bf_set_exp_bits(exp_bits);
    saved_prec = ctx->fp_env.prec;
    saved_flags = ctx->fp_env.flags;
    ctx->fp_env.prec = prec;
    ctx->fp_env.flags = flags;
    ret = JS_Call(ctx, func, JS_UNDEFINED, 0, NULL);
    /* always restore the floating point precision */
    ctx->fp_env.prec = saved_prec;
    ctx->fp_env.flags = saved_flags;
    return ret;
}

static JSValue js_float_env_proto_get_status(JSContext *ctx, JSValueConst this_val, int magic)
{
    JSFloatEnv *fe;
    fe = JS_GetOpaque2(ctx, this_val, JS_CLASS_FLOAT_ENV);
    if (!fe)
        return JS_EXCEPTION;
    switch(magic) {
    case FE_PREC:
        return JS_NewInt64(ctx, fe->prec);
    case FE_EXP:
        return JS_NewInt32(ctx, bf_get_exp_bits(fe->flags));
    case FE_RNDMODE:
        return JS_NewInt32(ctx, fe->flags & BF_RND_MASK);
    case FE_SUBNORMAL:
        return JS_NewBool(ctx, (fe->flags & BF_FLAG_SUBNORMAL) != 0);
    default:
        return JS_NewBool(ctx, (fe->status & magic) != 0);
    }
}

static int bigfloat_get_rnd_mode(JSContext *ctx, JSValueConst val)
{
    int rnd_mode;
    if (JS_ToInt32Sat(ctx, &rnd_mode, val))
        return -1;
    if (rnd_mode < BF_RNDN || rnd_mode > BF_RNDF) {
        JS_ThrowRangeError(ctx, "invalid rounding mode");
        return -1;
    }
    return rnd_mode;
}

static JSValue js_float_env_proto_set_status(JSContext *ctx, JSValueConst this_val, JSValueConst val, int magic)
{
    JSFloatEnv *fe;
    int b;
    int64_t prec;
    fe = JS_GetOpaque2(ctx, this_val, JS_CLASS_FLOAT_ENV);
    if (!fe)
        return JS_EXCEPTION;
    switch(magic) {
    case FE_PREC:
        if (JS_ToInt64Sat(ctx, &prec, val))
            return JS_EXCEPTION;
        if (prec < BF_PREC_MIN || prec > BF_PREC_MAX)
            return JS_ThrowRangeError(ctx, "invalid precision");
        fe->prec = prec;
        break;
    case FE_EXP:
        if (JS_ToInt32Sat(ctx, &b, val))
            return JS_EXCEPTION;
        if (b < BF_EXP_BITS_MIN || b > BF_EXP_BITS_MAX)
            return JS_ThrowRangeError(ctx, "invalid number of exponent bits");
        fe->flags = (fe->flags & ~(BF_EXP_BITS_MASK << BF_EXP_BITS_SHIFT)) |
            bf_set_exp_bits(b);
        break;
    case FE_RNDMODE:
        b = bigfloat_get_rnd_mode(ctx, val);
        if (b < 0)
            return JS_EXCEPTION;
        fe->flags = (fe->flags & ~BF_RND_MASK) | b;
        break;
    case FE_SUBNORMAL:
        b = JS_ToBool(ctx, val);
        fe->flags = (fe->flags & ~BF_FLAG_SUBNORMAL) | (b ? BF_FLAG_SUBNORMAL: 0);
        break;
    default:
        b = JS_ToBool(ctx, val);
        fe->status = (fe->status & ~magic) & ((-b) & magic);
        break;
    }
    return JS_UNDEFINED;
}

static JSValue js_float_env_clearStatus(JSContext *ctx,
                                        JSValueConst this_val,
                                        int argc, JSValueConst *argv)
{
    JSFloatEnv *fe = JS_GetOpaque2(ctx, this_val, JS_CLASS_FLOAT_ENV);
    if (!fe)
        return JS_EXCEPTION;
    fe->status = 0;
    return JS_UNDEFINED;
}

static const JSCFunctionListEntry js_float_env_funcs[] = {
    JS_CGETSET_DEF("prec", js_float_env_get_prec, NULL ),
    JS_CGETSET_DEF("expBits", js_float_env_get_expBits, NULL ),
    JS_CFUNC_DEF("setPrec", 2, js_float_env_setPrec ),
    JS_PROP_INT32_DEF("RNDN", BF_RNDN, 0 ),
    JS_PROP_INT32_DEF("RNDZ", BF_RNDZ, 0 ),
    JS_PROP_INT32_DEF("RNDU", BF_RNDU, 0 ),
    JS_PROP_INT32_DEF("RNDD", BF_RNDD, 0 ),
    JS_PROP_INT32_DEF("RNDNA", BF_RNDNA, 0 ),
    JS_PROP_INT32_DEF("RNDA", BF_RNDA, 0 ),
    JS_PROP_INT32_DEF("RNDF", BF_RNDF, 0 ),
    JS_PROP_INT32_DEF("precMin", BF_PREC_MIN, 0 ),
    JS_PROP_INT64_DEF("precMax", BF_PREC_MAX, 0 ),
    JS_PROP_INT32_DEF("expBitsMin", BF_EXP_BITS_MIN, 0 ),
    JS_PROP_INT32_DEF("expBitsMax", BF_EXP_BITS_MAX, 0 ),
};

static const JSCFunctionListEntry js_float_env_proto_funcs[] = {
    JS_CGETSET_MAGIC_DEF("prec", js_float_env_proto_get_status,
                         js_float_env_proto_set_status, FE_PREC ),
    JS_CGETSET_MAGIC_DEF("expBits", js_float_env_proto_get_status,
                         js_float_env_proto_set_status, FE_EXP ),
    JS_CGETSET_MAGIC_DEF("rndMode", js_float_env_proto_get_status,
                         js_float_env_proto_set_status, FE_RNDMODE ),
    JS_CGETSET_MAGIC_DEF("subnormal", js_float_env_proto_get_status,
                         js_float_env_proto_set_status, FE_SUBNORMAL ),
    JS_CGETSET_MAGIC_DEF("invalidOperation", js_float_env_proto_get_status,
                         js_float_env_proto_set_status, BF_ST_INVALID_OP ),
    JS_CGETSET_MAGIC_DEF("divideByZero", js_float_env_proto_get_status,
                         js_float_env_proto_set_status, BF_ST_DIVIDE_ZERO ),
    JS_CGETSET_MAGIC_DEF("overflow", js_float_env_proto_get_status,
                         js_float_env_proto_set_status, BF_ST_OVERFLOW ),
    JS_CGETSET_MAGIC_DEF("underflow", js_float_env_proto_get_status,
                         js_float_env_proto_set_status, BF_ST_UNDERFLOW ),
    JS_CGETSET_MAGIC_DEF("inexact", js_float_env_proto_get_status,
                         js_float_env_proto_set_status, BF_ST_INEXACT ),
    JS_CFUNC_DEF("clearStatus", 0, js_float_env_clearStatus ),
};

static JSValue js_thisBigFloatValue(JSContext *ctx, JSValueConst this_val)
{
    if (JS_IsBigFloat(this_val))
        return JS_DupValue(ctx, this_val);
    if (JS_VALUE_GET_TAG(this_val) == JS_TAG_OBJECT) {
        JSObject *p = JS_VALUE_GET_OBJ(this_val);
        if (p->class_id == JS_CLASS_BIG_FLOAT) {
            if (JS_IsBigFloat(p->u.object_data))
                return JS_DupValue(ctx, p->u.object_data);
        }
    }
    return JS_ThrowTypeError(ctx, "not a bigfloat");
}

static JSValue js_bigfloat_toString(JSContext *ctx, JSValueConst this_val,
                                    int argc, JSValueConst *argv)
{
    JSValue val;
    int base;
    JSValue ret;
    val = js_thisBigFloatValue(ctx, this_val);
    if (JS_IsException(val))
        return val;
    if (argc == 0 || JS_IsUndefined(argv[0])) {
        base = 10;
    } else {
        base = js_get_radix(ctx, argv[0]);
        if (base < 0)
            goto fail;
    }
    ret = js_ftoa(ctx, val, base, 0, BF_RNDN | BF_FTOA_FORMAT_FREE_MIN);
    JS_FreeValue(ctx, val);
    return ret;
 fail:
    JS_FreeValue(ctx, val);
    return JS_EXCEPTION;
}

static JSValue js_bigfloat_valueOf(JSContext *ctx, JSValueConst this_val,
                                   int argc, JSValueConst *argv)
{
    return js_thisBigFloatValue(ctx, this_val);
}

static JSValue js_bigfloat_toFixed(JSContext *ctx, JSValueConst this_val,
                                   int argc, JSValueConst *argv)
{
    JSValue val, ret;
    int64_t f;
    int rnd_mode, radix;
    val = js_thisBigFloatValue(ctx, this_val);
    if (JS_IsException(val))
        return val;
    if (JS_ToInt64Sat(ctx, &f, argv[0]))
        goto fail;
    if (f < 0 || f > BF_PREC_MAX) {
        JS_ThrowRangeError(ctx, "invalid number of digits");
        goto fail;
    }
    rnd_mode = BF_RNDNA;
    radix = 10;
    /* XXX: swap parameter order for rounding mode and radix */
    if (argc > 1) {
        rnd_mode = bigfloat_get_rnd_mode(ctx, argv[1]);
        if (rnd_mode < 0)
            goto fail;
    }
    if (argc > 2) {
        radix = js_get_radix(ctx, argv[2]);
        if (radix < 0)
            goto fail;
    }
    ret = js_ftoa(ctx, val, radix, f, rnd_mode | BF_FTOA_FORMAT_FRAC);
    JS_FreeValue(ctx, val);
    return ret;
 fail:
    JS_FreeValue(ctx, val);
    return JS_EXCEPTION;
}

static BOOL js_bigfloat_is_finite(JSContext *ctx, JSValueConst val)
{
    BOOL res;
    uint32_t tag;
    tag = JS_VALUE_GET_NORM_TAG(val);
    switch(tag) {
    case JS_TAG_BIG_FLOAT:
        {
            JSBigFloat *p = JS_VALUE_GET_PTR(val);
            res = bf_is_finite(&p->num);
        }
        break;
    default:
        res = FALSE;
        break;
    }
    return res;
}

static JSValue js_bigfloat_toExponential(JSContext *ctx, JSValueConst this_val,
                                         int argc, JSValueConst *argv)
{
    JSValue val, ret;
    int64_t f;
    int rnd_mode, radix;
    val = js_thisBigFloatValue(ctx, this_val);
    if (JS_IsException(val))
        return val;
    if (JS_ToInt64Sat(ctx, &f, argv[0]))
        goto fail;
    if (!js_bigfloat_is_finite(ctx, val)) {
        ret = JS_ToString(ctx, val);
    } else if (JS_IsUndefined(argv[0])) {
        ret = js_ftoa(ctx, val, 10, 0,
                      BF_RNDN | BF_FTOA_FORMAT_FREE_MIN | BF_FTOA_FORCE_EXP);
    } else {
        if (f < 0 || f > BF_PREC_MAX) {
            JS_ThrowRangeError(ctx, "invalid number of digits");
            goto fail;
        }
        rnd_mode = BF_RNDNA;
        radix = 10;
        if (argc > 1) {
            rnd_mode = bigfloat_get_rnd_mode(ctx, argv[1]);
            if (rnd_mode < 0)
                goto fail;
        }
        if (argc > 2) {
            radix = js_get_radix(ctx, argv[2]);
            if (radix < 0)
                goto fail;
        }
        ret = js_ftoa(ctx, val, radix, f + 1,
                      rnd_mode | BF_FTOA_FORMAT_FIXED | BF_FTOA_FORCE_EXP);
    }
    JS_FreeValue(ctx, val);
    return ret;
 fail:
    JS_FreeValue(ctx, val);
    return JS_EXCEPTION;
}

static JSValue js_bigfloat_toPrecision(JSContext *ctx, JSValueConst this_val,
                                     int argc, JSValueConst *argv)
{
    JSValue val, ret;
    int64_t p;
    int rnd_mode, radix;

    val = js_thisBigFloatValue(ctx, this_val);
    if (JS_IsException(val))
        return val;
    if (JS_IsUndefined(argv[0]))
        goto to_string;
    if (JS_ToInt64Sat(ctx, &p, argv[0]))
        goto fail;
    if (!js_bigfloat_is_finite(ctx, val)) {
    to_string:
        ret = JS_ToString(ctx, this_val);
    } else {
        if (p < 1 || p > BF_PREC_MAX) {
            JS_ThrowRangeError(ctx, "invalid number of digits");
            goto fail;
        }
        rnd_mode = BF_RNDNA;
        radix = 10;
        if (argc > 1) {
            rnd_mode = bigfloat_get_rnd_mode(ctx, argv[1]);
            if (rnd_mode < 0)
                goto fail;
        }
        if (argc > 2) {
            radix = js_get_radix(ctx, argv[2]);
            if (radix < 0)
                goto fail;
        }
        ret = js_ftoa(ctx, val, radix, p, rnd_mode | BF_FTOA_FORMAT_FIXED);
    }
    JS_FreeValue(ctx, val);
    return ret;
 fail:
    JS_FreeValue(ctx, val);
    return JS_EXCEPTION;
}

static const JSCFunctionListEntry js_bigfloat_proto_funcs[] = {
    JS_CFUNC_DEF("toString", 0, js_bigfloat_toString ),
    JS_CFUNC_DEF("valueOf", 0, js_bigfloat_valueOf ),
    JS_CFUNC_DEF("toPrecision", 1, js_bigfloat_toPrecision ),
    JS_CFUNC_DEF("toFixed", 1, js_bigfloat_toFixed ),
    JS_CFUNC_DEF("toExponential", 1, js_bigfloat_toExponential ),
};

static JSValue js_bigfloat_constructor(JSContext *ctx,
                                       JSValueConst new_target,
                                       int argc, JSValueConst *argv)
{
    JSValue val;
    if (!JS_IsUndefined(new_target))
        return JS_ThrowTypeError(ctx, "not a constructor");
    if (argc == 0) {
        bf_t *r;
        val = JS_NewBigFloat(ctx);
        if (JS_IsException(val))
            return val;
        r = JS_GetBigFloat(val);
        bf_set_zero(r, 0);
    } else {
        val = JS_DupValue(ctx, argv[0]);
    redo:
        switch(JS_VALUE_GET_NORM_TAG(val)) {
        case JS_TAG_BIG_FLOAT:
            break;
        case JS_TAG_FLOAT64:
            {
                bf_t *r;
                double d = JS_VALUE_GET_FLOAT64(val);
                val = JS_NewBigFloat(ctx);
                if (JS_IsException(val))
                    break;
                r = JS_GetBigFloat(val);
                if (bf_set_float64(r, d))
                    goto fail;
            }
            break;
        case JS_TAG_INT:
            {
                bf_t *r;
                int32_t v = JS_VALUE_GET_INT(val);
                val = JS_NewBigFloat(ctx);
                if (JS_IsException(val))
                    break;
                r = JS_GetBigFloat(val);
                if (bf_set_si(r, v))
                    goto fail;
            }
            break;
        case JS_TAG_BIG_INT:
            /* We keep the full precision of the integer */
            {
                JSBigFloat *p = JS_VALUE_GET_PTR(val);
                val = JS_MKPTR(JS_TAG_BIG_FLOAT, p);
            }
            break;
        case JS_TAG_BIG_DECIMAL:
            val = JS_ToStringFree(ctx, val);
            if (JS_IsException(val))
                break;
            goto redo;
        case JS_TAG_STRING:
            {
                const char *str, *p;
                size_t len;
                int err;
                str = JS_ToCStringLen(ctx, &len, val);
                JS_FreeValue(ctx, val);
                if (!str)
                    return JS_EXCEPTION;
                p = str;
                p += skip_spaces(p);
                if ((p - str) == len) {
                    bf_t *r;
                    val = JS_NewBigFloat(ctx);
                    if (JS_IsException(val))
                        break;
                    r = JS_GetBigFloat(val);
                    bf_set_zero(r, 0);
                    err = 0;
                } else {
                    val = js_atof(ctx, p, &p, 0, ATOD_ACCEPT_BIN_OCT |
                                  ATOD_TYPE_BIG_FLOAT |
                                  ATOD_ACCEPT_PREFIX_AFTER_SIGN);
                    if (JS_IsException(val)) {
                        JS_FreeCString(ctx, str);
                        return JS_EXCEPTION;
                    }
                    p += skip_spaces(p);
                    err = ((p - str) != len);
                }
                JS_FreeCString(ctx, str);
                if (err) {
                    JS_FreeValue(ctx, val);
                    return JS_ThrowSyntaxError(ctx, "invalid bigfloat literal");
                }
            }
            break;
        case JS_TAG_OBJECT:
            val = JS_ToPrimitiveFree(ctx, val, HINT_NUMBER);
            if (JS_IsException(val))
                break;
            goto redo;
        case JS_TAG_NULL:
        case JS_TAG_UNDEFINED:
        default:
            JS_FreeValue(ctx, val);
            return JS_ThrowTypeError(ctx, "cannot convert to bigfloat");
        }
    }
    return val;
 fail:
    JS_FreeValue(ctx, val);
    return JS_EXCEPTION;
}

static JSValue js_bigfloat_get_const(JSContext *ctx,
                                     JSValueConst this_val, int magic)
{
    bf_t *r;
    JSValue val;
    val = JS_NewBigFloat(ctx);
    if (JS_IsException(val))
        return val;
    r = JS_GetBigFloat(val);
    switch(magic) {
    case 0: /* PI */
        bf_const_pi(r, ctx->fp_env.prec, ctx->fp_env.flags);
        break;
    case 1: /* LN2 */
        bf_const_log2(r, ctx->fp_env.prec, ctx->fp_env.flags);
        break;
    case 2: /* MIN_VALUE */
    case 3: /* MAX_VALUE */
        {
            slimb_t e_range, e;
            e_range = (limb_t)1 << (bf_get_exp_bits(ctx->fp_env.flags) - 1);
            bf_set_ui(r, 1);
            if (magic == 2) {
                e = -e_range + 2;
                if (ctx->fp_env.flags & BF_FLAG_SUBNORMAL)
                    e -= ctx->fp_env.prec - 1;
                bf_mul_2exp(r, e, ctx->fp_env.prec, ctx->fp_env.flags);
            } else {
                bf_mul_2exp(r, ctx->fp_env.prec, ctx->fp_env.prec,
                            ctx->fp_env.flags);
                bf_add_si(r, r, -1, ctx->fp_env.prec, ctx->fp_env.flags);
                bf_mul_2exp(r, e_range - ctx->fp_env.prec, ctx->fp_env.prec,
                            ctx->fp_env.flags);
            }
        }
        break;
    case 4: /* EPSILON */
        bf_set_ui(r, 1);
        bf_mul_2exp(r, 1 - ctx->fp_env.prec,
                    ctx->fp_env.prec, ctx->fp_env.flags);
        break;
    default:
        abort();
    }
    return val;
}

static JSValue js_bigfloat_parseFloat(JSContext *ctx, JSValueConst this_val,
                                      int argc, JSValueConst *argv)
{
    bf_t *a;
    const char *str;
    JSValue ret;
    int radix;
    JSFloatEnv *fe;
    str = JS_ToCString(ctx, argv[0]);
    if (!str)
        return JS_EXCEPTION;
    if (JS_ToInt32(ctx, &radix, argv[1])) {
    fail:
        JS_FreeCString(ctx, str);
        return JS_EXCEPTION;
    }
    if (radix != 0 && (radix < 2 || radix > 36)) {
        JS_ThrowRangeError(ctx, "radix must be between 2 and 36");
        goto fail;
    }
    fe = &ctx->fp_env;
    if (argc > 2) {
        fe = JS_GetOpaque2(ctx, argv[2], JS_CLASS_FLOAT_ENV);
        if (!fe)
            goto fail;
    }
    ret = JS_NewBigFloat(ctx);
    if (JS_IsException(ret))
        goto done;
    a = JS_GetBigFloat(ret);
    /* XXX: use js_atof() */
    bf_atof(a, str, NULL, radix, fe->prec, fe->flags);
 done:
    JS_FreeCString(ctx, str);
    return ret;
}

static JSValue js_bigfloat_isFinite(JSContext *ctx, JSValueConst this_val,
                                    int argc, JSValueConst *argv)
{
    JSValueConst val = argv[0];
    JSBigFloat *p;
    if (JS_VALUE_GET_NORM_TAG(val) != JS_TAG_BIG_FLOAT)
        return JS_FALSE;
    p = JS_VALUE_GET_PTR(val);
    return JS_NewBool(ctx, bf_is_finite(&p->num));
}

static JSValue js_bigfloat_isNaN(JSContext *ctx, JSValueConst this_val,
                                 int argc, JSValueConst *argv)
{
    JSValueConst val = argv[0];
    JSBigFloat *p;
    if (JS_VALUE_GET_NORM_TAG(val) != JS_TAG_BIG_FLOAT)
        return JS_FALSE;
    p = JS_VALUE_GET_PTR(val);
    return JS_NewBool(ctx, bf_is_nan(&p->num));
}

static JSValue js_bigfloat_fop(JSContext *ctx, JSValueConst this_val,
                           int argc, JSValueConst *argv, int magic)
{
    bf_t a_s, *a, *r;
    JSFloatEnv *fe;
    int rnd_mode;
    JSValue op1, res;
    op1 = JS_ToNumeric(ctx, argv[0]);
    if (JS_IsException(op1))
        return op1;
    a = JS_ToBigFloat(ctx, &a_s, op1);
    fe = &ctx->fp_env;
    if (argc > 1) {
        fe = JS_GetOpaque2(ctx, argv[1], JS_CLASS_FLOAT_ENV);
        if (!fe)
            goto fail;
    }
    res = JS_NewBigFloat(ctx);
    if (JS_IsException(res)) {
    fail:
        if (a == &a_s)
            bf_delete(a);
        JS_FreeValue(ctx, op1);
        return JS_EXCEPTION;
    }
    r = JS_GetBigFloat(res);
    switch (magic) {
    case MATH_OP_ABS:
        bf_set(r, a);
        r->sign = 0;
        break;
    case MATH_OP_FLOOR:
        rnd_mode = BF_RNDD;
        goto rint;
    case MATH_OP_CEIL:
        rnd_mode = BF_RNDU;
        goto rint;
    case MATH_OP_ROUND:
        rnd_mode = BF_RNDNA;
        goto rint;
    case MATH_OP_TRUNC:
        rnd_mode = BF_RNDZ;
    rint:
        bf_set(r, a);
        fe->status |= bf_rint(r, rnd_mode);
        break;
    case MATH_OP_SQRT:
        fe->status |= bf_sqrt(r, a, fe->prec, fe->flags);
        break;
    case MATH_OP_FPROUND:
        bf_set(r, a);
        fe->status |= bf_round(r, fe->prec, fe->flags);
        break;
    case MATH_OP_ACOS:
        fe->status |= bf_acos(r, a, fe->prec, fe->flags);
        break;
    case MATH_OP_ASIN:
        fe->status |= bf_asin(r, a, fe->prec, fe->flags);
        break;
    case MATH_OP_ATAN:
        fe->status |= bf_atan(r, a, fe->prec, fe->flags);
        break;
    case MATH_OP_COS:
        fe->status |= bf_cos(r, a, fe->prec, fe->flags);
        break;
    case MATH_OP_EXP:
        fe->status |= bf_exp(r, a, fe->prec, fe->flags);
        break;
    case MATH_OP_LOG:
        fe->status |= bf_log(r, a, fe->prec, fe->flags);
        break;
    case MATH_OP_SIN:
        fe->status |= bf_sin(r, a, fe->prec, fe->flags);
        break;
    case MATH_OP_TAN:
        fe->status |= bf_tan(r, a, fe->prec, fe->flags);
        break;
    case MATH_OP_SIGN:
        if (bf_is_nan(a) || bf_is_zero(a)) {
            bf_set(r, a);
        } else {
            bf_set_si(r, 1 - 2 * a->sign);
        }
        break;
    default:
        abort();
    }
    if (a == &a_s)
        bf_delete(a);
    JS_FreeValue(ctx, op1);
    return res;
}

static JSValue js_bigfloat_fop2(JSContext *ctx, JSValueConst this_val,
                            int argc, JSValueConst *argv, int magic)
{
    bf_t a_s, *a, b_s, *b, r_s, *r = &r_s;
    JSFloatEnv *fe;
    JSValue op1, op2, res;

    op1 = JS_ToNumeric(ctx, argv[0]);
    if (JS_IsException(op1))
        return op1;
    op2 = JS_ToNumeric(ctx, argv[1]);
    if (JS_IsException(op2)) {
        JS_FreeValue(ctx, op1);
        return op2;
    }
    a = JS_ToBigFloat(ctx, &a_s, op1);
    b = JS_ToBigFloat(ctx, &b_s, op2);
    fe = &ctx->fp_env;
    if (argc > 2) {
        fe = JS_GetOpaque2(ctx, argv[2], JS_CLASS_FLOAT_ENV);
        if (!fe)
            goto fail;
    }
    res = JS_NewBigFloat(ctx);
    if (JS_IsException(res)) {
    fail:
        if (a == &a_s)
            bf_delete(a);
        if (b == &b_s)
            bf_delete(b);
        JS_FreeValue(ctx, op1);
        JS_FreeValue(ctx, op2);
        return JS_EXCEPTION;
    }
    r = JS_GetBigFloat(res);
    switch (magic) {
    case MATH_OP_ATAN2:
        fe->status |= bf_atan2(r, a, b, fe->prec, fe->flags);
        break;
    case MATH_OP_POW:
        fe->status |= bf_pow(r, a, b, fe->prec, fe->flags | BF_POW_JS_QUIRKS);
        break;
    case MATH_OP_FMOD:
        fe->status |= bf_rem(r, a, b, fe->prec, fe->flags, BF_RNDZ);
        break;
    case MATH_OP_REM:
        fe->status |= bf_rem(r, a, b, fe->prec, fe->flags, BF_RNDN);
        break;
    case MATH_OP_ADD:
        fe->status |= bf_add(r, a, b, fe->prec, fe->flags);
        break;
    case MATH_OP_SUB:
        fe->status |= bf_sub(r, a, b, fe->prec, fe->flags);
        break;
    case MATH_OP_MUL:
        fe->status |= bf_mul(r, a, b, fe->prec, fe->flags);
        break;
    case MATH_OP_DIV:
        fe->status |= bf_div(r, a, b, fe->prec, fe->flags);
        break;
    default:
        abort();
    }
    if (a == &a_s)
        bf_delete(a);
    if (b == &b_s)
        bf_delete(b);
    JS_FreeValue(ctx, op1);
    JS_FreeValue(ctx, op2);
    return res;
}

static const JSCFunctionListEntry js_bigfloat_funcs[] = {
    JS_CGETSET_MAGIC_DEF("PI", js_bigfloat_get_const, NULL, 0 ),
    JS_CGETSET_MAGIC_DEF("LN2", js_bigfloat_get_const, NULL, 1 ),
    JS_CGETSET_MAGIC_DEF("MIN_VALUE", js_bigfloat_get_const, NULL, 2 ),
    JS_CGETSET_MAGIC_DEF("MAX_VALUE", js_bigfloat_get_const, NULL, 3 ),
    JS_CGETSET_MAGIC_DEF("EPSILON", js_bigfloat_get_const, NULL, 4 ),
    JS_CFUNC_DEF("parseFloat", 1, js_bigfloat_parseFloat ),
    JS_CFUNC_DEF("isFinite", 1, js_bigfloat_isFinite ),
    JS_CFUNC_DEF("isNaN", 1, js_bigfloat_isNaN ),
    JS_CFUNC_MAGIC_DEF("abs", 1, js_bigfloat_fop, MATH_OP_ABS ),
    JS_CFUNC_MAGIC_DEF("fpRound", 1, js_bigfloat_fop, MATH_OP_FPROUND ),
    JS_CFUNC_MAGIC_DEF("floor", 1, js_bigfloat_fop, MATH_OP_FLOOR ),
    JS_CFUNC_MAGIC_DEF("ceil", 1, js_bigfloat_fop, MATH_OP_CEIL ),
    JS_CFUNC_MAGIC_DEF("round", 1, js_bigfloat_fop, MATH_OP_ROUND ),
    JS_CFUNC_MAGIC_DEF("trunc", 1, js_bigfloat_fop, MATH_OP_TRUNC ),
    JS_CFUNC_MAGIC_DEF("sqrt", 1, js_bigfloat_fop, MATH_OP_SQRT ),
    JS_CFUNC_MAGIC_DEF("acos", 1, js_bigfloat_fop, MATH_OP_ACOS ),
    JS_CFUNC_MAGIC_DEF("asin", 1, js_bigfloat_fop, MATH_OP_ASIN ),
    JS_CFUNC_MAGIC_DEF("atan", 1, js_bigfloat_fop, MATH_OP_ATAN ),
    JS_CFUNC_MAGIC_DEF("atan2", 2, js_bigfloat_fop2, MATH_OP_ATAN2 ),
    JS_CFUNC_MAGIC_DEF("cos", 1, js_bigfloat_fop, MATH_OP_COS ),
    JS_CFUNC_MAGIC_DEF("exp", 1, js_bigfloat_fop, MATH_OP_EXP ),
    JS_CFUNC_MAGIC_DEF("log", 1, js_bigfloat_fop, MATH_OP_LOG ),
    JS_CFUNC_MAGIC_DEF("pow", 2, js_bigfloat_fop2, MATH_OP_POW ),
    JS_CFUNC_MAGIC_DEF("sin", 1, js_bigfloat_fop, MATH_OP_SIN ),
    JS_CFUNC_MAGIC_DEF("tan", 1, js_bigfloat_fop, MATH_OP_TAN ),
    JS_CFUNC_MAGIC_DEF("sign", 1, js_bigfloat_fop, MATH_OP_SIGN ),
    JS_CFUNC_MAGIC_DEF("add", 2, js_bigfloat_fop2, MATH_OP_ADD ),
    JS_CFUNC_MAGIC_DEF("sub", 2, js_bigfloat_fop2, MATH_OP_SUB ),
    JS_CFUNC_MAGIC_DEF("mul", 2, js_bigfloat_fop2, MATH_OP_MUL ),
    JS_CFUNC_MAGIC_DEF("div", 2, js_bigfloat_fop2, MATH_OP_DIV ),
    JS_CFUNC_MAGIC_DEF("fmod", 2, js_bigfloat_fop2, MATH_OP_FMOD ),
    JS_CFUNC_MAGIC_DEF("remainder", 2, js_bigfloat_fop2, MATH_OP_REM ),
};

static JSValue js_string_to_bigfloat(JSContext *ctx, const char *buf,
                                     int radix, int flags, slimb_t *pexponent)
{
    bf_t *a;
    int ret;
    JSValue val;
    val = JS_NewBigFloat(ctx);
    if (JS_IsException(val))
        return val;
    a = JS_GetBigFloat(val);
    if (flags & ATOD_ACCEPT_SUFFIX) {
        /* return the exponent to get infinite precision */
        ret = bf_atof2(a, pexponent, buf, NULL, radix, BF_PREC_INF,
                       BF_RNDZ | BF_ATOF_EXPONENT);
    } else {
        ret = bf_atof(a, buf, NULL, radix, ctx->fp_env.prec,
                      ctx->fp_env.flags);
    }
    if (ret & BF_ST_MEM_ERROR) {
        JS_FreeValue(ctx, val);
        return JS_ThrowOutOfMemory(ctx);
    }
    return val;
}

static int js_unary_arith_bigfloat(JSContext *ctx,
                                   JSValue *pres, OPCodeEnum op, JSValue op1)
{
    bf_t a_s, *r, *a;
    int ret, v;
    JSValue res;
    if (op == OP_plus && !is_math_mode(ctx)) {
        JS_ThrowTypeError(ctx, "bigfloat argument with unary +");
        JS_FreeValue(ctx, op1);
        return -1;
    }
    res = JS_NewBigFloat(ctx);
    if (JS_IsException(res)) {
        JS_FreeValue(ctx, op1);
        return -1;
    }
    r = JS_GetBigFloat(res);
    a = JS_ToBigFloat(ctx, &a_s, op1);
    ret = 0;
    switch(op) {
    case OP_inc:
    case OP_dec:
        v = 2 * (op - OP_dec) - 1;
        ret = bf_add_si(r, a, v, ctx->fp_env.prec, ctx->fp_env.flags);
        break;
    case OP_plus:
        ret = bf_set(r, a);
        break;
    case OP_neg:
        ret = bf_set(r, a);
        bf_neg(r);
        break;
    default:
        abort();
    }
    if (a == &a_s)
        bf_delete(a);
    JS_FreeValue(ctx, op1);
    if (UNLIKELY(ret & BF_ST_MEM_ERROR)) {
        JS_FreeValue(ctx, res);
        throw_bf_exception(ctx, ret);
        return -1;
    }
    *pres = res;
    return 0;
}

static int js_binary_arith_bigfloat(JSContext *ctx, OPCodeEnum op,
                                    JSValue *pres, JSValue op1, JSValue op2)
{
    bf_t a_s, b_s, *r, *a, *b;
    int ret;
    JSValue res;
    res = JS_NewBigFloat(ctx);
    if (JS_IsException(res)) {
        JS_FreeValue(ctx, op1);
        JS_FreeValue(ctx, op2);
        return -1;
    }
    r = JS_GetBigFloat(res);
    a = JS_ToBigFloat(ctx, &a_s, op1);
    b = JS_ToBigFloat(ctx, &b_s, op2);
    bf_init(ctx->bf_ctx, r);
    switch(op) {
    case OP_add:
        ret = bf_add(r, a, b, ctx->fp_env.prec, ctx->fp_env.flags);
        break;
    case OP_sub:
        ret = bf_sub(r, a, b, ctx->fp_env.prec, ctx->fp_env.flags);
        break;
    case OP_mul:
        ret = bf_mul(r, a, b, ctx->fp_env.prec, ctx->fp_env.flags);
        break;
    case OP_div:
        ret = bf_div(r, a, b, ctx->fp_env.prec, ctx->fp_env.flags);
        break;
    case OP_math_mod:
        /* Euclidian remainder */
        ret = bf_rem(r, a, b, ctx->fp_env.prec, ctx->fp_env.flags,
                     BF_DIVREM_EUCLIDIAN);
        break;
    case OP_mod:
        ret = bf_rem(r, a, b, ctx->fp_env.prec, ctx->fp_env.flags,
                     BF_RNDZ);
        break;
    case OP_pow:
        ret = bf_pow(r, a, b, ctx->fp_env.prec,
                     ctx->fp_env.flags | BF_POW_JS_QUIRKS);
        break;
    default:
        abort();
    }
    if (a == &a_s)
        bf_delete(a);
    if (b == &b_s)
        bf_delete(b);
    JS_FreeValue(ctx, op1);
    JS_FreeValue(ctx, op2);
    if (UNLIKELY(ret & BF_ST_MEM_ERROR)) {
        JS_FreeValue(ctx, res);
        throw_bf_exception(ctx, ret);
        return -1;
    }
    *pres = res;
    return 0;
}

/* Note: also used for bigint */
int js_compare_bigfloat(JSContext *ctx, OPCodeEnum op, JSValue op1, JSValue op2)
{
    bf_t a_s, b_s, *a, *b;
    int res;
    a = JS_ToBigFloat(ctx, &a_s, op1);
    if (!a) {
        JS_FreeValue(ctx, op2);
        return -1;
    }
    b = JS_ToBigFloat(ctx, &b_s, op2);
    if (!b) {
        if (a == &a_s)
            bf_delete(a);
        JS_FreeValue(ctx, op1);
        return -1;
    }
    switch(op) {
    case OP_lt:
        res = bf_cmp_lt(a, b); /* if NaN return false */
        break;
    case OP_lte:
        res = bf_cmp_le(a, b); /* if NaN return false */
        break;
    case OP_gt:
        res = bf_cmp_lt(b, a); /* if NaN return false */
        break;
    case OP_gte:
        res = bf_cmp_le(b, a); /* if NaN return false */
        break;
    case OP_eq:
        res = bf_cmp_eq(a, b); /* if NaN return false */
        break;
    default:
        abort();
    }
    if (a == &a_s)
        bf_delete(a);
    if (b == &b_s)
        bf_delete(b);
    JS_FreeValue(ctx, op1);
    JS_FreeValue(ctx, op2);
    return res;
}

static JSValue js_mul_pow10_to_float64(JSContext *ctx, const bf_t *a,
                                       int64_t exponent)
{
    bf_t r_s, *r = &r_s;
    double d;
    int ret;
    /* always convert to Float64 */
    bf_init(ctx->bf_ctx, r);
    ret = bf_mul_pow_radix(r, a, 10, exponent,
                           53, bf_set_exp_bits(11) | BF_RNDN |
                           BF_FLAG_SUBNORMAL);
    bf_get_float64(r, &d, BF_RNDN);
    bf_delete(r);
    if (ret & BF_ST_MEM_ERROR)
        return JS_ThrowOutOfMemory(ctx);
    else
        return __JS_NewFloat64(ctx, d);
}

static int js_mul_pow10(JSContext *ctx, JSValue *sp)
{
    bf_t a_s, *a, *r;
    JSValue op1, op2, res;
    int64_t e;
    int ret;
    res = JS_NewBigFloat(ctx);
    if (JS_IsException(res))
        return -1;
    r = JS_GetBigFloat(res);
    op1 = sp[-2];
    op2 = sp[-1];
    a = JS_ToBigFloat(ctx, &a_s, op1);
    if (!a)
        return -1;
    if (JS_IsBigInt(ctx, op2)) {
        ret = JS_ToBigInt64(ctx, &e, op2);
    } else {
        ret = JS_ToInt64(ctx, &e, op2);
    }
    if (ret) {
        if (a == &a_s)
            bf_delete(a);
        JS_FreeValue(ctx, res);
        return -1;
    }
    bf_mul_pow_radix(r, a, 10, e, ctx->fp_env.prec, ctx->fp_env.flags);
    if (a == &a_s)
        bf_delete(a);
    JS_FreeValue(ctx, op1);
    JS_FreeValue(ctx, op2);
    sp[-2] = res;
    return 0;
}

void JS_AddIntrinsicBigFloat(JSContext *ctx)
{
    JSRuntime *rt = ctx->rt;
    JSValueConst obj1;
    rt->bigfloat_ops.to_string = js_bigfloat_to_string;
    rt->bigfloat_ops.from_string = js_string_to_bigfloat;
    rt->bigfloat_ops.unary_arith = js_unary_arith_bigfloat;
    rt->bigfloat_ops.binary_arith = js_binary_arith_bigfloat;
    rt->bigfloat_ops.compare = js_compare_bigfloat;
    rt->bigfloat_ops.mul_pow10_to_float64 = js_mul_pow10_to_float64;
    rt->bigfloat_ops.mul_pow10 = js_mul_pow10;
    ctx->class_proto[JS_CLASS_BIG_FLOAT] = JS_NewObject(ctx);
    JS_SetPropertyFunctionList(ctx, ctx->class_proto[JS_CLASS_BIG_FLOAT],
                               js_bigfloat_proto_funcs,
                               countof(js_bigfloat_proto_funcs));
    obj1 = JS_NewGlobalCConstructor(ctx, "BigFloat", js_bigfloat_constructor, 1,
                                    ctx->class_proto[JS_CLASS_BIG_FLOAT]);
    JS_SetPropertyFunctionList(ctx, obj1, js_bigfloat_funcs,
                               countof(js_bigfloat_funcs));
    ctx->class_proto[JS_CLASS_FLOAT_ENV] = JS_NewObject(ctx);
    JS_SetPropertyFunctionList(ctx, ctx->class_proto[JS_CLASS_FLOAT_ENV],
                               js_float_env_proto_funcs,
                               countof(js_float_env_proto_funcs));
    obj1 = JS_NewGlobalCConstructorOnly(ctx, "BigFloatEnv",
                                        js_float_env_constructor, 1,
                                        ctx->class_proto[JS_CLASS_FLOAT_ENV]);
    JS_SetPropertyFunctionList(ctx, obj1, js_float_env_funcs,
                               countof(js_float_env_funcs));
}
