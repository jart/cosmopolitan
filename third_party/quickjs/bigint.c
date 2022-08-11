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
#include "third_party/quickjs/internal.h"

asm(".ident\t\"\\n\\n\
QuickJS (MIT License)\\n\
Copyright (c) 2017-2021 Fabrice Bellard\\n\
Copyright (c) 2017-2021 Charlie Gordon\"");
asm(".include \"libc/disclaimer.inc\"");
/* clang-format off */

JSValue JS_NewBigInt(JSContext *ctx)
{
    JSBigFloat *p;
    p = js_malloc(ctx, sizeof(*p));
    if (!p)
        return JS_EXCEPTION;
    p->header.ref_count = 1;
    bf_init(ctx->bf_ctx, &p->num);
    return JS_MKPTR(JS_TAG_BIG_INT, p);
}

static JSValue JS_CompactBigInt1(JSContext *ctx, JSValue val,
                                 BOOL convert_to_safe_integer)
{
    int64_t v;
    bf_t *a;
    if (JS_VALUE_GET_TAG(val) != JS_TAG_BIG_INT)
        return val; /* fail safe */
    a = JS_GetBigInt(val);
    if (convert_to_safe_integer && bf_get_int64(&v, a, 0) == 0 &&
        v >= -MAX_SAFE_INTEGER && v <= MAX_SAFE_INTEGER) {
        JS_FreeValue(ctx, val);
        return JS_NewInt64(ctx, v);
    } else if (a->expn == BF_EXP_ZERO && a->sign) {
        JSBigFloat *p = JS_VALUE_GET_PTR(val);
        assert(p->header.ref_count == 1);
        a->sign = 0;
    }
    return val;
}

/* Convert the big int to a safe integer if in math mode. normalize
   the zero representation. Could also be used to convert the bigint
   to a short bigint value. The reference count of the value must be
   1. Cannot fail */
JSValue JS_CompactBigInt(JSContext *ctx, JSValue val)
{
    return JS_CompactBigInt1(ctx, val, is_math_mode(ctx));
}

/* return NaN if bad bigint literal */
JSValue JS_StringToBigInt(JSContext *ctx, JSValue val)
{
    const char *str, *p;
    size_t len;
    int flags;
    str = JS_ToCStringLen(ctx, &len, val);
    JS_FreeValue(ctx, val);
    if (!str)
        return JS_EXCEPTION;
    p = str;
    p += skip_spaces(p);
    if ((p - str) == len) {
        val = JS_NewBigInt64(ctx, 0);
    } else {
        flags = ATOD_INT_ONLY | ATOD_ACCEPT_BIN_OCT | ATOD_TYPE_BIG_INT;
        if (is_math_mode(ctx))
            flags |= ATOD_MODE_BIGINT;
        val = js_atof(ctx, p, &p, 0, flags);
        p += skip_spaces(p);
        if (!JS_IsException(val)) {
            if ((p - str) != len) {
                JS_FreeValue(ctx, val);
                val = JS_NAN;
            }
        }
    }
    JS_FreeCString(ctx, str);
    return val;
}

static JSValue JS_StringToBigIntErr(JSContext *ctx, JSValue val)
{
    val = JS_StringToBigInt(ctx, val);
    if (JS_VALUE_IS_NAN(val))
        return JS_ThrowSyntaxError(ctx, "invalid bigint literal");
    return val;
}

static JSValue JS_ToBigIntCtorFree(JSContext *ctx, JSValue val)
{
    uint32_t tag;
 redo:
    tag = JS_VALUE_GET_NORM_TAG(val);
    switch(tag) {
    case JS_TAG_INT:
    case JS_TAG_BOOL:
        val = JS_NewBigInt64(ctx, JS_VALUE_GET_INT(val));
        break;
    case JS_TAG_BIG_INT:
        break;
    case JS_TAG_FLOAT64:
    case JS_TAG_BIG_FLOAT:
        {
            bf_t *a, a_s;
            a = JS_ToBigFloat(ctx, &a_s, val);
            if (!bf_is_finite(a)) {
                JS_FreeValue(ctx, val);
                val = JS_ThrowRangeError(ctx, "cannot convert NaN or Infinity to bigint");
            } else {
                JSValue val1 = JS_NewBigInt(ctx);
                bf_t *r;
                int ret;
                if (JS_IsException(val1)) {
                    JS_FreeValue(ctx, val);
                    return JS_EXCEPTION;
                }
                r = JS_GetBigInt(val1);
                ret = bf_set(r, a);
                ret |= bf_rint(r, BF_RNDZ);
                JS_FreeValue(ctx, val);
                if (ret & BF_ST_MEM_ERROR) {
                    JS_FreeValue(ctx, val1);
                    val = JS_ThrowOutOfMemory(ctx);
                } else if (ret & BF_ST_INEXACT) {
                    JS_FreeValue(ctx, val1);
                    val = JS_ThrowRangeError(ctx, "cannot convert to bigint: not an integer");
                } else {
                    val = JS_CompactBigInt(ctx, val1);
                }
            }
            if (a == &a_s)
                bf_delete(a);
        }
        break;
    case JS_TAG_BIG_DECIMAL:
        val = JS_ToStringFree(ctx, val);
         if (JS_IsException(val))
            break;
        goto redo;
    case JS_TAG_STRING:
        val = JS_StringToBigIntErr(ctx, val);
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
        return JS_ThrowTypeError(ctx, "cannot convert to bigint");
    }
    return val;
}

static JSValue js_bigint_constructor(JSContext *ctx,
                                     JSValueConst new_target,
                                     int argc, JSValueConst *argv)
{
    if (!JS_IsUndefined(new_target))
        return JS_ThrowTypeError(ctx, "not a constructor");
    return JS_ToBigIntCtorFree(ctx, JS_DupValue(ctx, argv[0]));
}

static JSValue js_thisBigIntValue(JSContext *ctx, JSValueConst this_val)
{
    if (JS_IsBigInt(ctx, this_val))
        return JS_DupValue(ctx, this_val);
    if (JS_VALUE_GET_TAG(this_val) == JS_TAG_OBJECT) {
        JSObject *p = JS_VALUE_GET_OBJ(this_val);
        if (p->class_id == JS_CLASS_BIG_INT) {
            if (JS_IsBigInt(ctx, p->u.object_data))
                return JS_DupValue(ctx, p->u.object_data);
        }
    }
    return JS_ThrowTypeError(ctx, "not a bigint");
}

static JSValue js_bigint_toString(JSContext *ctx, JSValueConst this_val,
                                  int argc, JSValueConst *argv)
{
    JSValue val;
    int base;
    JSValue ret;
    val = js_thisBigIntValue(ctx, this_val);
    if (JS_IsException(val))
        return val;
    if (argc == 0 || JS_IsUndefined(argv[0])) {
        base = 10;
    } else {
        base = js_get_radix(ctx, argv[0]);
        if (base < 0)
            goto fail;
    }
    ret = js_bigint_to_string1(ctx, val, base);
    JS_FreeValue(ctx, val);
    return ret;
 fail:
    JS_FreeValue(ctx, val);
    return JS_EXCEPTION;
}

static JSValue js_bigint_valueOf(JSContext *ctx, JSValueConst this_val,
                                 int argc, JSValueConst *argv)
{
    return js_thisBigIntValue(ctx, this_val);
}

static JSValue js_bigint_div(JSContext *ctx,
                              JSValueConst this_val,
                              int argc, JSValueConst *argv, int magic)
{
    bf_t a_s, b_s, *a, *b, *r, *q;
    int status;
    JSValue q_val, r_val;
    q_val = JS_NewBigInt(ctx);
    if (JS_IsException(q_val))
        return JS_EXCEPTION;
    r_val = JS_NewBigInt(ctx);
    if (JS_IsException(r_val))
        goto fail;
    b = NULL;
    a = JS_ToBigInt(ctx, &a_s, argv[0]);
    if (!a)
        goto fail;
    b = JS_ToBigInt(ctx, &b_s, argv[1]);
    if (!b) {
        JS_FreeBigInt(ctx, a, &a_s);
        goto fail;
    }
    q = JS_GetBigInt(q_val);
    r = JS_GetBigInt(r_val);
    status = bf_divrem(q, r, a, b, BF_PREC_INF, BF_RNDZ, magic & 0xf);
    JS_FreeBigInt(ctx, a, &a_s);
    JS_FreeBigInt(ctx, b, &b_s);
    if (UNLIKELY(status)) {
        throw_bf_exception(ctx, status);
        goto fail;
    }
    q_val = JS_CompactBigInt(ctx, q_val);
    if (magic & 0x10) {
        JSValue ret;
        ret = JS_NewArray(ctx);
        if (JS_IsException(ret))
            goto fail;
        JS_SetPropertyUint32(ctx, ret, 0, q_val);
        JS_SetPropertyUint32(ctx, ret, 1, JS_CompactBigInt(ctx, r_val));
        return ret;
    } else {
        JS_FreeValue(ctx, r_val);
        return q_val;
    }
 fail:
    JS_FreeValue(ctx, q_val);
    JS_FreeValue(ctx, r_val);
    return JS_EXCEPTION;
}

static JSValue js_bigint_sqrt(JSContext *ctx,
                               JSValueConst this_val,
                               int argc, JSValueConst *argv, int magic)
{
    bf_t a_s, *a, *r, *rem;
    int status;
    JSValue r_val, rem_val;
    r_val = JS_NewBigInt(ctx);
    if (JS_IsException(r_val))
        return JS_EXCEPTION;
    rem_val = JS_NewBigInt(ctx);
    if (JS_IsException(rem_val))
        return JS_EXCEPTION;
    r = JS_GetBigInt(r_val);
    rem = JS_GetBigInt(rem_val);
    a = JS_ToBigInt(ctx, &a_s, argv[0]);
    if (!a)
        goto fail;
    status = bf_sqrtrem(r, rem, a);
    JS_FreeBigInt(ctx, a, &a_s);
    if (UNLIKELY(status & ~BF_ST_INEXACT)) {
        throw_bf_exception(ctx, status);
        goto fail;
    }
    r_val = JS_CompactBigInt(ctx, r_val);
    if (magic) {
        JSValue ret;
        ret = JS_NewArray(ctx);
        if (JS_IsException(ret))
            goto fail;
        JS_SetPropertyUint32(ctx, ret, 0, r_val);
        JS_SetPropertyUint32(ctx, ret, 1, JS_CompactBigInt(ctx, rem_val));
        return ret;
    } else {
        JS_FreeValue(ctx, rem_val);
        return r_val;
    }
 fail:
    JS_FreeValue(ctx, r_val);
    JS_FreeValue(ctx, rem_val);
    return JS_EXCEPTION;
}

static JSValue js_bigint_op1(JSContext *ctx,
                              JSValueConst this_val,
                              int argc, JSValueConst *argv,
                              int magic)
{
    bf_t a_s, *a;
    int64_t res;
    a = JS_ToBigInt(ctx, &a_s, argv[0]);
    if (!a)
        return JS_EXCEPTION;
    switch(magic) {
    case 0: /* floorLog2 */
        if (a->sign || a->expn <= 0) {
            res = -1;
        } else {
            res = a->expn - 1;
        }
        break;
    case 1: /* ctz */
        if (bf_is_zero(a)) {
            res = -1;
        } else {
            res = bf_get_exp_min(a);
        }
        break;
    default:
        abort();
    }
    JS_FreeBigInt(ctx, a, &a_s);
    return JS_NewBigInt64(ctx, res);
}

static JSValue js_bigint_asUintN(JSContext *ctx,
                                  JSValueConst this_val,
                                  int argc, JSValueConst *argv, int asIntN)
{
    uint64_t bits;
    bf_t a_s, *a = &a_s, *r, mask_s, *mask = &mask_s;
    JSValue res;
    if (JS_ToIndex(ctx, &bits, argv[0]))
        return JS_EXCEPTION;
    res = JS_NewBigInt(ctx);
    if (JS_IsException(res))
        return JS_EXCEPTION;
    r = JS_GetBigInt(res);
    a = JS_ToBigInt(ctx, &a_s, argv[1]);
    if (!a) {
        JS_FreeValue(ctx, res);
        return JS_EXCEPTION;
    }
    /* XXX: optimize */
    r = JS_GetBigInt(res);
    bf_init(ctx->bf_ctx, mask);
    bf_set_ui(mask, 1);
    bf_mul_2exp(mask, bits, BF_PREC_INF, BF_RNDZ);
    bf_add_si(mask, mask, -1, BF_PREC_INF, BF_RNDZ);
    bf_logic_and(r, a, mask);
    if (asIntN && bits != 0) {
        bf_set_ui(mask, 1);
        bf_mul_2exp(mask, bits - 1, BF_PREC_INF, BF_RNDZ);
        if (bf_cmpu(r, mask) >= 0) {
            bf_set_ui(mask, 1);
            bf_mul_2exp(mask, bits, BF_PREC_INF, BF_RNDZ);
            bf_sub(r, r, mask, BF_PREC_INF, BF_RNDZ);
        }
    }
    bf_delete(mask);
    JS_FreeBigInt(ctx, a, &a_s);
    return JS_CompactBigInt(ctx, res);
}

static const JSCFunctionListEntry js_bigint_funcs[] = {
    JS_CFUNC_MAGIC_DEF("asUintN", 2, js_bigint_asUintN, 0 ),
    JS_CFUNC_MAGIC_DEF("asIntN", 2, js_bigint_asUintN, 1 ),
    /* QuickJS extensions */
    JS_CFUNC_MAGIC_DEF("tdiv", 2, js_bigint_div, BF_RNDZ ),
    JS_CFUNC_MAGIC_DEF("fdiv", 2, js_bigint_div, BF_RNDD ),
    JS_CFUNC_MAGIC_DEF("cdiv", 2, js_bigint_div, BF_RNDU ),
    JS_CFUNC_MAGIC_DEF("ediv", 2, js_bigint_div, BF_DIVREM_EUCLIDIAN ),
    JS_CFUNC_MAGIC_DEF("tdivrem", 2, js_bigint_div, BF_RNDZ | 0x10 ),
    JS_CFUNC_MAGIC_DEF("fdivrem", 2, js_bigint_div, BF_RNDD | 0x10 ),
    JS_CFUNC_MAGIC_DEF("cdivrem", 2, js_bigint_div, BF_RNDU | 0x10 ),
    JS_CFUNC_MAGIC_DEF("edivrem", 2, js_bigint_div, BF_DIVREM_EUCLIDIAN | 0x10 ),
    JS_CFUNC_MAGIC_DEF("sqrt", 1, js_bigint_sqrt, 0 ),
    JS_CFUNC_MAGIC_DEF("sqrtrem", 1, js_bigint_sqrt, 1 ),
    JS_CFUNC_MAGIC_DEF("floorLog2", 1, js_bigint_op1, 0 ),
    JS_CFUNC_MAGIC_DEF("ctz", 1, js_bigint_op1, 1 ),
};

static const JSCFunctionListEntry js_bigint_proto_funcs[] = {
    JS_CFUNC_DEF("toString", 0, js_bigint_toString ),
    JS_CFUNC_DEF("valueOf", 0, js_bigint_valueOf ),
    JS_PROP_STRING_DEF("[Symbol.toStringTag]", "BigInt", JS_PROP_CONFIGURABLE ),
};

static JSValue js_string_to_bigint(JSContext *ctx, const char *buf,
                                   int radix, int flags, slimb_t *pexponent)
{
    bf_t a_s, *a = &a_s;
    int ret;
    JSValue val;
    val = JS_NewBigInt(ctx);
    if (JS_IsException(val))
        return val;
    a = JS_GetBigInt(val);
    ret = bf_atof(a, buf, NULL, radix, BF_PREC_INF, BF_RNDZ);
    if (ret & BF_ST_MEM_ERROR) {
        JS_FreeValue(ctx, val);
        return JS_ThrowOutOfMemory(ctx);
    }
    val = JS_CompactBigInt1(ctx, val, (flags & ATOD_MODE_BIGINT) != 0);
    return val;
}

static int js_unary_arith_bigint(JSContext *ctx,
                                 JSValue *pres, OPCodeEnum op, JSValue op1)
{
    bf_t a_s, *r, *a;
    int ret, v;
    JSValue res;
    if (op == OP_plus && !is_math_mode(ctx)) {
        JS_ThrowTypeError(ctx, "bigint argument with unary +");
        JS_FreeValue(ctx, op1);
        return -1;
    }
    res = JS_NewBigInt(ctx);
    if (JS_IsException(res)) {
        JS_FreeValue(ctx, op1);
        return -1;
    }
    r = JS_GetBigInt(res);
    a = JS_ToBigInt(ctx, &a_s, op1);
    ret = 0;
    switch(op) {
    case OP_inc:
    case OP_dec:
        v = 2 * (op - OP_dec) - 1;
        ret = bf_add_si(r, a, v, BF_PREC_INF, BF_RNDZ);
        break;
    case OP_plus:
        ret = bf_set(r, a);
        break;
    case OP_neg:
        ret = bf_set(r, a);
        bf_neg(r);
        break;
    case OP_not:
        ret = bf_add_si(r, a, 1, BF_PREC_INF, BF_RNDZ);
        bf_neg(r);
        break;
    default:
        abort();
    }
    JS_FreeBigInt(ctx, a, &a_s);
    JS_FreeValue(ctx, op1);
    if (UNLIKELY(ret)) {
        JS_FreeValue(ctx, res);
        throw_bf_exception(ctx, ret);
        return -1;
    }
    res = JS_CompactBigInt(ctx, res);
    *pres = res;
    return 0;
}

/* try to call the operation on the operatorSet field of 'obj'. Only
   used for "/" and "**" on the BigInt prototype in math mode */
static __exception int js_call_binary_op_simple(JSContext *ctx,
                                                JSValue *pret,
                                                JSValueConst obj,
                                                JSValueConst op1,
                                                JSValueConst op2,
                                                OPCodeEnum op)
{
    JSValue opset1_obj, method, ret, new_op1, new_op2;
    JSOperatorSetData *opset1;
    JSOverloadableOperatorEnum ovop;
    JSObject *p;
    JSValueConst args[2];
    opset1_obj = JS_GetProperty(ctx, obj, JS_ATOM_Symbol_operatorSet);
    if (JS_IsException(opset1_obj))
        goto exception;
    if (JS_IsUndefined(opset1_obj))
        return 0;
    opset1 = JS_GetOpaque2(ctx, opset1_obj, JS_CLASS_OPERATOR_SET);
    if (!opset1)
        goto exception;
    ovop = get_ovop_from_opcode(op);
    p = opset1->self_ops[ovop];
    if (!p) {
        JS_FreeValue(ctx, opset1_obj);
        return 0;
    }
    new_op1 = JS_ToNumeric(ctx, op1);
    if (JS_IsException(new_op1))
        goto exception;
    new_op2 = JS_ToNumeric(ctx, op2);
    if (JS_IsException(new_op2)) {
        JS_FreeValue(ctx, new_op1);
        goto exception;
    }
    method = JS_DupValue(ctx, JS_MKPTR(JS_TAG_OBJECT, p));
    args[0] = new_op1;
    args[1] = new_op2;
    ret = JS_CallFree(ctx, method, JS_UNDEFINED, 2, args);
    JS_FreeValue(ctx, new_op1);
    JS_FreeValue(ctx, new_op2);
    if (JS_IsException(ret))
        goto exception;
    JS_FreeValue(ctx, opset1_obj);
    *pret = ret;
    return 1;
 exception:
    JS_FreeValue(ctx, opset1_obj);
    *pret = JS_UNDEFINED;
    return -1;
}

static int js_binary_arith_bigint(JSContext *ctx, OPCodeEnum op,
                                  JSValue *pres, JSValue op1, JSValue op2)
{
    bf_t a_s, b_s, *r, *a, *b;
    int ret;
    JSValue res;
    res = JS_NewBigInt(ctx);
    if (JS_IsException(res))
        goto fail;
    a = JS_ToBigInt(ctx, &a_s, op1);
    if (!a)
        goto fail;
    b = JS_ToBigInt(ctx, &b_s, op2);
    if (!b) {
        JS_FreeBigInt(ctx, a, &a_s);
        goto fail;
    }
    r = JS_GetBigInt(res);
    ret = 0;
    switch(op) {
    case OP_add:
        ret = bf_add(r, a, b, BF_PREC_INF, BF_RNDZ);
        break;
    case OP_sub:
        ret = bf_sub(r, a, b, BF_PREC_INF, BF_RNDZ);
        break;
    case OP_mul:
        ret = bf_mul(r, a, b, BF_PREC_INF, BF_RNDZ);
        break;
    case OP_div:
        if (!is_math_mode(ctx)) {
            bf_t rem_s, *rem = &rem_s;
            bf_init(ctx->bf_ctx, rem);
            ret = bf_divrem(r, rem, a, b, BF_PREC_INF, BF_RNDZ,
                            BF_RNDZ);
            bf_delete(rem);
        } else {
            goto math_mode_div_pow;
        }
        break;
    case OP_math_mod:
        /* Euclidian remainder */
        ret = bf_rem(r, a, b, BF_PREC_INF, BF_RNDZ,
                     BF_DIVREM_EUCLIDIAN) & BF_ST_INVALID_OP;
        break;
    case OP_mod:
        ret = bf_rem(r, a, b, BF_PREC_INF, BF_RNDZ,
                     BF_RNDZ) & BF_ST_INVALID_OP;
        break;
    case OP_pow:
        if (b->sign) {
            if (!is_math_mode(ctx)) {
                ret = BF_ST_INVALID_OP;
            } else {
            math_mode_div_pow:
                JS_FreeValue(ctx, res);
                ret = js_call_binary_op_simple(ctx, &res, ctx->class_proto[JS_CLASS_BIG_INT], op1, op2, op);
                if (ret != 0) {
                    JS_FreeBigInt(ctx, a, &a_s);
                    JS_FreeBigInt(ctx, b, &b_s);
                    JS_FreeValue(ctx, op1);
                    JS_FreeValue(ctx, op2);
                    if (ret < 0) {
                        return -1;
                    } else {
                        *pres = res;
                        return 0;
                    }
                }
                /* if no BigInt power operator defined, return a
                   bigfloat */
                res = JS_NewBigFloat(ctx);
                if (JS_IsException(res)) {
                    JS_FreeBigInt(ctx, a, &a_s);
                    JS_FreeBigInt(ctx, b, &b_s);
                    goto fail;
                }
                r = JS_GetBigFloat(res);
                if (op == OP_div) {
                    ret = bf_div(r, a, b, ctx->fp_env.prec, ctx->fp_env.flags) & BF_ST_MEM_ERROR;
                } else {
                    ret = bf_pow(r, a, b, ctx->fp_env.prec,
                                 ctx->fp_env.flags | BF_POW_JS_QUIRKS) & BF_ST_MEM_ERROR;
                }
                JS_FreeBigInt(ctx, a, &a_s);
                JS_FreeBigInt(ctx, b, &b_s);
                JS_FreeValue(ctx, op1);
                JS_FreeValue(ctx, op2);
                if (UNLIKELY(ret)) {
                    JS_FreeValue(ctx, res);
                    throw_bf_exception(ctx, ret);
                    return -1;
                }
                *pres = res;
                return 0;
            }
        } else {
            ret = bf_pow(r, a, b, BF_PREC_INF, BF_RNDZ | BF_POW_JS_QUIRKS);
        }
        break;
        /* logical operations */
    case OP_shl:
    case OP_sar:
        {
            slimb_t v2;
#if LIMB_BITS == 32
            bf_get_int32(&v2, b, 0);
            if (v2 == INT32_MIN)
                v2 = INT32_MIN + 1;
#else
            bf_get_int64(&v2, b, 0);
            if (v2 == INT64_MIN)
                v2 = INT64_MIN + 1;
#endif
            if (op == OP_sar)
                v2 = -v2;
            ret = bf_set(r, a);
            ret |= bf_mul_2exp(r, v2, BF_PREC_INF, BF_RNDZ);
            if (v2 < 0) {
                ret |= bf_rint(r, BF_RNDD) & (BF_ST_OVERFLOW | BF_ST_MEM_ERROR);
            }
        }
        break;
    case OP_and:
        ret = bf_logic_and(r, a, b);
        break;
    case OP_or:
        ret = bf_logic_or(r, a, b);
        break;
    case OP_xor:
        ret = bf_logic_xor(r, a, b);
        break;
    default:
        abort();
    }
    JS_FreeBigInt(ctx, a, &a_s);
    JS_FreeBigInt(ctx, b, &b_s);
    JS_FreeValue(ctx, op1);
    JS_FreeValue(ctx, op2);
    if (UNLIKELY(ret)) {
        JS_FreeValue(ctx, res);
        throw_bf_exception(ctx, ret);
        return -1;
    }
    *pres = JS_CompactBigInt(ctx, res);
    return 0;
 fail:
    JS_FreeValue(ctx, res);
    JS_FreeValue(ctx, op1);
    JS_FreeValue(ctx, op2);
    return -1;
}

void JS_AddIntrinsicBigInt(JSContext *ctx)
{
    JSRuntime *rt = ctx->rt;
    JSValueConst obj1;
    rt->bigint_ops.to_string = js_bigint_to_string;
    rt->bigint_ops.from_string = js_string_to_bigint;
    rt->bigint_ops.unary_arith = js_unary_arith_bigint;
    rt->bigint_ops.binary_arith = js_binary_arith_bigint;
    rt->bigint_ops.compare = js_compare_bigfloat;
    ctx->class_proto[JS_CLASS_BIG_INT] = JS_NewObject(ctx);
    JS_SetPropertyFunctionList(ctx, ctx->class_proto[JS_CLASS_BIG_INT],
                               js_bigint_proto_funcs,
                               countof(js_bigint_proto_funcs));
    obj1 = JS_NewGlobalCConstructor(ctx, "BigInt", js_bigint_constructor, 1,
                                    ctx->class_proto[JS_CLASS_BIG_INT]);
    JS_SetPropertyFunctionList(ctx, obj1, js_bigint_funcs,
                               countof(js_bigint_funcs));
}

/* if the returned bigfloat is allocated it is equal to
   'buf'. Otherwise it is a pointer to the bigfloat in 'val'. */
static bf_t *JS_ToBigIntFree(JSContext *ctx, bf_t *buf, JSValue val)
{
    uint32_t tag;
    bf_t *r;
    JSBigFloat *p;

 redo:
    tag = JS_VALUE_GET_NORM_TAG(val);
    switch(tag) {
    case JS_TAG_INT:
    case JS_TAG_NULL:
    case JS_TAG_UNDEFINED:
        if (!is_math_mode(ctx))
            goto fail;
        /* fall tru */
    case JS_TAG_BOOL:
        r = buf;
        bf_init(ctx->bf_ctx, r);
        bf_set_si(r, JS_VALUE_GET_INT(val));
        break;
    case JS_TAG_FLOAT64:
        {
            double d = JS_VALUE_GET_FLOAT64(val);
            if (!is_math_mode(ctx))
                goto fail;
            if (!isfinite(d))
                goto fail;
            r = buf;
            bf_init(ctx->bf_ctx, r);
            d = trunc(d);
            bf_set_float64(r, d);
        }
        break;
    case JS_TAG_BIG_INT:
        p = JS_VALUE_GET_PTR(val);
        r = &p->num;
        break;
    case JS_TAG_BIG_FLOAT:
        if (!is_math_mode(ctx))
            goto fail;
        p = JS_VALUE_GET_PTR(val);
        if (!bf_is_finite(&p->num))
            goto fail;
        r = buf;
        bf_init(ctx->bf_ctx, r);
        bf_set(r, &p->num);
        bf_rint(r, BF_RNDZ);
        JS_FreeValue(ctx, val);
        break;
    case JS_TAG_STRING:
        val = JS_StringToBigIntErr(ctx, val);
        if (JS_IsException(val))
            return NULL;
        goto redo;
    case JS_TAG_OBJECT:
        val = JS_ToPrimitiveFree(ctx, val, HINT_NUMBER);
        if (JS_IsException(val))
            return NULL;
        goto redo;
    default:
    fail:
        JS_FreeValue(ctx, val);
        JS_ThrowTypeError(ctx, "cannot convert to bigint");
        return NULL;
    }
    return r;
}

bf_t *JS_ToBigInt(JSContext *ctx, bf_t *buf, JSValueConst val)
{
    return JS_ToBigIntFree(ctx, buf, JS_DupValue(ctx, val));
}

static __maybe_unused JSValue JS_ToBigIntValueFree(JSContext *ctx, JSValue val)
{
    if (JS_VALUE_GET_TAG(val) == JS_TAG_BIG_INT) {
        return val;
    } else {
        bf_t a_s, *a, *r;
        int ret;
        JSValue res;

        res = JS_NewBigInt(ctx);
        if (JS_IsException(res))
            return JS_EXCEPTION;
        a = JS_ToBigIntFree(ctx, &a_s, val);
        if (!a) {
            JS_FreeValue(ctx, res);
            return JS_EXCEPTION;
        }
        r = JS_GetBigInt(res);
        ret = bf_set(r, a);
        JS_FreeBigInt(ctx, a, &a_s);
        if (ret) {
            JS_FreeValue(ctx, res);
            return JS_ThrowOutOfMemory(ctx);
        }
        return JS_CompactBigInt(ctx, res);
    }
}

/* free the bf_t allocated by JS_ToBigInt */
void JS_FreeBigInt(JSContext *ctx, bf_t *a, bf_t *buf)
{
    if (a == buf) {
        bf_delete(a);
    } else {
        JSBigFloat *p = (JSBigFloat *)((uint8_t *)a -
                                       offsetof(JSBigFloat, num));
        JS_FreeValue(ctx, JS_MKPTR(JS_TAG_BIG_FLOAT, p));
    }
}

/* XXX: merge with JS_ToInt64Free with a specific flag */
int JS_ToBigInt64Free(JSContext *ctx, int64_t *pres, JSValue val)
{
    bf_t a_s, *a;
    a = JS_ToBigIntFree(ctx, &a_s, val);
    if (!a) {
        *pres = 0;
        return -1;
    }
    bf_get_int64(pres, a, BF_GET_INT_MOD);
    JS_FreeBigInt(ctx, a, &a_s);
    return 0;
}

int JS_ToBigInt64(JSContext *ctx, int64_t *pres, JSValueConst val)
{
    return JS_ToBigInt64Free(ctx, pres, JS_DupValue(ctx, val));
}
