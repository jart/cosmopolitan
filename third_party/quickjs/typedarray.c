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

static JSObject *get_typed_array(JSContext *ctx,
                                 JSValueConst this_val,
                                 int is_dataview)
{
    JSObject *p;
    if (JS_VALUE_GET_TAG(this_val) != JS_TAG_OBJECT)
        goto fail;
    p = JS_VALUE_GET_OBJ(this_val);
    if (is_dataview) {
        if (p->class_id != JS_CLASS_DATAVIEW)
            goto fail;
    } else {
        if (!(p->class_id >= JS_CLASS_UINT8C_ARRAY &&
              p->class_id <= JS_CLASS_FLOAT64_ARRAY)) {
        fail:
            JS_ThrowTypeError(ctx, "not a %s", is_dataview ? "DataView" : "TypedArray");
            return NULL;
        }
    }
    return p;
}

/* WARNING: 'p' must be a typed array */
BOOL typed_array_is_detached(JSContext *ctx, JSObject *p)
{
    JSTypedArray *ta = p->u.typed_array;
    JSArrayBuffer *abuf = ta->buffer->u.array_buffer;
    /* XXX: could simplify test by ensuring that
       p->u.array.u.ptr is NULL iff it is detached */
    return abuf->detached;
}

/* WARNING: 'p' must be a typed array. Works even if the array buffer
   is detached */
uint32_t typed_array_get_length(JSContext *ctx, JSObject *p)
{
    JSTypedArray *ta = p->u.typed_array;
    int size_log2 = typed_array_size_log2(p->class_id);
    return ta->length >> size_log2;
}

static int validate_typed_array(JSContext *ctx, JSValueConst this_val)
{
    JSObject *p;
    p = get_typed_array(ctx, this_val, 0);
    if (!p)
        return -1;
    if (typed_array_is_detached(ctx, p)) {
        JS_ThrowTypeErrorDetachedArrayBuffer(ctx);
        return -1;
    }
    return 0;
}

static JSValue js_typed_array_get_length(JSContext *ctx,
                                         JSValueConst this_val)
{
    JSObject *p;
    p = get_typed_array(ctx, this_val, 0);
    if (!p)
        return JS_EXCEPTION;
    return JS_NewInt32(ctx, p->u.array.count);
}

JSValue js_typed_array_get_buffer(JSContext *ctx, JSValueConst this_val, int is_dataview)
{
    JSObject *p;
    JSTypedArray *ta;
    p = get_typed_array(ctx, this_val, is_dataview);
    if (!p)
        return JS_EXCEPTION;
    ta = p->u.typed_array;
    return JS_DupValue(ctx, JS_MKPTR(JS_TAG_OBJECT, ta->buffer));
}

JSValue js_typed_array_get_byteLength(JSContext *ctx, JSValueConst this_val, int is_dataview)
{
    JSObject *p;
    JSTypedArray *ta;
    p = get_typed_array(ctx, this_val, is_dataview);
    if (!p)
        return JS_EXCEPTION;
    if (typed_array_is_detached(ctx, p)) {
        if (is_dataview) {
            return JS_ThrowTypeErrorDetachedArrayBuffer(ctx);
        } else {
            return JS_NewInt32(ctx, 0);
        }
    }
    ta = p->u.typed_array;
    return JS_NewInt32(ctx, ta->length);
}

JSValue js_typed_array_get_byteOffset(JSContext *ctx, JSValueConst this_val, int is_dataview)
{
    JSObject *p;
    JSTypedArray *ta;
    p = get_typed_array(ctx, this_val, is_dataview);
    if (!p)
        return JS_EXCEPTION;
    if (typed_array_is_detached(ctx, p)) {
        if (is_dataview) {
            return JS_ThrowTypeErrorDetachedArrayBuffer(ctx);
        } else {
            return JS_NewInt32(ctx, 0);
        }
    }
    ta = p->u.typed_array;
    return JS_NewInt32(ctx, ta->offset);
}

/* Return the buffer associated to the typed array or an exception if
   it is not a typed array or if the buffer is detached. pbyte_offset,
   pbyte_length or pbytes_per_element can be NULL. */
JSValue JS_GetTypedArrayBuffer(JSContext *ctx, JSValueConst obj,
                               size_t *pbyte_offset,
                               size_t *pbyte_length,
                               size_t *pbytes_per_element)
{
    JSObject *p;
    JSTypedArray *ta;
    p = get_typed_array(ctx, obj, FALSE);
    if (!p)
        return JS_EXCEPTION;
    if (typed_array_is_detached(ctx, p))
        return JS_ThrowTypeErrorDetachedArrayBuffer(ctx);
    ta = p->u.typed_array;
    if (pbyte_offset)
        *pbyte_offset = ta->offset;
    if (pbyte_length)
        *pbyte_length = ta->length;
    if (pbytes_per_element) {
        *pbytes_per_element = 1 << typed_array_size_log2(p->class_id);
    }
    return JS_DupValue(ctx, JS_MKPTR(JS_TAG_OBJECT, ta->buffer));
}

static JSValue js_typed_array_get_toStringTag(JSContext *ctx,
                                              JSValueConst this_val)
{
    JSObject *p;
    if (JS_VALUE_GET_TAG(this_val) != JS_TAG_OBJECT)
        return JS_UNDEFINED;
    p = JS_VALUE_GET_OBJ(this_val);
    if (!(p->class_id >= JS_CLASS_UINT8C_ARRAY &&
          p->class_id <= JS_CLASS_FLOAT64_ARRAY))
        return JS_UNDEFINED;
    return JS_AtomToString(ctx, ctx->rt->class_array[p->class_id].class_name);
}

static JSValue js_typed_array_set_internal(JSContext *ctx,
                                           JSValueConst dst,
                                           JSValueConst src,
                                           JSValueConst off)
{
    JSObject *p;
    JSObject *src_p;
    uint32_t i;
    int64_t src_len, offset;
    JSValue val, src_obj = JS_UNDEFINED;
    p = get_typed_array(ctx, dst, 0);
    if (!p)
        goto fail;
    if (JS_ToInt64Sat(ctx, &offset, off))
        goto fail;
    if (offset < 0)
        goto range_error;
    if (typed_array_is_detached(ctx, p)) {
    detached:
        JS_ThrowTypeErrorDetachedArrayBuffer(ctx);
        goto fail;
    }
    src_obj = JS_ToObject(ctx, src);
    if (JS_IsException(src_obj))
        goto fail;
    src_p = JS_VALUE_GET_OBJ(src_obj);
    if (src_p->class_id >= JS_CLASS_UINT8C_ARRAY &&
        src_p->class_id <= JS_CLASS_FLOAT64_ARRAY) {
        JSTypedArray *dest_ta = p->u.typed_array;
        JSArrayBuffer *dest_abuf = dest_ta->buffer->u.array_buffer;
        JSTypedArray *src_ta = src_p->u.typed_array;
        JSArrayBuffer *src_abuf = src_ta->buffer->u.array_buffer;
        int shift = typed_array_size_log2(p->class_id);
        if (src_abuf->detached)
            goto detached;
        src_len = src_p->u.array.count;
        if (offset > (int64_t)(p->u.array.count - src_len))
            goto range_error;
        /* copying between typed objects */
        if (src_p->class_id == p->class_id) {
            /* same type, use memmove */
            memmove(dest_abuf->data + dest_ta->offset + (offset << shift),
                    src_abuf->data + src_ta->offset, src_len << shift);
            goto done;
        }
        if (dest_abuf->data == src_abuf->data) {
            /* copying between the same buffer using different types of mappings
               would require a temporary buffer */
        }
        /* otherwise, default behavior is slow but correct */
    } else {
        if (js_get_length64(ctx, &src_len, src_obj))
            goto fail;
        if (offset > (int64_t)(p->u.array.count - src_len)) {
        range_error:
            JS_ThrowRangeError(ctx, "invalid array length");
            goto fail;
        }
    }
    for(i = 0; i < src_len; i++) {
        val = JS_GetPropertyUint32(ctx, src_obj, i);
        if (JS_IsException(val))
            goto fail;
        if (JS_SetPropertyUint32(ctx, dst, offset + i, val) < 0)
            goto fail;
    }
done:
    JS_FreeValue(ctx, src_obj);
    return JS_UNDEFINED;
fail:
    JS_FreeValue(ctx, src_obj);
    return JS_EXCEPTION;
}

static JSValue js_typed_array_set(JSContext *ctx,
                                  JSValueConst this_val,
                                  int argc, JSValueConst *argv)
{
    JSValueConst offset = JS_UNDEFINED;
    if (argc > 1) {
        offset = argv[1];
    }
    return js_typed_array_set_internal(ctx, this_val, argv[0], offset);
}

static JSValue js_create_typed_array_iterator(JSContext *ctx, JSValueConst this_val,
                                              int argc, JSValueConst *argv, int magic)
{
    if (validate_typed_array(ctx, this_val))
        return JS_EXCEPTION;
    return js_create_array_iterator(ctx, this_val, argc, argv, magic);
}

/* return < 0 if exception */
int js_typed_array_get_length_internal(JSContext *ctx, JSValueConst obj)
{
    JSObject *p;
    p = get_typed_array(ctx, obj, 0);
    if (!p)
        return -1;
    if (typed_array_is_detached(ctx, p)) {
        JS_ThrowTypeErrorDetachedArrayBuffer(ctx);
        return -1;
    }
    return p->u.array.count;
}

#if 0
/* validate a typed array and return its length */
static JSValue js_typed_array___getLength(JSContext *ctx,
                                          JSValueConst this_val,
                                          int argc, JSValueConst *argv)
{
    BOOL ignore_detached = JS_ToBool(ctx, argv[1]);
    if (ignore_detached) {
        return js_typed_array_get_length(ctx, argv[0]);
    } else {
        int len;
        len = js_typed_array_get_length_internal(ctx, argv[0]);
        if (len < 0)
            return JS_EXCEPTION;
        return JS_NewInt32(ctx, len);
    }
}
#endif

static JSValue js_typed_array_create(JSContext *ctx, JSValueConst ctor,
                                     int argc, JSValueConst *argv)
{
    JSValue ret;
    int new_len;
    int64_t len;
    ret = JS_CallConstructor(ctx, ctor, argc, argv);
    if (JS_IsException(ret))
        return ret;
    /* validate the typed array */
    new_len = js_typed_array_get_length_internal(ctx, ret);
    if (new_len < 0)
        goto fail;
    if (argc == 1) {
        /* ensure that it is large enough */
        if (JS_ToLengthFree(ctx, &len, JS_DupValue(ctx, argv[0])))
            goto fail;
        if (new_len < len) {
            JS_ThrowTypeError(ctx, "TypedArray length is too small");
        fail:
            JS_FreeValue(ctx, ret);
            return JS_EXCEPTION;
        }
    }
    return ret;
}

#if 0
static JSValue js_typed_array___create(JSContext *ctx,
                                       JSValueConst this_val,
                                       int argc, JSValueConst *argv)
{
    return js_typed_array_create(ctx, argv[0], max_int(argc - 1, 0), argv + 1);
}
#endif

JSValue js_typed_array___speciesCreate(JSContext *ctx,
                                       JSValueConst this_val,
                                       int argc, JSValueConst *argv)
{
    JSValueConst obj;
    JSObject *p;
    JSValue ctor, ret;
    int argc1;
    obj = argv[0];
    p = get_typed_array(ctx, obj, 0);
    if (!p)
        return JS_EXCEPTION;
    ctor = JS_SpeciesConstructor(ctx, obj, JS_UNDEFINED);
    if (JS_IsException(ctor))
        return ctor;
    argc1 = max_int(argc - 1, 0);
    if (JS_IsUndefined(ctor)) {
        ret = js_typed_array_constructor(ctx, JS_UNDEFINED, argc1, argv + 1,
                                         p->class_id);
    } else {
        ret = js_typed_array_create(ctx, ctor, argc1, argv + 1);
        JS_FreeValue(ctx, ctor);
    }
    return ret;
}

static JSValue js_typed_array_from(JSContext *ctx, JSValueConst this_val,
                                   int argc, JSValueConst *argv)
{
    // from(items, mapfn = void 0, this_arg = void 0)
    JSValueConst items = argv[0], mapfn, this_arg;
    JSValueConst args[2];
    JSValue stack[2];
    JSValue iter, arr, r, v, v2;
    int64_t k, len;
    int done, mapping;
    mapping = FALSE;
    mapfn = JS_UNDEFINED;
    this_arg = JS_UNDEFINED;
    r = JS_UNDEFINED;
    arr = JS_UNDEFINED;
    stack[0] = JS_UNDEFINED;
    stack[1] = JS_UNDEFINED;
    if (argc > 1) {
        mapfn = argv[1];
        if (!JS_IsUndefined(mapfn)) {
            if (check_function(ctx, mapfn))
                goto exception;
            mapping = 1;
            if (argc > 2)
                this_arg = argv[2];
        }
    }
    iter = JS_GetProperty(ctx, items, JS_ATOM_Symbol_iterator);
    if (JS_IsException(iter))
        goto exception;
    if (!JS_IsUndefined(iter)) {
        JS_FreeValue(ctx, iter);
        arr = JS_NewArray(ctx);
        if (JS_IsException(arr))
            goto exception;
        stack[0] = JS_DupValue(ctx, items);
        if (js_for_of_start(ctx, &stack[1], FALSE))
            goto exception;
        for (k = 0;; k++) {
            v = JS_IteratorNext(ctx, stack[0], stack[1], 0, NULL, &done);
            if (JS_IsException(v))
                goto exception_close;
            if (done)
                break;
            if (JS_DefinePropertyValueInt64(ctx, arr, k, v, JS_PROP_C_W_E | JS_PROP_THROW) < 0)
                goto exception_close;
        }
    } else {
        arr = JS_ToObject(ctx, items);
        if (JS_IsException(arr))
            goto exception;
    }
    if (js_get_length64(ctx, &len, arr) < 0)
        goto exception;
    v = JS_NewInt64(ctx, len);
    args[0] = v;
    r = js_typed_array_create(ctx, this_val, 1, args);
    JS_FreeValue(ctx, v);
    if (JS_IsException(r))
        goto exception;
    for(k = 0; k < len; k++) {
        v = JS_GetPropertyInt64(ctx, arr, k);
        if (JS_IsException(v))
            goto exception;
        if (mapping) {
            args[0] = v;
            args[1] = JS_NewInt32(ctx, k);
            v2 = JS_Call(ctx, mapfn, this_arg, 2, args);
            JS_FreeValue(ctx, v);
            v = v2;
            if (JS_IsException(v))
                goto exception;
        }
        if (JS_SetPropertyInt64(ctx, r, k, v) < 0)
            goto exception;
    }
    goto done;
 exception_close:
    if (!JS_IsUndefined(stack[0]))
        JS_IteratorClose(ctx, stack[0], TRUE);
 exception:
    JS_FreeValue(ctx, r);
    r = JS_EXCEPTION;
 done:
    JS_FreeValue(ctx, arr);
    JS_FreeValue(ctx, stack[0]);
    JS_FreeValue(ctx, stack[1]);
    return r;
}

static JSValue js_typed_array_of(JSContext *ctx, JSValueConst this_val,
                                 int argc, JSValueConst *argv)
{
    JSValue obj;
    JSValueConst args[1];
    int i;
    args[0] = JS_NewInt32(ctx, argc);
    obj = js_typed_array_create(ctx, this_val, 1, args);
    if (JS_IsException(obj))
        return obj;
    for(i = 0; i < argc; i++) {
        if (JS_SetPropertyUint32(ctx, obj, i, JS_DupValue(ctx, argv[i])) < 0) {
            JS_FreeValue(ctx, obj);
            return JS_EXCEPTION;
        }
    }
    return obj;
}

static JSValue js_typed_array_copyWithin(JSContext *ctx, JSValueConst this_val,
                                         int argc, JSValueConst *argv)
{
    JSObject *p;
    int len, to, from, final, count, shift;
    len = js_typed_array_get_length_internal(ctx, this_val);
    if (len < 0)
        return JS_EXCEPTION;
    if (JS_ToInt32Clamp(ctx, &to, argv[0], 0, len, len))
        return JS_EXCEPTION;
    if (JS_ToInt32Clamp(ctx, &from, argv[1], 0, len, len))
        return JS_EXCEPTION;
    final = len;
    if (argc > 2 && !JS_IsUndefined(argv[2])) {
        if (JS_ToInt32Clamp(ctx, &final, argv[2], 0, len, len))
            return JS_EXCEPTION;
    }
    count = min_int(final - from, len - to);
    if (count > 0) {
        p = JS_VALUE_GET_OBJ(this_val);
        if (typed_array_is_detached(ctx, p))
            return JS_ThrowTypeErrorDetachedArrayBuffer(ctx);
        shift = typed_array_size_log2(p->class_id);
        memmove(p->u.array.u.uint8_ptr + (to << shift),
                p->u.array.u.uint8_ptr + (from << shift),
                count << shift);
    }
    return JS_DupValue(ctx, this_val);
}

static JSValue js_typed_array_fill(JSContext *ctx, JSValueConst this_val,
                                   int argc, JSValueConst *argv)
{
    JSObject *p;
    int len, k, final, shift;
    uint64_t v64;
    len = js_typed_array_get_length_internal(ctx, this_val);
    if (len < 0)
        return JS_EXCEPTION;
    p = JS_VALUE_GET_OBJ(this_val);
    if (p->class_id == JS_CLASS_UINT8C_ARRAY) {
        int32_t v;
        if (JS_ToUint8ClampFree(ctx, &v, JS_DupValue(ctx, argv[0])))
            return JS_EXCEPTION;
        v64 = v;
    } else if (p->class_id <= JS_CLASS_UINT32_ARRAY) {
        uint32_t v;
        if (JS_ToUint32(ctx, &v, argv[0]))
            return JS_EXCEPTION;
        v64 = v;
    } else
#ifdef CONFIG_BIGNUM
    if (p->class_id <= JS_CLASS_BIG_UINT64_ARRAY) {
        if (JS_ToBigInt64(ctx, (int64_t *)&v64, argv[0]))
            return JS_EXCEPTION;
    } else
#endif
    {
        double d;
        if (JS_ToFloat64(ctx, &d, argv[0]))
            return JS_EXCEPTION;
        if (p->class_id == JS_CLASS_FLOAT32_ARRAY) {
            union {
                float f;
                uint32_t u32;
            } u;
            u.f = d;
            v64 = u.u32;
        } else {
            JSFloat64Union u;
            u.d = d;
            v64 = u.u64;
        }
    }
    k = 0;
    if (argc > 1) {
        if (JS_ToInt32Clamp(ctx, &k, argv[1], 0, len, len))
            return JS_EXCEPTION;
    }
    final = len;
    if (argc > 2 && !JS_IsUndefined(argv[2])) {
        if (JS_ToInt32Clamp(ctx, &final, argv[2], 0, len, len))
            return JS_EXCEPTION;
    }
    if (typed_array_is_detached(ctx, p))
        return JS_ThrowTypeErrorDetachedArrayBuffer(ctx);
    shift = typed_array_size_log2(p->class_id);
    switch(shift) {
    case 0:
        if (k < final) {
            memset(p->u.array.u.uint8_ptr + k, v64, final - k);
        }
        break;
    case 1:
        for(; k < final; k++) {
            p->u.array.u.uint16_ptr[k] = v64;
        }
        break;
    case 2:
        for(; k < final; k++) {
            p->u.array.u.uint32_ptr[k] = v64;
        }
        break;
    case 3:
        for(; k < final; k++) {
            p->u.array.u.uint64_ptr[k] = v64;
        }
        break;
    default:
        abort();
    }
    return JS_DupValue(ctx, this_val);
}

static JSValue js_typed_array_find(JSContext *ctx, JSValueConst this_val,
                                   int argc, JSValueConst *argv, int findIndex)
{
    JSValueConst func, this_arg;
    JSValueConst args[3];
    JSValue val, index_val, res;
    int len, k;
    val = JS_UNDEFINED;
    len = js_typed_array_get_length_internal(ctx, this_val);
    if (len < 0)
        goto exception;
    func = argv[0];
    if (check_function(ctx, func))
        goto exception;
    this_arg = JS_UNDEFINED;
    if (argc > 1)
        this_arg = argv[1];
    for(k = 0; k < len; k++) {
        index_val = JS_NewInt32(ctx, k);
        val = JS_GetPropertyValue(ctx, this_val, index_val);
        if (JS_IsException(val))
            goto exception;
        args[0] = val;
        args[1] = index_val;
        args[2] = this_val;
        res = JS_Call(ctx, func, this_arg, 3, args);
        if (JS_IsException(res))
            goto exception;
        if (JS_ToBoolFree(ctx, res)) {
            if (findIndex) {
                JS_FreeValue(ctx, val);
                return index_val;
            } else {
                return val;
            }
        }
        JS_FreeValue(ctx, val);
    }
    if (findIndex)
        return JS_NewInt32(ctx, -1);
    else
        return JS_UNDEFINED;
exception:
    JS_FreeValue(ctx, val);
    return JS_EXCEPTION;
}

#define special_indexOf 0
#define special_lastIndexOf 1
#define special_includes -1

static JSValue js_typed_array_indexOf(JSContext *ctx, JSValueConst this_val,
                                      int argc, JSValueConst *argv, int special)
{
    JSObject *p;
    int len, tag, is_int, is_bigint, k, stop, inc, res = -1;
    int64_t v64;
    double d;
    float f;
    len = js_typed_array_get_length_internal(ctx, this_val);
    if (len < 0)
        goto exception;
    if (len == 0)
        goto done;
    if (special == special_lastIndexOf) {
        k = len - 1;
        if (argc > 1) {
            if (JS_ToFloat64(ctx, &d, argv[1]))
                goto exception;
            if (isnan(d)) {
                k = 0;
            } else {
                if (d >= 0) {
                    if (d < k) {
                        k = d;
                    }
                } else {
                    d += len;
                    if (d < 0)
                        goto done;
                    k = d;
                }
            }
        }
        stop = -1;
        inc = -1;
    } else {
        k = 0;
        if (argc > 1) {
            if (JS_ToInt32Clamp(ctx, &k, argv[1], 0, len, len))
                goto exception;
        }
        stop = len;
        inc = 1;
    }
    p = JS_VALUE_GET_OBJ(this_val);
    /* if the array was detached, no need to go further (but no
       exception is raised) */
    if (typed_array_is_detached(ctx, p)) {
        /* "includes" scans all the properties, so "undefined" can match */
        if (special == special_includes && JS_IsUndefined(argv[0]) && len > 0)
            res = 0;
        goto done;
    }
    is_bigint = 0;
    is_int = 0; /* avoid warning */
    v64 = 0; /* avoid warning */
    tag = JS_VALUE_GET_NORM_TAG(argv[0]);
    if (tag == JS_TAG_INT) {
        is_int = 1;
        v64 = JS_VALUE_GET_INT(argv[0]);
        d = v64;
    } else
    if (tag == JS_TAG_FLOAT64) {
        d = JS_VALUE_GET_FLOAT64(argv[0]);
        v64 = d;
        is_int = (v64 == d);
    } else
#ifdef CONFIG_BIGNUM
    if (tag == JS_TAG_BIG_INT) {
        JSBigFloat *p1 = JS_VALUE_GET_PTR(argv[0]);
        if (p->class_id == JS_CLASS_BIG_INT64_ARRAY) {
            if (bf_get_int64(&v64, &p1->num, 0) != 0)
                goto done;
        } else if (p->class_id == JS_CLASS_BIG_UINT64_ARRAY) {
            if (bf_get_uint64((uint64_t *)&v64, &p1->num) != 0)
                goto done;
        } else {
            goto done;
        }
        d = 0;
        is_bigint = 1;
    } else
#endif
    {
        goto done;
    }
    switch (p->class_id) {
    case JS_CLASS_INT8_ARRAY:
        if (is_int && (int8_t)v64 == v64)
            goto scan8;
        break;
    case JS_CLASS_UINT8C_ARRAY:
    case JS_CLASS_UINT8_ARRAY:
        if (is_int && (uint8_t)v64 == v64) {
            const uint8_t *pv, *pp;
            uint16_t v;
        scan8:
            pv = p->u.array.u.uint8_ptr;
            v = v64;
            if (inc > 0) {
                pp = memchr(pv + k, v, len - k);
                if (pp)
                    res = pp - pv;
            } else {
                for (; k != stop; k += inc) {
                    if (pv[k] == v) {
                        res = k;
                        break;
                    }
                }
            }
        }
        break;
    case JS_CLASS_INT16_ARRAY:
        if (is_int && (int16_t)v64 == v64)
            goto scan16;
        break;
    case JS_CLASS_UINT16_ARRAY:
        if (is_int && (uint16_t)v64 == v64) {
            const uint16_t *pv;
            uint16_t v;
        scan16:
            pv = p->u.array.u.uint16_ptr;
            v = v64;
            for (; k != stop; k += inc) {
                if (pv[k] == v) {
                    res = k;
                    break;
                }
            }
        }
        break;
    case JS_CLASS_INT32_ARRAY:
        if (is_int && (int32_t)v64 == v64)
            goto scan32;
        break;
    case JS_CLASS_UINT32_ARRAY:
        if (is_int && (uint32_t)v64 == v64) {
            const uint32_t *pv;
            uint32_t v;
        scan32:
            pv = p->u.array.u.uint32_ptr;
            v = v64;
            for (; k != stop; k += inc) {
                if (pv[k] == v) {
                    res = k;
                    break;
                }
            }
        }
        break;
    case JS_CLASS_FLOAT32_ARRAY:
        if (is_bigint)
            break;
        if (isnan(d)) {
            const float *pv = p->u.array.u.float_ptr;
            /* special case: indexOf returns -1, includes finds NaN */
            if (special != special_includes)
                goto done;
            for (; k != stop; k += inc) {
                if (isnan(pv[k])) {
                    res = k;
                    break;
                }
            }
        } else if ((f = (float)d) == d) {
            const float *pv = p->u.array.u.float_ptr;
            for (; k != stop; k += inc) {
                if (pv[k] == f) {
                    res = k;
                    break;
                }
            }
        }
        break;
    case JS_CLASS_FLOAT64_ARRAY:
        if (is_bigint)
            break;
        if (isnan(d)) {
            const double *pv = p->u.array.u.double_ptr;
            /* special case: indexOf returns -1, includes finds NaN */
            if (special != special_includes)
                goto done;
            for (; k != stop; k += inc) {
                if (isnan(pv[k])) {
                    res = k;
                    break;
                }
            }
        } else {
            const double *pv = p->u.array.u.double_ptr;
            for (; k != stop; k += inc) {
                if (pv[k] == d) {
                    res = k;
                    break;
                }
            }
        }
        break;
#ifdef CONFIG_BIGNUM
    case JS_CLASS_BIG_INT64_ARRAY:
        if (is_bigint || (is_math_mode(ctx) && is_int &&
                          v64 >= -MAX_SAFE_INTEGER &&
                          v64 <= MAX_SAFE_INTEGER)) {
            goto scan64;
        }
        break;
    case JS_CLASS_BIG_UINT64_ARRAY:
        if (is_bigint || (is_math_mode(ctx) && is_int &&
                          v64 >= 0 && v64 <= MAX_SAFE_INTEGER)) {
            const uint64_t *pv;
            uint64_t v;
        scan64:
            pv = p->u.array.u.uint64_ptr;
            v = v64;
            for (; k != stop; k += inc) {
                if (pv[k] == v) {
                    res = k;
                    break;
                }
            }
        }
        break;
#endif
    }
done:
    if (special == special_includes)
        return JS_NewBool(ctx, res >= 0);
    else
        return JS_NewInt32(ctx, res);
exception:
    return JS_EXCEPTION;
}

static JSValue js_typed_array_join(JSContext *ctx, JSValueConst this_val,
                                   int argc, JSValueConst *argv, int toLocaleString)
{
    JSValue sep = JS_UNDEFINED, el;
    StringBuffer b_s, *b = &b_s;
    JSString *p = NULL;
    int i, n;
    int c;
    n = js_typed_array_get_length_internal(ctx, this_val);
    if (n < 0)
        goto exception;
    c = ',';    /* default separator */
    if (!toLocaleString && argc > 0 && !JS_IsUndefined(argv[0])) {
        sep = JS_ToString(ctx, argv[0]);
        if (JS_IsException(sep))
            goto exception;
        p = JS_VALUE_GET_STRING(sep);
        if (p->len == 1 && !p->is_wide_char)
            c = p->u.str8[0];
        else
            c = -1;
    }
    string_buffer_init(ctx, b, 0);
    /* XXX: optimize with direct access */
    for(i = 0; i < n; i++) {
        if (i > 0) {
            if (c >= 0) {
                if (string_buffer_putc8(b, c))
                    goto fail;
            } else {
                if (string_buffer_concat(b, p, 0, p->len))
                    goto fail;
            }
        }
        el = JS_GetPropertyUint32(ctx, this_val, i);
        /* Can return undefined for example if the typed array is detached */
        if (!JS_IsNull(el) && !JS_IsUndefined(el)) {
            if (JS_IsException(el))
                goto fail;
            if (toLocaleString) {
                el = JS_ToLocaleStringFree(ctx, el);
            }
            if (string_buffer_concat_value_free(b, el))
                goto fail;
        }
    }
    JS_FreeValue(ctx, sep);
    return string_buffer_end(b);
fail:
    string_buffer_free(b);
    JS_FreeValue(ctx, sep);
exception:
    return JS_EXCEPTION;
}

static JSValue js_typed_array_reverse(JSContext *ctx, JSValueConst this_val,
                                      int argc, JSValueConst *argv)
{
    JSObject *p;
    int len;
    len = js_typed_array_get_length_internal(ctx, this_val);
    if (len < 0)
        return JS_EXCEPTION;
    if (len > 0) {
        p = JS_VALUE_GET_OBJ(this_val);
        switch (typed_array_size_log2(p->class_id)) {
        case 0:
            {
                uint8_t *p1 = p->u.array.u.uint8_ptr;
                uint8_t *p2 = p1 + len - 1;
                while (p1 < p2) {
                    uint8_t v = *p1;
                    *p1++ = *p2;
                    *p2-- = v;
                }
            }
            break;
        case 1:
            {
                uint16_t *p1 = p->u.array.u.uint16_ptr;
                uint16_t *p2 = p1 + len - 1;
                while (p1 < p2) {
                    uint16_t v = *p1;
                    *p1++ = *p2;
                    *p2-- = v;
                }
            }
            break;
        case 2:
            {
                uint32_t *p1 = p->u.array.u.uint32_ptr;
                uint32_t *p2 = p1 + len - 1;
                while (p1 < p2) {
                    uint32_t v = *p1;
                    *p1++ = *p2;
                    *p2-- = v;
                }
            }
            break;
        case 3:
            {
                uint64_t *p1 = p->u.array.u.uint64_ptr;
                uint64_t *p2 = p1 + len - 1;
                while (p1 < p2) {
                    uint64_t v = *p1;
                    *p1++ = *p2;
                    *p2-- = v;
                }
            }
            break;
        default:
            abort();
        }
    }
    return JS_DupValue(ctx, this_val);
}

static JSValue js_typed_array_slice(JSContext *ctx, JSValueConst this_val,
                                    int argc, JSValueConst *argv)
{
    JSValueConst args[2];
    JSValue arr, val;
    JSObject *p, *p1;
    int n, len, start, final, count, shift;
    arr = JS_UNDEFINED;
    len = js_typed_array_get_length_internal(ctx, this_val);
    if (len < 0)
        goto exception;
    if (JS_ToInt32Clamp(ctx, &start, argv[0], 0, len, len))
        goto exception;
    final = len;
    if (!JS_IsUndefined(argv[1])) {
        if (JS_ToInt32Clamp(ctx, &final, argv[1], 0, len, len))
            goto exception;
    }
    count = max_int(final - start, 0);
    p = get_typed_array(ctx, this_val, 0);
    if (p == NULL)
        goto exception;
    shift = typed_array_size_log2(p->class_id);
    args[0] = this_val;
    args[1] = JS_NewInt32(ctx, count);
    arr = js_typed_array___speciesCreate(ctx, JS_UNDEFINED, 2, args);
    if (JS_IsException(arr))
        goto exception;
    if (count > 0) {
        if (validate_typed_array(ctx, this_val)
        ||  validate_typed_array(ctx, arr))
            goto exception;
        p1 = get_typed_array(ctx, arr, 0);
        if (p1 != NULL && p->class_id == p1->class_id &&
            typed_array_get_length(ctx, p1) >= count &&
            typed_array_get_length(ctx, p) >= start + count) {
            memcpy(p1->u.array.u.uint8_ptr,
                   p->u.array.u.uint8_ptr + (start << shift),
                   count << shift);
        } else {
            for (n = 0; n < count; n++) {
                val = JS_GetPropertyValue(ctx, this_val, JS_NewInt32(ctx, start + n));
                if (JS_IsException(val))
                    goto exception;
                if (JS_SetPropertyValue(ctx, arr, JS_NewInt32(ctx, n), val,
                                        JS_PROP_THROW) < 0)
                    goto exception;
            }
        }
    }
    return arr;
 exception:
    JS_FreeValue(ctx, arr);
    return JS_EXCEPTION;
}

static JSValue js_typed_array_subarray(JSContext *ctx, JSValueConst this_val,
                                       int argc, JSValueConst *argv)
{
    JSValueConst args[4];
    JSValue arr, byteOffset, ta_buffer;
    JSObject *p;
    int len, start, final, count, shift, offset;
    p = get_typed_array(ctx, this_val, 0);
    if (!p)
        goto exception;
    len = p->u.array.count;
    if (JS_ToInt32Clamp(ctx, &start, argv[0], 0, len, len))
        goto exception;
    final = len;
    if (!JS_IsUndefined(argv[1])) {
        if (JS_ToInt32Clamp(ctx, &final, argv[1], 0, len, len))
            goto exception;
    }
    count = max_int(final - start, 0);
    byteOffset = js_typed_array_get_byteOffset(ctx, this_val, 0);
    if (JS_IsException(byteOffset))
        goto exception;
    shift = typed_array_size_log2(p->class_id);
    offset = JS_VALUE_GET_INT(byteOffset) + (start << shift);
    JS_FreeValue(ctx, byteOffset);
    ta_buffer = js_typed_array_get_buffer(ctx, this_val, 0);
    if (JS_IsException(ta_buffer))
        goto exception;
    args[0] = this_val;
    args[1] = ta_buffer;
    args[2] = JS_NewInt32(ctx, offset);
    args[3] = JS_NewInt32(ctx, count);
    arr = js_typed_array___speciesCreate(ctx, JS_UNDEFINED, 4, args);
    JS_FreeValue(ctx, ta_buffer);
    return arr;
 exception:
    return JS_EXCEPTION;
}

/* TypedArray.prototype.sort */

static int js_cmp_doubles(double x, double y)
{
    if (isnan(x))    return isnan(y) ? 0 : +1;
    if (isnan(y))    return -1;
    if (x < y)       return -1;
    if (x > y)       return 1;
    if (x != 0)      return 0;
    if (signbit(x))  return signbit(y) ? 0 : -1;
    else             return signbit(y) ? 1 : 0;
}

static int js_TA_cmp_int8(const void *a, const void *b, void *opaque) {
    return *(const int8_t *)a - *(const int8_t *)b;
}

static int js_TA_cmp_uint8(const void *a, const void *b, void *opaque) {
    return *(const uint8_t *)a - *(const uint8_t *)b;
}

static int js_TA_cmp_int16(const void *a, const void *b, void *opaque) {
    return *(const int16_t *)a - *(const int16_t *)b;
}

static int js_TA_cmp_uint16(const void *a, const void *b, void *opaque) {
    return *(const uint16_t *)a - *(const uint16_t *)b;
}

static int js_TA_cmp_int32(const void *a, const void *b, void *opaque) {
    int32_t x = *(const int32_t *)a;
    int32_t y = *(const int32_t *)b;
    return (y < x) - (y > x);
}

static int js_TA_cmp_uint32(const void *a, const void *b, void *opaque) {
    uint32_t x = *(const uint32_t *)a;
    uint32_t y = *(const uint32_t *)b;
    return (y < x) - (y > x);
}

#ifdef CONFIG_BIGNUM
static int js_TA_cmp_int64(const void *a, const void *b, void *opaque) {
    int64_t x = *(const int64_t *)a;
    int64_t y = *(const int64_t *)b;
    return (y < x) - (y > x);
}

static int js_TA_cmp_uint64(const void *a, const void *b, void *opaque) {
    uint64_t x = *(const uint64_t *)a;
    uint64_t y = *(const uint64_t *)b;
    return (y < x) - (y > x);
}
#endif

static int js_TA_cmp_float32(const void *a, const void *b, void *opaque) {
    return js_cmp_doubles(*(const float *)a, *(const float *)b);
}

static int js_TA_cmp_float64(const void *a, const void *b, void *opaque) {
    return js_cmp_doubles(*(const double *)a, *(const double *)b);
}

static JSValue js_TA_get_int8(JSContext *ctx, const void *a) {
    return JS_NewInt32(ctx, *(const int8_t *)a);
}

static JSValue js_TA_get_uint8(JSContext *ctx, const void *a) {
    return JS_NewInt32(ctx, *(const uint8_t *)a);
}

static JSValue js_TA_get_int16(JSContext *ctx, const void *a) {
    return JS_NewInt32(ctx, *(const int16_t *)a);
}

static JSValue js_TA_get_uint16(JSContext *ctx, const void *a) {
    return JS_NewInt32(ctx, *(const uint16_t *)a);
}

static JSValue js_TA_get_int32(JSContext *ctx, const void *a) {
    return JS_NewInt32(ctx, *(const int32_t *)a);
}

static JSValue js_TA_get_uint32(JSContext *ctx, const void *a) {
    return JS_NewUint32(ctx, *(const uint32_t *)a);
}

#ifdef CONFIG_BIGNUM
static JSValue js_TA_get_int64(JSContext *ctx, const void *a) {
    return JS_NewBigInt64(ctx, *(int64_t *)a);
}

static JSValue js_TA_get_uint64(JSContext *ctx, const void *a) {
    return JS_NewBigUint64(ctx, *(uint64_t *)a);
}
#endif

static JSValue js_TA_get_float32(JSContext *ctx, const void *a) {
    return __JS_NewFloat64(ctx, *(const float *)a);
}

static JSValue js_TA_get_float64(JSContext *ctx, const void *a) {
    return __JS_NewFloat64(ctx, *(const double *)a);
}

struct TA_sort_context {
    JSContext *ctx;
    int exception;
    JSValueConst arr;
    JSValueConst cmp;
    JSValue (*getfun)(JSContext *ctx, const void *a);
    uint8_t *array_ptr; /* cannot change unless the array is detached */
    int elt_size;
};

static int js_TA_cmp_generic(const void *a, const void *b, void *opaque) {
    struct TA_sort_context *psc = opaque;
    JSContext *ctx = psc->ctx;
    uint32_t a_idx, b_idx;
    JSValueConst argv[2];
    JSValue res;
    int cmp;
    cmp = 0;
    if (!psc->exception) {
        a_idx = *(uint32_t *)a;
        b_idx = *(uint32_t *)b;
        argv[0] = psc->getfun(ctx, psc->array_ptr +
                              a_idx * (size_t)psc->elt_size);
        argv[1] = psc->getfun(ctx, psc->array_ptr +
                              b_idx * (size_t)(psc->elt_size));
        res = JS_Call(ctx, psc->cmp, JS_UNDEFINED, 2, argv);
        if (JS_IsException(res)) {
            psc->exception = 1;
            goto done;
        }
        if (JS_VALUE_GET_TAG(res) == JS_TAG_INT) {
            int val = JS_VALUE_GET_INT(res);
            cmp = (val > 0) - (val < 0);
        } else {
            double val;
            if (JS_ToFloat64Free(ctx, &val, res) < 0) {
                psc->exception = 1;
                goto done;
            } else {
                cmp = (val > 0) - (val < 0);
            }
        }
        if (cmp == 0) {
            /* make sort stable: compare array offsets */
            cmp = (a_idx > b_idx) - (a_idx < b_idx);
        }
        if (validate_typed_array(ctx, psc->arr) < 0) {
            psc->exception = 1;
        }
    done:
        JS_FreeValue(ctx, (JSValue)argv[0]);
        JS_FreeValue(ctx, (JSValue)argv[1]);
    }
    return cmp;
}

static JSValue js_typed_array_sort(JSContext *ctx, JSValueConst this_val,
                                   int argc, JSValueConst *argv)
{
    JSObject *p;
    int len;
    size_t elt_size;
    struct TA_sort_context tsc;
    void *array_ptr;
    int (*cmpfun)(const void *a, const void *b, void *opaque);
    tsc.ctx = ctx;
    tsc.exception = 0;
    tsc.arr = this_val;
    tsc.cmp = argv[0];
    len = js_typed_array_get_length_internal(ctx, this_val);
    if (len < 0)
        return JS_EXCEPTION;
    if (!JS_IsUndefined(tsc.cmp) && check_function(ctx, tsc.cmp))
        return JS_EXCEPTION;
    if (len > 1) {
        p = JS_VALUE_GET_OBJ(this_val);
        switch (p->class_id) {
        case JS_CLASS_INT8_ARRAY:
            tsc.getfun = js_TA_get_int8;
            cmpfun = js_TA_cmp_int8;
            break;
        case JS_CLASS_UINT8C_ARRAY:
        case JS_CLASS_UINT8_ARRAY:
            tsc.getfun = js_TA_get_uint8;
            cmpfun = js_TA_cmp_uint8;
            break;
        case JS_CLASS_INT16_ARRAY:
            tsc.getfun = js_TA_get_int16;
            cmpfun = js_TA_cmp_int16;
            break;
        case JS_CLASS_UINT16_ARRAY:
            tsc.getfun = js_TA_get_uint16;
            cmpfun = js_TA_cmp_uint16;
            break;
        case JS_CLASS_INT32_ARRAY:
            tsc.getfun = js_TA_get_int32;
            cmpfun = js_TA_cmp_int32;
            break;
        case JS_CLASS_UINT32_ARRAY:
            tsc.getfun = js_TA_get_uint32;
            cmpfun = js_TA_cmp_uint32;
            break;
#ifdef CONFIG_BIGNUM
        case JS_CLASS_BIG_INT64_ARRAY:
            tsc.getfun = js_TA_get_int64;
            cmpfun = js_TA_cmp_int64;
            break;
        case JS_CLASS_BIG_UINT64_ARRAY:
            tsc.getfun = js_TA_get_uint64;
            cmpfun = js_TA_cmp_uint64;
            break;
#endif
        case JS_CLASS_FLOAT32_ARRAY:
            tsc.getfun = js_TA_get_float32;
            cmpfun = js_TA_cmp_float32;
            break;
        case JS_CLASS_FLOAT64_ARRAY:
            tsc.getfun = js_TA_get_float64;
            cmpfun = js_TA_cmp_float64;
            break;
        default:
            abort();
        }
        array_ptr = p->u.array.u.ptr;
        elt_size = 1 << typed_array_size_log2(p->class_id);
        if (!JS_IsUndefined(tsc.cmp)) {
            uint32_t *array_idx;
            void *array_tmp;
            size_t i, j;
            /* XXX: a stable sort would use less memory */
            array_idx = js_malloc(ctx, len * sizeof(array_idx[0]));
            if (!array_idx)
                return JS_EXCEPTION;
            for(i = 0; i < len; i++)
                array_idx[i] = i;
            tsc.array_ptr = array_ptr;
            tsc.elt_size = elt_size;
            rqsort(array_idx, len, sizeof(array_idx[0]),
                   js_TA_cmp_generic, &tsc);
            if (tsc.exception)
                goto fail;
            array_tmp = js_malloc(ctx, len * elt_size);
            if (!array_tmp) {
            fail:
                js_free(ctx, array_idx);
                return JS_EXCEPTION;
            }
            memcpy(array_tmp, array_ptr, len * elt_size);
            switch(elt_size) {
            case 1:
                for(i = 0; i < len; i++) {
                    j = array_idx[i];
                    ((uint8_t *)array_ptr)[i] = ((uint8_t *)array_tmp)[j];
                }
                break;
            case 2:
                for(i = 0; i < len; i++) {
                    j = array_idx[i];
                    ((uint16_t *)array_ptr)[i] = ((uint16_t *)array_tmp)[j];
                }
                break;
            case 4:
                for(i = 0; i < len; i++) {
                    j = array_idx[i];
                    ((uint32_t *)array_ptr)[i] = ((uint32_t *)array_tmp)[j];
                }
                break;
            case 8:
                for(i = 0; i < len; i++) {
                    j = array_idx[i];
                    ((uint64_t *)array_ptr)[i] = ((uint64_t *)array_tmp)[j];
                }
                break;
            default:
                abort();
            }
            js_free(ctx, array_tmp);
            js_free(ctx, array_idx);
        } else {
            rqsort(array_ptr, len, elt_size, cmpfun, &tsc);
            if (tsc.exception)
                return JS_EXCEPTION;
        }
    }
    return JS_DupValue(ctx, this_val);
}

static const JSCFunctionListEntry js_typed_array_base_funcs[] = {
    JS_CFUNC_DEF("from", 1, js_typed_array_from ),
    JS_CFUNC_DEF("of", 0, js_typed_array_of ),
    JS_CGETSET_DEF("[Symbol.species]", js_get_this, NULL ),
    //JS_CFUNC_DEF("__getLength", 2, js_typed_array___getLength ),
    //JS_CFUNC_DEF("__create", 2, js_typed_array___create ),
    //JS_CFUNC_DEF("__speciesCreate", 2, js_typed_array___speciesCreate ),
};

static const JSCFunctionListEntry js_typed_array_base_proto_funcs[] = {
    JS_CGETSET_DEF("length", js_typed_array_get_length, NULL ),
    JS_CGETSET_MAGIC_DEF("buffer", js_typed_array_get_buffer, NULL, 0 ),
    JS_CGETSET_MAGIC_DEF("byteLength", js_typed_array_get_byteLength, NULL, 0 ),
    JS_CGETSET_MAGIC_DEF("byteOffset", js_typed_array_get_byteOffset, NULL, 0 ),
    JS_CFUNC_DEF("set", 1, js_typed_array_set ),
    JS_CFUNC_MAGIC_DEF("values", 0, js_create_typed_array_iterator, JS_ITERATOR_KIND_VALUE ),
    JS_ALIAS_DEF("[Symbol.iterator]", "values" ),
    JS_CFUNC_MAGIC_DEF("keys", 0, js_create_typed_array_iterator, JS_ITERATOR_KIND_KEY ),
    JS_CFUNC_MAGIC_DEF("entries", 0, js_create_typed_array_iterator, JS_ITERATOR_KIND_KEY_AND_VALUE ),
    JS_CGETSET_DEF("[Symbol.toStringTag]", js_typed_array_get_toStringTag, NULL ),
    JS_CFUNC_DEF("copyWithin", 2, js_typed_array_copyWithin ),
    JS_CFUNC_MAGIC_DEF("every", 1, js_array_every, special_every | special_TA ),
    JS_CFUNC_MAGIC_DEF("some", 1, js_array_every, special_some | special_TA ),
    JS_CFUNC_MAGIC_DEF("forEach", 1, js_array_every, special_forEach | special_TA ),
    JS_CFUNC_MAGIC_DEF("map", 1, js_array_every, special_map | special_TA ),
    JS_CFUNC_MAGIC_DEF("filter", 1, js_array_every, special_filter | special_TA ),
    JS_CFUNC_MAGIC_DEF("reduce", 1, js_array_reduce, special_reduce | special_TA ),
    JS_CFUNC_MAGIC_DEF("reduceRight", 1, js_array_reduce, special_reduceRight | special_TA ),
    JS_CFUNC_DEF("fill", 1, js_typed_array_fill ),
    JS_CFUNC_MAGIC_DEF("find", 1, js_typed_array_find, 0 ),
    JS_CFUNC_MAGIC_DEF("findIndex", 1, js_typed_array_find, 1 ),
    JS_CFUNC_DEF("reverse", 0, js_typed_array_reverse ),
    JS_CFUNC_DEF("slice", 2, js_typed_array_slice ),
    JS_CFUNC_DEF("subarray", 2, js_typed_array_subarray ),
    JS_CFUNC_DEF("sort", 1, js_typed_array_sort ),
    JS_CFUNC_MAGIC_DEF("join", 1, js_typed_array_join, 0 ),
    JS_CFUNC_MAGIC_DEF("toLocaleString", 0, js_typed_array_join, 1 ),
    JS_CFUNC_MAGIC_DEF("indexOf", 1, js_typed_array_indexOf, special_indexOf ),
    JS_CFUNC_MAGIC_DEF("lastIndexOf", 1, js_typed_array_indexOf, special_lastIndexOf ),
    JS_CFUNC_MAGIC_DEF("includes", 1, js_typed_array_indexOf, special_includes ),
    //JS_ALIAS_BASE_DEF("toString", "toString", 2 /* Array.prototype. */), @@@
};

static JSValue js_typed_array_base_constructor(JSContext *ctx,
                                               JSValueConst this_val,
                                               int argc, JSValueConst *argv)
{
    return JS_ThrowTypeError(ctx, "cannot be called");
}

/* 'obj' must be an allocated typed array object */
static int typed_array_init(JSContext *ctx, JSValueConst obj,
                            JSValue buffer, uint64_t offset, uint64_t len)
{
    JSTypedArray *ta;
    JSObject *p, *pbuffer;
    JSArrayBuffer *abuf;
    int size_log2;
    p = JS_VALUE_GET_OBJ(obj);
    size_log2 = typed_array_size_log2(p->class_id);
    ta = js_malloc(ctx, sizeof(*ta));
    if (!ta) {
        JS_FreeValue(ctx, buffer);
        return -1;
    }
    pbuffer = JS_VALUE_GET_OBJ(buffer);
    abuf = pbuffer->u.array_buffer;
    ta->obj = p;
    ta->buffer = pbuffer;
    ta->offset = offset;
    ta->length = len << size_log2;
    list_add_tail(&ta->link, &abuf->array_list);
    p->u.typed_array = ta;
    p->u.array.count = len;
    p->u.array.u.ptr = abuf->data + offset;
    return 0;
}

static JSValue js_array_from_iterator(JSContext *ctx, uint32_t *plen,
                                      JSValueConst obj, JSValueConst method)
{
    JSValue arr, iter, next_method = JS_UNDEFINED, val;
    BOOL done;
    uint32_t k;
    *plen = 0;
    arr = JS_NewArray(ctx);
    if (JS_IsException(arr))
        return arr;
    iter = JS_GetIterator2(ctx, obj, method);
    if (JS_IsException(iter))
        goto fail;
    next_method = JS_GetProperty(ctx, iter, JS_ATOM_next);
    if (JS_IsException(next_method))
        goto fail;
    k = 0;
    for(;;) {
        val = JS_IteratorNext(ctx, iter, next_method, 0, NULL, &done);
        if (JS_IsException(val))
            goto fail;
        if (done) {
            JS_FreeValue(ctx, val);
            break;
        }
        if (JS_CreateDataPropertyUint32(ctx, arr, k, val, JS_PROP_THROW) < 0)
            goto fail;
        k++;
    }
    JS_FreeValue(ctx, next_method);
    JS_FreeValue(ctx, iter);
    *plen = k;
    return arr;
 fail:
    JS_FreeValue(ctx, next_method);
    JS_FreeValue(ctx, iter);
    JS_FreeValue(ctx, arr);
    return JS_EXCEPTION;
}

static JSValue js_typed_array_constructor_obj(JSContext *ctx,
                                              JSValueConst new_target,
                                              JSValueConst obj,
                                              int classid)
{
    JSValue iter, ret, arr = JS_UNDEFINED, val, buffer;
    uint32_t i;
    int size_log2;
    int64_t len;
    size_log2 = typed_array_size_log2(classid);
    ret = js_create_from_ctor(ctx, new_target, classid);
    if (JS_IsException(ret))
        return JS_EXCEPTION;
    iter = JS_GetProperty(ctx, obj, JS_ATOM_Symbol_iterator);
    if (JS_IsException(iter))
        goto fail;
    if (!JS_IsUndefined(iter) && !JS_IsNull(iter)) {
        uint32_t len1;
        arr = js_array_from_iterator(ctx, &len1, obj, iter);
        JS_FreeValue(ctx, iter);
        if (JS_IsException(arr))
            goto fail;
        len = len1;
    } else {
        if (js_get_length64(ctx, &len, obj))
            goto fail;
        arr = JS_DupValue(ctx, obj);
    }
    buffer = js_array_buffer_constructor1(ctx, JS_UNDEFINED,
                                          len << size_log2);
    if (JS_IsException(buffer))
        goto fail;
    if (typed_array_init(ctx, ret, buffer, 0, len))
        goto fail;
    for(i = 0; i < len; i++) {
        val = JS_GetPropertyUint32(ctx, arr, i);
        if (JS_IsException(val))
            goto fail;
        if (JS_SetPropertyUint32(ctx, ret, i, val) < 0)
            goto fail;
    }
    JS_FreeValue(ctx, arr);
    return ret;
 fail:
    JS_FreeValue(ctx, arr);
    JS_FreeValue(ctx, ret);
    return JS_EXCEPTION;
}

static JSValue js_typed_array_constructor_ta(JSContext *ctx,
                                             JSValueConst new_target,
                                             JSValueConst src_obj,
                                             int classid)
{
    JSObject *p, *src_buffer;
    JSTypedArray *ta;
    JSValue ctor, obj, buffer;
    uint32_t len, i;
    int size_log2;
    JSArrayBuffer *src_abuf, *abuf;
    obj = js_create_from_ctor(ctx, new_target, classid);
    if (JS_IsException(obj))
        return obj;
    p = JS_VALUE_GET_OBJ(src_obj);
    if (typed_array_is_detached(ctx, p)) {
        JS_ThrowTypeErrorDetachedArrayBuffer(ctx);
        goto fail;
    }
    ta = p->u.typed_array;
    len = p->u.array.count;
    src_buffer = ta->buffer;
    src_abuf = src_buffer->u.array_buffer;
    if (!src_abuf->shared) {
        ctor = JS_SpeciesConstructor(ctx, JS_MKPTR(JS_TAG_OBJECT, src_buffer),
                                     JS_UNDEFINED);
        if (JS_IsException(ctor))
            goto fail;
    } else {
        /* force ArrayBuffer default constructor */
        ctor = JS_UNDEFINED;
    }
    size_log2 = typed_array_size_log2(classid);
    buffer = js_array_buffer_constructor1(ctx, ctor,
                                          (uint64_t)len << size_log2);
    JS_FreeValue(ctx, ctor);
    if (JS_IsException(buffer))
        goto fail;
    /* necessary because it could have been detached */
    if (typed_array_is_detached(ctx, p)) {
        JS_FreeValue(ctx, buffer);
        JS_ThrowTypeErrorDetachedArrayBuffer(ctx);
        goto fail;
    }
    abuf = JS_GetOpaque(buffer, JS_CLASS_ARRAY_BUFFER);
    if (typed_array_init(ctx, obj, buffer, 0, len))
        goto fail;
    if (p->class_id == classid) {
        /* same type: copy the content */
        memcpy(abuf->data, src_abuf->data + ta->offset, abuf->byte_length);
    } else {
        for(i = 0; i < len; i++) {
            JSValue val;
            val = JS_GetPropertyUint32(ctx, src_obj, i);
            if (JS_IsException(val))
                goto fail;
            if (JS_SetPropertyUint32(ctx, obj, i, val) < 0)
                goto fail;
        }
    }
    return obj;
 fail:
    JS_FreeValue(ctx, obj);
    return JS_EXCEPTION;
}

JSValue js_typed_array_constructor(JSContext *ctx,
                                   JSValueConst new_target,
                                   int argc, JSValueConst *argv,
                                   int classid)
{
    JSValue buffer, obj;
    JSArrayBuffer *abuf;
    int size_log2;
    uint64_t len, offset;
    size_log2 = typed_array_size_log2(classid);
    if (JS_VALUE_GET_TAG(argv[0]) != JS_TAG_OBJECT) {
        if (JS_ToIndex(ctx, &len, argv[0]))
            return JS_EXCEPTION;
        buffer = js_array_buffer_constructor1(ctx, JS_UNDEFINED,
                                              len << size_log2);
        if (JS_IsException(buffer))
            return JS_EXCEPTION;
        offset = 0;
    } else {
        JSObject *p = JS_VALUE_GET_OBJ(argv[0]);
        if (p->class_id == JS_CLASS_ARRAY_BUFFER ||
            p->class_id == JS_CLASS_SHARED_ARRAY_BUFFER) {
            abuf = p->u.array_buffer;
            if (JS_ToIndex(ctx, &offset, argv[1]))
                return JS_EXCEPTION;
            if (abuf->detached)
                return JS_ThrowTypeErrorDetachedArrayBuffer(ctx);
            if ((offset & ((1 << size_log2) - 1)) != 0 ||
                offset > abuf->byte_length)
                return JS_ThrowRangeError(ctx, "invalid offset");
            if (JS_IsUndefined(argv[2])) {
                if ((abuf->byte_length & ((1 << size_log2) - 1)) != 0)
                    goto invalid_length;
                len = (abuf->byte_length - offset) >> size_log2;
            } else {
                if (JS_ToIndex(ctx, &len, argv[2]))
                    return JS_EXCEPTION;
                if (abuf->detached)
                    return JS_ThrowTypeErrorDetachedArrayBuffer(ctx);
                if ((offset + (len << size_log2)) > abuf->byte_length) {
                invalid_length:
                    return JS_ThrowRangeError(ctx, "invalid length");
                }
            }
            buffer = JS_DupValue(ctx, argv[0]);
        } else {
            if (p->class_id >= JS_CLASS_UINT8C_ARRAY &&
                p->class_id <= JS_CLASS_FLOAT64_ARRAY) {
                return js_typed_array_constructor_ta(ctx, new_target, argv[0], classid);
            } else {
                return js_typed_array_constructor_obj(ctx, new_target, argv[0], classid);
            }
        }
    }
    obj = js_create_from_ctor(ctx, new_target, classid);
    if (JS_IsException(obj)) {
        JS_FreeValue(ctx, buffer);
        return JS_EXCEPTION;
    }
    if (typed_array_init(ctx, obj, buffer, offset, len)) {
        JS_FreeValue(ctx, obj);
        return JS_EXCEPTION;
    }
    return obj;
}

void js_typed_array_finalizer(JSRuntime *rt, JSValue val)
{
    JSObject *p = JS_VALUE_GET_OBJ(val);
    JSTypedArray *ta = p->u.typed_array;
    if (ta) {
        /* during the GC the finalizers are called in an arbitrary
           order so the ArrayBuffer finalizer may have been called */
        if (JS_IsLiveObject(rt, JS_MKPTR(JS_TAG_OBJECT, ta->buffer))) {
            list_del(&ta->link);
        }
        JS_FreeValueRT(rt, JS_MKPTR(JS_TAG_OBJECT, ta->buffer));
        js_free_rt(rt, ta);
    }
}

void js_typed_array_mark(JSRuntime *rt, JSValueConst val, JS_MarkFunc *mark_func)
{
    JSObject *p = JS_VALUE_GET_OBJ(val);
    JSTypedArray *ta = p->u.typed_array;
    if (ta) {
        JS_MarkValue(rt, JS_MKPTR(JS_TAG_OBJECT, ta->buffer), mark_func);
    }
}

static JSValue js_array_buffer_get_byteLength(JSContext *ctx,
                                              JSValueConst this_val,
                                              int class_id)
{
    JSArrayBuffer *abuf = JS_GetOpaque2(ctx, this_val, class_id);
    if (!abuf)
        return JS_EXCEPTION;
    /* return 0 if detached */
    return JS_NewUint32(ctx, abuf->byte_length);
}

static JSValue js_array_buffer_slice(JSContext *ctx,
                                     JSValueConst this_val,
                                     int argc, JSValueConst *argv, int class_id)
{
    JSArrayBuffer *abuf, *new_abuf;
    int64_t len, start, end, new_len;
    JSValue ctor, new_obj;
    abuf = JS_GetOpaque2(ctx, this_val, class_id);
    if (!abuf)
        return JS_EXCEPTION;
    if (abuf->detached)
        return JS_ThrowTypeErrorDetachedArrayBuffer(ctx);
    len = abuf->byte_length;
    if (JS_ToInt64Clamp(ctx, &start, argv[0], 0, len, len))
        return JS_EXCEPTION;
    end = len;
    if (!JS_IsUndefined(argv[1])) {
        if (JS_ToInt64Clamp(ctx, &end, argv[1], 0, len, len))
            return JS_EXCEPTION;
    }
    new_len = max_int64(end - start, 0);
    ctor = JS_SpeciesConstructor(ctx, this_val, JS_UNDEFINED);
    if (JS_IsException(ctor))
        return ctor;
    if (JS_IsUndefined(ctor)) {
        new_obj = js_array_buffer_constructor2(ctx, JS_UNDEFINED, new_len,
                                               class_id);
    } else {
        JSValue args[1];
        args[0] = JS_NewInt64(ctx, new_len);
        new_obj = JS_CallConstructor(ctx, ctor, 1, (JSValueConst *)args);
        JS_FreeValue(ctx, ctor);
        JS_FreeValue(ctx, args[0]);
    }
    if (JS_IsException(new_obj))
        return new_obj;
    new_abuf = JS_GetOpaque2(ctx, new_obj, class_id);
    if (!new_abuf)
        goto fail;
    if (js_same_value(ctx, new_obj, this_val)) {
        JS_ThrowTypeError(ctx, "cannot use identical ArrayBuffer");
        goto fail;
    }
    if (new_abuf->detached) {
        JS_ThrowTypeErrorDetachedArrayBuffer(ctx);
        goto fail;
    }
    if (new_abuf->byte_length < new_len) {
        JS_ThrowTypeError(ctx, "new ArrayBuffer is too small");
        goto fail;
    }
    /* must test again because of side effects */
    if (abuf->detached) {
        JS_ThrowTypeErrorDetachedArrayBuffer(ctx);
        goto fail;
    }
    memcpy(new_abuf->data, abuf->data + start, new_len);
    return new_obj;
 fail:
    JS_FreeValue(ctx, new_obj);
    return JS_EXCEPTION;
}

static uint8_t const typed_array_size_log2[JS_TYPED_ARRAY_COUNT] = {
    0, 0, 0, 1, 1, 2, 2,
#ifdef CONFIG_BIGNUM
    3, 3, /* BigInt64Array, BigUint64Array */
#endif
    2, 3
};

JSValue js_array_buffer_constructor3(JSContext *ctx,
                                     JSValueConst new_target,
                                     uint64_t len, JSClassID class_id,
                                     uint8_t *buf,
                                     JSFreeArrayBufferDataFunc *free_func,
                                     void *opaque, BOOL alloc_flag)
{
    JSRuntime *rt = ctx->rt;
    JSValue obj;
    JSArrayBuffer *abuf = NULL;

    obj = js_create_from_ctor(ctx, new_target, class_id);
    if (JS_IsException(obj))
        return obj;
    /* XXX: we are currently limited to 2 GB */
    if (len > INT32_MAX) {
        JS_ThrowRangeError(ctx, "invalid array buffer length");
        goto fail;
    }
    abuf = js_malloc(ctx, sizeof(*abuf));
    if (!abuf)
        goto fail;
    abuf->byte_length = len;
    if (alloc_flag) {
        if (class_id == JS_CLASS_SHARED_ARRAY_BUFFER &&
            rt->sab_funcs.sab_alloc) {
            abuf->data = rt->sab_funcs.sab_alloc(rt->sab_funcs.sab_opaque,
                                                 max_int(len, 1));
            if (!abuf->data)
                goto fail;
            bzero(abuf->data, len);
        } else {
            /* the allocation must be done after the object creation */
            abuf->data = js_mallocz(ctx, max_int(len, 1));
            if (!abuf->data)
                goto fail;
        }
    } else {
        if (class_id == JS_CLASS_SHARED_ARRAY_BUFFER &&
            rt->sab_funcs.sab_dup) {
            rt->sab_funcs.sab_dup(rt->sab_funcs.sab_opaque, buf);
        }
        abuf->data = buf;
    }
    init_list_head(&abuf->array_list);
    abuf->detached = FALSE;
    abuf->shared = (class_id == JS_CLASS_SHARED_ARRAY_BUFFER);
    abuf->opaque = opaque;
    abuf->free_func = free_func;
    if (alloc_flag && buf)
        memcpy(abuf->data, buf, len);
    JS_SetOpaque(obj, abuf);
    return obj;
 fail:
    JS_FreeValue(ctx, obj);
    js_free(ctx, abuf);
    return JS_EXCEPTION;
}

static void js_array_buffer_free(JSRuntime *rt, void *opaque, void *ptr)
{
    js_free_rt(rt, ptr);
}

JSValue js_array_buffer_constructor2(JSContext *ctx,
                                     JSValueConst new_target,
                                     uint64_t len, JSClassID class_id)
{
    return js_array_buffer_constructor3(ctx, new_target, len, class_id,
                                        NULL, js_array_buffer_free, NULL,
                                        TRUE);
}

JSValue js_array_buffer_constructor1(JSContext *ctx, JSValueConst new_target, uint64_t len)
{
    return js_array_buffer_constructor2(ctx, new_target, len,
                                        JS_CLASS_ARRAY_BUFFER);
}

JSValue JS_NewArrayBuffer(JSContext *ctx, uint8_t *buf, size_t len,
                          JSFreeArrayBufferDataFunc *free_func, void *opaque,
                          BOOL is_shared)
{
    return js_array_buffer_constructor3(ctx, JS_UNDEFINED, len,
                                        is_shared ? JS_CLASS_SHARED_ARRAY_BUFFER : JS_CLASS_ARRAY_BUFFER,
                                        buf, free_func, opaque, FALSE);
}

/* create a new ArrayBuffer of length 'len' and copy 'buf' to it */
JSValue JS_NewArrayBufferCopy(JSContext *ctx, const uint8_t *buf, size_t len)
{
    return js_array_buffer_constructor3(ctx, JS_UNDEFINED, len,
                                        JS_CLASS_ARRAY_BUFFER,
                                        (uint8_t *)buf,
                                        js_array_buffer_free, NULL,
                                        TRUE);
}

JSValue js_array_buffer_constructor(JSContext *ctx,
                                    JSValueConst new_target,
                                    int argc, JSValueConst *argv)
{
    uint64_t len;
    if (JS_ToIndex(ctx, &len, argv[0]))
        return JS_EXCEPTION;
    return js_array_buffer_constructor1(ctx, new_target, len);
}

static JSValue js_shared_array_buffer_constructor(JSContext *ctx,
                                                  JSValueConst new_target,
                                                  int argc, JSValueConst *argv)
{
    uint64_t len;
    if (JS_ToIndex(ctx, &len, argv[0]))
        return JS_EXCEPTION;
    return js_array_buffer_constructor2(ctx, new_target, len,
                                        JS_CLASS_SHARED_ARRAY_BUFFER);
}

/* also used for SharedArrayBuffer */
void js_array_buffer_finalizer(JSRuntime *rt, JSValue val)
{
    JSObject *p = JS_VALUE_GET_OBJ(val);
    JSArrayBuffer *abuf = p->u.array_buffer;
    if (abuf) {
        /* The ArrayBuffer finalizer may be called before the typed
           array finalizers using it, so abuf->array_list is not
           necessarily empty. */
        // assert(list_empty(&abuf->array_list));
        if (abuf->shared && rt->sab_funcs.sab_free) {
            rt->sab_funcs.sab_free(rt->sab_funcs.sab_opaque, abuf->data);
        } else {
            if (abuf->free_func)
                abuf->free_func(rt, abuf->opaque, abuf->data);
        }
        js_free_rt(rt, abuf);
    }
}

static JSValue js_array_buffer_isView(JSContext *ctx,
                                      JSValueConst this_val,
                                      int argc, JSValueConst *argv)
{
    JSObject *p;
    BOOL res;
    res = FALSE;
    if (JS_VALUE_GET_TAG(argv[0]) == JS_TAG_OBJECT) {
        p = JS_VALUE_GET_OBJ(argv[0]);
        if (p->class_id >= JS_CLASS_UINT8C_ARRAY &&
            p->class_id <= JS_CLASS_DATAVIEW) {
            res = TRUE;
        }
    }
    return JS_NewBool(ctx, res);
}

void JS_DetachArrayBuffer(JSContext *ctx, JSValueConst obj)
{
    JSArrayBuffer *abuf = JS_GetOpaque(obj, JS_CLASS_ARRAY_BUFFER);
    struct list_head *el;
    if (!abuf || abuf->detached)
        return;
    if (abuf->free_func)
        abuf->free_func(ctx->rt, abuf->opaque, abuf->data);
    abuf->data = NULL;
    abuf->byte_length = 0;
    abuf->detached = TRUE;
    list_for_each(el, &abuf->array_list) {
        JSTypedArray *ta;
        JSObject *p;
        ta = list_entry(el, JSTypedArray, link);
        p = ta->obj;
        /* Note: the typed array length and offset fields are not modified */
        if (p->class_id != JS_CLASS_DATAVIEW) {
            p->u.array.count = 0;
            p->u.array.u.ptr = NULL;
        }
    }
}

/* get an ArrayBuffer or SharedArrayBuffer */
JSArrayBuffer *js_get_array_buffer(JSContext *ctx, JSValueConst obj)
{
    JSObject *p;
    if (JS_VALUE_GET_TAG(obj) != JS_TAG_OBJECT)
        goto fail;
    p = JS_VALUE_GET_OBJ(obj);
    if (p->class_id != JS_CLASS_ARRAY_BUFFER &&
        p->class_id != JS_CLASS_SHARED_ARRAY_BUFFER) {
    fail:
        JS_ThrowTypeErrorInvalidClass(ctx, JS_CLASS_ARRAY_BUFFER);
        return NULL;
    }
    return p->u.array_buffer;
}

/* return NULL if exception. WARNING: any JS call can detach the
   buffer and render the returned pointer invalid */
uint8_t *JS_GetArrayBuffer(JSContext *ctx, size_t *psize, JSValueConst obj)
{
    JSArrayBuffer *abuf = js_get_array_buffer(ctx, obj);
    if (!abuf)
        goto fail;
    if (abuf->detached) {
        JS_ThrowTypeErrorDetachedArrayBuffer(ctx);
        goto fail;
    }
    *psize = abuf->byte_length;
    return abuf->data;
 fail:
    *psize = 0;
    return NULL;
}

static const JSCFunctionListEntry js_array_buffer_funcs[] = {
    JS_CFUNC_DEF("isView", 1, js_array_buffer_isView ),
    JS_CGETSET_DEF("[Symbol.species]", js_get_this, NULL ),
};

static const JSCFunctionListEntry js_shared_array_buffer_funcs[] = {
    JS_CGETSET_DEF("[Symbol.species]", js_get_this, NULL ),
};

static const JSCFunctionListEntry js_array_buffer_proto_funcs[] = {
    JS_CGETSET_MAGIC_DEF("byteLength", js_array_buffer_get_byteLength, NULL, JS_CLASS_ARRAY_BUFFER ),
    JS_CFUNC_MAGIC_DEF("slice", 2, js_array_buffer_slice, JS_CLASS_ARRAY_BUFFER ),
    JS_PROP_STRING_DEF("[Symbol.toStringTag]", "ArrayBuffer", JS_PROP_CONFIGURABLE ),
};

static const JSCFunctionListEntry js_shared_array_buffer_proto_funcs[] = {
    JS_CGETSET_MAGIC_DEF("byteLength", js_array_buffer_get_byteLength, NULL, JS_CLASS_SHARED_ARRAY_BUFFER ),
    JS_CFUNC_MAGIC_DEF("slice", 2, js_array_buffer_slice, JS_CLASS_SHARED_ARRAY_BUFFER ),
    JS_PROP_STRING_DEF("[Symbol.toStringTag]", "SharedArrayBuffer", JS_PROP_CONFIGURABLE ),
};

static JSValue js_dataview_constructor(JSContext *ctx,
                                       JSValueConst new_target,
                                       int argc, JSValueConst *argv)
{
    JSArrayBuffer *abuf;
    uint64_t offset;
    uint32_t len;
    JSValueConst buffer;
    JSValue obj;
    JSTypedArray *ta;
    JSObject *p;
    buffer = argv[0];
    abuf = js_get_array_buffer(ctx, buffer);
    if (!abuf)
        return JS_EXCEPTION;
    offset = 0;
    if (argc > 1) {
        if (JS_ToIndex(ctx, &offset, argv[1]))
            return JS_EXCEPTION;
    }
    if (abuf->detached)
        return JS_ThrowTypeErrorDetachedArrayBuffer(ctx);
    if (offset > abuf->byte_length)
        return JS_ThrowRangeError(ctx, "invalid byteOffset");
    len = abuf->byte_length - offset;
    if (argc > 2 && !JS_IsUndefined(argv[2])) {
        uint64_t l;
        if (JS_ToIndex(ctx, &l, argv[2]))
            return JS_EXCEPTION;
        if (l > len)
            return JS_ThrowRangeError(ctx, "invalid byteLength");
        len = l;
    }
    obj = js_create_from_ctor(ctx, new_target, JS_CLASS_DATAVIEW);
    if (JS_IsException(obj))
        return JS_EXCEPTION;
    if (abuf->detached) {
        /* could have been detached in js_create_from_ctor() */
        JS_ThrowTypeErrorDetachedArrayBuffer(ctx);
        goto fail;
    }
    ta = js_malloc(ctx, sizeof(*ta));
    if (!ta) {
    fail:
        JS_FreeValue(ctx, obj);
        return JS_EXCEPTION;
    }
    p = JS_VALUE_GET_OBJ(obj);
    ta->obj = p;
    ta->buffer = JS_VALUE_GET_OBJ(JS_DupValue(ctx, buffer));
    ta->offset = offset;
    ta->length = len;
    list_add_tail(&ta->link, &abuf->array_list);
    p->u.typed_array = ta;
    return obj;
}

static JSValue js_dataview_getValue(JSContext *ctx,
                                    JSValueConst this_obj,
                                    int argc, JSValueConst *argv, int class_id)
{
    JSTypedArray *ta;
    JSArrayBuffer *abuf;
    int is_swap, size;
    uint8_t *ptr;
    uint32_t v;
    uint64_t pos;
    ta = JS_GetOpaque2(ctx, this_obj, JS_CLASS_DATAVIEW);
    if (!ta)
        return JS_EXCEPTION;
    size = 1 << typed_array_size_log2(class_id);
    if (JS_ToIndex(ctx, &pos, argv[0]))
        return JS_EXCEPTION;
    is_swap = FALSE;
    if (argc > 1)
        is_swap = JS_ToBool(ctx, argv[1]);
#ifndef WORDS_BIGENDIAN
    is_swap ^= 1;
#endif
    abuf = ta->buffer->u.array_buffer;
    if (abuf->detached)
        return JS_ThrowTypeErrorDetachedArrayBuffer(ctx);
    if ((pos + size) > ta->length)
        return JS_ThrowRangeError(ctx, "out of bound");
    ptr = abuf->data + ta->offset + pos;
    switch(class_id) {
    case JS_CLASS_INT8_ARRAY:
        return JS_NewInt32(ctx, *(int8_t *)ptr);
    case JS_CLASS_UINT8_ARRAY:
        return JS_NewInt32(ctx, *(uint8_t *)ptr);
    case JS_CLASS_INT16_ARRAY:
        v = get_u16(ptr);
        if (is_swap)
            v = bswap16(v);
        return JS_NewInt32(ctx, (int16_t)v);
    case JS_CLASS_UINT16_ARRAY:
        v = get_u16(ptr);
        if (is_swap)
            v = bswap16(v);
        return JS_NewInt32(ctx, v);
    case JS_CLASS_INT32_ARRAY:
        v = get_u32(ptr);
        if (is_swap)
            v = bswap32(v);
        return JS_NewInt32(ctx, v);
    case JS_CLASS_UINT32_ARRAY:
        v = get_u32(ptr);
        if (is_swap)
            v = bswap32(v);
        return JS_NewUint32(ctx, v);
#ifdef CONFIG_BIGNUM
    case JS_CLASS_BIG_INT64_ARRAY:
        {
            uint64_t v;
            v = get_u64(ptr);
            if (is_swap)
                v = bswap64(v);
            return JS_NewBigInt64(ctx, v);
        }
        break;
    case JS_CLASS_BIG_UINT64_ARRAY:
        {
            uint64_t v;
            v = get_u64(ptr);
            if (is_swap)
                v = bswap64(v);
            return JS_NewBigUint64(ctx, v);
        }
        break;
#endif
    case JS_CLASS_FLOAT32_ARRAY:
        {
            union {
                float f;
                uint32_t i;
            } u;
            v = get_u32(ptr);
            if (is_swap)
                v = bswap32(v);
            u.i = v;
            return __JS_NewFloat64(ctx, u.f);
        }
    case JS_CLASS_FLOAT64_ARRAY:
        {
            union {
                double f;
                uint64_t i;
            } u;
            u.i = get_u64(ptr);
            if (is_swap)
                u.i = bswap64(u.i);
            return __JS_NewFloat64(ctx, u.f);
        }
    default:
        abort();
    }
}

static JSValue js_dataview_setValue(JSContext *ctx,
                                    JSValueConst this_obj,
                                    int argc, JSValueConst *argv, int class_id)
{
    JSTypedArray *ta;
    JSArrayBuffer *abuf;
    int is_swap, size;
    uint8_t *ptr;
    uint64_t v64;
    uint32_t v;
    uint64_t pos;
    JSValueConst val;
    ta = JS_GetOpaque2(ctx, this_obj, JS_CLASS_DATAVIEW);
    if (!ta)
        return JS_EXCEPTION;
    size = 1 << typed_array_size_log2(class_id);
    if (JS_ToIndex(ctx, &pos, argv[0]))
        return JS_EXCEPTION;
    val = argv[1];
    v = 0; /* avoid warning */
    v64 = 0; /* avoid warning */
    if (class_id <= JS_CLASS_UINT32_ARRAY) {
        if (JS_ToUint32(ctx, &v, val))
            return JS_EXCEPTION;
    } else
#ifdef CONFIG_BIGNUM
    if (class_id <= JS_CLASS_BIG_UINT64_ARRAY) {
        if (JS_ToBigInt64(ctx, (int64_t *)&v64, val))
            return JS_EXCEPTION;
    } else
#endif
    {
        double d;
        if (JS_ToFloat64(ctx, &d, val))
            return JS_EXCEPTION;
        if (class_id == JS_CLASS_FLOAT32_ARRAY) {
            union {
                float f;
                uint32_t i;
            } u;
            u.f = d;
            v = u.i;
        } else {
            JSFloat64Union u;
            u.d = d;
            v64 = u.u64;
        }
    }
    is_swap = FALSE;
    if (argc > 2)
        is_swap = JS_ToBool(ctx, argv[2]);
#ifndef WORDS_BIGENDIAN
    is_swap ^= 1;
#endif
    abuf = ta->buffer->u.array_buffer;
    if (abuf->detached)
        return JS_ThrowTypeErrorDetachedArrayBuffer(ctx);
    if ((pos + size) > ta->length)
        return JS_ThrowRangeError(ctx, "out of bound");
    ptr = abuf->data + ta->offset + pos;
    switch(class_id) {
    case JS_CLASS_INT8_ARRAY:
    case JS_CLASS_UINT8_ARRAY:
        *ptr = v;
        break;
    case JS_CLASS_INT16_ARRAY:
    case JS_CLASS_UINT16_ARRAY:
        if (is_swap)
            v = bswap16(v);
        put_u16(ptr, v);
        break;
    case JS_CLASS_INT32_ARRAY:
    case JS_CLASS_UINT32_ARRAY:
    case JS_CLASS_FLOAT32_ARRAY:
        if (is_swap)
            v = bswap32(v);
        put_u32(ptr, v);
        break;
#ifdef CONFIG_BIGNUM
    case JS_CLASS_BIG_INT64_ARRAY:
    case JS_CLASS_BIG_UINT64_ARRAY:
#endif
    case JS_CLASS_FLOAT64_ARRAY:
        if (is_swap)
            v64 = bswap64(v64);
        put_u64(ptr, v64);
        break;
    default:
        abort();
    }
    return JS_UNDEFINED;
}

static const JSCFunctionListEntry js_dataview_proto_funcs[] = {
    JS_CGETSET_MAGIC_DEF("buffer", js_typed_array_get_buffer, NULL, 1 ),
    JS_CGETSET_MAGIC_DEF("byteLength", js_typed_array_get_byteLength, NULL, 1 ),
    JS_CGETSET_MAGIC_DEF("byteOffset", js_typed_array_get_byteOffset, NULL, 1 ),
    JS_CFUNC_MAGIC_DEF("getInt8", 1, js_dataview_getValue, JS_CLASS_INT8_ARRAY ),
    JS_CFUNC_MAGIC_DEF("getUint8", 1, js_dataview_getValue, JS_CLASS_UINT8_ARRAY ),
    JS_CFUNC_MAGIC_DEF("getInt16", 1, js_dataview_getValue, JS_CLASS_INT16_ARRAY ),
    JS_CFUNC_MAGIC_DEF("getUint16", 1, js_dataview_getValue, JS_CLASS_UINT16_ARRAY ),
    JS_CFUNC_MAGIC_DEF("getInt32", 1, js_dataview_getValue, JS_CLASS_INT32_ARRAY ),
    JS_CFUNC_MAGIC_DEF("getUint32", 1, js_dataview_getValue, JS_CLASS_UINT32_ARRAY ),
#ifdef CONFIG_BIGNUM
    JS_CFUNC_MAGIC_DEF("getBigInt64", 1, js_dataview_getValue, JS_CLASS_BIG_INT64_ARRAY ),
    JS_CFUNC_MAGIC_DEF("getBigUint64", 1, js_dataview_getValue, JS_CLASS_BIG_UINT64_ARRAY ),
#endif
    JS_CFUNC_MAGIC_DEF("getFloat32", 1, js_dataview_getValue, JS_CLASS_FLOAT32_ARRAY ),
    JS_CFUNC_MAGIC_DEF("getFloat64", 1, js_dataview_getValue, JS_CLASS_FLOAT64_ARRAY ),
    JS_CFUNC_MAGIC_DEF("setInt8", 2, js_dataview_setValue, JS_CLASS_INT8_ARRAY ),
    JS_CFUNC_MAGIC_DEF("setUint8", 2, js_dataview_setValue, JS_CLASS_UINT8_ARRAY ),
    JS_CFUNC_MAGIC_DEF("setInt16", 2, js_dataview_setValue, JS_CLASS_INT16_ARRAY ),
    JS_CFUNC_MAGIC_DEF("setUint16", 2, js_dataview_setValue, JS_CLASS_UINT16_ARRAY ),
    JS_CFUNC_MAGIC_DEF("setInt32", 2, js_dataview_setValue, JS_CLASS_INT32_ARRAY ),
    JS_CFUNC_MAGIC_DEF("setUint32", 2, js_dataview_setValue, JS_CLASS_UINT32_ARRAY ),
#ifdef CONFIG_BIGNUM
    JS_CFUNC_MAGIC_DEF("setBigInt64", 2, js_dataview_setValue, JS_CLASS_BIG_INT64_ARRAY ),
    JS_CFUNC_MAGIC_DEF("setBigUint64", 2, js_dataview_setValue, JS_CLASS_BIG_UINT64_ARRAY ),
#endif
    JS_CFUNC_MAGIC_DEF("setFloat32", 2, js_dataview_setValue, JS_CLASS_FLOAT32_ARRAY ),
    JS_CFUNC_MAGIC_DEF("setFloat64", 2, js_dataview_setValue, JS_CLASS_FLOAT64_ARRAY ),
    JS_PROP_STRING_DEF("[Symbol.toStringTag]", "DataView", JS_PROP_CONFIGURABLE ),
};

void JS_AddIntrinsicTypedArrays(JSContext *ctx)
{
    JSValue typed_array_base_proto, typed_array_base_func;
    JSValueConst array_buffer_func, shared_array_buffer_func;
    int i;
    ctx->class_proto[JS_CLASS_ARRAY_BUFFER] = JS_NewObject(ctx);
    JS_SetPropertyFunctionList(ctx, ctx->class_proto[JS_CLASS_ARRAY_BUFFER],
                               js_array_buffer_proto_funcs,
                               countof(js_array_buffer_proto_funcs));
    array_buffer_func = JS_NewGlobalCConstructorOnly(ctx, "ArrayBuffer",
                                                     js_array_buffer_constructor, 1,
                                                     ctx->class_proto[JS_CLASS_ARRAY_BUFFER]);
    JS_SetPropertyFunctionList(ctx, array_buffer_func,
                               js_array_buffer_funcs,
                               countof(js_array_buffer_funcs));
    ctx->class_proto[JS_CLASS_SHARED_ARRAY_BUFFER] = JS_NewObject(ctx);
    JS_SetPropertyFunctionList(ctx, ctx->class_proto[JS_CLASS_SHARED_ARRAY_BUFFER],
                               js_shared_array_buffer_proto_funcs,
                               countof(js_shared_array_buffer_proto_funcs));
    shared_array_buffer_func = JS_NewGlobalCConstructorOnly(ctx, "SharedArrayBuffer",
                                                 js_shared_array_buffer_constructor, 1,
                                                 ctx->class_proto[JS_CLASS_SHARED_ARRAY_BUFFER]);
    JS_SetPropertyFunctionList(ctx, shared_array_buffer_func,
                               js_shared_array_buffer_funcs,
                               countof(js_shared_array_buffer_funcs));
    typed_array_base_proto = JS_NewObject(ctx);
    JS_SetPropertyFunctionList(ctx, typed_array_base_proto,
                               js_typed_array_base_proto_funcs,
                               countof(js_typed_array_base_proto_funcs));
    /* TypedArray.prototype.toString must be the same object as Array.prototype.toString */
    JSValue obj = JS_GetProperty(ctx, ctx->class_proto[JS_CLASS_ARRAY], JS_ATOM_toString);
    /* XXX: should use alias method in JSCFunctionListEntry */ //@@@
    JS_DefinePropertyValue(ctx, typed_array_base_proto, JS_ATOM_toString, obj,
                           JS_PROP_WRITABLE | JS_PROP_CONFIGURABLE);
    typed_array_base_func = JS_NewCFunction(ctx, js_typed_array_base_constructor,
                                            "TypedArray", 0);
    JS_SetPropertyFunctionList(ctx, typed_array_base_func,
                               js_typed_array_base_funcs,
                               countof(js_typed_array_base_funcs));
    JS_SetConstructor(ctx, typed_array_base_func, typed_array_base_proto);
    for(i = JS_CLASS_UINT8C_ARRAY; i < JS_CLASS_UINT8C_ARRAY + JS_TYPED_ARRAY_COUNT; i++) {
        JSValue func_obj;
        char buf[ATOM_GET_STR_BUF_SIZE];
        const char *name;
        ctx->class_proto[i] = JS_NewObjectProto(ctx, typed_array_base_proto);
        JS_DefinePropertyValueStr(ctx, ctx->class_proto[i],
                                  "BYTES_PER_ELEMENT",
                                  JS_NewInt32(ctx, 1 << typed_array_size_log2(i)),
                                  0);
        name = JS_AtomGetStr(ctx, buf, sizeof(buf),
                             JS_ATOM_Uint8ClampedArray + i - JS_CLASS_UINT8C_ARRAY);
        func_obj = JS_NewCFunction3(ctx, (JSCFunction *)js_typed_array_constructor,
                                    name, 3, JS_CFUNC_constructor_magic, i,
                                    typed_array_base_func);
        JS_NewGlobalCConstructor2(ctx, func_obj, name, ctx->class_proto[i]);
        JS_DefinePropertyValueStr(ctx, func_obj,
                                  "BYTES_PER_ELEMENT",
                                  JS_NewInt32(ctx, 1 << typed_array_size_log2(i)),
                                  0);
    }
    JS_FreeValue(ctx, typed_array_base_proto);
    JS_FreeValue(ctx, typed_array_base_func);
    /* DataView */
    ctx->class_proto[JS_CLASS_DATAVIEW] = JS_NewObject(ctx);
    JS_SetPropertyFunctionList(ctx, ctx->class_proto[JS_CLASS_DATAVIEW],
                               js_dataview_proto_funcs,
                               countof(js_dataview_proto_funcs));
    JS_NewGlobalCConstructorOnly(ctx, "DataView",
                                 js_dataview_constructor, 1,
                                 ctx->class_proto[JS_CLASS_DATAVIEW]);
    /* Atomics */
#ifdef CONFIG_ATOMICS
    JS_AddIntrinsicAtomics(ctx);
#endif
}
