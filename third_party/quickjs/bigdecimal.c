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
#include "libc/str/str.h"
#include "third_party/quickjs/internal.h"

asm(".ident\t\"\\n\\n\
QuickJS (MIT License)\\n\
Copyright (c) 2017-2021 Fabrice Bellard\\n\
Copyright (c) 2017-2021 Charlie Gordon\"");
asm(".include \"libc/disclaimer.inc\"");
/* clang-format off */

JSValue JS_NewBigDecimal(JSContext *ctx)
{
    JSBigDecimal *p;
    p = js_malloc(ctx, sizeof(*p));
    if (!p)
        return JS_EXCEPTION;
    p->header.ref_count = 1;
    bfdec_init(ctx->bf_ctx, &p->num);
    return JS_MKPTR(JS_TAG_BIG_DECIMAL, p);
}

/* return NULL if invalid type */
bfdec_t *JS_ToBigDecimal(JSContext *ctx, JSValueConst val)
{
    uint32_t tag;
    JSBigDecimal *p;
    bfdec_t *r;
    tag = JS_VALUE_GET_NORM_TAG(val);
    switch(tag) {
    case JS_TAG_BIG_DECIMAL:
        p = JS_VALUE_GET_PTR(val);
        r = &p->num;
        break;
    default:
        JS_ThrowTypeError(ctx, "bigdecimal expected");
        r = NULL;
        break;
    }
    return r;
}

static JSValue JS_ToBigDecimalFree(JSContext *ctx, JSValue val,
                                   BOOL allow_null_or_undefined)
{
 redo:
    switch(JS_VALUE_GET_NORM_TAG(val)) {
    case JS_TAG_BIG_DECIMAL:
        break;
    case JS_TAG_NULL:
        if (!allow_null_or_undefined)
            goto fail;
        /* fall thru */
    case JS_TAG_BOOL:
    case JS_TAG_INT:
        {
            bfdec_t *r;
            int32_t v = JS_VALUE_GET_INT(val);
            val = JS_NewBigDecimal(ctx);
            if (JS_IsException(val))
                break;
            r = JS_GetBigDecimal(val);
            if (bfdec_set_si(r, v)) {
                JS_FreeValue(ctx, val);
                val = JS_EXCEPTION;
                break;
            }
        }
        break;
    case JS_TAG_FLOAT64:
    case JS_TAG_BIG_INT:
    case JS_TAG_BIG_FLOAT:
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
                bfdec_t *r;
                val = JS_NewBigDecimal(ctx);
                if (JS_IsException(val))
                    break;
                r = JS_GetBigDecimal(val);
                bfdec_set_zero(r, 0);
                err = 0;
            } else {
                val = js_atof(ctx, p, &p, 0, ATOD_TYPE_BIG_DECIMAL);
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
                return JS_ThrowSyntaxError(ctx, "invalid bigdecimal literal");
            }
        }
        break;
    case JS_TAG_OBJECT:
        val = JS_ToPrimitiveFree(ctx, val, HINT_NUMBER);
        if (JS_IsException(val))
            break;
        goto redo;
    case JS_TAG_UNDEFINED:
        {
            bfdec_t *r;
            if (!allow_null_or_undefined)
                goto fail;
            val = JS_NewBigDecimal(ctx);
            if (JS_IsException(val))
                break;
            r = JS_GetBigDecimal(val);
            bfdec_set_nan(r);
        }
        break;
    default:
    fail:
        JS_FreeValue(ctx, val);
        return JS_ThrowTypeError(ctx, "cannot convert to bigdecimal");
    }
    return val;
}

static JSValue js_bigdecimal_constructor(JSContext *ctx,
                                         JSValueConst new_target,
                                         int argc, JSValueConst *argv)
{
    JSValue val;
    if (!JS_IsUndefined(new_target))
        return JS_ThrowTypeError(ctx, "not a constructor");
    if (argc == 0) {
        bfdec_t *r;
        val = JS_NewBigDecimal(ctx);
        if (JS_IsException(val))
            return val;
        r = JS_GetBigDecimal(val);
        bfdec_set_zero(r, 0);
    } else {
        val = JS_ToBigDecimalFree(ctx, JS_DupValue(ctx, argv[0]), FALSE);
    }
    return val;
}

static JSValue js_thisBigDecimalValue(JSContext *ctx, JSValueConst this_val)
{
    if (JS_IsBigDecimal(this_val))
        return JS_DupValue(ctx, this_val);

    if (JS_VALUE_GET_TAG(this_val) == JS_TAG_OBJECT) {
        JSObject *p = JS_VALUE_GET_OBJ(this_val);
        if (p->class_id == JS_CLASS_BIG_DECIMAL) {
            if (JS_IsBigDecimal(p->u.object_data))
                return JS_DupValue(ctx, p->u.object_data);
        }
    }
    return JS_ThrowTypeError(ctx, "not a bigdecimal");
}

static JSValue js_bigdecimal_toString(JSContext *ctx, JSValueConst this_val,
                                      int argc, JSValueConst *argv)
{
    JSValue val;

    val = js_thisBigDecimalValue(ctx, this_val);
    if (JS_IsException(val))
        return val;
    return JS_ToStringFree(ctx, val);
}

static JSValue js_bigdecimal_valueOf(JSContext *ctx, JSValueConst this_val,
                                   int argc, JSValueConst *argv)
{
    return js_thisBigDecimalValue(ctx, this_val);
}

static int js_bigdecimal_get_rnd_mode(JSContext *ctx, JSValueConst obj)
{
    const char *str;
    size_t size;
    int rnd_mode;

    str = JS_ToCStringLen(ctx, &size, obj);
    if (!str)
        return -1;
    if (strlen(str) != size)
        goto invalid_rounding_mode;
    if (!strcmp(str, "floor")) {
        rnd_mode = BF_RNDD;
    } else if (!strcmp(str, "ceiling")) {
        rnd_mode = BF_RNDU;
    } else if (!strcmp(str, "down")) {
        rnd_mode = BF_RNDZ;
    } else if (!strcmp(str, "up")) {
        rnd_mode = BF_RNDA;
    } else if (!strcmp(str, "half-even")) {
        rnd_mode = BF_RNDN;
    } else if (!strcmp(str, "half-up")) {
        rnd_mode = BF_RNDNA;
    } else {
    invalid_rounding_mode:
        JS_FreeCString(ctx, str);
        JS_ThrowTypeError(ctx, "invalid rounding mode");
        return -1;
    }
    JS_FreeCString(ctx, str);
    return rnd_mode;
}

typedef struct {
    int64_t prec;
    bf_flags_t flags;
} BigDecimalEnv;

static int js_bigdecimal_get_env(JSContext *ctx, BigDecimalEnv *fe,
                                 JSValueConst obj)
{
    JSValue prop;
    int64_t val;
    BOOL has_prec;
    int rnd_mode;
    if (!JS_IsObject(obj)) {
        JS_ThrowTypeErrorNotAnObject(ctx);
        return -1;
    }
    prop = JS_GetProperty(ctx, obj, JS_ATOM_roundingMode);
    if (JS_IsException(prop))
        return -1;
    rnd_mode = js_bigdecimal_get_rnd_mode(ctx, prop);
    JS_FreeValue(ctx, prop);
    if (rnd_mode < 0)
        return -1;
    fe->flags = rnd_mode;
    prop = JS_GetProperty(ctx, obj, JS_ATOM_maximumSignificantDigits);
    if (JS_IsException(prop))
        return -1;
    has_prec = FALSE;
    if (!JS_IsUndefined(prop)) {
        if (JS_ToInt64SatFree(ctx, &val, prop))
            return -1;
        if (val < 1 || val > BF_PREC_MAX)
            goto invalid_precision;
        fe->prec = val;
        has_prec = TRUE;
    }
    prop = JS_GetProperty(ctx, obj, JS_ATOM_maximumFractionDigits);
    if (JS_IsException(prop))
        return -1;
    if (!JS_IsUndefined(prop)) {
        if (has_prec) {
            JS_FreeValue(ctx, prop);
            JS_ThrowTypeError(ctx, "cannot provide both maximumSignificantDigits and maximumFractionDigits");
            return -1;
        }
        if (JS_ToInt64SatFree(ctx, &val, prop))
            return -1;
        if (val < 0 || val > BF_PREC_MAX) {
        invalid_precision:
            JS_ThrowTypeError(ctx, "invalid precision");
            return -1;
        }
        fe->prec = val;
        fe->flags |= BF_FLAG_RADPNT_PREC;
        has_prec = TRUE;
    }
    if (!has_prec) {
        JS_ThrowTypeError(ctx, "precision must be present");
        return -1;
    }
    return 0;
}

static JSValue js_bigdecimal_fop(JSContext *ctx, JSValueConst this_val,
                                 int argc, JSValueConst *argv, int magic)
{
    bfdec_t *a, *b, r_s, *r = &r_s;
    JSValue op1, op2, res;
    BigDecimalEnv fe_s, *fe = &fe_s;
    int op_count, ret;
    if (magic == MATH_OP_SQRT ||
        magic == MATH_OP_ROUND)
        op_count = 1;
    else
        op_count = 2;
    op1 = JS_ToNumeric(ctx, argv[0]);
    if (JS_IsException(op1))
        return op1;
    a = JS_ToBigDecimal(ctx, op1);
    if (!a) {
        JS_FreeValue(ctx, op1);
        return JS_EXCEPTION;
    }
    if (op_count >= 2) {
        op2 = JS_ToNumeric(ctx, argv[1]);
        if (JS_IsException(op2)) {
            JS_FreeValue(ctx, op1);
            return op2;
        }
        b = JS_ToBigDecimal(ctx, op2);
        if (!b)
            goto fail;
    } else {
        op2 = JS_UNDEFINED;
        b = NULL;
    }
    fe->flags = BF_RNDZ;
    fe->prec = BF_PREC_INF;
    if (op_count < argc) {
        if (js_bigdecimal_get_env(ctx, fe, argv[op_count]))
            goto fail;
    }
    res = JS_NewBigDecimal(ctx);
    if (JS_IsException(res)) {
    fail:
        JS_FreeValue(ctx, op1);
        JS_FreeValue(ctx, op2);
        return JS_EXCEPTION;
    }
    r = JS_GetBigDecimal(res);
    switch (magic) {
    case MATH_OP_ADD:
        ret = bfdec_add(r, a, b, fe->prec, fe->flags);
        break;
    case MATH_OP_SUB:
        ret = bfdec_sub(r, a, b, fe->prec, fe->flags);
        break;
    case MATH_OP_MUL:
        ret = bfdec_mul(r, a, b, fe->prec, fe->flags);
        break;
    case MATH_OP_DIV:
        ret = bfdec_div(r, a, b, fe->prec, fe->flags);
        break;
    case MATH_OP_FMOD:
        ret = bfdec_rem(r, a, b, fe->prec, fe->flags, BF_RNDZ);
        break;
    case MATH_OP_SQRT:
        ret = bfdec_sqrt(r, a, fe->prec, fe->flags);
        break;
    case MATH_OP_ROUND:
        ret = bfdec_set(r, a);
        if (!(ret & BF_ST_MEM_ERROR))
            ret = bfdec_round(r, fe->prec, fe->flags);
        break;
    default:
        abort();
    }
    JS_FreeValue(ctx, op1);
    JS_FreeValue(ctx, op2);
    ret &= BF_ST_MEM_ERROR | BF_ST_DIVIDE_ZERO | BF_ST_INVALID_OP |
        BF_ST_OVERFLOW;
    if (ret != 0) {
        JS_FreeValue(ctx, res);
        return throw_bf_exception(ctx, ret);
    } else {
        return res;
    }
}

static JSValue js_bigdecimal_toFixed(JSContext *ctx, JSValueConst this_val,
                                 int argc, JSValueConst *argv)
{
    JSValue val, ret;
    int64_t f;
    int rnd_mode;
    val = js_thisBigDecimalValue(ctx, this_val);
    if (JS_IsException(val))
        return val;
    if (JS_ToInt64Sat(ctx, &f, argv[0]))
        goto fail;
    if (f < 0 || f > BF_PREC_MAX) {
        JS_ThrowRangeError(ctx, "invalid number of digits");
        goto fail;
    }
    rnd_mode = BF_RNDNA;
    if (argc > 1) {
        rnd_mode = js_bigdecimal_get_rnd_mode(ctx, argv[1]);
        if (rnd_mode < 0)
            goto fail;
    }
    ret = js_bigdecimal_to_string1(ctx, val, f, rnd_mode | BF_FTOA_FORMAT_FRAC);
    JS_FreeValue(ctx, val);
    return ret;
 fail:
    JS_FreeValue(ctx, val);
    return JS_EXCEPTION;
}

static JSValue js_bigdecimal_toExponential(JSContext *ctx, JSValueConst this_val,
                                       int argc, JSValueConst *argv)
{
    JSValue val, ret;
    int64_t f;
    int rnd_mode;
    val = js_thisBigDecimalValue(ctx, this_val);
    if (JS_IsException(val))
        return val;
    if (JS_ToInt64Sat(ctx, &f, argv[0]))
        goto fail;
    if (JS_IsUndefined(argv[0])) {
        ret = js_bigdecimal_to_string1(ctx, val, 0,
                  BF_RNDN | BF_FTOA_FORMAT_FREE_MIN | BF_FTOA_FORCE_EXP);
    } else {
        if (f < 0 || f > BF_PREC_MAX) {
            JS_ThrowRangeError(ctx, "invalid number of digits");
            goto fail;
        }
        rnd_mode = BF_RNDNA;
        if (argc > 1) {
            rnd_mode = js_bigdecimal_get_rnd_mode(ctx, argv[1]);
            if (rnd_mode < 0)
                goto fail;
        }
        ret = js_bigdecimal_to_string1(ctx, val, f + 1,
                      rnd_mode | BF_FTOA_FORMAT_FIXED | BF_FTOA_FORCE_EXP);
    }
    JS_FreeValue(ctx, val);
    return ret;
 fail:
    JS_FreeValue(ctx, val);
    return JS_EXCEPTION;
}

static JSValue js_bigdecimal_toPrecision(JSContext *ctx, JSValueConst this_val,
                                     int argc, JSValueConst *argv)
{
    JSValue val, ret;
    int64_t p;
    int rnd_mode;
    val = js_thisBigDecimalValue(ctx, this_val);
    if (JS_IsException(val))
        return val;
    if (JS_IsUndefined(argv[0])) {
        return JS_ToStringFree(ctx, val);
    }
    if (JS_ToInt64Sat(ctx, &p, argv[0]))
        goto fail;
    if (p < 1 || p > BF_PREC_MAX) {
        JS_ThrowRangeError(ctx, "invalid number of digits");
        goto fail;
    }
    rnd_mode = BF_RNDNA;
    if (argc > 1) {
        rnd_mode = js_bigdecimal_get_rnd_mode(ctx, argv[1]);
        if (rnd_mode < 0)
            goto fail;
    }
    ret = js_bigdecimal_to_string1(ctx, val, p,
                                   rnd_mode | BF_FTOA_FORMAT_FIXED);
    JS_FreeValue(ctx, val);
    return ret;
 fail:
    JS_FreeValue(ctx, val);
    return JS_EXCEPTION;
}

static const JSCFunctionListEntry js_bigdecimal_proto_funcs[] = {
    JS_CFUNC_DEF("toString", 0, js_bigdecimal_toString ),
    JS_CFUNC_DEF("valueOf", 0, js_bigdecimal_valueOf ),
    JS_CFUNC_DEF("toPrecision", 1, js_bigdecimal_toPrecision ),
    JS_CFUNC_DEF("toFixed", 1, js_bigdecimal_toFixed ),
    JS_CFUNC_DEF("toExponential", 1, js_bigdecimal_toExponential ),
};

static const JSCFunctionListEntry js_bigdecimal_funcs[] = {
    JS_CFUNC_MAGIC_DEF("add", 2, js_bigdecimal_fop, MATH_OP_ADD ),
    JS_CFUNC_MAGIC_DEF("sub", 2, js_bigdecimal_fop, MATH_OP_SUB ),
    JS_CFUNC_MAGIC_DEF("mul", 2, js_bigdecimal_fop, MATH_OP_MUL ),
    JS_CFUNC_MAGIC_DEF("div", 2, js_bigdecimal_fop, MATH_OP_DIV ),
    JS_CFUNC_MAGIC_DEF("mod", 2, js_bigdecimal_fop, MATH_OP_FMOD ),
    JS_CFUNC_MAGIC_DEF("round", 1, js_bigdecimal_fop, MATH_OP_ROUND ),
    JS_CFUNC_MAGIC_DEF("sqrt", 1, js_bigdecimal_fop, MATH_OP_SQRT ),
};

static JSValue js_string_to_bigdecimal(JSContext *ctx, const char *buf,
                                       int radix, int flags, slimb_t *pexponent)
{
    bfdec_t *a;
    int ret;
    JSValue val;
    val = JS_NewBigDecimal(ctx);
    if (JS_IsException(val))
        return val;
    a = JS_GetBigDecimal(val);
    ret = bfdec_atof(a, buf, NULL, BF_PREC_INF,
                     BF_RNDZ | BF_ATOF_NO_NAN_INF);
    if (ret & BF_ST_MEM_ERROR) {
        JS_FreeValue(ctx, val);
        return JS_ThrowOutOfMemory(ctx);
    }
    return val;
}

static int js_unary_arith_bigdecimal(JSContext *ctx,
                                     JSValue *pres, OPCodeEnum op, JSValue op1)
{
    bfdec_t *r, *a;
    int ret, v;
    JSValue res;
    if (op == OP_plus && !is_math_mode(ctx)) {
        JS_ThrowTypeError(ctx, "bigdecimal argument with unary +");
        JS_FreeValue(ctx, op1);
        return -1;
    }
    res = JS_NewBigDecimal(ctx);
    if (JS_IsException(res)) {
        JS_FreeValue(ctx, op1);
        return -1;
    }
    r = JS_GetBigDecimal(res);
    a = JS_ToBigDecimal(ctx, op1);
    ret = 0;
    switch(op) {
    case OP_inc:
    case OP_dec:
        v = 2 * (op - OP_dec) - 1;
        ret = bfdec_add_si(r, a, v, BF_PREC_INF, BF_RNDZ);
        break;
    case OP_plus:
        ret = bfdec_set(r, a);
        break;
    case OP_neg:
        ret = bfdec_set(r, a);
        bfdec_neg(r);
        break;
    default:
        abort();
    }
    JS_FreeValue(ctx, op1);
    if (UNLIKELY(ret)) {
        JS_FreeValue(ctx, res);
        throw_bf_exception(ctx, ret);
        return -1;
    }
    *pres = res;
    return 0;
}

/* b must be a positive integer */
static int js_bfdec_pow(bfdec_t *r, const bfdec_t *a, const bfdec_t *b)
{
    bfdec_t b1;
    int32_t b2;
    int ret;
    bfdec_init(b->ctx, &b1);
    ret = bfdec_set(&b1, b);
    if (ret) {
        bfdec_delete(&b1);
        return ret;
    }
    ret = bfdec_rint(&b1, BF_RNDZ);
    if (ret) {
        bfdec_delete(&b1);
        return BF_ST_INVALID_OP; /* must be an integer */
    }
    ret = bfdec_get_int32(&b2, &b1);
    bfdec_delete(&b1);
    if (ret)
        return ret; /* overflow */
    if (b2 < 0)
        return BF_ST_INVALID_OP; /* must be positive */
    return bfdec_pow_ui(r, a, b2);
}

static int js_compare_bigdecimal(JSContext *ctx, OPCodeEnum op,
                                 JSValue op1, JSValue op2)
{
    bfdec_t *a, *b;
    int res;
    /* Note: binary floats are converted to bigdecimal with
       toString(). It is not mathematically correct but is consistent
       with the BigDecimal() constructor behavior */
    op1 = JS_ToBigDecimalFree(ctx, op1, TRUE);
    if (JS_IsException(op1)) {
        JS_FreeValue(ctx, op2);
        return -1;
    }
    op2 = JS_ToBigDecimalFree(ctx, op2, TRUE);
    if (JS_IsException(op2)) {
        JS_FreeValue(ctx, op1);
        return -1;
    }
    a = JS_ToBigDecimal(ctx, op1);
    b = JS_ToBigDecimal(ctx, op2);
    switch(op) {
    case OP_lt:
        res = bfdec_cmp_lt(a, b); /* if NaN return false */
        break;
    case OP_lte:
        res = bfdec_cmp_le(a, b); /* if NaN return false */
        break;
    case OP_gt:
        res = bfdec_cmp_lt(b, a); /* if NaN return false */
        break;
    case OP_gte:
        res = bfdec_cmp_le(b, a); /* if NaN return false */
        break;
    case OP_eq:
        res = bfdec_cmp_eq(a, b); /* if NaN return false */
        break;
    default:
        abort();
    }
    JS_FreeValue(ctx, op1);
    JS_FreeValue(ctx, op2);
    return res;
}

static int js_binary_arith_bigdecimal(JSContext *ctx, OPCodeEnum op,
                                      JSValue *pres, JSValue op1, JSValue op2)
{
    bfdec_t *r, *a, *b;
    int ret;
    JSValue res;
    res = JS_NewBigDecimal(ctx);
    if (JS_IsException(res))
        goto fail;
    r = JS_GetBigDecimal(res);
    a = JS_ToBigDecimal(ctx, op1);
    if (!a)
        goto fail;
    b = JS_ToBigDecimal(ctx, op2);
    if (!b)
        goto fail;
    switch(op) {
    case OP_add:
        ret = bfdec_add(r, a, b, BF_PREC_INF, BF_RNDZ);
        break;
    case OP_sub:
        ret = bfdec_sub(r, a, b, BF_PREC_INF, BF_RNDZ);
        break;
    case OP_mul:
        ret = bfdec_mul(r, a, b, BF_PREC_INF, BF_RNDZ);
        break;
    case OP_div:
        ret = bfdec_div(r, a, b, BF_PREC_INF, BF_RNDZ);
        break;
    case OP_math_mod:
        /* Euclidian remainder */
        ret = bfdec_rem(r, a, b, BF_PREC_INF, BF_RNDZ, BF_DIVREM_EUCLIDIAN);
        break;
    case OP_mod:
        ret = bfdec_rem(r, a, b, BF_PREC_INF, BF_RNDZ, BF_RNDZ);
        break;
    case OP_pow:
        ret = js_bfdec_pow(r, a, b);
        break;
    default:
        abort();
    }
    JS_FreeValue(ctx, op1);
    JS_FreeValue(ctx, op2);
    if (UNLIKELY(ret)) {
        JS_FreeValue(ctx, res);
        throw_bf_exception(ctx, ret);
        return -1;
    }
    *pres = res;
    return 0;
 fail:
    JS_FreeValue(ctx, res);
    JS_FreeValue(ctx, op1);
    JS_FreeValue(ctx, op2);
    return -1;
}
 
void JS_AddIntrinsicBigDecimal(JSContext *ctx)
{
    JSRuntime *rt = ctx->rt;
    JSValueConst obj1;
    rt->bigdecimal_ops.to_string = js_bigdecimal_to_string;
    rt->bigdecimal_ops.from_string = js_string_to_bigdecimal;
    rt->bigdecimal_ops.unary_arith = js_unary_arith_bigdecimal;
    rt->bigdecimal_ops.binary_arith = js_binary_arith_bigdecimal;
    rt->bigdecimal_ops.compare = js_compare_bigdecimal;
    ctx->class_proto[JS_CLASS_BIG_DECIMAL] = JS_NewObject(ctx);
    JS_SetPropertyFunctionList(ctx, ctx->class_proto[JS_CLASS_BIG_DECIMAL],
                               js_bigdecimal_proto_funcs,
                               countof(js_bigdecimal_proto_funcs));
    obj1 = JS_NewGlobalCConstructor(ctx, "BigDecimal",
                                    js_bigdecimal_constructor, 1,
                                    ctx->class_proto[JS_CLASS_BIG_DECIMAL]);
    JS_SetPropertyFunctionList(ctx, obj1, js_bigdecimal_funcs,
                               countof(js_bigdecimal_funcs));
}

void JS_EnableBignumExt(JSContext *ctx, BOOL enable)
{
    ctx->bignum_ext = enable;
}
