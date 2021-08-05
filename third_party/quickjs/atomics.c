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
#include "third_party/quickjs/internal.h"

asm(".ident\t\"\\n\\n\
QuickJS (MIT License)\\n\
Copyright (c) 2017-2021 Fabrice Bellard\\n\
Copyright (c) 2017-2021 Charlie Gordon\"");
asm(".include \"libc/disclaimer.inc\"");
/* clang-format off */

#ifdef CONFIG_ATOMICS

typedef enum AtomicsOpEnum {
    ATOMICS_OP_ADD,
    ATOMICS_OP_AND,
    ATOMICS_OP_OR,
    ATOMICS_OP_SUB,
    ATOMICS_OP_XOR,
    ATOMICS_OP_EXCHANGE,
    ATOMICS_OP_COMPARE_EXCHANGE,
    ATOMICS_OP_LOAD,
} AtomicsOpEnum;

static void *js_atomics_get_ptr(JSContext *ctx,
                                JSArrayBuffer **pabuf,
                                int *psize_log2, JSClassID *pclass_id,
                                JSValueConst obj, JSValueConst idx_val,
                                int is_waitable)
{
    JSObject *p;
    JSTypedArray *ta;
    JSArrayBuffer *abuf;
    void *ptr;
    uint64_t idx;
    BOOL err;
    int size_log2;
    if (JS_VALUE_GET_TAG(obj) != JS_TAG_OBJECT)
        goto fail;
    p = JS_VALUE_GET_OBJ(obj);
#ifdef CONFIG_BIGNUM
    if (is_waitable)
        err = (p->class_id != JS_CLASS_INT32_ARRAY &&
               p->class_id != JS_CLASS_BIG_INT64_ARRAY);
    else
        err = !(p->class_id >= JS_CLASS_INT8_ARRAY &&
                p->class_id <= JS_CLASS_BIG_UINT64_ARRAY);
#else
    if (is_waitable)
        err = (p->class_id != JS_CLASS_INT32_ARRAY);
    else
        err = !(p->class_id >= JS_CLASS_INT8_ARRAY &&
                p->class_id <= JS_CLASS_UINT32_ARRAY);
#endif
    if (err) {
    fail:
        JS_ThrowTypeError(ctx, "integer TypedArray expected");
        return NULL;
    }
    ta = p->u.typed_array;
    abuf = ta->buffer->u.array_buffer;
    if (!abuf->shared) {
        if (is_waitable == 2) {
            JS_ThrowTypeError(ctx, "not a SharedArrayBuffer TypedArray");
            return NULL;
        }
        if (abuf->detached) {
            JS_ThrowTypeErrorDetachedArrayBuffer(ctx);
            return NULL;
        }
    }
    if (JS_ToIndex(ctx, &idx, idx_val)) {
        return NULL;
    }
    /* if the array buffer is detached, p->u.array.count = 0 */
    if (idx >= p->u.array.count) {
        JS_ThrowRangeError(ctx, "out-of-bound access");
        return NULL;
    }
    size_log2 = typed_array_size_log2(p->class_id);
    ptr = p->u.array.u.uint8_ptr + ((uintptr_t)idx << size_log2);
    if (pabuf)
        *pabuf = abuf;
    if (psize_log2)
        *psize_log2 = size_log2;
    if (pclass_id)
        *pclass_id = p->class_id;
    return ptr;
}

static JSValue js_atomics_op(JSContext *ctx,
                             JSValueConst this_obj,
                             int argc, JSValueConst *argv, int op)
{
    int size_log2;
#ifdef CONFIG_BIGNUM
    uint64_t v, a, rep_val;
#else
    uint32_t v, a, rep_val;
#endif
    void *ptr;
    JSValue ret;
    JSClassID class_id;
    JSArrayBuffer *abuf;

    ptr = js_atomics_get_ptr(ctx, &abuf, &size_log2, &class_id,
                             argv[0], argv[1], 0);
    if (!ptr)
        return JS_EXCEPTION;
    rep_val = 0;
    if (op == ATOMICS_OP_LOAD) {
        v = 0;
    } else {
#ifdef CONFIG_BIGNUM
        if (size_log2 == 3) {
            int64_t v64;
            if (JS_ToBigInt64(ctx, &v64, argv[2]))
                return JS_EXCEPTION;
            v = v64;
            if (op == ATOMICS_OP_COMPARE_EXCHANGE) {
                if (JS_ToBigInt64(ctx, &v64, argv[3]))
                    return JS_EXCEPTION;
                rep_val = v64;
            }
        } else
#endif
        {
                uint32_t v32;
                if (JS_ToUint32(ctx, &v32, argv[2]))
                    return JS_EXCEPTION;
                v = v32;
                if (op == ATOMICS_OP_COMPARE_EXCHANGE) {
                    if (JS_ToUint32(ctx, &v32, argv[3]))
                        return JS_EXCEPTION;
                    rep_val = v32;
                }
        }
        if (abuf->detached)
            return JS_ThrowTypeErrorDetachedArrayBuffer(ctx);
   }

   switch(op | (size_log2 << 3)) {

#ifdef CONFIG_BIGNUM
#define OP(op_name, func_name)                          \
    case ATOMICS_OP_ ## op_name | (0 << 3):             \
       a = func_name((_Atomic(uint8_t) *)ptr, v);       \
       break;                                           \
    case ATOMICS_OP_ ## op_name | (1 << 3):             \
        a = func_name((_Atomic(uint16_t) *)ptr, v);     \
        break;                                          \
    case ATOMICS_OP_ ## op_name | (2 << 3):             \
        a = func_name((_Atomic(uint32_t) *)ptr, v);     \
        break;                                          \
    case ATOMICS_OP_ ## op_name | (3 << 3):             \
        a = func_name((_Atomic(uint64_t) *)ptr, v);     \
        break;
#else
#define OP(op_name, func_name)                          \
    case ATOMICS_OP_ ## op_name | (0 << 3):             \
       a = func_name((_Atomic(uint8_t) *)ptr, v);       \
       break;                                           \
    case ATOMICS_OP_ ## op_name | (1 << 3):             \
        a = func_name((_Atomic(uint16_t) *)ptr, v);     \
        break;                                          \
    case ATOMICS_OP_ ## op_name | (2 << 3):             \
        a = func_name((_Atomic(uint32_t) *)ptr, v);     \
        break;
#endif
        OP(ADD, atomic_fetch_add)
        OP(AND, atomic_fetch_and)
        OP(OR, atomic_fetch_or)
        OP(SUB, atomic_fetch_sub)
        OP(XOR, atomic_fetch_xor)
        OP(EXCHANGE, atomic_exchange)
#undef OP

    case ATOMICS_OP_LOAD | (0 << 3):
        a = atomic_load((_Atomic(uint8_t) *)ptr);
        break;
    case ATOMICS_OP_LOAD | (1 << 3):
        a = atomic_load((_Atomic(uint16_t) *)ptr);
        break;
    case ATOMICS_OP_LOAD | (2 << 3):
        a = atomic_load((_Atomic(uint32_t) *)ptr);
        break;
#ifdef CONFIG_BIGNUM
    case ATOMICS_OP_LOAD | (3 << 3):
        a = atomic_load((_Atomic(uint64_t) *)ptr);
        break;
#endif

    case ATOMICS_OP_COMPARE_EXCHANGE | (0 << 3):
        {
            uint8_t v1 = v;
            atomic_compare_exchange_strong((_Atomic(uint8_t) *)ptr, &v1, rep_val);
            a = v1;
        }
        break;
    case ATOMICS_OP_COMPARE_EXCHANGE | (1 << 3):
        {
            uint16_t v1 = v;
            atomic_compare_exchange_strong((_Atomic(uint16_t) *)ptr, &v1, rep_val);
            a = v1;
        }
        break;
    case ATOMICS_OP_COMPARE_EXCHANGE | (2 << 3):
        {
            uint32_t v1 = v;
            atomic_compare_exchange_strong((_Atomic(uint32_t) *)ptr, &v1, rep_val);
            a = v1;
        }
        break;
#ifdef CONFIG_BIGNUM
    case ATOMICS_OP_COMPARE_EXCHANGE | (3 << 3):
        {
            uint64_t v1 = v;
            atomic_compare_exchange_strong((_Atomic(uint64_t) *)ptr, &v1, rep_val);
            a = v1;
        }
        break;
#endif
    default:
        abort();
    }

    switch(class_id) {
    case JS_CLASS_INT8_ARRAY:
        a = (int8_t)a;
        goto done;
    case JS_CLASS_UINT8_ARRAY:
        a = (uint8_t)a;
        goto done;
    case JS_CLASS_INT16_ARRAY:
        a = (int16_t)a;
        goto done;
    case JS_CLASS_UINT16_ARRAY:
        a = (uint16_t)a;
        goto done;
    case JS_CLASS_INT32_ARRAY:
    done:
        ret = JS_NewInt32(ctx, a);
        break;
    case JS_CLASS_UINT32_ARRAY:
        ret = JS_NewUint32(ctx, a);
        break;
#ifdef CONFIG_BIGNUM
    case JS_CLASS_BIG_INT64_ARRAY:
        ret = JS_NewBigInt64(ctx, a);
        break;
    case JS_CLASS_BIG_UINT64_ARRAY:
        ret = JS_NewBigUint64(ctx, a);
        break;
#endif
    default:
        abort();
    }
    return ret;
}

static JSValue js_atomics_store(JSContext *ctx,
                                JSValueConst this_obj,
                                int argc, JSValueConst *argv)
{
    int size_log2;
    void *ptr;
    JSValue ret;
    JSArrayBuffer *abuf;

    ptr = js_atomics_get_ptr(ctx, &abuf, &size_log2, NULL,
                             argv[0], argv[1], 0);
    if (!ptr)
        return JS_EXCEPTION;
#ifdef CONFIG_BIGNUM
    if (size_log2 == 3) {
        int64_t v64;
        ret = JS_ToBigIntValueFree(ctx, JS_DupValue(ctx, argv[2]));
        if (JS_IsException(ret))
            return ret;
        if (JS_ToBigInt64(ctx, &v64, ret)) {
            JS_FreeValue(ctx, ret);
            return JS_EXCEPTION;
        }
        if (abuf->detached)
            return JS_ThrowTypeErrorDetachedArrayBuffer(ctx);
        atomic_store((_Atomic(uint64_t) *)ptr, v64);
    } else
#endif
    {
        uint32_t v;
        /* XXX: spec, would be simpler to return the written value */
        ret = JS_ToIntegerFree(ctx, JS_DupValue(ctx, argv[2]));
        if (JS_IsException(ret))
            return ret;
        if (JS_ToUint32(ctx, &v, ret)) {
            JS_FreeValue(ctx, ret);
            return JS_EXCEPTION;
        }
        if (abuf->detached)
            return JS_ThrowTypeErrorDetachedArrayBuffer(ctx);
        switch(size_log2) {
        case 0:
            atomic_store((_Atomic(uint8_t) *)ptr, v);
            break;
        case 1:
            atomic_store((_Atomic(uint16_t) *)ptr, v);
            break;
        case 2:
            atomic_store((_Atomic(uint32_t) *)ptr, v);
            break;
        default:
            abort();
        }
    }
    return ret;
}

static JSValue js_atomics_isLockFree(JSContext *ctx,
                                     JSValueConst this_obj,
                                     int argc, JSValueConst *argv)
{
    int v, ret;
    if (JS_ToInt32Sat(ctx, &v, argv[0]))
        return JS_EXCEPTION;
    ret = (v == 1 || v == 2 || v == 4
#ifdef CONFIG_BIGNUM
           || v == 8
#endif
           );
    return JS_NewBool(ctx, ret);
}

typedef struct JSAtomicsWaiter {
    struct list_head link;
    BOOL linked;
    pthread_cond_t cond;
    int32_t *ptr;
} JSAtomicsWaiter;

static pthread_mutex_t js_atomics_mutex = PTHREAD_MUTEX_INITIALIZER;
static struct list_head js_atomics_waiter_list =
    LIST_HEAD_INIT(js_atomics_waiter_list);

static JSValue js_atomics_wait(JSContext *ctx,
                               JSValueConst this_obj,
                               int argc, JSValueConst *argv)
{
    int64_t v;
    int32_t v32;
    void *ptr;
    int64_t timeout;
    struct timespec ts;
    JSAtomicsWaiter waiter_s, *waiter;
    int ret, size_log2, res;
    double d;
    ptr = js_atomics_get_ptr(ctx, NULL, &size_log2, NULL,
                             argv[0], argv[1], 2);
    if (!ptr)
        return JS_EXCEPTION;
#ifdef CONFIG_BIGNUM
    if (size_log2 == 3) {
        if (JS_ToBigInt64(ctx, &v, argv[2]))
            return JS_EXCEPTION;
    } else
#endif
    {
        if (JS_ToInt32(ctx, &v32, argv[2]))
            return JS_EXCEPTION;
        v = v32;
    }
    if (JS_ToFloat64(ctx, &d, argv[3]))
        return JS_EXCEPTION;
    if (isnan(d) || d > INT64_MAX)
        timeout = INT64_MAX;
    else if (d < 0)
        timeout = 0;
    else
        timeout = (int64_t)d;
    if (!ctx->rt->can_block)
        return JS_ThrowTypeError(ctx, "cannot block in this thread");
    /* XXX: inefficient if large number of waiters, should hash on
       'ptr' value */
    /* XXX: use Linux futexes when available ? */
    pthread_mutex_lock(&js_atomics_mutex);
    if (size_log2 == 3) {
        res = *(int64_t *)ptr != v;
    } else {
        res = *(int32_t *)ptr != v;
    }
    if (res) {
        pthread_mutex_unlock(&js_atomics_mutex);
        return JS_AtomToString(ctx, JS_ATOM_not_equal);
    }
    waiter = &waiter_s;
    waiter->ptr = ptr;
    pthread_cond_init(&waiter->cond, NULL);
    waiter->linked = TRUE;
    list_add_tail(&waiter->link, &js_atomics_waiter_list);
    if (timeout == INT64_MAX) {
        pthread_cond_wait(&waiter->cond, &js_atomics_mutex);
        ret = 0;
    } else {
        /* XXX: use clock monotonic */
        clock_gettime(CLOCK_REALTIME, &ts);
        ts.tv_sec += timeout / 1000;
        ts.tv_nsec += (timeout % 1000) * 1000000;
        if (ts.tv_nsec >= 1000000000) {
            ts.tv_nsec -= 1000000000;
            ts.tv_sec++;
        }
        ret = pthread_cond_timedwait(&waiter->cond, &js_atomics_mutex,
                                     &ts);
    }
    if (waiter->linked)
        list_del(&waiter->link);
    pthread_mutex_unlock(&js_atomics_mutex);
    pthread_cond_destroy(&waiter->cond);
    if (ret == ETIMEDOUT) {
        return JS_AtomToString(ctx, JS_ATOM_timed_out);
    } else {
        return JS_AtomToString(ctx, JS_ATOM_ok);
    }
}

static JSValue js_atomics_notify(JSContext *ctx,
                                 JSValueConst this_obj,
                                 int argc, JSValueConst *argv)
{
    struct list_head *el, *el1, waiter_list;
    int32_t count, n;
    void *ptr;
    JSAtomicsWaiter *waiter;
    JSArrayBuffer *abuf;

    ptr = js_atomics_get_ptr(ctx, &abuf, NULL, NULL, argv[0], argv[1], 1);
    if (!ptr)
        return JS_EXCEPTION;

    if (JS_IsUndefined(argv[2])) {
        count = INT32_MAX;
    } else {
        if (JS_ToInt32Clamp(ctx, &count, argv[2], 0, INT32_MAX, 0))
            return JS_EXCEPTION;
    }
    if (abuf->detached)
        return JS_ThrowTypeErrorDetachedArrayBuffer(ctx);

    n = 0;
    if (abuf->shared && count > 0) {
        pthread_mutex_lock(&js_atomics_mutex);
        init_list_head(&waiter_list);
        list_for_each_safe(el, el1, &js_atomics_waiter_list) {
            waiter = list_entry(el, JSAtomicsWaiter, link);
            if (waiter->ptr == ptr) {
                list_del(&waiter->link);
                waiter->linked = FALSE;
                list_add_tail(&waiter->link, &waiter_list);
                n++;
                if (n >= count)
                    break;
            }
        }
        list_for_each(el, &waiter_list) {
            waiter = list_entry(el, JSAtomicsWaiter, link);
            pthread_cond_signal(&waiter->cond);
        }
        pthread_mutex_unlock(&js_atomics_mutex);
    }
    return JS_NewInt32(ctx, n);
}

static const JSCFunctionListEntry js_atomics_funcs[] = {
    JS_CFUNC_MAGIC_DEF("add", 3, js_atomics_op, ATOMICS_OP_ADD ),
    JS_CFUNC_MAGIC_DEF("and", 3, js_atomics_op, ATOMICS_OP_AND ),
    JS_CFUNC_MAGIC_DEF("or", 3, js_atomics_op, ATOMICS_OP_OR ),
    JS_CFUNC_MAGIC_DEF("sub", 3, js_atomics_op, ATOMICS_OP_SUB ),
    JS_CFUNC_MAGIC_DEF("xor", 3, js_atomics_op, ATOMICS_OP_XOR ),
    JS_CFUNC_MAGIC_DEF("exchange", 3, js_atomics_op, ATOMICS_OP_EXCHANGE ),
    JS_CFUNC_MAGIC_DEF("compareExchange", 4, js_atomics_op, ATOMICS_OP_COMPARE_EXCHANGE ),
    JS_CFUNC_MAGIC_DEF("load", 2, js_atomics_op, ATOMICS_OP_LOAD ),
    JS_CFUNC_DEF("store", 3, js_atomics_store ),
    JS_CFUNC_DEF("isLockFree", 1, js_atomics_isLockFree ),
    JS_CFUNC_DEF("wait", 4, js_atomics_wait ),
    JS_CFUNC_DEF("notify", 3, js_atomics_notify ),
    JS_PROP_STRING_DEF("[Symbol.toStringTag]", "Atomics", JS_PROP_CONFIGURABLE ),
};

static const JSCFunctionListEntry js_atomics_obj[] = {
    JS_OBJECT_DEF("Atomics", js_atomics_funcs, countof(js_atomics_funcs), JS_PROP_WRITABLE | JS_PROP_CONFIGURABLE ),
};

void JS_AddIntrinsicAtomics(JSContext *ctx)
{
    /* add Atomics as autoinit object */
    JS_SetPropertyFunctionList(ctx, ctx->global_obj, js_atomics_obj, countof(js_atomics_obj));
}

#endif /* CONFIG_ATOMICS */
