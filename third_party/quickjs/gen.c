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

typedef enum JSGeneratorStateEnum {
    JS_GENERATOR_STATE_SUSPENDED_START,
    JS_GENERATOR_STATE_SUSPENDED_YIELD,
    JS_GENERATOR_STATE_SUSPENDED_YIELD_STAR,
    JS_GENERATOR_STATE_EXECUTING,
    JS_GENERATOR_STATE_COMPLETED,
} JSGeneratorStateEnum;

typedef struct JSGeneratorData {
    JSGeneratorStateEnum state;
    JSAsyncFunctionState func_state;
} JSGeneratorData;

static void free_generator_stack_rt(JSRuntime *rt, JSGeneratorData *s)
{
    if (s->state == JS_GENERATOR_STATE_COMPLETED)
        return;
    async_func_free(rt, &s->func_state);
    s->state = JS_GENERATOR_STATE_COMPLETED;
}

void js_generator_finalizer(JSRuntime *rt, JSValue obj)
{
    JSGeneratorData *s = JS_GetOpaque(obj, JS_CLASS_GENERATOR);
    if (s) {
        free_generator_stack_rt(rt, s);
        js_free_rt(rt, s);
    }
}

static void free_generator_stack(JSContext *ctx, JSGeneratorData *s)
{
    free_generator_stack_rt(ctx->rt, s);
}

void js_generator_mark(JSRuntime *rt, JSValueConst val, JS_MarkFunc *mark_func)
{
    JSObject *p = JS_VALUE_GET_OBJ(val);
    JSGeneratorData *s = p->u.generator_data;
    if (!s || s->state == JS_GENERATOR_STATE_COMPLETED)
        return;
    async_func_mark(rt, &s->func_state, mark_func);
}

static JSValue js_generator_next(JSContext *ctx, JSValueConst this_val,
                                 int argc, JSValueConst *argv,
                                 BOOL *pdone, int magic)
{
    JSGeneratorData *s = JS_GetOpaque(this_val, JS_CLASS_GENERATOR);
    JSStackFrame *sf;
    JSValue ret, func_ret;
    *pdone = TRUE;
    if (!s)
        return JS_ThrowTypeError(ctx, "not a generator");
    sf = &s->func_state.frame;
    switch(s->state) {
    default:
    case JS_GENERATOR_STATE_SUSPENDED_START:
        if (magic == GEN_MAGIC_NEXT) {
            goto exec_no_arg;
        } else {
            free_generator_stack(ctx, s);
            goto done;
        }
        break;
    case JS_GENERATOR_STATE_SUSPENDED_YIELD_STAR:
    case JS_GENERATOR_STATE_SUSPENDED_YIELD:
        /* cur_sp[-1] was set to JS_UNDEFINED in the previous call */
        ret = JS_DupValue(ctx, argv[0]);
        if (magic == GEN_MAGIC_THROW &&
            s->state == JS_GENERATOR_STATE_SUSPENDED_YIELD) {
            JS_Throw(ctx, ret);
            s->func_state.throw_flag = TRUE;
        } else {
            sf->cur_sp[-1] = ret;
            sf->cur_sp[0] = JS_NewInt32(ctx, magic);
            sf->cur_sp++;
        exec_no_arg:
            s->func_state.throw_flag = FALSE;
        }
        s->state = JS_GENERATOR_STATE_EXECUTING;
        func_ret = async_func_resume(ctx, &s->func_state);
        s->state = JS_GENERATOR_STATE_SUSPENDED_YIELD;
        if (JS_IsException(func_ret)) {
            /* finalize the execution in case of exception */
            free_generator_stack(ctx, s);
            return func_ret;
        }
        if (JS_VALUE_GET_TAG(func_ret) == JS_TAG_INT) {
            /* get the returned yield value at the top of the stack */
            ret = sf->cur_sp[-1];
            sf->cur_sp[-1] = JS_UNDEFINED;
            if (JS_VALUE_GET_INT(func_ret) == FUNC_RET_YIELD_STAR) {
                s->state = JS_GENERATOR_STATE_SUSPENDED_YIELD_STAR;
                /* return (value, done) object */
                *pdone = 2;
            } else {
                *pdone = FALSE;
            }
        } else {
            /* end of iterator */
            ret = sf->cur_sp[-1];
            sf->cur_sp[-1] = JS_UNDEFINED;
            JS_FreeValue(ctx, func_ret);
            free_generator_stack(ctx, s);
        }
        break;
    case JS_GENERATOR_STATE_COMPLETED:
    done:
        /* execution is finished */
        switch(magic) {
        default:
        case GEN_MAGIC_NEXT:
            ret = JS_UNDEFINED;
            break;
        case GEN_MAGIC_RETURN:
            ret = JS_DupValue(ctx, argv[0]);
            break;
        case GEN_MAGIC_THROW:
            ret = JS_Throw(ctx, JS_DupValue(ctx, argv[0]));
            break;
        }
        break;
    case JS_GENERATOR_STATE_EXECUTING:
        ret = JS_ThrowTypeError(ctx, "cannot invoke a running generator");
        break;
    }
    return ret;
}

JSValue js_generator_function_call(JSContext *ctx, JSValueConst func_obj, 
                                   JSValueConst this_obj, int argc, 
                                   JSValueConst *argv, int flags)
{
    JSValue obj, func_ret;
    JSGeneratorData *s;
    s = js_mallocz(ctx, sizeof(*s));
    if (!s)
        return JS_EXCEPTION;
    s->state = JS_GENERATOR_STATE_SUSPENDED_START;
    if (async_func_init(ctx, &s->func_state, func_obj, this_obj, argc, argv)) {
        s->state = JS_GENERATOR_STATE_COMPLETED;
        goto fail;
    }
    /* execute the function up to 'OP_initial_yield' */
    func_ret = async_func_resume(ctx, &s->func_state);
    if (JS_IsException(func_ret))
        goto fail;
    JS_FreeValue(ctx, func_ret);
    obj = js_create_from_ctor(ctx, func_obj, JS_CLASS_GENERATOR);
    if (JS_IsException(obj))
        goto fail;
    JS_SetOpaque(obj, s);
    return obj;
 fail:
    free_generator_stack_rt(ctx->rt, s);
    js_free(ctx, s);
    return JS_EXCEPTION;
}

static JSValue js_async_generator_resolve_function(JSContext *ctx,
                                                   JSValueConst this_obj,
                                                   int argc, JSValueConst *argv,
                                                   int magic, JSValue *func_data);

static int js_async_generator_resolve_function_create(JSContext *ctx,
                                                      JSValueConst generator,
                                                      JSValue *resolving_funcs,
                                                      BOOL is_resume_next)
{
    int i;
    JSValue func;
    for(i = 0; i < 2; i++) {
        func = JS_NewCFunctionData(ctx, js_async_generator_resolve_function, 1,
                                   i + is_resume_next * 2, 1, &generator);
        if (JS_IsException(func)) {
            if (i == 1)
                JS_FreeValue(ctx, resolving_funcs[0]);
            return -1;
        }
        resolving_funcs[i] = func;
    }
    return 0;
}

static int js_async_generator_await(JSContext *ctx,
                                    JSAsyncGeneratorData *s,
                                    JSValueConst value)
{
    JSValue promise, resolving_funcs[2], resolving_funcs1[2];
    int i, res;
    promise = js_promise_resolve(ctx, ctx->promise_ctor,
                                 1, &value, 0);
    if (JS_IsException(promise))
        goto fail;
    if (js_async_generator_resolve_function_create(ctx, JS_MKPTR(JS_TAG_OBJECT, s->generator),
                                                   resolving_funcs, FALSE)) {
        JS_FreeValue(ctx, promise);
        goto fail;
    }
    /* Note: no need to create 'thrownawayCapability' as in
       the spec */
    for(i = 0; i < 2; i++)
        resolving_funcs1[i] = JS_UNDEFINED;
    res = perform_promise_then(ctx, promise,
                               (JSValueConst *)resolving_funcs,
                               (JSValueConst *)resolving_funcs1);
    JS_FreeValue(ctx, promise);
    for(i = 0; i < 2; i++)
        JS_FreeValue(ctx, resolving_funcs[i]);
    if (res)
        goto fail;
    return 0;
 fail:
    return -1;
}

static void js_async_generator_resolve_or_reject(JSContext *ctx,
                                                 JSAsyncGeneratorData *s,
                                                 JSValueConst result,
                                                 int is_reject)
{
    JSAsyncGeneratorRequest *next;
    JSValue ret;
    next = list_entry(s->queue.next, JSAsyncGeneratorRequest, link);
    list_del(&next->link);
    ret = JS_Call(ctx, next->resolving_funcs[is_reject], JS_UNDEFINED, 1,
                  &result);
    JS_FreeValue(ctx, ret);
    JS_FreeValue(ctx, next->result);
    JS_FreeValue(ctx, next->promise);
    JS_FreeValue(ctx, next->resolving_funcs[0]);
    JS_FreeValue(ctx, next->resolving_funcs[1]);
    js_free(ctx, next);
}

static void js_async_generator_resolve(JSContext *ctx,
                                       JSAsyncGeneratorData *s,
                                       JSValueConst value,
                                       BOOL done)
{
    JSValue result;
    result = js_create_iterator_result(ctx, JS_DupValue(ctx, value), done);
    /* XXX: better exception handling ? */
    js_async_generator_resolve_or_reject(ctx, s, result, 0);
    JS_FreeValue(ctx, result);
 }

static void js_async_generator_reject(JSContext *ctx,
                                       JSAsyncGeneratorData *s,
                                       JSValueConst exception)
{
    js_async_generator_resolve_or_reject(ctx, s, exception, 1);
}

static void js_async_generator_complete(JSContext *ctx,
                                        JSAsyncGeneratorData *s)
{
    if (s->state != JS_ASYNC_GENERATOR_STATE_COMPLETED) {
        s->state = JS_ASYNC_GENERATOR_STATE_COMPLETED;
        async_func_free(ctx->rt, &s->func_state);
    }
}

static int js_async_generator_completed_return(JSContext *ctx,
                                               JSAsyncGeneratorData *s,
                                               JSValueConst value)
{
    JSValue promise, resolving_funcs[2], resolving_funcs1[2];
    int res;

    promise = js_promise_resolve(ctx, ctx->promise_ctor,
                                 1, (JSValueConst *)&value, 0);
    if (JS_IsException(promise))
        return -1;
    if (js_async_generator_resolve_function_create(ctx,
                                                   JS_MKPTR(JS_TAG_OBJECT, s->generator),
                                                   resolving_funcs1,
                                                   TRUE)) {
        JS_FreeValue(ctx, promise);
        return -1;
    }
    resolving_funcs[0] = JS_UNDEFINED;
    resolving_funcs[1] = JS_UNDEFINED;
    res = perform_promise_then(ctx, promise,
                               (JSValueConst *)resolving_funcs1,
                               (JSValueConst *)resolving_funcs);
    JS_FreeValue(ctx, resolving_funcs1[0]);
    JS_FreeValue(ctx, resolving_funcs1[1]);
    JS_FreeValue(ctx, promise);
    return res;
}

void js_async_generator_resume_next(JSContext *ctx, JSAsyncGeneratorData *s)
{
    JSAsyncGeneratorRequest *next;
    JSValue func_ret, value;
    for(;;) {
        if (list_empty(&s->queue))
            break;
        next = list_entry(s->queue.next, JSAsyncGeneratorRequest, link);
        switch(s->state) {
        case JS_ASYNC_GENERATOR_STATE_EXECUTING:
            /* only happens when restarting execution after await() */
            goto resume_exec;
        case JS_ASYNC_GENERATOR_STATE_AWAITING_RETURN:
            goto done;
        case JS_ASYNC_GENERATOR_STATE_SUSPENDED_START:
            if (next->completion_type == GEN_MAGIC_NEXT) {
                goto exec_no_arg;
            } else {
                js_async_generator_complete(ctx, s);
            }
            break;
        case JS_ASYNC_GENERATOR_STATE_COMPLETED:
            if (next->completion_type == GEN_MAGIC_NEXT) {
                js_async_generator_resolve(ctx, s, JS_UNDEFINED, TRUE);
            } else if (next->completion_type == GEN_MAGIC_RETURN) {
                s->state = JS_ASYNC_GENERATOR_STATE_AWAITING_RETURN;
                js_async_generator_completed_return(ctx, s, next->result);
                goto done;
            } else {
                js_async_generator_reject(ctx, s, next->result);
            }
            goto done;
        case JS_ASYNC_GENERATOR_STATE_SUSPENDED_YIELD:
        case JS_ASYNC_GENERATOR_STATE_SUSPENDED_YIELD_STAR:
            value = JS_DupValue(ctx, next->result);
            if (next->completion_type == GEN_MAGIC_THROW &&
                s->state == JS_ASYNC_GENERATOR_STATE_SUSPENDED_YIELD) {
                JS_Throw(ctx, value);
                s->func_state.throw_flag = TRUE;
            } else {
                /* 'yield' returns a value. 'yield *' also returns a value
                   in case the 'throw' method is called */
                s->func_state.frame.cur_sp[-1] = value;
                s->func_state.frame.cur_sp[0] =
                    JS_NewInt32(ctx, next->completion_type);
                s->func_state.frame.cur_sp++;
            exec_no_arg:
                s->func_state.throw_flag = FALSE;
            }
            s->state = JS_ASYNC_GENERATOR_STATE_EXECUTING;
        resume_exec:
            func_ret = async_func_resume(ctx, &s->func_state);
            if (JS_IsException(func_ret)) {
                value = JS_GetException(ctx);
                js_async_generator_complete(ctx, s);
                js_async_generator_reject(ctx, s, value);
                JS_FreeValue(ctx, value);
            } else if (JS_VALUE_GET_TAG(func_ret) == JS_TAG_INT) {
                int func_ret_code;
                value = s->func_state.frame.cur_sp[-1];
                s->func_state.frame.cur_sp[-1] = JS_UNDEFINED;
                func_ret_code = JS_VALUE_GET_INT(func_ret);
                switch(func_ret_code) {
                case FUNC_RET_YIELD:
                case FUNC_RET_YIELD_STAR:
                    if (func_ret_code == FUNC_RET_YIELD_STAR)
                        s->state = JS_ASYNC_GENERATOR_STATE_SUSPENDED_YIELD_STAR;
                    else
                        s->state = JS_ASYNC_GENERATOR_STATE_SUSPENDED_YIELD;
                    js_async_generator_resolve(ctx, s, value, FALSE);
                    JS_FreeValue(ctx, value);
                    break;
                case FUNC_RET_AWAIT:
                    js_async_generator_await(ctx, s, value);
                    JS_FreeValue(ctx, value);
                    goto done;
                default:
                    abort();
                }
            } else {
                assert(JS_IsUndefined(func_ret));
                /* end of function */
                value = s->func_state.frame.cur_sp[-1];
                s->func_state.frame.cur_sp[-1] = JS_UNDEFINED;
                js_async_generator_complete(ctx, s);
                js_async_generator_resolve(ctx, s, value, TRUE);
                JS_FreeValue(ctx, value);
            }
            break;
        default:
            abort();
        }
    }
 done: ;
}

static JSValue js_async_generator_resolve_function(JSContext *ctx,
                                                   JSValueConst this_obj,
                                                   int argc, JSValueConst *argv,
                                                   int magic, JSValue *func_data)
{
    BOOL is_reject = magic & 1;
    JSAsyncGeneratorData *s = JS_GetOpaque(func_data[0], JS_CLASS_ASYNC_GENERATOR);
    JSValueConst arg = argv[0];
    /* XXX: what if s == NULL */
    if (magic >= 2) {
        /* resume next case in AWAITING_RETURN state */
        assert(s->state == JS_ASYNC_GENERATOR_STATE_AWAITING_RETURN ||
               s->state == JS_ASYNC_GENERATOR_STATE_COMPLETED);
        s->state = JS_ASYNC_GENERATOR_STATE_COMPLETED;
        if (is_reject) {
            js_async_generator_reject(ctx, s, arg);
        } else {
            js_async_generator_resolve(ctx, s, arg, TRUE);
        }
    } else {
        /* restart function execution after await() */
        assert(s->state == JS_ASYNC_GENERATOR_STATE_EXECUTING);
        s->func_state.throw_flag = is_reject;
        if (is_reject) {
            JS_Throw(ctx, JS_DupValue(ctx, arg));
        } else {
            /* return value of await */
            s->func_state.frame.cur_sp[-1] = JS_DupValue(ctx, arg);
        }
        js_async_generator_resume_next(ctx, s);
    }
    return JS_UNDEFINED;
}

void js_async_generator_free(JSRuntime *rt, JSAsyncGeneratorData *s)
{
    struct list_head *el, *el1;
    JSAsyncGeneratorRequest *req;
    list_for_each_safe(el, el1, &s->queue) {
        req = list_entry(el, JSAsyncGeneratorRequest, link);
        JS_FreeValueRT(rt, req->result);
        JS_FreeValueRT(rt, req->promise);
        JS_FreeValueRT(rt, req->resolving_funcs[0]);
        JS_FreeValueRT(rt, req->resolving_funcs[1]);
        js_free_rt(rt, req);
    }
    if (s->state != JS_ASYNC_GENERATOR_STATE_COMPLETED &&
        s->state != JS_ASYNC_GENERATOR_STATE_AWAITING_RETURN) {
        async_func_free(rt, &s->func_state);
    }
    js_free_rt(rt, s);
}

static const JSCFunctionListEntry js_generator_function_proto_funcs[] = {
    JS_PROP_STRING_DEF("[Symbol.toStringTag]", "GeneratorFunction", JS_PROP_CONFIGURABLE),
};

static const JSCFunctionListEntry js_generator_proto_funcs[] = {
    JS_ITERATOR_NEXT_DEF("next", 1, js_generator_next, GEN_MAGIC_NEXT ),
    JS_ITERATOR_NEXT_DEF("return", 1, js_generator_next, GEN_MAGIC_RETURN ),
    JS_ITERATOR_NEXT_DEF("throw", 1, js_generator_next, GEN_MAGIC_THROW ),
    JS_PROP_STRING_DEF("[Symbol.toStringTag]", "Generator", JS_PROP_CONFIGURABLE),
};

void JS_AddIntrinsicGenerator(JSContext *ctx)
{
    JSValue obj1;
    ctx->class_proto[JS_CLASS_GENERATOR] = JS_NewObjectProto(ctx, ctx->iterator_proto);
    JS_SetPropertyFunctionList(ctx, ctx->class_proto[JS_CLASS_GENERATOR],
                               js_generator_proto_funcs,
                               countof(js_generator_proto_funcs));
    ctx->class_proto[JS_CLASS_GENERATOR_FUNCTION] = JS_NewObjectProto(ctx, ctx->function_proto);
    obj1 = JS_NewCFunctionMagic(ctx, js_function_constructor,
                                "GeneratorFunction", 1,
                                JS_CFUNC_constructor_or_func_magic, JS_FUNC_GENERATOR);
    JS_SetPropertyFunctionList(ctx,
                               ctx->class_proto[JS_CLASS_GENERATOR_FUNCTION],
                               js_generator_function_proto_funcs,
                               countof(js_generator_function_proto_funcs));
    JS_SetConstructor2(ctx, ctx->class_proto[JS_CLASS_GENERATOR_FUNCTION],
                       ctx->class_proto[JS_CLASS_GENERATOR],
                       JS_PROP_CONFIGURABLE, JS_PROP_CONFIGURABLE);
    JS_SetConstructor2(ctx, obj1, ctx->class_proto[JS_CLASS_GENERATOR_FUNCTION],
                       0, JS_PROP_CONFIGURABLE);
    JS_FreeValue(ctx, obj1);
}
