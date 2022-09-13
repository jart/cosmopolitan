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
#include "libc/mem/mem.h"
#include "libc/mem/gc.internal.h"
#include "libc/runtime/runtime.h"
#include "third_party/quickjs/internal.h"

asm(".ident\t\"\\n\\n\
QuickJS (MIT License)\\n\
Copyright (c) 2017-2021 Fabrice Bellard\\n\
Copyright (c) 2017-2021 Charlie Gordon\"");
asm(".include \"libc/disclaimer.inc\"");
/* clang-format off */

static const uint16_t func_kind_to_class_id[] = {
    [JS_FUNC_NORMAL] = JS_CLASS_BYTECODE_FUNCTION,
    [JS_FUNC_GENERATOR] = JS_CLASS_GENERATOR_FUNCTION,
    [JS_FUNC_ASYNC] = JS_CLASS_ASYNC_FUNCTION,
    [JS_FUNC_ASYNC_GENERATOR] = JS_CLASS_ASYNC_GENERATOR_FUNCTION,
};

static JSValue JS_CallConstructorInternal(JSContext *,
                                          JSValueConst,
                                          JSValueConst,
                                          int, JSValue *, int);

JSValue js_closure2(JSContext *ctx, JSValue func_obj,
                    JSFunctionBytecode *b,
                    JSVarRef **cur_var_refs,
                    JSStackFrame *sf)
{
    JSObject *p;
    JSVarRef **var_refs;
    int i;
    p = JS_VALUE_GET_OBJ(func_obj);
    p->u.func.function_bytecode = b;
    p->u.func.home_object = NULL;
    p->u.func.var_refs = NULL;
    if (b->closure_var_count) {
        var_refs = js_mallocz(ctx, sizeof(var_refs[0]) * b->closure_var_count);
        if (!var_refs)
            goto fail;
        p->u.func.var_refs = var_refs;
        for(i = 0; i < b->closure_var_count; i++) {
            JSClosureVar *cv = &b->closure_var[i];
            JSVarRef *var_ref;
            if (cv->is_local) {
                /* reuse the existing variable reference if it already exists */
                var_ref = get_var_ref(ctx, sf, cv->var_idx, cv->is_arg);
                if (!var_ref)
                    goto fail;
            } else {
                var_ref = cur_var_refs[cv->var_idx];
                var_ref->header.ref_count++;
            }
            var_refs[i] = var_ref;
        }
    }
    return func_obj;
 fail:
    /* bfunc is freed when func_obj is freed */
    JS_FreeValue(ctx, func_obj);
    return JS_EXCEPTION;
}

JSValue js_closure(JSContext *ctx, JSValue bfunc,
                   JSVarRef **cur_var_refs,
                   JSStackFrame *sf)
{
    JSFunctionBytecode *b;
    JSValue func_obj;
    JSAtom name_atom;
    b = JS_VALUE_GET_PTR(bfunc);
    func_obj = JS_NewObjectClass(ctx, func_kind_to_class_id[b->func_kind]);
    if (JS_IsException(func_obj)) {
        JS_FreeValue(ctx, bfunc);
        return JS_EXCEPTION;
    }
    func_obj = js_closure2(ctx, func_obj, b, cur_var_refs, sf);
    if (JS_IsException(func_obj)) {
        /* bfunc has been freed */
        goto fail;
    }
    name_atom = b->func_name;
    if (name_atom == JS_ATOM_NULL)
        name_atom = JS_ATOM_empty_string;
    js_function_set_properties(ctx, func_obj, name_atom,
                               b->defined_arg_count);
    if (b->func_kind & JS_FUNC_GENERATOR) {
        JSValue proto;
        int proto_class_id;
        /* generators have a prototype field which is used as
           prototype for the generator object */
        if (b->func_kind == JS_FUNC_ASYNC_GENERATOR)
            proto_class_id = JS_CLASS_ASYNC_GENERATOR;
        else
            proto_class_id = JS_CLASS_GENERATOR;
        proto = JS_NewObjectProto(ctx, ctx->class_proto[proto_class_id]);
        if (JS_IsException(proto))
            goto fail;
        JS_DefinePropertyValue(ctx, func_obj, JS_ATOM_prototype, proto,
                               JS_PROP_WRITABLE);
    } else if (b->has_prototype) {
        /* add the 'prototype' property: delay instantiation to avoid
           creating cycles for every javascript function. The prototype
           object is created on the fly when first accessed */
        JS_SetConstructorBit(ctx, func_obj, TRUE);
        JS_DefineAutoInitProperty(ctx, func_obj, JS_ATOM_prototype,
                                  JS_AUTOINIT_ID_PROTOTYPE, NULL,
                                  JS_PROP_WRITABLE);
    }
    return func_obj;
 fail:
    /* bfunc is freed when func_obj is freed */
    JS_FreeValue(ctx, func_obj);
    return JS_EXCEPTION;
}

JSValue js_call_c_function(JSContext *ctx, JSValueConst func_obj,
                           JSValueConst this_obj,
                           int argc, JSValueConst *argv, int flags)
{
    JSRuntime *rt = ctx->rt;
    JSCFunctionType func;
    JSObject *p;
    JSStackFrame sf_s, *sf = &sf_s, *prev_sf;
    JSValue ret_val;
    JSValueConst *arg_buf;
    int arg_count, i;
    JSCFunctionEnum cproto;
    p = JS_VALUE_GET_OBJ(func_obj);
    cproto = p->u.cfunc.cproto;
    arg_count = p->u.cfunc.length;
    /* better to always check stack overflow */
    if (js_check_stack_overflow(rt, sizeof(arg_buf[0]) * arg_count))
        return JS_ThrowStackOverflow(ctx);
    prev_sf = rt->current_stack_frame;
    sf->prev_frame = prev_sf;
    rt->current_stack_frame = sf;
    ctx = p->u.cfunc.realm; /* change the current realm */
#ifdef CONFIG_BIGNUM
    /* we only propagate the bignum mode as some runtime functions
       test it */
    if (prev_sf)
        sf->js_mode = prev_sf->js_mode & JS_MODE_MATH;
    else
        sf->js_mode = 0;
#else
    sf->js_mode = 0;
#endif
    sf->cur_func = (JSValue)func_obj;
    sf->arg_count = argc;
    arg_buf = argv;
    if (UNLIKELY(argc < arg_count)) {
        /* ensure that at least argc_count arguments are readable */
        arg_buf = gc(malloc(sizeof(arg_buf[0]) * arg_count));
        for(i = 0; i < argc; i++)
            arg_buf[i] = argv[i];
        for(i = argc; i < arg_count; i++)
            arg_buf[i] = JS_UNDEFINED;
        sf->arg_count = arg_count;
    }
    sf->arg_buf = (JSValue*)arg_buf;
    func = p->u.cfunc.c_function;
    switch(cproto) {
    case JS_CFUNC_constructor:
    case JS_CFUNC_constructor_or_func:
        if (!(flags & JS_CALL_FLAG_CONSTRUCTOR)) {
            if (cproto == JS_CFUNC_constructor) {
            not_a_constructor:
                ret_val = JS_ThrowTypeError(ctx, "must be called with new");
                break;
            } else {
                this_obj = JS_UNDEFINED;
            }
        }
        /* here this_obj is new_target */
        /* fall thru */
    case JS_CFUNC_generic:
        ret_val = func.generic(ctx, this_obj, argc, arg_buf);
        break;
    case JS_CFUNC_constructor_magic:
    case JS_CFUNC_constructor_or_func_magic:
        if (!(flags & JS_CALL_FLAG_CONSTRUCTOR)) {
            if (cproto == JS_CFUNC_constructor_magic) {
                goto not_a_constructor;
            } else {
                this_obj = JS_UNDEFINED;
            }
        }
        /* fall thru */
    case JS_CFUNC_generic_magic:
        ret_val = func.generic_magic(ctx, this_obj, argc, arg_buf,
                                     p->u.cfunc.magic);
        break;
    case JS_CFUNC_getter:
        ret_val = func.getter(ctx, this_obj);
        break;
    case JS_CFUNC_setter:
        ret_val = func.setter(ctx, this_obj, arg_buf[0]);
        break;
    case JS_CFUNC_getter_magic:
        ret_val = func.getter_magic(ctx, this_obj, p->u.cfunc.magic);
        break;
    case JS_CFUNC_setter_magic:
        ret_val = func.setter_magic(ctx, this_obj, arg_buf[0], p->u.cfunc.magic);
        break;
    case JS_CFUNC_f_f:
        {
            double d1;
            if (UNLIKELY(JS_ToFloat64(ctx, &d1, arg_buf[0]))) {
                ret_val = JS_EXCEPTION;
                break;
            }
            ret_val = JS_NewFloat64(ctx, func.f_f(d1));
        }
        break;
    case JS_CFUNC_f_f_f:
        {
            double d1, d2;
            if (UNLIKELY(JS_ToFloat64(ctx, &d1, arg_buf[0]))) {
                ret_val = JS_EXCEPTION;
                break;
            }
            if (UNLIKELY(JS_ToFloat64(ctx, &d2, arg_buf[1]))) {
                ret_val = JS_EXCEPTION;
                break;
            }
            ret_val = JS_NewFloat64(ctx, func.f_f_f(d1, d2));
        }
        break;
    case JS_CFUNC_iterator_next:
        {
            int done;
            ret_val = func.iterator_next(ctx, this_obj, argc, arg_buf,
                                         &done, p->u.cfunc.magic);
            if (!JS_IsException(ret_val) && done != 2) {
                ret_val = js_create_iterator_result(ctx, ret_val, done);
            }
        }
        break;
    default:
        abort();
    }
    rt->current_stack_frame = sf->prev_frame;
    return ret_val;
}

JSValue js_call_bound_function(JSContext *ctx, JSValueConst func_obj,
                               JSValueConst this_obj,
                               int argc, JSValueConst *argv, int flags)
{
    JSObject *p;
    JSBoundFunction *bf;
    JSValueConst *arg_buf, new_target;
    int arg_count, i;
    p = JS_VALUE_GET_OBJ(func_obj);
    bf = p->u.bound_function;
    arg_count = bf->argc + argc;
    if (js_check_stack_overflow(ctx->rt, sizeof(JSValue) * arg_count))
        return JS_ThrowStackOverflow(ctx);
    arg_buf = gc(malloc(sizeof(JSValue) * arg_count));
    for(i = 0; i < bf->argc; i++) {
        arg_buf[i] = bf->argv[i];
    }
    for(i = 0; i < argc; i++) {
        arg_buf[bf->argc + i] = argv[i];
    }
    if (flags & JS_CALL_FLAG_CONSTRUCTOR) {
        new_target = this_obj;
        if (js_same_value(ctx, func_obj, new_target))
            new_target = bf->func_obj;
        return JS_CallConstructor2(ctx, bf->func_obj, new_target,
                                   arg_count, arg_buf);
    } else {
        return JS_Call(ctx, bf->func_obj, bf->this_val,
                       arg_count, arg_buf);
    }
}

JSValue JS_CallConstructor2(JSContext *ctx, JSValueConst func_obj,
                            JSValueConst new_target,
                            int argc, JSValueConst *argv)
{
    return JS_CallConstructorInternal(ctx, func_obj, new_target,
                                      argc, (JSValue *)argv,
                                      JS_CALL_FLAG_COPY_ARGV);
}

JSValue JS_CallConstructor(JSContext *ctx, JSValueConst func_obj,
                           int argc, JSValueConst *argv)
{
    return JS_CallConstructorInternal(ctx, func_obj, func_obj,
                                      argc, (JSValue *)argv,
                                      JS_CALL_FLAG_COPY_ARGV);
}

JSValue JS_Invoke(JSContext *ctx, JSValueConst this_val, JSAtom atom,
                  int argc, JSValueConst *argv)
{
    JSValue func_obj;
    func_obj = JS_GetProperty(ctx, this_val, atom);
    if (JS_IsException(func_obj))
        return func_obj;
    return JS_CallFree(ctx, func_obj, this_val, argc, argv);
}

JSValue JS_InvokeFree(JSContext *ctx, JSValue this_val, JSAtom atom,
                      int argc, JSValueConst *argv)
{
    JSValue res = JS_Invoke(ctx, this_val, atom, argc, argv);
    JS_FreeValue(ctx, this_val);
    return res;
}

static JSValue js_build_arguments(JSContext *ctx, int argc, JSValueConst *argv)
{
    JSValue val, *tab;
    JSProperty *pr;
    JSObject *p;
    int i;
    val = JS_NewObjectProtoClass(ctx, ctx->class_proto[JS_CLASS_OBJECT],
                                 JS_CLASS_ARGUMENTS);
    if (JS_IsException(val))
        return val;
    p = JS_VALUE_GET_OBJ(val);
    /* add the length field (cannot fail) */
    pr = add_property(ctx, p, JS_ATOM_length,
                      JS_PROP_WRITABLE | JS_PROP_CONFIGURABLE);
    pr->u.value = JS_NewInt32(ctx, argc);
    /* initialize the fast array part */
    tab = NULL;
    if (argc > 0) {
        tab = js_malloc(ctx, sizeof(tab[0]) * argc);
        if (!tab) {
            JS_FreeValue(ctx, val);
            return JS_EXCEPTION;
        }
        for(i = 0; i < argc; i++) {
            tab[i] = JS_DupValue(ctx, argv[i]);
        }
    }
    p->u.array.u.values = tab;
    p->u.array.count = argc;
    JS_DefinePropertyValue(ctx, val, JS_ATOM_Symbol_iterator,
                           JS_DupValue(ctx, ctx->array_proto_values),
                           JS_PROP_CONFIGURABLE | JS_PROP_WRITABLE);
    /* add callee property to throw a TypeError in strict mode */
    JS_DefineProperty(ctx, val, JS_ATOM_callee, JS_UNDEFINED,
                      ctx->throw_type_error, ctx->throw_type_error,
                      JS_PROP_HAS_GET | JS_PROP_HAS_SET);
    return val;
}

/* legacy arguments object: add references to the function arguments */
static JSValue js_build_mapped_arguments(JSContext *ctx, int argc,
                                         JSValueConst *argv,
                                         JSStackFrame *sf, int arg_count)
{
    JSValue val;
    JSProperty *pr;
    JSObject *p;
    int i;
    val = JS_NewObjectProtoClass(ctx, ctx->class_proto[JS_CLASS_OBJECT],
                                 JS_CLASS_MAPPED_ARGUMENTS);
    if (JS_IsException(val))
        return val;
    p = JS_VALUE_GET_OBJ(val);
    /* add the length field (cannot fail) */
    pr = add_property(ctx, p, JS_ATOM_length,
                      JS_PROP_WRITABLE | JS_PROP_CONFIGURABLE);
    pr->u.value = JS_NewInt32(ctx, argc);
    for(i = 0; i < arg_count; i++) {
        JSVarRef *var_ref;
        var_ref = get_var_ref(ctx, sf, i, TRUE);
        if (!var_ref)
            goto fail;
        pr = add_property(ctx, p, __JS_AtomFromUInt32(i), JS_PROP_C_W_E | JS_PROP_VARREF);
        if (!pr) {
            free_var_ref(ctx->rt, var_ref);
            goto fail;
        }
        pr->u.var_ref = var_ref;
    }
    /* the arguments not mapped to the arguments of the function can
       be normal properties */
    for(i = arg_count; i < argc; i++) {
        if (JS_DefinePropertyValueUint32(ctx, val, i,
                                         JS_DupValue(ctx, argv[i]),
                                         JS_PROP_C_W_E) < 0)
            goto fail;
    }
    JS_DefinePropertyValue(ctx, val, JS_ATOM_Symbol_iterator,
                           JS_DupValue(ctx, ctx->array_proto_values),
                           JS_PROP_CONFIGURABLE | JS_PROP_WRITABLE);
    /* callee returns this function in non strict mode */
    JS_DefinePropertyValue(ctx, val, JS_ATOM_callee,
                           JS_DupValue(ctx, ctx->rt->current_stack_frame->cur_func),
                           JS_PROP_CONFIGURABLE | JS_PROP_WRITABLE);
    return val;
 fail:
    JS_FreeValue(ctx, val);
    return JS_EXCEPTION;
}

static JSValue js_import_meta(JSContext *ctx)
{
    JSAtom filename;
    JSModuleDef *m;
    filename = JS_GetScriptOrModuleName(ctx, 0);
    if (filename == JS_ATOM_NULL)
        goto fail;
    /* XXX: inefficient, need to add a module or script pointer in
       JSFunctionBytecode */
    m = js_find_loaded_module(ctx, filename);
    JS_FreeAtom(ctx, filename);
    if (!m) {
    fail:
        JS_ThrowTypeError(ctx, "import.meta not supported in this context");
        return JS_EXCEPTION;
    }
    return JS_GetImportMeta(ctx, m);
}

static JSValue js_build_rest(JSContext *ctx, int first, int argc, JSValueConst *argv)
{
    JSValue val;
    int i, ret;
    val = JS_NewArray(ctx);
    if (JS_IsException(val))
        return val;
    for (i = first; i < argc; i++) {
        ret = JS_DefinePropertyValueUint32(ctx, val, i - first,
                                           JS_DupValue(ctx, argv[i]),
                                           JS_PROP_C_W_E);
        if (ret < 0) {
            JS_FreeValue(ctx, val);
            return JS_EXCEPTION;
        }
    }
    return val;
}

static int JS_CheckBrand(JSContext *ctx, JSValueConst obj, JSValueConst func)
{
    JSObject *p, *p1, *home_obj;
    JSShapeProperty *prs;
    JSProperty *pr;
    JSValueConst brand;
    /* get the home object of 'func' */
    if (UNLIKELY(JS_VALUE_GET_TAG(func) != JS_TAG_OBJECT)) {
    not_obj:
        JS_ThrowTypeErrorNotAnObject(ctx);
        return -1;
    }
    p1 = JS_VALUE_GET_OBJ(func);
    if (!js_class_has_bytecode(p1->class_id))
        goto not_obj;
    home_obj = p1->u.func.home_object;
    if (!home_obj)
        goto not_obj;
    prs = find_own_property(&pr, home_obj, JS_ATOM_Private_brand);
    if (!prs) {
        JS_ThrowTypeError(ctx, "expecting <brand> private field");
        return -1;
    }
    brand = pr->u.value;
    /* safety check */
    if (UNLIKELY(JS_VALUE_GET_TAG(brand) != JS_TAG_SYMBOL))
        goto not_obj;
    /* get the brand array of 'obj' */
    if (UNLIKELY(JS_VALUE_GET_TAG(obj) != JS_TAG_OBJECT))
        goto not_obj;
    p = JS_VALUE_GET_OBJ(obj);
    prs = find_own_property(&pr, p, js_symbol_to_atom(ctx, (JSValue)brand));
    if (!prs) {
        JS_ThrowTypeError(ctx, "invalid brand on object");
        return -1;
    }
    return 0;
}

/* descr must be a non-numeric string atom */
static JSValue JS_NewSymbolFromAtom(JSContext *ctx, JSAtom descr,
                                    int atom_type)
{
    JSRuntime *rt = ctx->rt;
    JSString *p;
    assert(!__JS_AtomIsTaggedInt(descr));
    assert(descr < rt->atom_size);
    p = rt->atom_array[descr];
    JS_DupValue(ctx, JS_MKPTR(JS_TAG_STRING, p));
    return JS_NewSymbol(ctx, p, atom_type);
}

static int JS_AddBrand(JSContext *ctx, JSValueConst obj, JSValueConst home_obj)
{
    JSObject *p, *p1;
    JSShapeProperty *prs;
    JSProperty *pr;
    JSValue brand;
    JSAtom brand_atom;
    if (UNLIKELY(JS_VALUE_GET_TAG(home_obj) != JS_TAG_OBJECT)) {
        JS_ThrowTypeErrorNotAnObject(ctx);
        return -1;
    }
    p = JS_VALUE_GET_OBJ(home_obj);
    prs = find_own_property(&pr, p, JS_ATOM_Private_brand);
    if (!prs) {
        brand = JS_NewSymbolFromAtom(ctx, JS_ATOM_brand, JS_ATOM_TYPE_PRIVATE);
        if (JS_IsException(brand))
            return -1;
        /* if the brand is not present, add it */
        pr = add_property(ctx, p, JS_ATOM_Private_brand, JS_PROP_C_W_E);
        if (!pr) {
            JS_FreeValue(ctx, brand);
            return -1;
        }
        pr->u.value = JS_DupValue(ctx, brand);
    } else {
        brand = JS_DupValue(ctx, pr->u.value);
    }
    brand_atom = js_symbol_to_atom(ctx, brand);
    if (UNLIKELY(JS_VALUE_GET_TAG(obj) != JS_TAG_OBJECT)) {
        JS_ThrowTypeErrorNotAnObject(ctx);
        JS_FreeAtom(ctx, brand_atom);
        return -1;
    }
    p1 = JS_VALUE_GET_OBJ(obj);
    pr = add_property(ctx, p1, brand_atom, JS_PROP_C_W_E);
    JS_FreeAtom(ctx, brand_atom);
    if (!pr)
        return -1;
    pr->u.value = JS_UNDEFINED;
    return 0;
}

static JSValue js_dynamic_import_job(JSContext *ctx,
                                     int argc, JSValueConst *argv)
{
    JSValueConst *resolving_funcs = argv;
    JSValueConst basename_val = argv[2];
    JSValueConst specifier = argv[3];
    JSModuleDef *m;
    const char *basename = NULL, *filename;
    JSValue ret, err, ns;
    if (!JS_IsString(basename_val)) {
        JS_ThrowTypeError(ctx, "no function filename for import()");
        goto exception;
    }
    basename = JS_ToCString(ctx, basename_val);
    if (!basename)
        goto exception;
    filename = JS_ToCString(ctx, specifier);
    if (!filename)
        goto exception;
    m = JS_RunModule(ctx, basename, filename);
    JS_FreeCString(ctx, filename);
    if (!m)
        goto exception;
    /* return the module namespace */
    ns = js_get_module_ns(ctx, m);
    if (JS_IsException(ns))
        goto exception;
    ret = JS_Call(ctx, resolving_funcs[0], JS_UNDEFINED,
                   1, (JSValueConst *)&ns);
    JS_FreeValue(ctx, ret); /* XXX: what to do if exception ? */
    JS_FreeValue(ctx, ns);
    JS_FreeCString(ctx, basename);
    return JS_UNDEFINED;
 exception:
    err = JS_GetException(ctx);
    ret = JS_Call(ctx, resolving_funcs[1], JS_UNDEFINED,
                   1, (JSValueConst *)&err);
    JS_FreeValue(ctx, ret); /* XXX: what to do if exception ? */
    JS_FreeValue(ctx, err);
    JS_FreeCString(ctx, basename);
    return JS_UNDEFINED;
}

static JSValue js_dynamic_import(JSContext *ctx, JSValueConst specifier)
{
    JSAtom basename;
    JSValue promise, resolving_funcs[2], basename_val;
    JSValueConst args[4];
    basename = JS_GetScriptOrModuleName(ctx, 0);
    if (basename == JS_ATOM_NULL)
        basename_val = JS_NULL;
    else
        basename_val = JS_AtomToValue(ctx, basename);
    JS_FreeAtom(ctx, basename);
    if (JS_IsException(basename_val))
        return basename_val;
    promise = JS_NewPromiseCapability(ctx, resolving_funcs);
    if (JS_IsException(promise)) {
        JS_FreeValue(ctx, basename_val);
        return promise;
    }
    args[0] = resolving_funcs[0];
    args[1] = resolving_funcs[1];
    args[2] = basename_val;
    args[3] = specifier;
    JS_EnqueueJob(ctx, js_dynamic_import_job, 4, args);
    JS_FreeValue(ctx, basename_val);
    JS_FreeValue(ctx, resolving_funcs[0]);
    JS_FreeValue(ctx, resolving_funcs[1]);
    return promise;
}

/* use for strict variable access: test if the variable exists */
static int JS_CheckGlobalVar(JSContext *ctx, JSAtom prop)
{
    JSObject *p;
    JSShapeProperty *prs;
    int ret;
    /* no exotic behavior is possible in global_var_obj */
    p = JS_VALUE_GET_OBJ(ctx->global_var_obj);
    prs = find_own_property1(p, prop);
    if (prs) {
        ret = TRUE;
    } else {
        ret = JS_HasProperty(ctx, ctx->global_obj, prop);
        if (ret < 0)
            return -1;
    }
    return ret;
}

static JSValue JS_GetGlobalVar(JSContext *ctx, JSAtom prop,
                               BOOL throw_ref_error)
{
    JSObject *p;
    JSShapeProperty *prs;
    JSProperty *pr;
    /* no exotic behavior is possible in global_var_obj */
    p = JS_VALUE_GET_OBJ(ctx->global_var_obj);
    prs = find_own_property(&pr, p, prop);
    if (prs) {
        /* XXX: should handle JS_PROP_TMASK properties */
        if (UNLIKELY(JS_IsUninitialized(pr->u.value)))
            return JS_ThrowReferenceErrorUninitialized(ctx, prs->atom);
        return JS_DupValue(ctx, pr->u.value);
    }
    return JS_GetPropertyInternal(ctx, ctx->global_obj, prop,
                                 ctx->global_obj, throw_ref_error);
}

/* flag = 0: normal variable write
   flag = 1: initialize lexical variable
   flag = 2: normal variable write, strict check was done before
*/
static int JS_SetGlobalVar(JSContext *ctx, JSAtom prop, JSValue val,
                           int flag)
{
    JSObject *p;
    JSShapeProperty *prs;
    JSProperty *pr;
    int flags;
    /* no exotic behavior is possible in global_var_obj */
    p = JS_VALUE_GET_OBJ(ctx->global_var_obj);
    prs = find_own_property(&pr, p, prop);
    if (prs) {
        /* XXX: should handle JS_PROP_AUTOINIT properties? */
        if (flag != 1) {
            if (UNLIKELY(JS_IsUninitialized(pr->u.value))) {
                JS_FreeValue(ctx, val);
                JS_ThrowReferenceErrorUninitialized(ctx, prs->atom);
                return -1;
            }
            if (UNLIKELY(!(prs->flags & JS_PROP_WRITABLE))) {
                JS_FreeValue(ctx, val);
                return JS_ThrowTypeErrorReadOnly(ctx, JS_PROP_THROW, prop);
            }
        }
        set_value(ctx, &pr->u.value, val);
        return 0;
    }
    flags = JS_PROP_THROW_STRICT;
    if (is_strict_mode(ctx))
        flags |= JS_PROP_NO_ADD;
    return JS_SetPropertyInternal(ctx, ctx->global_obj, prop, val, flags);
}

/* flags is 0, DEFINE_GLOBAL_LEX_VAR or DEFINE_GLOBAL_FUNC_VAR */
/* XXX: could support exotic global object. */
static int JS_CheckDefineGlobalVar(JSContext *ctx, JSAtom prop, int flags)
{
    JSObject *p;
    JSShapeProperty *prs;
    p = JS_VALUE_GET_OBJ(ctx->global_obj);
    prs = find_own_property1(p, prop);
    /* XXX: should handle JS_PROP_AUTOINIT */
    if (flags & DEFINE_GLOBAL_LEX_VAR) {
        if (prs && !(prs->flags & JS_PROP_CONFIGURABLE))
            goto fail_redeclaration;
    } else {
        if (!prs && !p->extensible)
            goto define_error;
        if (flags & DEFINE_GLOBAL_FUNC_VAR) {
            if (prs) {
                if (!(prs->flags & JS_PROP_CONFIGURABLE) &&
                    ((prs->flags & JS_PROP_TMASK) == JS_PROP_GETSET ||
                     ((prs->flags & (JS_PROP_WRITABLE | JS_PROP_ENUMERABLE)) !=
                      (JS_PROP_WRITABLE | JS_PROP_ENUMERABLE)))) {
                define_error:
                    JS_ThrowTypeErrorAtom(ctx, "cannot define variable '%s'",
                                          prop);
                    return -1;
                }
            }
        }
    }
    /* check if there already is a lexical declaration */
    p = JS_VALUE_GET_OBJ(ctx->global_var_obj);
    prs = find_own_property1(p, prop);
    if (prs) {
    fail_redeclaration:
        JS_ThrowSyntaxErrorVarRedeclaration(ctx, prop);
        return -1;
    }
    return 0;
}

/* def_flags is (0, DEFINE_GLOBAL_LEX_VAR) |
   JS_PROP_CONFIGURABLE | JS_PROP_WRITABLE */
/* XXX: could support exotic global object. */
static int JS_DefineGlobalVar(JSContext *ctx, JSAtom prop, int def_flags)
{
    JSObject *p;
    JSShapeProperty *prs;
    JSProperty *pr;
    JSValue val;
    int flags;
    if (def_flags & DEFINE_GLOBAL_LEX_VAR) {
        p = JS_VALUE_GET_OBJ(ctx->global_var_obj);
        flags = JS_PROP_ENUMERABLE | (def_flags & JS_PROP_WRITABLE) |
            JS_PROP_CONFIGURABLE;
        val = JS_UNINITIALIZED;
    } else {
        p = JS_VALUE_GET_OBJ(ctx->global_obj);
        flags = JS_PROP_ENUMERABLE | JS_PROP_WRITABLE |
            (def_flags & JS_PROP_CONFIGURABLE);
        val = JS_UNDEFINED;
    }
    prs = find_own_property1(p, prop);
    if (prs)
        return 0;
    if (!p->extensible)
        return 0;
    pr = add_property(ctx, p, prop, flags);
    if (UNLIKELY(!pr))
        return -1;
    pr->u.value = val;
    return 0;
}

/* 'def_flags' is 0 or JS_PROP_CONFIGURABLE. */
/* XXX: could support exotic global object. */
static int JS_DefineGlobalFunction(JSContext *ctx, JSAtom prop,
                                   JSValueConst func, int def_flags)
{
    JSObject *p;
    JSShapeProperty *prs;
    int flags;
    p = JS_VALUE_GET_OBJ(ctx->global_obj);
    prs = find_own_property1(p, prop);
    flags = JS_PROP_HAS_VALUE | JS_PROP_THROW;
    if (!prs || (prs->flags & JS_PROP_CONFIGURABLE)) {
        flags |= JS_PROP_ENUMERABLE | JS_PROP_WRITABLE | def_flags |
            JS_PROP_HAS_CONFIGURABLE | JS_PROP_HAS_WRITABLE | JS_PROP_HAS_ENUMERABLE;
    }
    if (JS_DefineProperty(ctx, ctx->global_obj, prop, func,
                          JS_UNDEFINED, JS_UNDEFINED, flags) < 0)
        return -1;
    return 0;
}

static void close_lexical_var(JSContext *ctx, JSStackFrame *sf, int idx, int is_arg)
{
    struct list_head *el, *el1;
    JSVarRef *var_ref;
    int var_idx = idx;
    list_for_each_safe(el, el1, &sf->var_ref_list) {
        var_ref = list_entry(el, JSVarRef, header.link);
        if (var_idx == var_ref->var_idx && var_ref->is_arg == is_arg) {
            var_ref->value = JS_DupValue(ctx, sf->var_buf[var_idx]);
            var_ref->pvalue = &var_ref->value;
            list_del(&var_ref->header.link);
            /* the reference is no longer to a local variable */
            var_ref->is_detached = TRUE;
            add_gc_object(ctx->rt, &var_ref->header, JS_GC_OBJ_TYPE_VAR_REF);
        }
    }
}

/* construct a reference to a global variable */
static int JS_GetGlobalVarRef(JSContext *ctx, JSAtom prop, JSValue *sp)
{
    JSObject *p;
    JSShapeProperty *prs;
    JSProperty *pr;
    /* no exotic behavior is possible in global_var_obj */
    p = JS_VALUE_GET_OBJ(ctx->global_var_obj);
    prs = find_own_property(&pr, p, prop);
    if (prs) {
        /* XXX: should handle JS_PROP_AUTOINIT properties? */
        /* XXX: conformance: do these tests in
           OP_put_var_ref/OP_get_var_ref ? */
        if (UNLIKELY(JS_IsUninitialized(pr->u.value))) {
            JS_ThrowReferenceErrorUninitialized(ctx, prs->atom);
            return -1;
        }
        if (UNLIKELY(!(prs->flags & JS_PROP_WRITABLE))) {
            return JS_ThrowTypeErrorReadOnly(ctx, JS_PROP_THROW, prop);
        }
        sp[0] = JS_DupValue(ctx, ctx->global_var_obj);
    } else {
        int ret;
        ret = JS_HasProperty(ctx, ctx->global_obj, prop);
        if (ret < 0)
            return -1;
        if (ret) {
            sp[0] = JS_DupValue(ctx, ctx->global_obj);
        } else {
            sp[0] = JS_UNDEFINED;
        }
    }
    sp[1] = JS_AtomToValue(ctx, prop);
    return 0;
}

static JSValue build_for_in_iterator(JSContext *ctx, JSValue obj)
{
    JSObject *p;
    JSPropertyEnum *tab_atom;
    int i;
    JSValue enum_obj, obj1;
    JSForInIterator *it;
    uint32_t tag, tab_atom_count;
    tag = JS_VALUE_GET_TAG(obj);
    if (tag != JS_TAG_OBJECT && tag != JS_TAG_NULL && tag != JS_TAG_UNDEFINED) {
        obj = JS_ToObjectFree(ctx, obj);
    }
    it = js_malloc(ctx, sizeof(*it));
    if (!it) {
        JS_FreeValue(ctx, obj);
        return JS_EXCEPTION;
    }
    enum_obj = JS_NewObjectProtoClass(ctx, JS_NULL, JS_CLASS_FOR_IN_ITERATOR);
    if (JS_IsException(enum_obj)) {
        js_free(ctx, it);
        JS_FreeValue(ctx, obj);
        return JS_EXCEPTION;
    }
    it->is_array = FALSE;
    it->obj = obj;
    it->idx = 0;
    p = JS_VALUE_GET_OBJ(enum_obj);
    p->u.for_in_iterator = it;
    if (tag == JS_TAG_NULL || tag == JS_TAG_UNDEFINED)
        return enum_obj;
    /* fast path: assume no enumerable properties in the prototype chain */
    obj1 = JS_DupValue(ctx, obj);
    for(;;) {
        obj1 = JS_GetPrototypeFree(ctx, obj1);
        if (JS_IsNull(obj1))
            break;
        if (JS_IsException(obj1))
            goto fail;
        if (JS_GetOwnPropertyNamesInternal(ctx, &tab_atom, &tab_atom_count,
                                           JS_VALUE_GET_OBJ(obj1),
                                           JS_GPN_STRING_MASK | JS_GPN_ENUM_ONLY)) {
            JS_FreeValue(ctx, obj1);
            goto fail;
        }
        js_free_prop_enum(ctx, tab_atom, tab_atom_count);
        if (tab_atom_count != 0) {
            JS_FreeValue(ctx, obj1);
            goto slow_path;
        }
        /* must check for timeout to avoid infinite loop */
        if (js_poll_interrupts(ctx)) {
            JS_FreeValue(ctx, obj1);
            goto fail;
        }
    }
    p = JS_VALUE_GET_OBJ(obj);
    if (p->fast_array) {
        JSShape *sh;
        JSShapeProperty *prs;
        /* check that there are no enumerable normal fields */
        sh = p->shape;
        for(i = 0, prs = get_shape_prop(sh); i < sh->prop_count; i++, prs++) {
            if (prs->flags & JS_PROP_ENUMERABLE)
                goto normal_case;
        }
        /* for fast arrays, we only store the number of elements */
        it->is_array = TRUE;
        it->array_length = p->u.array.count;
    } else {
    normal_case:
        if (JS_GetOwnPropertyNamesInternal(ctx, &tab_atom, &tab_atom_count, p,
                                   JS_GPN_STRING_MASK | JS_GPN_ENUM_ONLY))
            goto fail;
        for(i = 0; i < tab_atom_count; i++) {
            JS_SetPropertyInternal(ctx, enum_obj, tab_atom[i].atom, JS_NULL, 0);
        }
        js_free_prop_enum(ctx, tab_atom, tab_atom_count);
    }
    return enum_obj;
 slow_path:
    /* non enumerable properties hide the enumerables ones in the
       prototype chain */
    obj1 = JS_DupValue(ctx, obj);
    for(;;) {
        if (JS_GetOwnPropertyNamesInternal(ctx, &tab_atom, &tab_atom_count,
                                           JS_VALUE_GET_OBJ(obj1),
                                           JS_GPN_STRING_MASK | JS_GPN_SET_ENUM)) {
            JS_FreeValue(ctx, obj1);
            goto fail;
        }
        for(i = 0; i < tab_atom_count; i++) {
            JS_DefinePropertyValue(ctx, enum_obj, tab_atom[i].atom, JS_NULL,
                                   (tab_atom[i].is_enumerable ?
                                    JS_PROP_ENUMERABLE : 0));
        }
        js_free_prop_enum(ctx, tab_atom, tab_atom_count);
        obj1 = JS_GetPrototypeFree(ctx, obj1);
        if (JS_IsNull(obj1))
            break;
        if (JS_IsException(obj1))
            goto fail;
        /* must check for timeout to avoid infinite loop */
        if (js_poll_interrupts(ctx)) {
            JS_FreeValue(ctx, obj1);
            goto fail;
        }
    }
    return enum_obj;
 fail:
    JS_FreeValue(ctx, enum_obj);
    return JS_EXCEPTION;
}

/* obj -> enum_obj */
static __exception int js_for_in_start(JSContext *ctx, JSValue *sp)
{
    sp[-1] = build_for_in_iterator(ctx, sp[-1]);
    if (JS_IsException(sp[-1]))
        return -1;
    return 0;
}

/* enum_obj -> enum_obj value done */
static __exception int js_for_in_next(JSContext *ctx, JSValue *sp)
{
    JSValueConst enum_obj;
    JSObject *p;
    JSAtom prop;
    JSForInIterator *it;
    int ret;
    enum_obj = sp[-1];
    /* fail safe */
    if (JS_VALUE_GET_TAG(enum_obj) != JS_TAG_OBJECT)
        goto done;
    p = JS_VALUE_GET_OBJ(enum_obj);
    if (p->class_id != JS_CLASS_FOR_IN_ITERATOR)
        goto done;
    it = p->u.for_in_iterator;
    for(;;) {
        if (it->is_array) {
            if (it->idx >= it->array_length)
                goto done;
            prop = __JS_AtomFromUInt32(it->idx);
            it->idx++;
        } else {
            JSShape *sh = p->shape;
            JSShapeProperty *prs;
            if (it->idx >= sh->prop_count)
                goto done;
            prs = get_shape_prop(sh) + it->idx;
            prop = prs->atom;
            it->idx++;
            if (prop == JS_ATOM_NULL || !(prs->flags & JS_PROP_ENUMERABLE))
                continue;
        }
        /* check if the property was deleted */
        ret = JS_HasProperty(ctx, it->obj, prop);
        if (ret < 0)
            return ret;
        if (ret)
            break;
    }
    /* return the property */
    sp[0] = JS_AtomToValue(ctx, prop);
    sp[1] = JS_FALSE;
    return 0;
 done:
    /* return the end */
    sp[0] = JS_UNDEFINED;
    sp[1] = JS_TRUE;
    return 0;
}

/* enum_rec [objs] -> enum_rec [objs] value done. There are 'offset'
   objs. If 'done' is true or in case of exception, 'enum_rec' is set
   to undefined. If 'done' is true, 'value' is always set to
   undefined. */
static __exception int js_for_of_next(JSContext *ctx, JSValue *sp, int offset)
{
    JSValue value = JS_UNDEFINED;
    int done = 1;
    if (LIKELY(!JS_IsUndefined(sp[offset]))) {
        value = JS_IteratorNext(ctx, sp[offset], sp[offset + 1], 0, NULL, &done);
        if (JS_IsException(value))
            done = -1;
        if (done) {
            /* value is JS_UNDEFINED or JS_EXCEPTION */
            /* replace the iteration object with undefined */
            JS_FreeValue(ctx, sp[offset]);
            sp[offset] = JS_UNDEFINED;
            if (done < 0) {
                return -1;
            } else {
                JS_FreeValue(ctx, value);
                value = JS_UNDEFINED;
            }
        }
    }
    sp[0] = value;
    sp[1] = JS_NewBool(ctx, done);
    return 0;
}

static __exception int js_iterator_get_value_done(JSContext *ctx, JSValue *sp)
{
    JSValue obj, value;
    BOOL done;
    obj = sp[-1];
    if (!JS_IsObject(obj)) {
        JS_ThrowTypeError(ctx, "iterator must return an object");
        return -1;
    }
    value = JS_IteratorGetCompleteValue(ctx, obj, &done);
    if (JS_IsException(value))
        return -1;
    JS_FreeValue(ctx, obj);
    sp[-1] = value;
    sp[0] = JS_NewBool(ctx, done);
    return 0;
}

static JSValue JS_GetPrivateField(JSContext *ctx, JSValueConst obj,
                                  JSValueConst name)
{
    JSObject *p;
    JSShapeProperty *prs;
    JSProperty *pr;
    JSAtom prop;
    if (UNLIKELY(JS_VALUE_GET_TAG(obj) != JS_TAG_OBJECT))
        return JS_ThrowTypeErrorNotAnObject(ctx);
    /* safety check */
    if (UNLIKELY(JS_VALUE_GET_TAG(name) != JS_TAG_SYMBOL))
        return JS_ThrowTypeErrorNotASymbol(ctx);
    prop = js_symbol_to_atom(ctx, (JSValue)name);
    p = JS_VALUE_GET_OBJ(obj);
    prs = find_own_property(&pr, p, prop);
    if (!prs) {
        JS_ThrowTypeErrorPrivateNotFound(ctx, prop);
        return JS_EXCEPTION;
    }
    return JS_DupValue(ctx, pr->u.value);
}

static int JS_SetPrivateField(JSContext *ctx, JSValueConst obj,
                              JSValueConst name, JSValue val)
{
    JSObject *p;
    JSShapeProperty *prs;
    JSProperty *pr;
    JSAtom prop;
    if (UNLIKELY(JS_VALUE_GET_TAG(obj) != JS_TAG_OBJECT)) {
        JS_ThrowTypeErrorNotAnObject(ctx);
        goto fail;
    }
    /* safety check */
    if (UNLIKELY(JS_VALUE_GET_TAG(name) != JS_TAG_SYMBOL)) {
        JS_ThrowTypeErrorNotASymbol(ctx);
        goto fail;
    }
    prop = js_symbol_to_atom(ctx, (JSValue)name);
    p = JS_VALUE_GET_OBJ(obj);
    prs = find_own_property(&pr, p, prop);
    if (!prs) {
        JS_ThrowTypeErrorPrivateNotFound(ctx, prop);
    fail:
        JS_FreeValue(ctx, val);
        return -1;
    }
    set_value(ctx, &pr->u.value, val);
    return 0;
}

/* Private fields can be added even on non extensible objects or
   Proxies */
static int JS_DefinePrivateField(JSContext *ctx, JSValueConst obj,
                                 JSValueConst name, JSValue val)
{
    JSObject *p;
    JSShapeProperty *prs;
    JSProperty *pr;
    JSAtom prop;
    if (UNLIKELY(JS_VALUE_GET_TAG(obj) != JS_TAG_OBJECT)) {
        JS_ThrowTypeErrorNotAnObject(ctx);
        goto fail;
    }
    /* safety check */
    if (UNLIKELY(JS_VALUE_GET_TAG(name) != JS_TAG_SYMBOL)) {
        JS_ThrowTypeErrorNotASymbol(ctx);
        goto fail;
    }
    prop = js_symbol_to_atom(ctx, (JSValue)name);
    p = JS_VALUE_GET_OBJ(obj);
    prs = find_own_property(&pr, p, prop);
    if (prs) {
        JS_ThrowTypeErrorAtom(ctx, "private class field '%s' already exists",
                              prop);
        goto fail;
    }
    pr = add_property(ctx, p, prop, JS_PROP_C_W_E);
    if (UNLIKELY(!pr)) {
    fail:
        JS_FreeValue(ctx, val);
        return -1;
    }
    pr->u.value = val;
    return 0;
}

/* Modify the name of a method according to the atom and
   'flags'. 'flags' is a bitmask of JS_PROP_HAS_GET and
   JS_PROP_HAS_SET. Also set the home object of the method.
   Return < 0 if exception. */
static int js_method_set_properties(JSContext *ctx, JSValueConst func_obj,
                                    JSAtom name, int flags, JSValueConst home_obj)
{
    JSValue name_str;
    name_str = js_get_function_name(ctx, name);
    if (flags & JS_PROP_HAS_GET) {
        name_str = JS_ConcatString3(ctx, "get ", name_str, "");
    } else if (flags & JS_PROP_HAS_SET) {
        name_str = JS_ConcatString3(ctx, "set ", name_str, "");
    }
    if (JS_IsException(name_str))
        return -1;
    if (JS_DefinePropertyValue(ctx, func_obj, JS_ATOM_name, name_str,
                               JS_PROP_CONFIGURABLE) < 0)
        return -1;
    js_method_set_home_object(ctx, func_obj, home_obj);
    return 0;
}

static int js_op_define_class(JSContext *ctx, JSValue *sp,
                              JSAtom class_name, int class_flags,
                              JSVarRef **cur_var_refs,
                              JSStackFrame *sf, BOOL is_computed_name)
{
    JSValue bfunc, parent_class, proto = JS_UNDEFINED;
    JSValue ctor = JS_UNDEFINED, parent_proto = JS_UNDEFINED;
    JSFunctionBytecode *b;
    parent_class = sp[-2];
    bfunc = sp[-1];
    if (class_flags & JS_DEFINE_CLASS_HAS_HERITAGE) {
        if (JS_IsNull(parent_class)) {
            parent_proto = JS_NULL;
            parent_class = JS_DupValue(ctx, ctx->function_proto);
        } else {
            if (!JS_IsConstructor(ctx, parent_class)) {
                JS_ThrowTypeError(ctx, "parent class must be constructor");
                goto fail;
            }
            parent_proto = JS_GetProperty(ctx, parent_class, JS_ATOM_prototype);
            if (JS_IsException(parent_proto))
                goto fail;
            if (!JS_IsNull(parent_proto) && !JS_IsObject(parent_proto)) {
                JS_ThrowTypeError(ctx, "parent prototype must be an object or null");
                goto fail;
            }
        }
    } else {
        /* parent_class is JS_UNDEFINED in this case */
        parent_proto = JS_DupValue(ctx, ctx->class_proto[JS_CLASS_OBJECT]);
        parent_class = JS_DupValue(ctx, ctx->function_proto);
    }
    proto = JS_NewObjectProto(ctx, parent_proto);
    if (JS_IsException(proto))
        goto fail;
    b = JS_VALUE_GET_PTR(bfunc);
    assert(b->func_kind == JS_FUNC_NORMAL);
    ctor = JS_NewObjectProtoClass(ctx, parent_class,
                                  JS_CLASS_BYTECODE_FUNCTION);
    if (JS_IsException(ctor))
        goto fail;
    ctor = js_closure2(ctx, ctor, b, cur_var_refs, sf);
    bfunc = JS_UNDEFINED;
    if (JS_IsException(ctor))
        goto fail;
    js_method_set_home_object(ctx, ctor, proto);
    JS_SetConstructorBit(ctx, ctor, TRUE);
    JS_DefinePropertyValue(ctx, ctor, JS_ATOM_length,
                           JS_NewInt32(ctx, b->defined_arg_count),
                           JS_PROP_CONFIGURABLE);
    if (is_computed_name) {
        if (JS_DefineObjectNameComputed(ctx, ctor, sp[-3],
                                        JS_PROP_CONFIGURABLE) < 0)
            goto fail;
    } else {
        if (JS_DefineObjectName(ctx, ctor, class_name, JS_PROP_CONFIGURABLE) < 0)
            goto fail;
    }
    /* the constructor property must be first. It can be overriden by
       computed property names */
    if (JS_DefinePropertyValue(ctx, proto, JS_ATOM_constructor,
                               JS_DupValue(ctx, ctor),
                               JS_PROP_CONFIGURABLE |
                               JS_PROP_WRITABLE | JS_PROP_THROW) < 0)
        goto fail;
    /* set the prototype property */
    if (JS_DefinePropertyValue(ctx, ctor, JS_ATOM_prototype,
                               JS_DupValue(ctx, proto), JS_PROP_THROW) < 0)
        goto fail;
    set_cycle_flag(ctx, ctor);
    set_cycle_flag(ctx, proto);
    JS_FreeValue(ctx, parent_proto);
    JS_FreeValue(ctx, parent_class);
    sp[-2] = ctor;
    sp[-1] = proto;
    return 0;
 fail:
    JS_FreeValue(ctx, parent_class);
    JS_FreeValue(ctx, parent_proto);
    JS_FreeValue(ctx, bfunc);
    JS_FreeValue(ctx, proto);
    JS_FreeValue(ctx, ctor);
    sp[-2] = JS_UNDEFINED;
    sp[-1] = JS_UNDEFINED;
    return -1;
}

static __exception int js_append_enumerate(JSContext *ctx, JSValue *sp)
{
    JSValue iterator, enumobj, method, value;
    int is_array_iterator;
    JSValue *arrp;
    uint32_t i, count32, pos;
    if (JS_VALUE_GET_TAG(sp[-2]) != JS_TAG_INT) {
        JS_ThrowInternalError(ctx, "invalid index for append");
        return -1;
    }
    pos = JS_VALUE_GET_INT(sp[-2]);
    /* XXX: further optimisations:
       - use ctx->array_proto_values?
       - check if array_iterator_prototype next method is built-in and
         avoid constructing actual iterator object?
       - build this into js_for_of_start and use in all `for (x of o)` loops
     */
    iterator = JS_GetProperty(ctx, sp[-1], JS_ATOM_Symbol_iterator);
    if (JS_IsException(iterator))
        return -1;
    is_array_iterator = JS_IsCFunction(ctx, iterator,
                                       (JSCFunction *)js_create_array_iterator,
                                       JS_ITERATOR_KIND_VALUE);
    JS_FreeValue(ctx, iterator);
    enumobj = JS_GetIterator(ctx, sp[-1], FALSE);
    if (JS_IsException(enumobj))
        return -1;
    method = JS_GetProperty(ctx, enumobj, JS_ATOM_next);
    if (JS_IsException(method)) {
        JS_FreeValue(ctx, enumobj);
        return -1;
    }
    if (is_array_iterator
    &&  JS_IsCFunction(ctx, method, (JSCFunction *)js_array_iterator_next, 0)
    &&  js_get_fast_array(ctx, sp[-1], &arrp, &count32)) {
        uint32_t len;
        if (js_get_length32(ctx, &len, sp[-1]))
            goto exception;
        /* if len > count32, the elements >= count32 might be read in
           the prototypes and might have side effects */
        if (len != count32)
            goto general_case;
        /* Handle fast arrays explicitly */
        for (i = 0; i < count32; i++) {
            if (JS_DefinePropertyValueUint32(ctx, sp[-3], pos++,
                                             JS_DupValue(ctx, arrp[i]), JS_PROP_C_W_E) < 0)
                goto exception;
        }
    } else {
    general_case:
        for (;;) {
            BOOL done;
            value = JS_IteratorNext(ctx, enumobj, method, 0, NULL, &done);
            if (JS_IsException(value))
                goto exception;
            if (done) {
                /* value is JS_UNDEFINED */
                break;
            }
            if (JS_DefinePropertyValueUint32(ctx, sp[-3], pos++, value, JS_PROP_C_W_E) < 0)
                goto exception;
        }
    }
    /* Note: could raise an error if too many elements */
    sp[-2] = JS_NewInt32(ctx, pos);
    JS_FreeValue(ctx, enumobj);
    JS_FreeValue(ctx, method);
    return 0;
exception:
    JS_IteratorClose(ctx, enumobj, TRUE);
    JS_FreeValue(ctx, enumobj);
    JS_FreeValue(ctx, method);
    return -1;
}

static dontdiscard int js_operator_instanceof(JSContext *ctx, JSValue *sp)
{
    JSValue op1, op2;
    BOOL ret;
    op1 = sp[-2];
    op2 = sp[-1];
    ret = JS_IsInstanceOf(ctx, op1, op2);
    if (ret < 0)
        return ret;
    JS_FreeValue(ctx, op1);
    JS_FreeValue(ctx, op2);
    sp[-2] = JS_NewBool(ctx, ret);
    return 0;
}

static dontdiscard int js_operator_typeof(JSContext *ctx, JSValueConst op1)
{
    JSAtom atom;
    uint32_t tag;
    tag = JS_VALUE_GET_NORM_TAG(op1);
    switch(tag) {
#ifdef CONFIG_BIGNUM
    case JS_TAG_BIG_INT:
        atom = JS_ATOM_bigint;
        break;
    case JS_TAG_BIG_FLOAT:
        atom = JS_ATOM_bigfloat;
        break;
    case JS_TAG_BIG_DECIMAL:
        atom = JS_ATOM_bigdecimal;
        break;
#endif
    case JS_TAG_INT:
    case JS_TAG_FLOAT64:
        atom = JS_ATOM_number;
        break;
    case JS_TAG_UNDEFINED:
        atom = JS_ATOM_undefined;
        break;
    case JS_TAG_BOOL:
        atom = JS_ATOM_boolean;
        break;
    case JS_TAG_STRING:
        atom = JS_ATOM_string;
        break;
    case JS_TAG_OBJECT:
        {
            JSObject *p;
            p = JS_VALUE_GET_OBJ(op1);
            if (UNLIKELY(p->is_HTMLDDA))
                atom = JS_ATOM_undefined;
            else if (JS_IsFunction(ctx, op1))
                atom = JS_ATOM_function;
            else
                goto obj_type;
        }
        break;
    case JS_TAG_NULL:
    obj_type:
        atom = JS_ATOM_object;
        break;
    case JS_TAG_SYMBOL:
        atom = JS_ATOM_symbol;
        break;
    default:
        atom = JS_ATOM_unknown;
        break;
    }
    return atom;
}

static dontdiscard int js_operator_delete(JSContext *ctx, JSValue *sp)
{
    JSValue op1, op2;
    JSAtom atom;
    int ret;
    op1 = sp[-2];
    op2 = sp[-1];
    atom = JS_ValueToAtom(ctx, op2);
    if (UNLIKELY(atom == JS_ATOM_NULL))
        return -1;
    ret = JS_DeleteProperty(ctx, op1, atom, JS_PROP_THROW_STRICT);
    JS_FreeAtom(ctx, atom);
    if (UNLIKELY(ret < 0))
        return -1;
    JS_FreeValue(ctx, op1);
    JS_FreeValue(ctx, op2);
    sp[-2] = JS_NewBool(ctx, ret);
    return 0;
}

static dontdiscard int js_has_unscopable(JSContext *ctx, JSValueConst obj, JSAtom atom)
{
    JSValue arr, val;
    int ret;
    arr = JS_GetProperty(ctx, obj, JS_ATOM_Symbol_unscopables);
    if (JS_IsException(arr))
        return -1;
    ret = 0;
    if (JS_IsObject(arr)) {
        val = JS_GetProperty(ctx, arr, atom);
        ret = JS_ToBoolFree(ctx, val);
    }
    JS_FreeValue(ctx, arr);
    return ret;
}

/* Note: it is important that no exception is returned by this function */
static BOOL is_backtrace_needed(JSContext *ctx, JSValueConst obj)
{
    JSObject *p;
    if (JS_VALUE_GET_TAG(obj) != JS_TAG_OBJECT)
        return FALSE;
    p = JS_VALUE_GET_OBJ(obj);
    if (p->class_id != JS_CLASS_ERROR)
        return FALSE;
    if (find_own_property1(p, JS_ATOM_stack))
        return FALSE;
    return TRUE;
}

/* used to avoid catching interrupt exceptions */
static BOOL JS_IsUncatchableError(JSContext *ctx, JSValueConst val)
{
    JSObject *p;
    if (JS_VALUE_GET_TAG(val) != JS_TAG_OBJECT)
        return FALSE;
    p = JS_VALUE_GET_OBJ(val);
    return p->class_id == JS_CLASS_ERROR && p->is_uncatchable_error;
}

static void close_var_refs(JSRuntime *rt, JSStackFrame *sf)
{
    struct list_head *el, *el1;
    JSVarRef *var_ref;
    int var_idx;
    list_for_each_safe(el, el1, &sf->var_ref_list) {
        var_ref = list_entry(el, JSVarRef, header.link);
        var_idx = var_ref->var_idx;
        if (var_ref->is_arg)
            var_ref->value = JS_DupValueRT(rt, sf->arg_buf[var_idx]);
        else
            var_ref->value = JS_DupValueRT(rt, sf->var_buf[var_idx]);
        var_ref->pvalue = &var_ref->value;
        /* the reference is no longer to a local variable */
        var_ref->is_detached = TRUE;
        add_gc_object(rt, &var_ref->header, JS_GC_OBJ_TYPE_VAR_REF);
    }
}

/* argv[] is modified if (flags & JS_CALL_FLAG_COPY_ARGV) = 0. */
static JSValue JS_CallInternal(JSContext *caller_ctx, JSValueConst func_obj,
                               JSValueConst this_obj, JSValueConst new_target,
                               int argc, JSValue *argv, int flags)
{
    JSRuntime *rt = caller_ctx->rt;
    JSContext *ctx;
    JSObject *p;
    JSFunctionBytecode *b;
    JSStackFrame sf_s, *sf = &sf_s;
    const uint8_t *pc;
    int opcode, arg_allocated_size, i;
    JSValue *local_buf, *stack_buf, *var_buf, *arg_buf, *sp, ret_val, *pval;
    JSVarRef **var_refs;
    size_t alloca_size;
#if !DIRECT_DISPATCH
#define SWITCH(pc)      switch (opcode = *pc++)
#define CASE(op)        case op
#define DEFAULT         default
#define BREAK           break
#else
    static const void * const dispatch_table[256] = {
#define DEF(id, size, n_pop, n_push, f) && case_OP_ ## id,
#if SHORT_OPCODES
#define def(id, size, n_pop, n_push, f)
#else
#define def(id, size, n_pop, n_push, f) && case_default,
#endif
#include "third_party/quickjs/quickjs-opcode.inc"
        [ OP_COUNT ... 255 ] = &&case_default
    };
#define SWITCH(pc)      goto *dispatch_table[opcode = *pc++];
#define CASE(op)        case_ ## op
#define DEFAULT         case_default
#define BREAK           SWITCH(pc)
#endif
    if (js_poll_interrupts(caller_ctx))
        return JS_EXCEPTION;
    if (UNLIKELY(JS_VALUE_GET_TAG(func_obj) != JS_TAG_OBJECT)) {
        if (flags & JS_CALL_FLAG_GENERATOR) {
            JSAsyncFunctionState *s = JS_VALUE_GET_PTR(func_obj);
            /* func_obj get contains a pointer to JSFuncAsyncState */
            /* the stack frame is already allocated */
            sf = &s->frame;
            p = JS_VALUE_GET_OBJ(sf->cur_func);
            b = p->u.func.function_bytecode;
            ctx = b->realm;
            var_refs = p->u.func.var_refs;
            local_buf = arg_buf = sf->arg_buf;
            var_buf = sf->var_buf;
            stack_buf = sf->var_buf + b->var_count;
            sp = sf->cur_sp;
            sf->cur_sp = NULL; /* cur_sp is NULL if the function is running */
            pc = sf->cur_pc;
            sf->prev_frame = rt->current_stack_frame;
            rt->current_stack_frame = sf;
            if (s->throw_flag)
                goto exception;
            else
                goto restart;
        } else {
            goto not_a_function;
        }
    }
    p = JS_VALUE_GET_OBJ(func_obj);
    if (UNLIKELY(p->class_id != JS_CLASS_BYTECODE_FUNCTION)) {
        JSClassCall *call_func;
        call_func = rt->class_array[p->class_id].call;
        if (!call_func) {
        not_a_function:
            return JS_ThrowTypeError(caller_ctx, "not a function");
        }
        return call_func(caller_ctx, func_obj, this_obj, argc,
                         (JSValueConst *)argv, flags);
    }
    b = p->u.func.function_bytecode;
    if (UNLIKELY(argc < b->arg_count || (flags & JS_CALL_FLAG_COPY_ARGV))) {
        arg_allocated_size = b->arg_count;
    } else {
        arg_allocated_size = 0;
    }
    alloca_size = sizeof(JSValue) * (arg_allocated_size + b->var_count +
                                     b->stack_size);
    if (js_check_stack_overflow(rt, alloca_size))
        return JS_ThrowStackOverflow(caller_ctx);
    sf->js_mode = b->js_mode;
    arg_buf = argv;
    sf->arg_count = argc;
    sf->cur_func = (JSValue)func_obj;
    init_list_head(&sf->var_ref_list);
    var_refs = p->u.func.var_refs;
    local_buf = gc(malloc(alloca_size));
    if (UNLIKELY(arg_allocated_size)) {
        int n = min_int(argc, b->arg_count);
        arg_buf = local_buf;
        for(i = 0; i < n; i++)
            arg_buf[i] = JS_DupValue(caller_ctx, argv[i]);
        for(; i < b->arg_count; i++)
            arg_buf[i] = JS_UNDEFINED;
        sf->arg_count = b->arg_count;
    }
    var_buf = local_buf + arg_allocated_size;
    sf->var_buf = var_buf;
    sf->arg_buf = arg_buf;
    for(i = 0; i < b->var_count; i++)
        var_buf[i] = JS_UNDEFINED;
    stack_buf = var_buf + b->var_count;
    sp = stack_buf;
    pc = b->byte_code_buf;
    sf->prev_frame = rt->current_stack_frame;
    rt->current_stack_frame = sf;
    ctx = b->realm; /* set the current realm */
 restart:
    for(;;) {
        int call_argc;
        JSValue *call_argv;
        SWITCH(pc) {
        CASE(OP_push_i32):
            *sp++ = JS_NewInt32(ctx, get_u32(pc));
            pc += 4;
            BREAK;
        CASE(OP_push_const):
            *sp++ = JS_DupValue(ctx, b->cpool[get_u32(pc)]);
            pc += 4;
            BREAK;
#if SHORT_OPCODES
        CASE(OP_push_minus1):
        CASE(OP_push_0):
        CASE(OP_push_1):
        CASE(OP_push_2):
        CASE(OP_push_3):
        CASE(OP_push_4):
        CASE(OP_push_5):
        CASE(OP_push_6):
        CASE(OP_push_7):
            *sp++ = JS_NewInt32(ctx, opcode - OP_push_0);
            BREAK;
        CASE(OP_push_i8):
            *sp++ = JS_NewInt32(ctx, get_i8(pc));
            pc += 1;
            BREAK;
        CASE(OP_push_i16):
            *sp++ = JS_NewInt32(ctx, get_i16(pc));
            pc += 2;
            BREAK;
        CASE(OP_push_const8):
            *sp++ = JS_DupValue(ctx, b->cpool[*pc++]);
            BREAK;
        CASE(OP_fclosure8):
            *sp++ = js_closure(ctx, JS_DupValue(ctx, b->cpool[*pc++]), var_refs, sf);
            if (UNLIKELY(JS_IsException(sp[-1])))
                goto exception;
            BREAK;
        CASE(OP_push_empty_string):
            *sp++ = JS_AtomToString(ctx, JS_ATOM_empty_string);
            BREAK;
        CASE(OP_get_length):
            {
                JSValue val;
                val = JS_GetProperty(ctx, sp[-1], JS_ATOM_length);
                if (UNLIKELY(JS_IsException(val)))
                    goto exception;
                JS_FreeValue(ctx, sp[-1]);
                sp[-1] = val;
            }
            BREAK;
#endif
        CASE(OP_push_atom_value):
            *sp++ = JS_AtomToValue(ctx, get_u32(pc));
            pc += 4;
            BREAK;
        CASE(OP_undefined):
            *sp++ = JS_UNDEFINED;
            BREAK;
        CASE(OP_null):
            *sp++ = JS_NULL;
            BREAK;
        CASE(OP_push_this):
            /* OP_push_this is only called at the start of a function */
            {
                JSValue val;
                if (!(b->js_mode & JS_MODE_STRICT)) {
                    uint32_t tag = JS_VALUE_GET_TAG(this_obj);
                    if (LIKELY(tag == JS_TAG_OBJECT))
                        goto normal_this;
                    if (tag == JS_TAG_NULL || tag == JS_TAG_UNDEFINED) {
                        val = JS_DupValue(ctx, ctx->global_obj);
                    } else {
                        val = JS_ToObject(ctx, this_obj);
                        if (JS_IsException(val))
                            goto exception;
                    }
                } else {
                normal_this:
                    val = JS_DupValue(ctx, this_obj);
                }
                *sp++ = val;
            }
            BREAK;
        CASE(OP_push_false):
            *sp++ = JS_FALSE;
            BREAK;
        CASE(OP_push_true):
            *sp++ = JS_TRUE;
            BREAK;
        CASE(OP_object):
            *sp++ = JS_NewObject(ctx);
            if (UNLIKELY(JS_IsException(sp[-1])))
                goto exception;
            BREAK;
        CASE(OP_special_object):
            {
                int arg = *pc++;
                switch(arg) {
                case OP_SPECIAL_OBJECT_ARGUMENTS:
                    *sp++ = js_build_arguments(ctx, argc, (JSValueConst *)argv);
                    if (UNLIKELY(JS_IsException(sp[-1])))
                        goto exception;
                    break;
                case OP_SPECIAL_OBJECT_MAPPED_ARGUMENTS:
                    *sp++ = js_build_mapped_arguments(ctx, argc, (JSValueConst *)argv,
                                                      sf, min_int(argc, b->arg_count));
                    if (UNLIKELY(JS_IsException(sp[-1])))
                        goto exception;
                    break;
                case OP_SPECIAL_OBJECT_THIS_FUNC:
                    *sp++ = JS_DupValue(ctx, sf->cur_func);
                    break;
                case OP_SPECIAL_OBJECT_NEW_TARGET:
                    *sp++ = JS_DupValue(ctx, new_target);
                    break;
                case OP_SPECIAL_OBJECT_HOME_OBJECT:
                    {
                        JSObject *p1;
                        p1 = p->u.func.home_object;
                        if (UNLIKELY(!p1))
                            *sp++ = JS_UNDEFINED;
                        else
                            *sp++ = JS_DupValue(ctx, JS_MKPTR(JS_TAG_OBJECT, p1));
                    }
                    break;
                case OP_SPECIAL_OBJECT_VAR_OBJECT:
                    *sp++ = JS_NewObjectProto(ctx, JS_NULL);
                    if (UNLIKELY(JS_IsException(sp[-1])))
                        goto exception;
                    break;
                case OP_SPECIAL_OBJECT_IMPORT_META:
                    *sp++ = js_import_meta(ctx);
                    if (UNLIKELY(JS_IsException(sp[-1])))
                        goto exception;
                    break;
                default:
                    abort();
                }
            }
            BREAK;
        CASE(OP_rest):
            {
                int first = get_u16(pc);
                pc += 2;
                *sp++ = js_build_rest(ctx, first, argc, (JSValueConst *)argv);
                if (UNLIKELY(JS_IsException(sp[-1])))
                    goto exception;
            }
            BREAK;
        CASE(OP_drop):
            JS_FreeValue(ctx, sp[-1]);
            sp--;
            BREAK;
        CASE(OP_nip):
            JS_FreeValue(ctx, sp[-2]);
            sp[-2] = sp[-1];
            sp--;
            BREAK;
        CASE(OP_nip1): /* a b c -> b c */
            JS_FreeValue(ctx, sp[-3]);
            sp[-3] = sp[-2];
            sp[-2] = sp[-1];
            sp--;
            BREAK;
        CASE(OP_dup):
            sp[0] = JS_DupValue(ctx, sp[-1]);
            sp++;
            BREAK;
        CASE(OP_dup2): /* a b -> a b a b */
            sp[0] = JS_DupValue(ctx, sp[-2]);
            sp[1] = JS_DupValue(ctx, sp[-1]);
            sp += 2;
            BREAK;
        CASE(OP_dup3): /* a b c -> a b c a b c */
            sp[0] = JS_DupValue(ctx, sp[-3]);
            sp[1] = JS_DupValue(ctx, sp[-2]);
            sp[2] = JS_DupValue(ctx, sp[-1]);
            sp += 3;
            BREAK;
        CASE(OP_dup1): /* a b -> a a b */
            sp[0] = sp[-1];
            sp[-1] = JS_DupValue(ctx, sp[-2]);
            sp++;
            BREAK;
        CASE(OP_insert2): /* obj a -> a obj a (dup_x1) */
            sp[0] = sp[-1];
            sp[-1] = sp[-2];
            sp[-2] = JS_DupValue(ctx, sp[0]);
            sp++;
            BREAK;
        CASE(OP_insert3): /* obj prop a -> a obj prop a (dup_x2) */
            sp[0] = sp[-1];
            sp[-1] = sp[-2];
            sp[-2] = sp[-3];
            sp[-3] = JS_DupValue(ctx, sp[0]);
            sp++;
            BREAK;
        CASE(OP_insert4): /* this obj prop a -> a this obj prop a */
            sp[0] = sp[-1];
            sp[-1] = sp[-2];
            sp[-2] = sp[-3];
            sp[-3] = sp[-4];
            sp[-4] = JS_DupValue(ctx, sp[0]);
            sp++;
            BREAK;
        CASE(OP_perm3): /* obj a b -> a obj b (213) */
            {
                JSValue tmp;
                tmp = sp[-2];
                sp[-2] = sp[-3];
                sp[-3] = tmp;
            }
            BREAK;
        CASE(OP_rot3l): /* x a b -> a b x (231) */
            {
                JSValue tmp;
                tmp = sp[-3];
                sp[-3] = sp[-2];
                sp[-2] = sp[-1];
                sp[-1] = tmp;
            }
            BREAK;
        CASE(OP_rot4l): /* x a b c -> a b c x */
            {
                JSValue tmp;
                tmp = sp[-4];
                sp[-4] = sp[-3];
                sp[-3] = sp[-2];
                sp[-2] = sp[-1];
                sp[-1] = tmp;
            }
            BREAK;
        CASE(OP_rot5l): /* x a b c d -> a b c d x */
            {
                JSValue tmp;
                tmp = sp[-5];
                sp[-5] = sp[-4];
                sp[-4] = sp[-3];
                sp[-3] = sp[-2];
                sp[-2] = sp[-1];
                sp[-1] = tmp;
            }
            BREAK;
        CASE(OP_rot3r): /* a b x -> x a b (312) */
            {
                JSValue tmp;
                tmp = sp[-1];
                sp[-1] = sp[-2];
                sp[-2] = sp[-3];
                sp[-3] = tmp;
            }
            BREAK;
        CASE(OP_perm4): /* obj prop a b -> a obj prop b */
            {
                JSValue tmp;
                tmp = sp[-2];
                sp[-2] = sp[-3];
                sp[-3] = sp[-4];
                sp[-4] = tmp;
            }
            BREAK;
        CASE(OP_perm5): /* this obj prop a b -> a this obj prop b */
            {
                JSValue tmp;
                tmp = sp[-2];
                sp[-2] = sp[-3];
                sp[-3] = sp[-4];
                sp[-4] = sp[-5];
                sp[-5] = tmp;
            }
            BREAK;
        CASE(OP_swap): /* a b -> b a */
            {
                JSValue tmp;
                tmp = sp[-2];
                sp[-2] = sp[-1];
                sp[-1] = tmp;
            }
            BREAK;
        CASE(OP_swap2): /* a b c d -> c d a b */
            {
                JSValue tmp1, tmp2;
                tmp1 = sp[-4];
                tmp2 = sp[-3];
                sp[-4] = sp[-2];
                sp[-3] = sp[-1];
                sp[-2] = tmp1;
                sp[-1] = tmp2;
            }
            BREAK;
        CASE(OP_fclosure):
            {
                JSValue bfunc = JS_DupValue(ctx, b->cpool[get_u32(pc)]);
                pc += 4;
                *sp++ = js_closure(ctx, bfunc, var_refs, sf);
                if (UNLIKELY(JS_IsException(sp[-1])))
                    goto exception;
            }
            BREAK;
#if SHORT_OPCODES
        CASE(OP_call0):
        CASE(OP_call1):
        CASE(OP_call2):
        CASE(OP_call3):
            call_argc = opcode - OP_call0;
            goto has_call_argc;
#endif
        CASE(OP_call):
        CASE(OP_tail_call):
            {
                call_argc = get_u16(pc);
                pc += 2;
                goto has_call_argc;
            has_call_argc:
                call_argv = sp - call_argc;
                sf->cur_pc = pc;
                ret_val = JS_CallInternal(ctx, call_argv[-1], JS_UNDEFINED,
                                          JS_UNDEFINED, call_argc, call_argv, 0);
                if (UNLIKELY(JS_IsException(ret_val)))
                    goto exception;
                if (opcode == OP_tail_call)
                    goto done;
                for(i = -1; i < call_argc; i++)
                    JS_FreeValue(ctx, call_argv[i]);
                sp -= call_argc + 1;
                *sp++ = ret_val;
            }
            BREAK;
        CASE(OP_call_constructor):
            {
                call_argc = get_u16(pc);
                pc += 2;
                call_argv = sp - call_argc;
                sf->cur_pc = pc;
                ret_val = JS_CallConstructorInternal(ctx, call_argv[-2],
                                                     call_argv[-1],
                                                     call_argc, call_argv, 0);
                if (UNLIKELY(JS_IsException(ret_val)))
                    goto exception;
                for(i = -2; i < call_argc; i++)
                    JS_FreeValue(ctx, call_argv[i]);
                sp -= call_argc + 2;
                *sp++ = ret_val;
            }
            BREAK;
        CASE(OP_call_method):
        CASE(OP_tail_call_method):
            {
                call_argc = get_u16(pc);
                pc += 2;
                call_argv = sp - call_argc;
                sf->cur_pc = pc;
                ret_val = JS_CallInternal(ctx, call_argv[-1], call_argv[-2],
                                          JS_UNDEFINED, call_argc, call_argv, 0);
                if (UNLIKELY(JS_IsException(ret_val)))
                    goto exception;
                if (opcode == OP_tail_call_method)
                    goto done;
                for(i = -2; i < call_argc; i++)
                    JS_FreeValue(ctx, call_argv[i]);
                sp -= call_argc + 2;
                *sp++ = ret_val;
            }
            BREAK;
        CASE(OP_array_from):
            {
                int i, ret;
                call_argc = get_u16(pc);
                pc += 2;
                ret_val = JS_NewArray(ctx);
                if (UNLIKELY(JS_IsException(ret_val)))
                    goto exception;
                call_argv = sp - call_argc;
                for(i = 0; i < call_argc; i++) {
                    ret = JS_DefinePropertyValue(ctx, ret_val, __JS_AtomFromUInt32(i), call_argv[i],
                                                 JS_PROP_C_W_E | JS_PROP_THROW);
                    call_argv[i] = JS_UNDEFINED;
                    if (ret < 0) {
                        JS_FreeValue(ctx, ret_val);
                        goto exception;
                    }
                }
                sp -= call_argc;
                *sp++ = ret_val;
            }
            BREAK;
        CASE(OP_apply):
            {
                int magic;
                magic = get_u16(pc);
                pc += 2;
                ret_val = js_function_apply(ctx, sp[-3], 2, (JSValueConst *)&sp[-2], magic);
                if (UNLIKELY(JS_IsException(ret_val)))
                    goto exception;
                JS_FreeValue(ctx, sp[-3]);
                JS_FreeValue(ctx, sp[-2]);
                JS_FreeValue(ctx, sp[-1]);
                sp -= 3;
                *sp++ = ret_val;
            }
            BREAK;
        CASE(OP_return):
            ret_val = *--sp;
            goto done;
        CASE(OP_return_undef):
            ret_val = JS_UNDEFINED;
            goto done;
        CASE(OP_check_ctor_return):
            /* return TRUE if 'this' should be returned */
            if (!JS_IsObject(sp[-1])) {
                if (!JS_IsUndefined(sp[-1])) {
                    JS_ThrowTypeError(caller_ctx, "derived class constructor must return an object or undefined");
                    goto exception;
                }
                sp[0] = JS_TRUE;
            } else {
                sp[0] = JS_FALSE;
            }
            sp++;
            BREAK;
        CASE(OP_check_ctor):
            if (JS_IsUndefined(new_target)) {
                JS_ThrowTypeError(ctx, "class constructors must be invoked with 'new'");
                goto exception;
            }
            BREAK;
        CASE(OP_check_brand):
            if (JS_CheckBrand(ctx, sp[-2], sp[-1]) < 0)
                goto exception;
            BREAK;
        CASE(OP_add_brand):
            if (JS_AddBrand(ctx, sp[-2], sp[-1]) < 0)
                goto exception;
            JS_FreeValue(ctx, sp[-2]);
            JS_FreeValue(ctx, sp[-1]);
            sp -= 2;
            BREAK;
        CASE(OP_throw):
            JS_Throw(ctx, *--sp);
            goto exception;
        CASE(OP_throw_error):
            {
                JSAtom atom;
                int type;
                atom = get_u32(pc);
                type = pc[4];
                pc += 5;
                if (type == JS_THROW_VAR_RO)
                    JS_ThrowTypeErrorReadOnly(ctx, JS_PROP_THROW, atom);
                else
                if (type == JS_THROW_VAR_REDECL)
                    JS_ThrowSyntaxErrorVarRedeclaration(ctx, atom);
                else
                if (type == JS_THROW_VAR_UNINITIALIZED)
                    JS_ThrowReferenceErrorUninitialized(ctx, atom);
                else
                if (type == JS_THROW_ERROR_DELETE_SUPER)
                    JS_ThrowReferenceError(ctx, "unsupported reference to 'super'");
                else
                if (type == JS_THROW_ERROR_ITERATOR_THROW)
                    JS_ThrowTypeError(ctx, "iterator does not have a throw method");
                else
                    JS_ThrowInternalError(ctx, "invalid throw var type %d", type);
            }
            goto exception;
        CASE(OP_eval):
            {
                JSValueConst obj;
                int scope_idx;
                call_argc = get_u16(pc);
                scope_idx = get_u16(pc + 2) - 1;
                pc += 4;
                call_argv = sp - call_argc;
                sf->cur_pc = pc;
                if (js_same_value(ctx, call_argv[-1], ctx->eval_obj)) {
                    if (call_argc >= 1)
                        obj = call_argv[0];
                    else
                        obj = JS_UNDEFINED;
                    ret_val = JS_EvalObject(ctx, JS_UNDEFINED, obj,
                                            JS_EVAL_TYPE_DIRECT, scope_idx);
                } else {
                    ret_val = JS_CallInternal(ctx, call_argv[-1], JS_UNDEFINED,
                                              JS_UNDEFINED, call_argc, call_argv, 0);
                }
                if (UNLIKELY(JS_IsException(ret_val)))
                    goto exception;
                for(i = -1; i < call_argc; i++)
                    JS_FreeValue(ctx, call_argv[i]);
                sp -= call_argc + 1;
                *sp++ = ret_val;
            }
            BREAK;
            /* could merge with OP_apply */
        CASE(OP_apply_eval):
            {
                int scope_idx;
                uint32_t len;
                JSValue *tab;
                JSValueConst obj;
                scope_idx = get_u16(pc) - 1;
                pc += 2;
                tab = build_arg_list(ctx, &len, sp[-1]);
                if (!tab)
                    goto exception;
                if (js_same_value(ctx, sp[-2], ctx->eval_obj)) {
                    if (len >= 1)
                        obj = tab[0];
                    else
                        obj = JS_UNDEFINED;
                    ret_val = JS_EvalObject(ctx, JS_UNDEFINED, obj,
                                            JS_EVAL_TYPE_DIRECT, scope_idx);
                } else {
                    ret_val = JS_Call(ctx, sp[-2], JS_UNDEFINED, len,
                                      (JSValueConst *)tab);
                }
                free_arg_list(ctx, tab, len);
                if (UNLIKELY(JS_IsException(ret_val)))
                    goto exception;
                JS_FreeValue(ctx, sp[-2]);
                JS_FreeValue(ctx, sp[-1]);
                sp -= 2;
                *sp++ = ret_val;
            }
            BREAK;
        CASE(OP_regexp):
            {
                sp[-2] = js_regexp_constructor_internal(ctx, JS_UNDEFINED,
                                                        sp[-2], sp[-1]);
                sp--;
            }
            BREAK;
        CASE(OP_get_super):
            {
                JSValue proto;
                proto = JS_GetPrototype(ctx, sp[-1]);
                if (JS_IsException(proto))
                    goto exception;
                JS_FreeValue(ctx, sp[-1]);
                sp[-1] = proto;
            }
            BREAK;
        CASE(OP_import):
            {
                JSValue val;
                val = js_dynamic_import(ctx, sp[-1]);
                if (JS_IsException(val))
                    goto exception;
                JS_FreeValue(ctx, sp[-1]);
                sp[-1] = val;
            }
            BREAK;
        CASE(OP_check_var):
            {
                int ret;
                JSAtom atom;
                atom = get_u32(pc);
                pc += 4;
                ret = JS_CheckGlobalVar(ctx, atom);
                if (ret < 0)
                    goto exception;
                *sp++ = JS_NewBool(ctx, ret);
            }
            BREAK;
        CASE(OP_get_var_undef):
        CASE(OP_get_var):
            {
                JSValue val;
                JSAtom atom;
                atom = get_u32(pc);
                pc += 4;
                val = JS_GetGlobalVar(ctx, atom, opcode - OP_get_var_undef);
                if (UNLIKELY(JS_IsException(val)))
                    goto exception;
                *sp++ = val;
            }
            BREAK;
        CASE(OP_put_var):
        CASE(OP_put_var_init):
            {
                int ret;
                JSAtom atom;
                atom = get_u32(pc);
                pc += 4;
                ret = JS_SetGlobalVar(ctx, atom, sp[-1], opcode - OP_put_var);
                sp--;
                if (UNLIKELY(ret < 0))
                    goto exception;
            }
            BREAK;
        CASE(OP_put_var_strict):
            {
                int ret;
                JSAtom atom;
                atom = get_u32(pc);
                pc += 4;
                /* sp[-2] is JS_TRUE or JS_FALSE */
                if (UNLIKELY(!JS_VALUE_GET_INT(sp[-2]))) {
                    JS_ThrowReferenceErrorNotDefined(ctx, atom);
                    goto exception;
                }
                ret = JS_SetGlobalVar(ctx, atom, sp[-1], 2);
                sp -= 2;
                if (UNLIKELY(ret < 0))
                    goto exception;
            }
            BREAK;
        CASE(OP_check_define_var):
            {
                JSAtom atom;
                int flags;
                atom = get_u32(pc);
                flags = pc[4];
                pc += 5;
                if (JS_CheckDefineGlobalVar(ctx, atom, flags))
                    goto exception;
            }
            BREAK;
        CASE(OP_define_var):
            {
                JSAtom atom;
                int flags;
                atom = get_u32(pc);
                flags = pc[4];
                pc += 5;
                if (JS_DefineGlobalVar(ctx, atom, flags))
                    goto exception;
            }
            BREAK;
        CASE(OP_define_func):
            {
                JSAtom atom;
                int flags;
                atom = get_u32(pc);
                flags = pc[4];
                pc += 5;
                if (JS_DefineGlobalFunction(ctx, atom, sp[-1], flags))
                    goto exception;
                JS_FreeValue(ctx, sp[-1]);
                sp--;
            }
            BREAK;
        CASE(OP_get_loc):
            {
                int idx;
                idx = get_u16(pc);
                pc += 2;
                sp[0] = JS_DupValue(ctx, var_buf[idx]);
                sp++;
            }
            BREAK;
        CASE(OP_put_loc):
            {
                int idx;
                idx = get_u16(pc);
                pc += 2;
                set_value(ctx, &var_buf[idx], sp[-1]);
                sp--;
            }
            BREAK;
        CASE(OP_set_loc):
            {
                int idx;
                idx = get_u16(pc);
                pc += 2;
                set_value(ctx, &var_buf[idx], JS_DupValue(ctx, sp[-1]));
            }
            BREAK;
        CASE(OP_get_arg):
            {
                int idx;
                idx = get_u16(pc);
                pc += 2;
                sp[0] = JS_DupValue(ctx, arg_buf[idx]);
                sp++;
            }
            BREAK;
        CASE(OP_put_arg):
            {
                int idx;
                idx = get_u16(pc);
                pc += 2;
                set_value(ctx, &arg_buf[idx], sp[-1]);
                sp--;
            }
            BREAK;
        CASE(OP_set_arg):
            {
                int idx;
                idx = get_u16(pc);
                pc += 2;
                set_value(ctx, &arg_buf[idx], JS_DupValue(ctx, sp[-1]));
            }
            BREAK;
#if SHORT_OPCODES
        CASE(OP_get_loc8): *sp++ = JS_DupValue(ctx, var_buf[*pc++]); BREAK;
        CASE(OP_put_loc8): set_value(ctx, &var_buf[*pc++], *--sp); BREAK;
        CASE(OP_set_loc8): set_value(ctx, &var_buf[*pc++], JS_DupValue(ctx, sp[-1])); BREAK;
        CASE(OP_get_loc0): *sp++ = JS_DupValue(ctx, var_buf[0]); BREAK;
        CASE(OP_get_loc1): *sp++ = JS_DupValue(ctx, var_buf[1]); BREAK;
        CASE(OP_get_loc2): *sp++ = JS_DupValue(ctx, var_buf[2]); BREAK;
        CASE(OP_get_loc3): *sp++ = JS_DupValue(ctx, var_buf[3]); BREAK;
        CASE(OP_put_loc0): set_value(ctx, &var_buf[0], *--sp); BREAK;
        CASE(OP_put_loc1): set_value(ctx, &var_buf[1], *--sp); BREAK;
        CASE(OP_put_loc2): set_value(ctx, &var_buf[2], *--sp); BREAK;
        CASE(OP_put_loc3): set_value(ctx, &var_buf[3], *--sp); BREAK;
        CASE(OP_set_loc0): set_value(ctx, &var_buf[0], JS_DupValue(ctx, sp[-1])); BREAK;
        CASE(OP_set_loc1): set_value(ctx, &var_buf[1], JS_DupValue(ctx, sp[-1])); BREAK;
        CASE(OP_set_loc2): set_value(ctx, &var_buf[2], JS_DupValue(ctx, sp[-1])); BREAK;
        CASE(OP_set_loc3): set_value(ctx, &var_buf[3], JS_DupValue(ctx, sp[-1])); BREAK;
        CASE(OP_get_arg0): *sp++ = JS_DupValue(ctx, arg_buf[0]); BREAK;
        CASE(OP_get_arg1): *sp++ = JS_DupValue(ctx, arg_buf[1]); BREAK;
        CASE(OP_get_arg2): *sp++ = JS_DupValue(ctx, arg_buf[2]); BREAK;
        CASE(OP_get_arg3): *sp++ = JS_DupValue(ctx, arg_buf[3]); BREAK;
        CASE(OP_put_arg0): set_value(ctx, &arg_buf[0], *--sp); BREAK;
        CASE(OP_put_arg1): set_value(ctx, &arg_buf[1], *--sp); BREAK;
        CASE(OP_put_arg2): set_value(ctx, &arg_buf[2], *--sp); BREAK;
        CASE(OP_put_arg3): set_value(ctx, &arg_buf[3], *--sp); BREAK;
        CASE(OP_set_arg0): set_value(ctx, &arg_buf[0], JS_DupValue(ctx, sp[-1])); BREAK;
        CASE(OP_set_arg1): set_value(ctx, &arg_buf[1], JS_DupValue(ctx, sp[-1])); BREAK;
        CASE(OP_set_arg2): set_value(ctx, &arg_buf[2], JS_DupValue(ctx, sp[-1])); BREAK;
        CASE(OP_set_arg3): set_value(ctx, &arg_buf[3], JS_DupValue(ctx, sp[-1])); BREAK;
        CASE(OP_get_var_ref0): *sp++ = JS_DupValue(ctx, *var_refs[0]->pvalue); BREAK;
        CASE(OP_get_var_ref1): *sp++ = JS_DupValue(ctx, *var_refs[1]->pvalue); BREAK;
        CASE(OP_get_var_ref2): *sp++ = JS_DupValue(ctx, *var_refs[2]->pvalue); BREAK;
        CASE(OP_get_var_ref3): *sp++ = JS_DupValue(ctx, *var_refs[3]->pvalue); BREAK;
        CASE(OP_put_var_ref0): set_value(ctx, var_refs[0]->pvalue, *--sp); BREAK;
        CASE(OP_put_var_ref1): set_value(ctx, var_refs[1]->pvalue, *--sp); BREAK;
        CASE(OP_put_var_ref2): set_value(ctx, var_refs[2]->pvalue, *--sp); BREAK;
        CASE(OP_put_var_ref3): set_value(ctx, var_refs[3]->pvalue, *--sp); BREAK;
        CASE(OP_set_var_ref0): set_value(ctx, var_refs[0]->pvalue, JS_DupValue(ctx, sp[-1])); BREAK;
        CASE(OP_set_var_ref1): set_value(ctx, var_refs[1]->pvalue, JS_DupValue(ctx, sp[-1])); BREAK;
        CASE(OP_set_var_ref2): set_value(ctx, var_refs[2]->pvalue, JS_DupValue(ctx, sp[-1])); BREAK;
        CASE(OP_set_var_ref3): set_value(ctx, var_refs[3]->pvalue, JS_DupValue(ctx, sp[-1])); BREAK;
#endif
        CASE(OP_get_var_ref):
            {
                int idx;
                JSValue val;
                idx = get_u16(pc);
                pc += 2;
                val = *var_refs[idx]->pvalue;
                sp[0] = JS_DupValue(ctx, val);
                sp++;
            }
            BREAK;
        CASE(OP_put_var_ref):
            {
                int idx;
                idx = get_u16(pc);
                pc += 2;
                set_value(ctx, var_refs[idx]->pvalue, sp[-1]);
                sp--;
            }
            BREAK;
        CASE(OP_set_var_ref):
            {
                int idx;
                idx = get_u16(pc);
                pc += 2;
                set_value(ctx, var_refs[idx]->pvalue, JS_DupValue(ctx, sp[-1]));
            }
            BREAK;
        CASE(OP_get_var_ref_check):
            {
                int idx;
                JSValue val;
                idx = get_u16(pc);
                pc += 2;
                val = *var_refs[idx]->pvalue;
                if (UNLIKELY(JS_IsUninitialized(val))) {
                    JS_ThrowReferenceErrorUninitialized2(ctx, b, idx, TRUE);
                    goto exception;
                }
                sp[0] = JS_DupValue(ctx, val);
                sp++;
            }
            BREAK;
        CASE(OP_put_var_ref_check):
            {
                int idx;
                idx = get_u16(pc);
                pc += 2;
                if (UNLIKELY(JS_IsUninitialized(*var_refs[idx]->pvalue))) {
                    JS_ThrowReferenceErrorUninitialized2(ctx, b, idx, TRUE);
                    goto exception;
                }
                set_value(ctx, var_refs[idx]->pvalue, sp[-1]);
                sp--;
            }
            BREAK;
        CASE(OP_put_var_ref_check_init):
            {
                int idx;
                idx = get_u16(pc);
                pc += 2;
                if (UNLIKELY(!JS_IsUninitialized(*var_refs[idx]->pvalue))) {
                    JS_ThrowReferenceErrorUninitialized2(ctx, b, idx, TRUE);
                    goto exception;
                }
                set_value(ctx, var_refs[idx]->pvalue, sp[-1]);
                sp--;
            }
            BREAK;
        CASE(OP_set_loc_uninitialized):
            {
                int idx;
                idx = get_u16(pc);
                pc += 2;
                set_value(ctx, &var_buf[idx], JS_UNINITIALIZED);
            }
            BREAK;
        CASE(OP_get_loc_check):
            {
                int idx;
                idx = get_u16(pc);
                pc += 2;
                if (UNLIKELY(JS_IsUninitialized(var_buf[idx]))) {
                    JS_ThrowReferenceErrorUninitialized2(ctx, b, idx, FALSE);
                    goto exception;
                }
                sp[0] = JS_DupValue(ctx, var_buf[idx]);
                sp++;
            }
            BREAK;
        CASE(OP_put_loc_check):
            {
                int idx;
                idx = get_u16(pc);
                pc += 2;
                if (UNLIKELY(JS_IsUninitialized(var_buf[idx]))) {
                    JS_ThrowReferenceErrorUninitialized2(ctx, b, idx, FALSE);
                    goto exception;
                }
                set_value(ctx, &var_buf[idx], sp[-1]);
                sp--;
            }
            BREAK;
        CASE(OP_put_loc_check_init):
            {
                int idx;
                idx = get_u16(pc);
                pc += 2;
                if (UNLIKELY(!JS_IsUninitialized(var_buf[idx]))) {
                    JS_ThrowReferenceError(ctx, "'this' can be initialized only once");
                    goto exception;
                }
                set_value(ctx, &var_buf[idx], sp[-1]);
                sp--;
            }
            BREAK;
        CASE(OP_close_loc):
            {
                int idx;
                idx = get_u16(pc);
                pc += 2;
                close_lexical_var(ctx, sf, idx, FALSE);
            }
            BREAK;
        CASE(OP_make_loc_ref):
        CASE(OP_make_arg_ref):
        CASE(OP_make_var_ref_ref):
            {
                JSVarRef *var_ref;
                JSProperty *pr;
                JSAtom atom;
                int idx;
                atom = get_u32(pc);
                idx = get_u16(pc + 4);
                pc += 6;
                *sp++ = JS_NewObjectProto(ctx, JS_NULL);
                if (UNLIKELY(JS_IsException(sp[-1])))
                    goto exception;
                if (opcode == OP_make_var_ref_ref) {
                    var_ref = var_refs[idx];
                    var_ref->header.ref_count++;
                } else {
                    var_ref = get_var_ref(ctx, sf, idx, opcode == OP_make_arg_ref);
                    if (!var_ref)
                        goto exception;
                }
                pr = add_property(ctx, JS_VALUE_GET_OBJ(sp[-1]), atom,
                                  JS_PROP_WRITABLE | JS_PROP_VARREF);
                if (!pr) {
                    free_var_ref(rt, var_ref);
                    goto exception;
                }
                pr->u.var_ref = var_ref;
                *sp++ = JS_AtomToValue(ctx, atom);
            }
            BREAK;
        CASE(OP_make_var_ref):
            {
                JSAtom atom;
                atom = get_u32(pc);
                pc += 4;
                if (JS_GetGlobalVarRef(ctx, atom, sp))
                    goto exception;
                sp += 2;
            }
            BREAK;
        CASE(OP_goto):
            pc += (int32_t)get_u32(pc);
            if (UNLIKELY(js_poll_interrupts(ctx)))
                goto exception;
            BREAK;
#if SHORT_OPCODES
        CASE(OP_goto16):
            pc += (int16_t)get_u16(pc);
            if (UNLIKELY(js_poll_interrupts(ctx)))
                goto exception;
            BREAK;
        CASE(OP_goto8):
            pc += (int8_t)pc[0];
            if (UNLIKELY(js_poll_interrupts(ctx)))
                goto exception;
            BREAK;
#endif
        CASE(OP_if_true):
            {
                int res;
                JSValue op1;
                op1 = sp[-1];
                pc += 4;
                if ((uint32_t)JS_VALUE_GET_TAG(op1) <= JS_TAG_UNDEFINED) {
                    res = JS_VALUE_GET_INT(op1);
                } else {
                    res = JS_ToBoolFree(ctx, op1);
                }
                sp--;
                if (res) {
                    pc += (int32_t)get_u32(pc - 4) - 4;
                }
                if (UNLIKELY(js_poll_interrupts(ctx)))
                    goto exception;
            }
            BREAK;
        CASE(OP_if_false):
            {
                int res;
                JSValue op1;
                op1 = sp[-1];
                pc += 4;
                if ((uint32_t)JS_VALUE_GET_TAG(op1) <= JS_TAG_UNDEFINED) {
                    res = JS_VALUE_GET_INT(op1);
                } else {
                    res = JS_ToBoolFree(ctx, op1);
                }
                sp--;
                if (!res) {
                    pc += (int32_t)get_u32(pc - 4) - 4;
                }
                if (UNLIKELY(js_poll_interrupts(ctx)))
                    goto exception;
            }
            BREAK;
#if SHORT_OPCODES
        CASE(OP_if_true8):
            {
                int res;
                JSValue op1;
                op1 = sp[-1];
                pc += 1;
                if ((uint32_t)JS_VALUE_GET_TAG(op1) <= JS_TAG_UNDEFINED) {
                    res = JS_VALUE_GET_INT(op1);
                } else {
                    res = JS_ToBoolFree(ctx, op1);
                }
                sp--;
                if (res) {
                    pc += (int8_t)pc[-1] - 1;
                }
                if (UNLIKELY(js_poll_interrupts(ctx)))
                    goto exception;
            }
            BREAK;
        CASE(OP_if_false8):
            {
                int res;
                JSValue op1;
                op1 = sp[-1];
                pc += 1;
                if ((uint32_t)JS_VALUE_GET_TAG(op1) <= JS_TAG_UNDEFINED) {
                    res = JS_VALUE_GET_INT(op1);
                } else {
                    res = JS_ToBoolFree(ctx, op1);
                }
                sp--;
                if (!res) {
                    pc += (int8_t)pc[-1] - 1;
                }
                if (UNLIKELY(js_poll_interrupts(ctx)))
                    goto exception;
            }
            BREAK;
#endif
        CASE(OP_catch):
            {
                int32_t diff;
                diff = get_u32(pc);
                sp[0] = JS_NewCatchOffset(ctx, pc + diff - b->byte_code_buf);
                sp++;
                pc += 4;
            }
            BREAK;
        CASE(OP_gosub):
            {
                int32_t diff;
                diff = get_u32(pc);
                /* XXX: should have a different tag to avoid security flaw */
                sp[0] = JS_NewInt32(ctx, pc + 4 - b->byte_code_buf);
                sp++;
                pc += diff;
            }
            BREAK;
        CASE(OP_ret):
            {
                JSValue op1;
                uint32_t pos;
                op1 = sp[-1];
                if (UNLIKELY(JS_VALUE_GET_TAG(op1) != JS_TAG_INT))
                    goto ret_fail;
                pos = JS_VALUE_GET_INT(op1);
                if (UNLIKELY(pos >= b->byte_code_len)) {
                ret_fail:
                    JS_ThrowInternalError(ctx, "invalid ret value");
                    goto exception;
                }
                sp--;
                pc = b->byte_code_buf + pos;
            }
            BREAK;
        CASE(OP_for_in_start):
            if (js_for_in_start(ctx, sp))
                goto exception;
            BREAK;
        CASE(OP_for_in_next):
            if (js_for_in_next(ctx, sp))
                goto exception;
            sp += 2;
            BREAK;
        CASE(OP_for_of_start):
            if (js_for_of_start(ctx, sp, FALSE))
                goto exception;
            sp += 1;
            *sp++ = JS_NewCatchOffset(ctx, 0);
            BREAK;
        CASE(OP_for_of_next):
            {
                int offset = -3 - pc[0];
                pc += 1;
                if (js_for_of_next(ctx, sp, offset))
                    goto exception;
                sp += 2;
            }
            BREAK;
        CASE(OP_for_await_of_start):
            if (js_for_of_start(ctx, sp, TRUE))
                goto exception;
            sp += 1;
            *sp++ = JS_NewCatchOffset(ctx, 0);
            BREAK;
        CASE(OP_iterator_get_value_done):
            if (js_iterator_get_value_done(ctx, sp))
                goto exception;
            sp += 1;
            BREAK;
        CASE(OP_iterator_check_object):
            if (UNLIKELY(!JS_IsObject(sp[-1]))) {
                JS_ThrowTypeError(ctx, "iterator must return an object");
                goto exception;
            }
            BREAK;
        CASE(OP_iterator_close):
            /* iter_obj next catch_offset -> */
            sp--; /* drop the catch offset to avoid getting caught by exception */
            JS_FreeValue(ctx, sp[-1]); /* drop the next method */
            sp--;
            if (!JS_IsUndefined(sp[-1])) {
                if (JS_IteratorClose(ctx, sp[-1], FALSE))
                    goto exception;
                JS_FreeValue(ctx, sp[-1]);
            }
            sp--;
            BREAK;
        CASE(OP_iterator_close_return):
            {
                JSValue ret_val;
                /* iter_obj next catch_offset ... ret_val ->
                   ret_eval iter_obj next catch_offset */
                ret_val = *--sp;
                while (sp > stack_buf &&
                       JS_VALUE_GET_TAG(sp[-1]) != JS_TAG_CATCH_OFFSET) {
                    JS_FreeValue(ctx, *--sp);
                }
                if (UNLIKELY(sp < stack_buf + 3)) {
                    JS_ThrowInternalError(ctx, "iterator_close_return");
                    JS_FreeValue(ctx, ret_val);
                    goto exception;
                }
                sp[0] = sp[-1];
                sp[-1] = sp[-2];
                sp[-2] = sp[-3];
                sp[-3] = ret_val;
                sp++;
            }
            BREAK;
        CASE(OP_iterator_next):
            /* stack: iter_obj next catch_offset val */
            {
                JSValue ret;
                ret = JS_Call(ctx, sp[-3], sp[-4],
                              1, (JSValueConst *)(sp - 1));
                if (JS_IsException(ret))
                    goto exception;
                JS_FreeValue(ctx, sp[-1]);
                sp[-1] = ret;
            }
            BREAK;
        CASE(OP_iterator_call):
            /* stack: iter_obj next catch_offset val */
            {
                JSValue method, ret;
                BOOL ret_flag;
                int flags;
                flags = *pc++;
                method = JS_GetProperty(ctx, sp[-4], (flags & 1) ?
                                        JS_ATOM_throw : JS_ATOM_return);
                if (JS_IsException(method))
                    goto exception;
                if (JS_IsUndefined(method) || JS_IsNull(method)) {
                    ret_flag = TRUE;
                } else {
                    if (flags & 2) {
                        /* no argument */
                        ret = JS_CallFree(ctx, method, sp[-4],
                                          0, NULL);
                    } else {
                        ret = JS_CallFree(ctx, method, sp[-4],
                                          1, (JSValueConst *)(sp - 1));
                    }
                    if (JS_IsException(ret))
                        goto exception;
                    JS_FreeValue(ctx, sp[-1]);
                    sp[-1] = ret;
                    ret_flag = FALSE;
                }
                sp[0] = JS_NewBool(ctx, ret_flag);
                sp += 1;
            }
            BREAK;
        CASE(OP_lnot):
            {
                int res;
                JSValue op1;
                op1 = sp[-1];
                if ((uint32_t)JS_VALUE_GET_TAG(op1) <= JS_TAG_UNDEFINED) {
                    res = JS_VALUE_GET_INT(op1) != 0;
                } else {
                    res = JS_ToBoolFree(ctx, op1);
                }
                sp[-1] = JS_NewBool(ctx, !res);
            }
            BREAK;
        CASE(OP_get_field):
            {
                JSValue val;
                JSAtom atom;
                atom = get_u32(pc);
                pc += 4;
                val = JS_GetProperty(ctx, sp[-1], atom);
                if (UNLIKELY(JS_IsException(val)))
                    goto exception;
                JS_FreeValue(ctx, sp[-1]);
                sp[-1] = val;
            }
            BREAK;
        CASE(OP_get_field2):
            {
                JSValue val;
                JSAtom atom;
                atom = get_u32(pc);
                pc += 4;
                val = JS_GetProperty(ctx, sp[-1], atom);
                if (UNLIKELY(JS_IsException(val)))
                    goto exception;
                *sp++ = val;
            }
            BREAK;
        CASE(OP_put_field):
            {
                int ret;
                JSAtom atom;
                atom = get_u32(pc);
                pc += 4;
                ret = JS_SetPropertyInternal(ctx, sp[-2], atom, sp[-1],
                                             JS_PROP_THROW_STRICT);
                JS_FreeValue(ctx, sp[-2]);
                sp -= 2;
                if (UNLIKELY(ret < 0))
                    goto exception;
            }
            BREAK;
        CASE(OP_private_symbol):
            {
                JSAtom atom;
                JSValue val;
                atom = get_u32(pc);
                pc += 4;
                val = JS_NewSymbolFromAtom(ctx, atom, JS_ATOM_TYPE_PRIVATE);
                if (JS_IsException(val))
                    goto exception;
                *sp++ = val;
            }
            BREAK;
        CASE(OP_get_private_field):
            {
                JSValue val;
                val = JS_GetPrivateField(ctx, sp[-2], sp[-1]);
                JS_FreeValue(ctx, sp[-1]);
                JS_FreeValue(ctx, sp[-2]);
                sp[-2] = val;
                sp--;
                if (UNLIKELY(JS_IsException(val)))
                    goto exception;
            }
            BREAK;
        CASE(OP_put_private_field):
            {
                int ret;
                ret = JS_SetPrivateField(ctx, sp[-3], sp[-1], sp[-2]);
                JS_FreeValue(ctx, sp[-3]);
                JS_FreeValue(ctx, sp[-1]);
                sp -= 3;
                if (UNLIKELY(ret < 0))
                    goto exception;
            }
            BREAK;
        CASE(OP_define_private_field):
            {
                int ret;
                ret = JS_DefinePrivateField(ctx, sp[-3], sp[-2], sp[-1]);
                JS_FreeValue(ctx, sp[-2]);
                sp -= 2;
                if (UNLIKELY(ret < 0))
                    goto exception;
            }
            BREAK;
        CASE(OP_define_field):
            {
                int ret;
                JSAtom atom;
                atom = get_u32(pc);
                pc += 4;
                ret = JS_DefinePropertyValue(ctx, sp[-2], atom, sp[-1],
                                             JS_PROP_C_W_E | JS_PROP_THROW);
                sp--;
                if (UNLIKELY(ret < 0))
                    goto exception;
            }
            BREAK;
        CASE(OP_set_name):
            {
                int ret;
                JSAtom atom;
                atom = get_u32(pc);
                pc += 4;
                ret = JS_DefineObjectName(ctx, sp[-1], atom, JS_PROP_CONFIGURABLE);
                if (UNLIKELY(ret < 0))
                    goto exception;
            }
            BREAK;
        CASE(OP_set_name_computed):
            {
                int ret;
                ret = JS_DefineObjectNameComputed(ctx, sp[-1], sp[-2], JS_PROP_CONFIGURABLE);
                if (UNLIKELY(ret < 0))
                    goto exception;
            }
            BREAK;
        CASE(OP_set_proto):
            {
                JSValue proto;
                proto = sp[-1];
                if (JS_IsObject(proto) || JS_IsNull(proto)) {
                    if (JS_SetPrototypeInternal(ctx, sp[-2], proto, TRUE) < 0)
                        goto exception;
                }
                JS_FreeValue(ctx, proto);
                sp--;
            }
            BREAK;
        CASE(OP_set_home_object):
            js_method_set_home_object(ctx, sp[-1], sp[-2]);
            BREAK;
        CASE(OP_define_method):
        CASE(OP_define_method_computed):
            {
                JSValue getter, setter, value;
                JSValueConst obj;
                JSAtom atom;
                int flags, ret, op_flags;
                BOOL is_computed;
                is_computed = (opcode == OP_define_method_computed);
                if (is_computed) {
                    atom = JS_ValueToAtom(ctx, sp[-2]);
                    if (UNLIKELY(atom == JS_ATOM_NULL))
                        goto exception;
                    opcode += OP_define_method - OP_define_method_computed;
                } else {
                    atom = get_u32(pc);
                    pc += 4;
                }
                op_flags = *pc++;
                obj = sp[-2 - is_computed];
                flags = JS_PROP_HAS_CONFIGURABLE | JS_PROP_CONFIGURABLE |
                    JS_PROP_HAS_ENUMERABLE | JS_PROP_THROW;
                if (op_flags & OP_DEFINE_METHOD_ENUMERABLE)
                    flags |= JS_PROP_ENUMERABLE;
                op_flags &= 3;
                value = JS_UNDEFINED;
                getter = JS_UNDEFINED;
                setter = JS_UNDEFINED;
                if (op_flags == OP_DEFINE_METHOD_METHOD) {
                    value = sp[-1];
                    flags |= JS_PROP_HAS_VALUE | JS_PROP_HAS_WRITABLE | JS_PROP_WRITABLE;
                } else if (op_flags == OP_DEFINE_METHOD_GETTER) {
                    getter = sp[-1];
                    flags |= JS_PROP_HAS_GET;
                } else {
                    setter = sp[-1];
                    flags |= JS_PROP_HAS_SET;
                }
                ret = js_method_set_properties(ctx, sp[-1], atom, flags, obj);
                if (ret >= 0) {
                    ret = JS_DefineProperty(ctx, obj, atom, value,
                                            getter, setter, flags);
                }
                JS_FreeValue(ctx, sp[-1]);
                if (is_computed) {
                    JS_FreeAtom(ctx, atom);
                    JS_FreeValue(ctx, sp[-2]);
                }
                sp -= 1 + is_computed;
                if (UNLIKELY(ret < 0))
                    goto exception;
            }
            BREAK;
        CASE(OP_define_class):
        CASE(OP_define_class_computed):
            {
                int class_flags;
                JSAtom atom;
                atom = get_u32(pc);
                class_flags = pc[4];
                pc += 5;
                if (js_op_define_class(ctx, sp, atom, class_flags,
                                       var_refs, sf,
                                       (opcode == OP_define_class_computed)) < 0)
                    goto exception;
            }
            BREAK;
        CASE(OP_get_array_el):
            {
                JSValue val;
                val = JS_GetPropertyValue(ctx, sp[-2], sp[-1]);
                JS_FreeValue(ctx, sp[-2]);
                sp[-2] = val;
                sp--;
                if (UNLIKELY(JS_IsException(val)))
                    goto exception;
            }
            BREAK;
        CASE(OP_get_array_el2):
            {
                JSValue val;
                val = JS_GetPropertyValue(ctx, sp[-2], sp[-1]);
                sp[-1] = val;
                if (UNLIKELY(JS_IsException(val)))
                    goto exception;
            }
            BREAK;
        CASE(OP_get_ref_value):
            {
                JSValue val;
                if (UNLIKELY(JS_IsUndefined(sp[-2]))) {
                    JSAtom atom = JS_ValueToAtom(ctx, sp[-1]);
                    if (atom != JS_ATOM_NULL) {
                        JS_ThrowReferenceErrorNotDefined(ctx, atom);
                        JS_FreeAtom(ctx, atom);
                    }
                    goto exception;
                }
                val = JS_GetPropertyValue(ctx, sp[-2],
                                          JS_DupValue(ctx, sp[-1]));
                if (UNLIKELY(JS_IsException(val)))
                    goto exception;
                sp[0] = val;
                sp++;
            }
            BREAK;
        CASE(OP_get_super_value):
            {
                JSValue val;
                JSAtom atom;
                atom = JS_ValueToAtom(ctx, sp[-1]);
                if (UNLIKELY(atom == JS_ATOM_NULL))
                    goto exception;
                val = JS_GetPropertyInternal(ctx, sp[-2], atom, sp[-3], FALSE);
                JS_FreeAtom(ctx, atom);
                if (UNLIKELY(JS_IsException(val)))
                    goto exception;
                JS_FreeValue(ctx, sp[-1]);
                JS_FreeValue(ctx, sp[-2]);
                JS_FreeValue(ctx, sp[-3]);
                sp[-3] = val;
                sp -= 2;
            }
            BREAK;
        CASE(OP_put_array_el):
            {
                int ret;
                ret = JS_SetPropertyValue(ctx, sp[-3], sp[-2], sp[-1], JS_PROP_THROW_STRICT);
                JS_FreeValue(ctx, sp[-3]);
                sp -= 3;
                if (UNLIKELY(ret < 0))
                    goto exception;
            }
            BREAK;
        CASE(OP_put_ref_value):
            {
                int ret, flags;
                flags = JS_PROP_THROW_STRICT;
                if (UNLIKELY(JS_IsUndefined(sp[-3]))) {
                    if (is_strict_mode(ctx)) {
                        JSAtom atom = JS_ValueToAtom(ctx, sp[-2]);
                        if (atom != JS_ATOM_NULL) {
                            JS_ThrowReferenceErrorNotDefined(ctx, atom);
                            JS_FreeAtom(ctx, atom);
                        }
                        goto exception;
                    } else {
                        sp[-3] = JS_DupValue(ctx, ctx->global_obj);
                    }
                } else {
                    if (is_strict_mode(ctx))
                        flags |= JS_PROP_NO_ADD;
                }
                ret = JS_SetPropertyValue(ctx, sp[-3], sp[-2], sp[-1], flags);
                JS_FreeValue(ctx, sp[-3]);
                sp -= 3;
                if (UNLIKELY(ret < 0))
                    goto exception;
            }
            BREAK;
        CASE(OP_put_super_value):
            {
                int ret;
                JSAtom atom;
                if (JS_VALUE_GET_TAG(sp[-3]) != JS_TAG_OBJECT) {
                    JS_ThrowTypeErrorNotAnObject(ctx);
                    goto exception;
                }
                atom = JS_ValueToAtom(ctx, sp[-2]);
                if (UNLIKELY(atom == JS_ATOM_NULL))
                    goto exception;
                ret = JS_SetPropertyGeneric(ctx, sp[-3], atom, sp[-1], sp[-4],
                                            JS_PROP_THROW_STRICT);
                JS_FreeAtom(ctx, atom);
                JS_FreeValue(ctx, sp[-4]);
                JS_FreeValue(ctx, sp[-3]);
                JS_FreeValue(ctx, sp[-2]);
                sp -= 4;
                if (ret < 0)
                    goto exception;
            }
            BREAK;
        CASE(OP_define_array_el):
            {
                int ret;
                ret = JS_DefinePropertyValueValue(ctx, sp[-3], JS_DupValue(ctx, sp[-2]), sp[-1],
                                                  JS_PROP_C_W_E | JS_PROP_THROW);
                sp -= 1;
                if (UNLIKELY(ret < 0))
                    goto exception;
            }
            BREAK;
        CASE(OP_append):    /* array pos enumobj -- array pos */
            {
                if (js_append_enumerate(ctx, sp))
                    goto exception;
                JS_FreeValue(ctx, *--sp);
            }
            BREAK;
        CASE(OP_copy_data_properties):    /* target source excludeList */
            {
                /* stack offsets (-1 based):
                   2 bits for target,
                   3 bits for source,
                   2 bits for exclusionList */
                int mask;
                mask = *pc++;
                if (JS_CopyDataProperties(ctx, sp[-1 - (mask & 3)],
                                          sp[-1 - ((mask >> 2) & 7)],
                                          sp[-1 - ((mask >> 5) & 7)], 0))
                    goto exception;
            }
            BREAK;
        CASE(OP_add):
            {
                JSValue op1, op2;
                op1 = sp[-2];
                op2 = sp[-1];
                if (LIKELY(JS_VALUE_IS_BOTH_INT(op1, op2))) {
                    int64_t r;
                    r = (int64_t)JS_VALUE_GET_INT(op1) + JS_VALUE_GET_INT(op2);
                    if (UNLIKELY((int)r != r))
                        goto add_slow;
                    sp[-2] = JS_NewInt32(ctx, r);
                    sp--;
                } else if (JS_VALUE_IS_BOTH_FLOAT(op1, op2)) {
                    sp[-2] = __JS_NewFloat64(ctx, JS_VALUE_GET_FLOAT64(op1) +
                                             JS_VALUE_GET_FLOAT64(op2));
                    sp--;
                } else {
                add_slow:
                    if (js_add_slow(ctx, sp))
                        goto exception;
                    sp--;
                }
            }
            BREAK;
        CASE(OP_add_loc):
            {
                JSValue *pv;
                int idx;
                idx = *pc;
                pc += 1;
                pv = &var_buf[idx];
                if (LIKELY(JS_VALUE_IS_BOTH_INT(*pv, sp[-1]))) {
                    int64_t r;
                    r = (int64_t)JS_VALUE_GET_INT(*pv) +
                        JS_VALUE_GET_INT(sp[-1]);
                    if (UNLIKELY((int)r != r))
                        goto add_loc_slow;
                    *pv = JS_NewInt32(ctx, r);
                    sp--;
                } else if (JS_VALUE_GET_TAG(*pv) == JS_TAG_STRING) {
                    JSValue op1;
                    op1 = sp[-1];
                    sp--;
                    op1 = JS_ToPrimitiveFree(ctx, op1, HINT_NONE);
                    if (JS_IsException(op1))
                        goto exception;
                    op1 = JS_ConcatString(ctx, JS_DupValue(ctx, *pv), op1);
                    if (JS_IsException(op1))
                        goto exception;
                    set_value(ctx, pv, op1);
                } else {
                    JSValue ops[2];
                add_loc_slow:
                    /* In case of exception, js_add_slow frees ops[0]
                       and ops[1], so we must duplicate *pv */
                    ops[0] = JS_DupValue(ctx, *pv);
                    ops[1] = sp[-1];
                    sp--;
                    if (js_add_slow(ctx, ops + 2))
                        goto exception;
                    set_value(ctx, pv, ops[0]);
                }
            }
            BREAK;
        CASE(OP_sub):
            {
                JSValue op1, op2;
                op1 = sp[-2];
                op2 = sp[-1];
                if (LIKELY(JS_VALUE_IS_BOTH_INT(op1, op2))) {
                    int64_t r;
                    r = (int64_t)JS_VALUE_GET_INT(op1) - JS_VALUE_GET_INT(op2);
                    if (UNLIKELY((int)r != r))
                        goto binary_arith_slow;
                    sp[-2] = JS_NewInt32(ctx, r);
                    sp--;
                } else if (JS_VALUE_IS_BOTH_FLOAT(op1, op2)) {
                    sp[-2] = __JS_NewFloat64(ctx, JS_VALUE_GET_FLOAT64(op1) -
                                             JS_VALUE_GET_FLOAT64(op2));
                    sp--;
                } else {
                    goto binary_arith_slow;
                }
            }
            BREAK;
        CASE(OP_mul):
            {
                JSValue op1, op2;
                double d;
                op1 = sp[-2];
                op2 = sp[-1];
                if (LIKELY(JS_VALUE_IS_BOTH_INT(op1, op2))) {
                    int32_t v1, v2;
                    int64_t r;
                    v1 = JS_VALUE_GET_INT(op1);
                    v2 = JS_VALUE_GET_INT(op2);
                    r = (int64_t)v1 * v2;
                    if (UNLIKELY((int)r != r)) {
#ifdef CONFIG_BIGNUM
                        if (UNLIKELY(sf->js_mode & JS_MODE_MATH) &&
                            (r < -MAX_SAFE_INTEGER || r > MAX_SAFE_INTEGER))
                            goto binary_arith_slow;
#endif
                        d = (double)r;
                        goto mul_fp_res;
                    }
                    /* need to test zero case for -0 result */
                    if (UNLIKELY(r == 0 && (v1 | v2) < 0)) {
                        d = -0.0;
                        goto mul_fp_res;
                    }
                    sp[-2] = JS_NewInt32(ctx, r);
                    sp--;
                } else if (JS_VALUE_IS_BOTH_FLOAT(op1, op2)) {
#ifdef CONFIG_BIGNUM
                    if (UNLIKELY(sf->js_mode & JS_MODE_MATH))
                        goto binary_arith_slow;
#endif
                    d = JS_VALUE_GET_FLOAT64(op1) * JS_VALUE_GET_FLOAT64(op2);
                mul_fp_res:
                    sp[-2] = __JS_NewFloat64(ctx, d);
                    sp--;
                } else {
                    goto binary_arith_slow;
                }
            }
            BREAK;
        CASE(OP_div):
            {
                JSValue op1, op2;
                op1 = sp[-2];
                op2 = sp[-1];
                if (LIKELY(JS_VALUE_IS_BOTH_INT(op1, op2))) {
                    int v1, v2;
                    if (UNLIKELY(sf->js_mode & JS_MODE_MATH))
                        goto binary_arith_slow;
                    v1 = JS_VALUE_GET_INT(op1);
                    v2 = JS_VALUE_GET_INT(op2);
                    sp[-2] = JS_NewFloat64(ctx, (double)v1 / (double)v2);
                    sp--;
                } else {
                    goto binary_arith_slow;
                }
            }
            BREAK;
        CASE(OP_mod):
#ifdef CONFIG_BIGNUM
        CASE(OP_math_mod):
#endif
            {
                JSValue op1, op2;
                op1 = sp[-2];
                op2 = sp[-1];
                if (LIKELY(JS_VALUE_IS_BOTH_INT(op1, op2))) {
                    int v1, v2, r;
                    v1 = JS_VALUE_GET_INT(op1);
                    v2 = JS_VALUE_GET_INT(op2);
                    /* We must avoid v2 = 0, v1 = INT32_MIN and v2 =
                       -1 and the cases where the result is -0. */
                    if (UNLIKELY(v1 < 0 || v2 <= 0))
                        goto binary_arith_slow;
                    r = v1 % v2;
                    sp[-2] = JS_NewInt32(ctx, r);
                    sp--;
                } else {
                    goto binary_arith_slow;
                }
            }
            BREAK;
        CASE(OP_pow):
        binary_arith_slow:
            if (js_binary_arith_slow(ctx, sp, opcode))
                goto exception;
            sp--;
            BREAK;
        CASE(OP_plus):
            {
                JSValue op1;
                uint32_t tag;
                op1 = sp[-1];
                tag = JS_VALUE_GET_TAG(op1);
                if (tag == JS_TAG_INT || JS_TAG_IS_FLOAT64(tag)) {
                } else {
                    if (js_unary_arith_slow(ctx, sp, opcode))
                        goto exception;
                }
            }
            BREAK;
        CASE(OP_neg):
            {
                JSValue op1;
                uint32_t tag;
                int val;
                double d;
                op1 = sp[-1];
                tag = JS_VALUE_GET_TAG(op1);
                if (tag == JS_TAG_INT) {
                    val = JS_VALUE_GET_INT(op1);
                    /* Note: -0 cannot be expressed as integer */
                    if (UNLIKELY(val == 0)) {
                        d = -0.0;
                        goto neg_fp_res;
                    }
                    if (UNLIKELY(val == INT32_MIN)) {
                        d = -(double)val;
                        goto neg_fp_res;
                    }
                    sp[-1] = JS_NewInt32(ctx, -val);
                } else if (JS_TAG_IS_FLOAT64(tag)) {
                    d = -JS_VALUE_GET_FLOAT64(op1);
                neg_fp_res:
                    sp[-1] = __JS_NewFloat64(ctx, d);
                } else {
                    if (js_unary_arith_slow(ctx, sp, opcode))
                        goto exception;
                }
            }
            BREAK;
        CASE(OP_inc):
            {
                JSValue op1;
                int val;
                op1 = sp[-1];
                if (JS_VALUE_GET_TAG(op1) == JS_TAG_INT) {
                    val = JS_VALUE_GET_INT(op1);
                    if (UNLIKELY(val == INT32_MAX))
                        goto inc_slow;
                    sp[-1] = JS_NewInt32(ctx, val + 1);
                } else {
                inc_slow:
                    if (js_unary_arith_slow(ctx, sp, opcode))
                        goto exception;
                }
            }
            BREAK;
        CASE(OP_dec):
            {
                JSValue op1;
                int val;
                op1 = sp[-1];
                if (JS_VALUE_GET_TAG(op1) == JS_TAG_INT) {
                    val = JS_VALUE_GET_INT(op1);
                    if (UNLIKELY(val == INT32_MIN))
                        goto dec_slow;
                    sp[-1] = JS_NewInt32(ctx, val - 1);
                } else {
                dec_slow:
                    if (js_unary_arith_slow(ctx, sp, opcode))
                        goto exception;
                }
            }
            BREAK;
        CASE(OP_post_inc):
        CASE(OP_post_dec):
            if (js_post_inc_slow(ctx, sp, opcode))
                goto exception;
            sp++;
            BREAK;
        CASE(OP_inc_loc):
            {
                JSValue op1;
                int val;
                int idx;
                idx = *pc;
                pc += 1;
                op1 = var_buf[idx];
                if (JS_VALUE_GET_TAG(op1) == JS_TAG_INT) {
                    val = JS_VALUE_GET_INT(op1);
                    if (UNLIKELY(val == INT32_MAX))
                        goto inc_loc_slow;
                    var_buf[idx] = JS_NewInt32(ctx, val + 1);
                } else {
                inc_loc_slow:
                    /* must duplicate otherwise the variable value may
                       be destroyed before JS code accesses it */
                    op1 = JS_DupValue(ctx, op1);
                    if (js_unary_arith_slow(ctx, &op1 + 1, OP_inc))
                        goto exception;
                    set_value(ctx, &var_buf[idx], op1);
                }
            }
            BREAK;
        CASE(OP_dec_loc):
            {
                JSValue op1;
                int val;
                int idx;
                idx = *pc;
                pc += 1;
                op1 = var_buf[idx];
                if (JS_VALUE_GET_TAG(op1) == JS_TAG_INT) {
                    val = JS_VALUE_GET_INT(op1);
                    if (UNLIKELY(val == INT32_MIN))
                        goto dec_loc_slow;
                    var_buf[idx] = JS_NewInt32(ctx, val - 1);
                } else {
                dec_loc_slow:
                    /* must duplicate otherwise the variable value may
                       be destroyed before JS code accesses it */
                    op1 = JS_DupValue(ctx, op1);
                    if (js_unary_arith_slow(ctx, &op1 + 1, OP_dec))
                        goto exception;
                    set_value(ctx, &var_buf[idx], op1);
                }
            }
            BREAK;
        CASE(OP_not):
            {
                JSValue op1;
                op1 = sp[-1];
                if (JS_VALUE_GET_TAG(op1) == JS_TAG_INT) {
                    sp[-1] = JS_NewInt32(ctx, ~JS_VALUE_GET_INT(op1));
                } else {
                    if (js_not_slow(ctx, sp))
                        goto exception;
                }
            }
            BREAK;
        CASE(OP_shl):
            {
                JSValue op1, op2;
                op1 = sp[-2];
                op2 = sp[-1];
                if (LIKELY(JS_VALUE_IS_BOTH_INT(op1, op2))) {
                    uint32_t v1, v2;
                    v1 = JS_VALUE_GET_INT(op1);
                    v2 = JS_VALUE_GET_INT(op2);
#ifdef CONFIG_BIGNUM
                    {
                        int64_t r;
                        if (UNLIKELY(sf->js_mode & JS_MODE_MATH)) {
                            if (v2 > 0x1f)
                                goto shl_slow;
                            r = (int64_t)v1 << v2;
                            if ((int)r != r)
                                goto shl_slow;
                        } else {
                            v2 &= 0x1f;
                        }
                    }
#else
                    v2 &= 0x1f;
#endif
                    sp[-2] = JS_NewInt32(ctx, v1 << v2);
                    sp--;
                } else {
#ifdef CONFIG_BIGNUM
                shl_slow:
#endif
                    if (js_binary_logic_slow(ctx, sp, opcode))
                        goto exception;
                    sp--;
                }
            }
            BREAK;
        CASE(OP_shr):
            {
                JSValue op1, op2;
                op1 = sp[-2];
                op2 = sp[-1];
                if (LIKELY(JS_VALUE_IS_BOTH_INT(op1, op2))) {
                    uint32_t v2;
                    v2 = JS_VALUE_GET_INT(op2);
                    /* v1 >>> v2 retains its JS semantics if CONFIG_BIGNUM */
                    v2 &= 0x1f;
                    sp[-2] = JS_NewUint32(ctx,
                                          (uint32_t)JS_VALUE_GET_INT(op1) >>
                                          v2);
                    sp--;
                } else {
                    if (js_shr_slow(ctx, sp))
                        goto exception;
                    sp--;
                }
            }
            BREAK;
        CASE(OP_sar):
            {
                JSValue op1, op2;
                op1 = sp[-2];
                op2 = sp[-1];
                if (LIKELY(JS_VALUE_IS_BOTH_INT(op1, op2))) {
                    uint32_t v2;
                    v2 = JS_VALUE_GET_INT(op2);
#ifdef CONFIG_BIGNUM
                    if (UNLIKELY(v2 > 0x1f)) {
                        if (UNLIKELY(sf->js_mode & JS_MODE_MATH))
                            goto sar_slow;
                        else
                            v2 &= 0x1f;
                    }
#else
                    v2 &= 0x1f;
#endif
                    sp[-2] = JS_NewInt32(ctx,
                                          (int)JS_VALUE_GET_INT(op1) >> v2);
                    sp--;
                } else {
#ifdef CONFIG_BIGNUM
                sar_slow:
#endif
                    if (js_binary_logic_slow(ctx, sp, opcode))
                        goto exception;
                    sp--;
                }
            }
            BREAK;
        CASE(OP_and):
            {
                JSValue op1, op2;
                op1 = sp[-2];
                op2 = sp[-1];
                if (LIKELY(JS_VALUE_IS_BOTH_INT(op1, op2))) {
                    sp[-2] = JS_NewInt32(ctx,
                                         JS_VALUE_GET_INT(op1) &
                                         JS_VALUE_GET_INT(op2));
                    sp--;
                } else {
                    if (js_binary_logic_slow(ctx, sp, opcode))
                        goto exception;
                    sp--;
                }
            }
            BREAK;
        CASE(OP_or):
            {
                JSValue op1, op2;
                op1 = sp[-2];
                op2 = sp[-1];
                if (LIKELY(JS_VALUE_IS_BOTH_INT(op1, op2))) {
                    sp[-2] = JS_NewInt32(ctx,
                                         JS_VALUE_GET_INT(op1) |
                                         JS_VALUE_GET_INT(op2));
                    sp--;
                } else {
                    if (js_binary_logic_slow(ctx, sp, opcode))
                        goto exception;
                    sp--;
                }
            }
            BREAK;
        CASE(OP_xor):
            {
                JSValue op1, op2;
                op1 = sp[-2];
                op2 = sp[-1];
                if (LIKELY(JS_VALUE_IS_BOTH_INT(op1, op2))) {
                    sp[-2] = JS_NewInt32(ctx,
                                         JS_VALUE_GET_INT(op1) ^
                                         JS_VALUE_GET_INT(op2));
                    sp--;
                } else {
                    if (js_binary_logic_slow(ctx, sp, opcode))
                        goto exception;
                    sp--;
                }
            }
            BREAK;
#define OP_CMP(opcode, binary_op, slow_call)              \
            CASE(opcode):                                 \
                {                                         \
                JSValue op1, op2;                         \
                op1 = sp[-2];                             \
                op2 = sp[-1];                                   \
                if (LIKELY(JS_VALUE_IS_BOTH_INT(op1, op2))) {           \
                    sp[-2] = JS_NewBool(ctx, JS_VALUE_GET_INT(op1) binary_op JS_VALUE_GET_INT(op2)); \
                    sp--;                                               \
                } else {                                                \
                    if (slow_call)                                      \
                        goto exception;                                 \
                    sp--;                                               \
                }                                                       \
                }                                                       \
            BREAK
            OP_CMP(OP_lt, <, js_relational_slow(ctx, sp, opcode));
            OP_CMP(OP_lte, <=, js_relational_slow(ctx, sp, opcode));
            OP_CMP(OP_gt, >, js_relational_slow(ctx, sp, opcode));
            OP_CMP(OP_gte, >=, js_relational_slow(ctx, sp, opcode));
            OP_CMP(OP_eq, ==, js_eq_slow(ctx, sp, 0));
            OP_CMP(OP_neq, !=, js_eq_slow(ctx, sp, 1));
            OP_CMP(OP_strict_eq, ==, js_strict_eq_slow(ctx, sp, 0));
            OP_CMP(OP_strict_neq, !=, js_strict_eq_slow(ctx, sp, 1));
#ifdef CONFIG_BIGNUM
        CASE(OP_mul_pow10):
            if (rt->bigfloat_ops.mul_pow10(ctx, sp))
                goto exception;
            sp--;
            BREAK;
#endif
        CASE(OP_in):
            if (js_operator_in(ctx, sp))
                goto exception;
            sp--;
            BREAK;
        CASE(OP_instanceof):
            if (js_operator_instanceof(ctx, sp))
                goto exception;
            sp--;
            BREAK;
        CASE(OP_typeof):
            {
                JSValue op1;
                JSAtom atom;
                op1 = sp[-1];
                atom = js_operator_typeof(ctx, op1);
                JS_FreeValue(ctx, op1);
                sp[-1] = JS_AtomToString(ctx, atom);
            }
            BREAK;
        CASE(OP_delete):
            if (js_operator_delete(ctx, sp))
                goto exception;
            sp--;
            BREAK;
        CASE(OP_delete_var):
            {
                JSAtom atom;
                int ret;
                atom = get_u32(pc);
                pc += 4;
                ret = JS_DeleteProperty(ctx, ctx->global_obj, atom, 0);
                if (UNLIKELY(ret < 0))
                    goto exception;
                *sp++ = JS_NewBool(ctx, ret);
            }
            BREAK;
        CASE(OP_to_object):
            if (JS_VALUE_GET_TAG(sp[-1]) != JS_TAG_OBJECT) {
                ret_val = JS_ToObject(ctx, sp[-1]);
                if (JS_IsException(ret_val))
                    goto exception;
                JS_FreeValue(ctx, sp[-1]);
                sp[-1] = ret_val;
            }
            BREAK;
        CASE(OP_to_propkey):
            switch (JS_VALUE_GET_TAG(sp[-1])) {
            case JS_TAG_INT:
            case JS_TAG_STRING:
            case JS_TAG_SYMBOL:
                break;
            default:
                ret_val = JS_ToPropertyKey(ctx, sp[-1]);
                if (JS_IsException(ret_val))
                    goto exception;
                JS_FreeValue(ctx, sp[-1]);
                sp[-1] = ret_val;
                break;
            }
            BREAK;
        CASE(OP_to_propkey2):
            /* must be tested first */
            if (UNLIKELY(JS_IsUndefined(sp[-2]) || JS_IsNull(sp[-2]))) {
                JS_ThrowTypeError(ctx, "value has no property");
                goto exception;
            }
            switch (JS_VALUE_GET_TAG(sp[-1])) {
            case JS_TAG_INT:
            case JS_TAG_STRING:
            case JS_TAG_SYMBOL:
                break;
            default:
                ret_val = JS_ToPropertyKey(ctx, sp[-1]);
                if (JS_IsException(ret_val))
                    goto exception;
                JS_FreeValue(ctx, sp[-1]);
                sp[-1] = ret_val;
                break;
            }
            BREAK;
#if 0
        CASE(OP_to_string):
            if (JS_VALUE_GET_TAG(sp[-1]) != JS_TAG_STRING) {
                ret_val = JS_ToString(ctx, sp[-1]);
                if (JS_IsException(ret_val))
                    goto exception;
                JS_FreeValue(ctx, sp[-1]);
                sp[-1] = ret_val;
            }
            BREAK;
#endif
        CASE(OP_with_get_var):
        CASE(OP_with_put_var):
        CASE(OP_with_delete_var):
        CASE(OP_with_make_ref):
        CASE(OP_with_get_ref):
        CASE(OP_with_get_ref_undef):
            {
                JSAtom atom;
                int32_t diff;
                JSValue obj, val;
                int ret, is_with;
                atom = get_u32(pc);
                diff = get_u32(pc + 4);
                is_with = pc[8];
                pc += 9;
                obj = sp[-1];
                ret = JS_HasProperty(ctx, obj, atom);
                if (UNLIKELY(ret < 0))
                    goto exception;
                if (ret) {
                    if (is_with) {
                        ret = js_has_unscopable(ctx, obj, atom);
                        if (UNLIKELY(ret < 0))
                            goto exception;
                        if (ret)
                            goto no_with;
                    }
                    switch (opcode) {
                    case OP_with_get_var:
                        val = JS_GetProperty(ctx, obj, atom);
                        if (UNLIKELY(JS_IsException(val)))
                            goto exception;
                        set_value(ctx, &sp[-1], val);
                        break;
                    case OP_with_put_var:
                        /* XXX: check if strict mode */
                        ret = JS_SetPropertyInternal(ctx, obj, atom, sp[-2],
                                                     JS_PROP_THROW_STRICT);
                        JS_FreeValue(ctx, sp[-1]);
                        sp -= 2;
                        if (UNLIKELY(ret < 0))
                            goto exception;
                        break;
                    case OP_with_delete_var:
                        ret = JS_DeleteProperty(ctx, obj, atom, 0);
                        if (UNLIKELY(ret < 0))
                            goto exception;
                        JS_FreeValue(ctx, sp[-1]);
                        sp[-1] = JS_NewBool(ctx, ret);
                        break;
                    case OP_with_make_ref:
                        /* produce a pair object/propname on the stack */
                        *sp++ = JS_AtomToValue(ctx, atom);
                        break;
                    case OP_with_get_ref:
                        /* produce a pair object/method on the stack */
                        val = JS_GetProperty(ctx, obj, atom);
                        if (UNLIKELY(JS_IsException(val)))
                            goto exception;
                        *sp++ = val;
                        break;
                    case OP_with_get_ref_undef:
                        /* produce a pair undefined/function on the stack */
                        val = JS_GetProperty(ctx, obj, atom);
                        if (UNLIKELY(JS_IsException(val)))
                            goto exception;
                        JS_FreeValue(ctx, sp[-1]);
                        sp[-1] = JS_UNDEFINED;
                        *sp++ = val;
                        break;
                    }
                    pc += diff - 5;
                } else {
                no_with:
                    /* if not jumping, drop the object argument */
                    JS_FreeValue(ctx, sp[-1]);
                    sp--;
                }
            }
            BREAK;
        CASE(OP_await):
            ret_val = JS_NewInt32(ctx, FUNC_RET_AWAIT);
            goto done_generator;
        CASE(OP_yield):
            ret_val = JS_NewInt32(ctx, FUNC_RET_YIELD);
            goto done_generator;
        CASE(OP_yield_star):
        CASE(OP_async_yield_star):
            ret_val = JS_NewInt32(ctx, FUNC_RET_YIELD_STAR);
            goto done_generator;
        CASE(OP_return_async):
        CASE(OP_initial_yield):
            ret_val = JS_UNDEFINED;
            goto done_generator;
        CASE(OP_nop):
            BREAK;
        CASE(OP_is_undefined_or_null):
            if (JS_VALUE_GET_TAG(sp[-1]) == JS_TAG_UNDEFINED ||
                JS_VALUE_GET_TAG(sp[-1]) == JS_TAG_NULL) {
                goto set_true;
            } else {
                goto free_and_set_false;
            }
#if SHORT_OPCODES
        CASE(OP_is_undefined):
            if (JS_VALUE_GET_TAG(sp[-1]) == JS_TAG_UNDEFINED) {
                goto set_true;
            } else {
                goto free_and_set_false;
            }
        CASE(OP_is_null):
            if (JS_VALUE_GET_TAG(sp[-1]) == JS_TAG_NULL) {
                goto set_true;
            } else {
                goto free_and_set_false;
            }
            /* XXX: could merge to a single opcode */
        CASE(OP_typeof_is_undefined):
            /* different from OP_is_undefined because of isHTMLDDA */
            if (js_operator_typeof(ctx, sp[-1]) == JS_ATOM_undefined) {
                goto free_and_set_true;
            } else {
                goto free_and_set_false;
            }
        CASE(OP_typeof_is_function):
            if (js_operator_typeof(ctx, sp[-1]) == JS_ATOM_function) {
                goto free_and_set_true;
            } else {
                goto free_and_set_false;
            }
        free_and_set_true:
            JS_FreeValue(ctx, sp[-1]);
#endif
        set_true:
            sp[-1] = JS_TRUE;
            BREAK;
        free_and_set_false:
            JS_FreeValue(ctx, sp[-1]);
            sp[-1] = JS_FALSE;
            BREAK;
        CASE(OP_invalid):
        DEFAULT:
            JS_ThrowInternalError(ctx, "invalid opcode: pc=%u opcode=0x%02x",
                                  (int)(pc - b->byte_code_buf - 1), opcode);
            goto exception;
        }
    }
 exception:
    if (is_backtrace_needed(ctx, rt->current_exception)) {
        /* add the backtrace information now (it is not done
           before if the exception happens in a bytecode
           operation */
        sf->cur_pc = pc;
        build_backtrace(ctx, rt->current_exception, NULL, 0, 0);
    }
    if (!JS_IsUncatchableError(ctx, rt->current_exception)) {
        while (sp > stack_buf) {
            JSValue val = *--sp;
            JS_FreeValue(ctx, val);
            if (JS_VALUE_GET_TAG(val) == JS_TAG_CATCH_OFFSET) {
                int pos = JS_VALUE_GET_INT(val);
                if (pos == 0) {
                    /* enumerator: close it with a throw */
                    JS_FreeValue(ctx, sp[-1]); /* drop the next method */
                    sp--;
                    JS_IteratorClose(ctx, sp[-1], TRUE);
                } else {
                    *sp++ = rt->current_exception;
                    rt->current_exception = JS_NULL;
                    pc = b->byte_code_buf + pos;
                    goto restart;
                }
            }
        }
    }
    ret_val = JS_EXCEPTION;
    /* the local variables are freed by the caller in the generator
       case. Hence the label 'done' should never be reached in a
       generator function. */
    if (b->func_kind != JS_FUNC_NORMAL) {
    done_generator:
        sf->cur_pc = pc;
        sf->cur_sp = sp;
    } else {
    done:
        if (UNLIKELY(!list_empty(&sf->var_ref_list))) {
            /* variable references reference the stack: must close them */
            close_var_refs(rt, sf);
        }
        /* free the local variables and stack */
        for(pval = local_buf; pval < sp; pval++) {
            JS_FreeValue(ctx, *pval);
        }
    }
    rt->current_stack_frame = sf->prev_frame;
    return ret_val;
}

JSValue JS_Call(JSContext *ctx, JSValueConst func_obj, JSValueConst this_obj,
                int argc, JSValueConst *argv)
{
    return JS_CallInternal(ctx, func_obj, this_obj, JS_UNDEFINED,
                           argc, (JSValue *)argv, JS_CALL_FLAG_COPY_ARGV);
}

JSValue JS_CallFree(JSContext *ctx, JSValue func_obj, JSValueConst this_obj,
                    int argc, JSValueConst *argv)
{
    JSValue res = JS_CallInternal(ctx, func_obj, this_obj, JS_UNDEFINED,
                                  argc, (JSValue *)argv, JS_CALL_FLAG_COPY_ARGV);
    JS_FreeValue(ctx, func_obj);
    return res;
}

/* argv[] is modified if (flags & JS_CALL_FLAG_COPY_ARGV) = 0. */
static JSValue JS_CallConstructorInternal(JSContext *ctx,
                                          JSValueConst func_obj,
                                          JSValueConst new_target,
                                          int argc, JSValue *argv, int flags)
{
    JSObject *p;
    JSFunctionBytecode *b;
    if (js_poll_interrupts(ctx))
        return JS_EXCEPTION;
    flags |= JS_CALL_FLAG_CONSTRUCTOR;
    if (UNLIKELY(JS_VALUE_GET_TAG(func_obj) != JS_TAG_OBJECT))
        goto not_a_function;
    p = JS_VALUE_GET_OBJ(func_obj);
    if (UNLIKELY(!p->is_constructor))
        return JS_ThrowTypeError(ctx, "not a constructor");
    if (UNLIKELY(p->class_id != JS_CLASS_BYTECODE_FUNCTION)) {
        JSClassCall *call_func;
        call_func = ctx->rt->class_array[p->class_id].call;
        if (!call_func) {
        not_a_function:
            return JS_ThrowTypeError(ctx, "not a function");
        }
        return call_func(ctx, func_obj, new_target, argc,
                         (JSValueConst *)argv, flags);
    }
    b = p->u.func.function_bytecode;
    if (b->is_derived_class_constructor) {
        return JS_CallInternal(ctx, func_obj, JS_UNDEFINED, new_target, argc, argv, flags);
    } else {
        JSValue obj, ret;
        /* legacy constructor behavior */
        obj = js_create_from_ctor(ctx, new_target, JS_CLASS_OBJECT);
        if (JS_IsException(obj))
            return JS_EXCEPTION;
        ret = JS_CallInternal(ctx, func_obj, obj, new_target, argc, argv, flags);
        if (JS_VALUE_GET_TAG(ret) == JS_TAG_OBJECT ||
            JS_IsException(ret)) {
            JS_FreeValue(ctx, obj);
            return ret;
        } else {
            JS_FreeValue(ctx, ret);
            return obj;
        }
    }
}

/* XXX: add a specific eval mode so that Function("}), ({") is rejected */
JSValue js_function_constructor(JSContext *ctx, JSValueConst new_target,
                                int argc, JSValueConst *argv, int magic)
{
    JSFunctionKindEnum func_kind = magic;
    int i, n, ret;
    JSValue s, proto, obj = JS_UNDEFINED;
    StringBuffer b_s, *b = &b_s;
    string_buffer_init(ctx, b, 0);
    string_buffer_putc8(b, '(');
    if (func_kind == JS_FUNC_ASYNC || func_kind == JS_FUNC_ASYNC_GENERATOR) {
        string_buffer_puts8(b, "async ");
    }
    string_buffer_puts8(b, "function");
    if (func_kind == JS_FUNC_GENERATOR || func_kind == JS_FUNC_ASYNC_GENERATOR) {
        string_buffer_putc8(b, '*');
    }
    string_buffer_puts8(b, " anonymous(");
    n = argc - 1;
    for(i = 0; i < n; i++) {
        if (i != 0) {
            string_buffer_putc8(b, ',');
        }
        if (string_buffer_concat_value(b, argv[i]))
            goto fail;
    }
    string_buffer_puts8(b, "\n) {\n");
    if (n >= 0) {
        if (string_buffer_concat_value(b, argv[n]))
            goto fail;
    }
    string_buffer_puts8(b, "\n})");
    s = string_buffer_end(b);
    if (JS_IsException(s))
        goto fail1;
    obj = JS_EvalObject(ctx, ctx->global_obj, s, JS_EVAL_TYPE_INDIRECT, -1);
    JS_FreeValue(ctx, s);
    if (JS_IsException(obj))
        goto fail1;
    if (!JS_IsUndefined(new_target)) {
        /* set the prototype */
        proto = JS_GetProperty(ctx, new_target, JS_ATOM_prototype);
        if (JS_IsException(proto))
            goto fail1;
        if (!JS_IsObject(proto)) {
            JSContext *realm;
            JS_FreeValue(ctx, proto);
            realm = JS_GetFunctionRealm(ctx, new_target);
            if (!realm)
                goto fail1;
            proto = JS_DupValue(ctx, realm->class_proto[func_kind_to_class_id[func_kind]]);
        }
        ret = JS_SetPrototypeInternal(ctx, obj, proto, TRUE);
        JS_FreeValue(ctx, proto);
        if (ret < 0)
            goto fail1;
    }
    return obj;
 fail:
    string_buffer_free(b);
 fail1:
    JS_FreeValue(ctx, obj);
    return JS_EXCEPTION;
}

/* JSAsyncFunctionState (used by generator and async functions) */
int async_func_init(JSContext *ctx, JSAsyncFunctionState *s,
                    JSValueConst func_obj, JSValueConst this_obj,
                    int argc, JSValueConst *argv)
{
    JSObject *p;
    JSFunctionBytecode *b;
    JSStackFrame *sf;
    int local_count, i, arg_buf_len, n;
    sf = &s->frame;
    init_list_head(&sf->var_ref_list);
    p = JS_VALUE_GET_OBJ(func_obj);
    b = p->u.func.function_bytecode;
    sf->js_mode = b->js_mode;
    sf->cur_pc = b->byte_code_buf;
    arg_buf_len = max_int(b->arg_count, argc);
    local_count = arg_buf_len + b->var_count + b->stack_size;
    sf->arg_buf = js_malloc(ctx, sizeof(JSValue) * max_int(local_count, 1));
    if (!sf->arg_buf)
        return -1;
    sf->cur_func = JS_DupValue(ctx, func_obj);
    s->this_val = JS_DupValue(ctx, this_obj);
    s->argc = argc;
    sf->arg_count = arg_buf_len;
    sf->var_buf = sf->arg_buf + arg_buf_len;
    sf->cur_sp = sf->var_buf + b->var_count;
    for(i = 0; i < argc; i++)
        sf->arg_buf[i] = JS_DupValue(ctx, argv[i]);
    n = arg_buf_len + b->var_count;
    for(i = argc; i < n; i++)
        sf->arg_buf[i] = JS_UNDEFINED;
    return 0;
}

void async_func_mark(JSRuntime *rt, JSAsyncFunctionState *s, JS_MarkFunc *mark_func)
{
    JSStackFrame *sf;
    JSValue *sp;
    sf = &s->frame;
    JS_MarkValue(rt, sf->cur_func, mark_func);
    JS_MarkValue(rt, s->this_val, mark_func);
    if (sf->cur_sp) {
        /* if the function is running, cur_sp is not known so we
           cannot mark the stack. Marking the variables is not needed
           because a running function cannot be part of a removable
           cycle */
        for(sp = sf->arg_buf; sp < sf->cur_sp; sp++)
            JS_MarkValue(rt, *sp, mark_func);
    }
}

void async_func_free(JSRuntime *rt, JSAsyncFunctionState *s)
{
    JSStackFrame *sf;
    JSValue *sp;
    sf = &s->frame;
    /* close the closure variables. */
    close_var_refs(rt, sf);
    if (sf->arg_buf) {
        /* cannot free the function if it is running */
        assert(sf->cur_sp != NULL);
        for(sp = sf->arg_buf; sp < sf->cur_sp; sp++) {
            JS_FreeValueRT(rt, *sp);
        }
        js_free_rt(rt, sf->arg_buf);
    }
    JS_FreeValueRT(rt, sf->cur_func);
    JS_FreeValueRT(rt, s->this_val);
}

JSValue async_func_resume(JSContext *ctx, JSAsyncFunctionState *s)
{
    JSValue func_obj;
    if (js_check_stack_overflow(ctx->rt, 0))
        return JS_ThrowStackOverflow(ctx);
    /* the tag does not matter provided it is not an object */
    func_obj = JS_MKPTR(JS_TAG_INT, s);
    return JS_CallInternal(ctx, func_obj, s->this_val, JS_UNDEFINED,
                           s->argc, s->frame.arg_buf, JS_CALL_FLAG_GENERATOR);
}
