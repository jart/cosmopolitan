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
#include "third_party/quickjs/cutils.h"
#include "third_party/quickjs/libregexp.h"
#include "third_party/quickjs/list.h"
#include "third_party/quickjs/quickjs.h"
#include "libc/assert.h"
#include "libc/fmt/fmt.h"
#include "libc/fmt/fmt.h"
#include "libc/inttypes.h"
#include "libc/mem/alloca.h"
#include "third_party/gdtoa/gdtoa.h"
#include "libc/fmt/conv.h"
#include "libc/runtime/fenv.h"
#include "libc/time/time.h"
#include "libc/time/time.h"
#include "libc/calls/weirdtypes.h"
#include "libc/time/struct/tm.h"
#include "libc/log/log.h"
#include "third_party/quickjs/internal.h"
#include "third_party/quickjs/leb128.h"
#include "third_party/quickjs/internal.h"
#include "third_party/quickjs/internal.h"
#include "third_party/quickjs/internal.h"
#include "third_party/quickjs/internal.h"
#include "libc/str/str.h"
#include "libc/mem/mem.h"
#include "libc/runtime/runtime.h"
#include "third_party/quickjs/libbf.h"

asm(".ident\t\"\\n\\n\
QuickJS (MIT License)\\n\
Copyright (c) 2017-2021 Fabrice Bellard\\n\
Copyright (c) 2017-2021 Charlie Gordon\"");
asm(".include \"libc/disclaimer.inc\"");

/* clang-format off */

static const char js_atom_init[] =
#define DEF(name, str) str "\0"
#include "third_party/quickjs/quickjs-atom.inc"
#undef DEF
;

static int JS_InitAtoms(JSRuntime *rt);
static JSAtom __JS_NewAtomInit(JSRuntime *rt, const char *str, int len,
                               int atom_type);
static __maybe_unused void JS_DumpAtoms(JSRuntime *rt);
static __maybe_unused void JS_DumpString(JSRuntime *rt,
                                         const JSString *p);
static __maybe_unused void JS_DumpObjectHeader(JSRuntime *rt);
static __maybe_unused void JS_DumpObject(JSRuntime *rt, JSObject *p);
static __maybe_unused void JS_DumpGCObject(JSRuntime *rt, JSGCObjectHeader *p);
static __maybe_unused void JS_DumpValueShort(JSRuntime *rt,
                                             JSValueConst val);
static __maybe_unused void JS_DumpValue(JSContext *ctx, JSValueConst val);
static __maybe_unused void JS_PrintValue(JSContext *ctx,
                                         const char *str,
                                         JSValueConst val);
static __maybe_unused void JS_DumpShapes(JSRuntime *rt);
static void js_c_function_finalizer(JSRuntime *rt, JSValue val);
static void js_c_function_mark(JSRuntime *rt, JSValueConst val,
                               JS_MarkFunc *mark_func);
static void js_bound_function_finalizer(JSRuntime *rt, JSValue val);
static void js_bound_function_mark(JSRuntime *rt, JSValueConst val,
                                   JS_MarkFunc *mark_func);
static void js_for_in_iterator_finalizer(JSRuntime *rt, JSValue val);
static void js_for_in_iterator_mark(JSRuntime *rt, JSValueConst val,
                                    JS_MarkFunc *mark_func);
static void js_proxy_finalizer(JSRuntime *rt, JSValue val);
static void js_proxy_mark(JSRuntime *rt, JSValueConst val,
                          JS_MarkFunc *mark_func);
static void js_promise_finalizer(JSRuntime *rt, JSValue val);
static void js_promise_mark(JSRuntime *rt, JSValueConst val,
                            JS_MarkFunc *mark_func);
static void js_promise_resolve_function_finalizer(JSRuntime *rt, JSValue val);
static void js_promise_resolve_function_mark(JSRuntime *rt, JSValueConst val,
                                             JS_MarkFunc *mark_func);
#ifdef CONFIG_BIGNUM
static void js_operator_set_finalizer(JSRuntime *rt, JSValue val);
static void js_operator_set_mark(JSRuntime *rt, JSValueConst val,
                                 JS_MarkFunc *mark_func);
#endif
static void gc_decref(JSRuntime *rt);
static int JS_NewClass1(JSRuntime *rt, JSClassID class_id,
                        const JSClassDef *class_def, JSAtom name);

#ifdef CONFIG_BIGNUM
static void js_float_env_finalizer(JSRuntime *rt, JSValue val);
static JSValue JS_ToBigDecimalFree(JSContext *ctx, JSValue val,
                                   BOOL allow_null_or_undefined);
#endif
static int JS_CreateProperty(JSContext *ctx, JSObject *p,
                             JSAtom prop, JSValueConst val,
                             JSValueConst getter, JSValueConst setter,
                             int flags);
static JSValue JS_EvalInternal(JSContext *ctx, JSValueConst this_obj,
                               const char *input, size_t input_len,
                               const char *filename, int flags, int scope_idx);
static JSValue js_dynamic_import(JSContext *ctx, JSValueConst specifier);
static JSValue js_new_promise_capability(JSContext *ctx,
                                         JSValue *resolving_funcs,
                                         JSValueConst ctor);
static JSValue JS_ToNumber(JSContext *ctx, JSValueConst val);
static int JS_NumberIsInteger(JSContext *ctx, JSValueConst val);
static BOOL JS_NumberIsNegativeOrMinusZero(JSContext *ctx, JSValueConst val);
static void JS_AddIntrinsicBasicObjects(JSContext *ctx);
static int js_shape_prepare_update(JSContext *ctx, JSObject *p,
                                   JSShapeProperty **pprs);
static JSValue JS_CreateAsyncFromSyncIterator(JSContext *ctx,
                                              JSValueConst sync_iter);
static void js_c_function_data_finalizer(JSRuntime *rt, JSValue val);
static void js_c_function_data_mark(JSRuntime *rt, JSValueConst val,
                                    JS_MarkFunc *mark_func);
static JSValue js_c_function_data_call(JSContext *ctx, JSValueConst func_obj,
                                       JSValueConst this_val,
                                       int argc, JSValueConst *argv, int flags);
static JSValue js_instantiate_prototype(JSContext *ctx, JSObject *p, JSAtom atom, void *opaque);
static JSValue js_module_ns_autoinit(JSContext *ctx, JSObject *p, JSAtom atom,
                                 void *opaque);
static JSValue JS_InstantiateFunctionListItem2(JSContext *ctx, JSObject *p,
                                               JSAtom atom, void *opaque);
void JS_SetUncatchableError(JSContext *ctx, JSValueConst val, BOOL flag);

static const JSClassExoticMethods js_arguments_exotic_methods;
static const JSClassExoticMethods js_string_exotic_methods;
static const JSClassExoticMethods js_proxy_exotic_methods;
static const JSClassExoticMethods js_module_ns_exotic_methods;
static JSClassID js_class_id_alloc = JS_CLASS_INIT_COUNT;

static size_t js_malloc_usable_size_unknown(const void *ptr)
{
    return 0;
}

void *js_malloc_rt(JSRuntime *rt, size_t size)
{
    return rt->mf.js_malloc(&rt->malloc_state, size);
}

void js_free_rt(JSRuntime *rt, void *ptr)
{
    rt->mf.js_free(&rt->malloc_state, ptr);
}

void *js_realloc_rt(JSRuntime *rt, void *ptr, size_t size)
{
    return rt->mf.js_realloc(&rt->malloc_state, ptr, size);
}

size_t js_malloc_usable_size_rt(JSRuntime *rt, const void *ptr)
{
    return rt->mf.js_malloc_usable_size(ptr);
}

void *js_mallocz_rt(JSRuntime *rt, size_t size)
{
    void *ptr;
    ptr = js_malloc_rt(rt, size);
    if (!ptr)
        return NULL;
    bzero(ptr, size);
    return ptr;
}

#ifdef CONFIG_BIGNUM
/* called by libbf */
static void *js_bf_realloc(void *opaque, void *ptr, size_t size)
{
    JSRuntime *rt = opaque;
    return js_realloc_rt(rt, ptr, size);
}
#endif /* CONFIG_BIGNUM */

/* Throw out of memory in case of error */
void *js_malloc(JSContext *ctx, size_t size)
{
    void *ptr;
    ptr = js_malloc_rt(ctx->rt, size);
    if (UNLIKELY(!ptr)) {
        JS_ThrowOutOfMemory(ctx);
        return NULL;
    }
    return ptr;
}

/* Throw out of memory in case of error */
void *js_mallocz(JSContext *ctx, size_t size)
{
    void *ptr;
    ptr = js_mallocz_rt(ctx->rt, size);
    if (UNLIKELY(!ptr)) {
        JS_ThrowOutOfMemory(ctx);
        return NULL;
    }
    return ptr;
}

void js_free(JSContext *ctx, void *ptr)
{
    js_free_rt(ctx->rt, ptr);
}

/* Throw out of memory in case of error */
void *js_realloc(JSContext *ctx, void *ptr, size_t size)
{
    void *ret;
    ret = js_realloc_rt(ctx->rt, ptr, size);
    if (UNLIKELY(!ret && size != 0)) {
        JS_ThrowOutOfMemory(ctx);
        return NULL;
    }
    return ret;
}

/* store extra allocated size in *pslack if successful */
void *js_realloc2(JSContext *ctx, void *ptr, size_t size, size_t *pslack)
{
    void *ret;
    ret = js_realloc_rt(ctx->rt, ptr, size);
    if (UNLIKELY(!ret && size != 0)) {
        JS_ThrowOutOfMemory(ctx);
        return NULL;
    }
    if (pslack) {
        size_t new_size = js_malloc_usable_size_rt(ctx->rt, ret);
        *pslack = (new_size > size) ? new_size - size : 0;
    }
    return ret;
}

size_t js_malloc_usable_size(JSContext *ctx, const void *ptr)
{
    return js_malloc_usable_size_rt(ctx->rt, ptr);
}

/* Throw out of memory exception in case of error */
char *js_strndup(JSContext *ctx, const char *s, size_t n)
{
    char *ptr;
    ptr = js_malloc(ctx, n + 1);
    if (ptr) {
        memcpy(ptr, s, n);
        ptr[n] = '\0';
    }
    return ptr;
}

char *js_strdup(JSContext *ctx, const char *str)
{
    return js_strndup(ctx, str, strlen(str));
}

int js_realloc_array(JSContext *ctx, void **parray,
                     int elem_size, int *psize, int req_size)
{
    int new_size;
    size_t slack;
    void *new_array;
    /* XXX: potential arithmetic overflow */
    new_size = max_int(req_size, *psize * 3 / 2);
    new_array = js_realloc2(ctx, *parray, new_size * elem_size, &slack);
    if (!new_array)
        return -1;
    new_size += slack / elem_size;
    *psize = new_size;
    *parray = new_array;
    return 0;
}

static void js_float_env_finalizer(JSRuntime *rt, JSValue val)
{
    JSFloatEnv *fe = JS_GetOpaque(val, JS_CLASS_FLOAT_ENV);
    js_free_rt(rt, fe);
}

JSClassShortDef const js_std_class_def[] = {
    { JS_ATOM_Object, NULL, NULL },                             /* JS_CLASS_OBJECT */
    { JS_ATOM_Array, js_array_finalizer, js_array_mark },       /* JS_CLASS_ARRAY */
    { JS_ATOM_Error, NULL, NULL }, /* JS_CLASS_ERROR */
    { JS_ATOM_Number, js_object_data_finalizer, js_object_data_mark }, /* JS_CLASS_NUMBER */
    { JS_ATOM_String, js_object_data_finalizer, js_object_data_mark }, /* JS_CLASS_STRING */
    { JS_ATOM_Boolean, js_object_data_finalizer, js_object_data_mark }, /* JS_CLASS_BOOLEAN */
    { JS_ATOM_Symbol, js_object_data_finalizer, js_object_data_mark }, /* JS_CLASS_SYMBOL */
    { JS_ATOM_Arguments, js_array_finalizer, js_array_mark },   /* JS_CLASS_ARGUMENTS */
    { JS_ATOM_Arguments, NULL, NULL },                          /* JS_CLASS_MAPPED_ARGUMENTS */
    { JS_ATOM_Date, js_object_data_finalizer, js_object_data_mark }, /* JS_CLASS_DATE */
    { JS_ATOM_Object, NULL, NULL },                             /* JS_CLASS_MODULE_NS */
    { JS_ATOM_Function, js_c_function_finalizer, js_c_function_mark }, /* JS_CLASS_C_FUNCTION */
    { JS_ATOM_Function, js_bytecode_function_finalizer, js_bytecode_function_mark }, /* JS_CLASS_BYTECODE_FUNCTION */
    { JS_ATOM_Function, js_bound_function_finalizer, js_bound_function_mark }, /* JS_CLASS_BOUND_FUNCTION */
    { JS_ATOM_Function, js_c_function_data_finalizer, js_c_function_data_mark }, /* JS_CLASS_C_FUNCTION_DATA */
    { JS_ATOM_GeneratorFunction, js_bytecode_function_finalizer, js_bytecode_function_mark },  /* JS_CLASS_GENERATOR_FUNCTION */
    { JS_ATOM_ForInIterator, js_for_in_iterator_finalizer, js_for_in_iterator_mark },      /* JS_CLASS_FOR_IN_ITERATOR */
    { JS_ATOM_RegExp, js_regexp_finalizer, NULL },                              /* JS_CLASS_REGEXP */
    { JS_ATOM_ArrayBuffer, js_array_buffer_finalizer, NULL },                   /* JS_CLASS_ARRAY_BUFFER */
    { JS_ATOM_SharedArrayBuffer, js_array_buffer_finalizer, NULL },             /* JS_CLASS_SHARED_ARRAY_BUFFER */
    { JS_ATOM_Uint8ClampedArray, js_typed_array_finalizer, js_typed_array_mark }, /* JS_CLASS_UINT8C_ARRAY */
    { JS_ATOM_Int8Array, js_typed_array_finalizer, js_typed_array_mark },       /* JS_CLASS_INT8_ARRAY */
    { JS_ATOM_Uint8Array, js_typed_array_finalizer, js_typed_array_mark },      /* JS_CLASS_UINT8_ARRAY */
    { JS_ATOM_Int16Array, js_typed_array_finalizer, js_typed_array_mark },      /* JS_CLASS_INT16_ARRAY */
    { JS_ATOM_Uint16Array, js_typed_array_finalizer, js_typed_array_mark },     /* JS_CLASS_UINT16_ARRAY */
    { JS_ATOM_Int32Array, js_typed_array_finalizer, js_typed_array_mark },      /* JS_CLASS_INT32_ARRAY */
    { JS_ATOM_Uint32Array, js_typed_array_finalizer, js_typed_array_mark },     /* JS_CLASS_UINT32_ARRAY */
#ifdef CONFIG_BIGNUM
    { JS_ATOM_BigInt64Array, js_typed_array_finalizer, js_typed_array_mark },   /* JS_CLASS_BIG_INT64_ARRAY */
    { JS_ATOM_BigUint64Array, js_typed_array_finalizer, js_typed_array_mark },  /* JS_CLASS_BIG_UINT64_ARRAY */
#endif
    { JS_ATOM_Float32Array, js_typed_array_finalizer, js_typed_array_mark },    /* JS_CLASS_FLOAT32_ARRAY */
    { JS_ATOM_Float64Array, js_typed_array_finalizer, js_typed_array_mark },    /* JS_CLASS_FLOAT64_ARRAY */
    { JS_ATOM_DataView, js_typed_array_finalizer, js_typed_array_mark },        /* JS_CLASS_DATAVIEW */
#ifdef CONFIG_BIGNUM
    { JS_ATOM_BigInt, js_object_data_finalizer, js_object_data_mark },      /* JS_CLASS_BIG_INT */
    { JS_ATOM_BigFloat, js_object_data_finalizer, js_object_data_mark },    /* JS_CLASS_BIG_FLOAT */
    { JS_ATOM_BigFloatEnv, js_float_env_finalizer, NULL },      /* JS_CLASS_FLOAT_ENV */
    { JS_ATOM_BigDecimal, js_object_data_finalizer, js_object_data_mark },    /* JS_CLASS_BIG_DECIMAL */
    { JS_ATOM_OperatorSet, js_operator_set_finalizer, js_operator_set_mark },    /* JS_CLASS_OPERATOR_SET */
#endif
    { JS_ATOM_Map, js_map_finalizer, js_map_mark },             /* JS_CLASS_MAP */
    { JS_ATOM_Set, js_map_finalizer, js_map_mark },             /* JS_CLASS_SET */
    { JS_ATOM_WeakMap, js_map_finalizer, js_map_mark },         /* JS_CLASS_WEAKMAP */
    { JS_ATOM_WeakSet, js_map_finalizer, js_map_mark },         /* JS_CLASS_WEAKSET */
    { JS_ATOM_Map_Iterator, js_map_iterator_finalizer, js_map_iterator_mark }, /* JS_CLASS_MAP_ITERATOR */
    { JS_ATOM_Set_Iterator, js_map_iterator_finalizer, js_map_iterator_mark }, /* JS_CLASS_SET_ITERATOR */
    { JS_ATOM_Array_Iterator, js_array_iterator_finalizer, js_array_iterator_mark }, /* JS_CLASS_ARRAY_ITERATOR */
    { JS_ATOM_String_Iterator, js_array_iterator_finalizer, js_array_iterator_mark }, /* JS_CLASS_STRING_ITERATOR */
    { JS_ATOM_RegExp_String_Iterator, js_regexp_string_iterator_finalizer, js_regexp_string_iterator_mark }, /* JS_CLASS_REGEXP_STRING_ITERATOR */
    { JS_ATOM_Generator, js_generator_finalizer, js_generator_mark }, /* JS_CLASS_GENERATOR */
};

int init_class_range(JSRuntime *rt, JSClassShortDef const *tab, int start, int count)
{
    JSClassDef cm_s, *cm = &cm_s;
    int i, class_id;
    for(i = 0; i < count; i++) {
        class_id = i + start;
        bzero(cm, sizeof(*cm));
        cm->finalizer = tab[i].finalizer;
        cm->gc_mark = tab[i].gc_mark;
        if (JS_NewClass1(rt, class_id, cm, tab[i].class_name) < 0)
            return -1;
    }
    return 0;
}

#ifdef CONFIG_BIGNUM
static JSValue JS_ThrowUnsupportedOperation(JSContext *ctx)
{
    return JS_ThrowTypeError(ctx, "unsupported operation");
}

static JSValue invalid_to_string(JSContext *ctx, JSValueConst val)
{
    return JS_ThrowUnsupportedOperation(ctx);
}

static JSValue invalid_from_string(JSContext *ctx, const char *buf,
                                   int radix, int flags, slimb_t *pexponent)
{
    return JS_NAN;
}

static int invalid_unary_arith(JSContext *ctx,
                               JSValue *pres, OPCodeEnum op, JSValue op1)
{
    JS_FreeValue(ctx, op1);
    JS_ThrowUnsupportedOperation(ctx);
    return -1;
}

static int invalid_binary_arith(JSContext *ctx, OPCodeEnum op,
                                JSValue *pres, JSValue op1, JSValue op2)
{
    JS_FreeValue(ctx, op1);
    JS_FreeValue(ctx, op2);
    JS_ThrowUnsupportedOperation(ctx);
    return -1;
}

static JSValue invalid_mul_pow10_to_float64(JSContext *ctx, const bf_t *a,
                                            int64_t exponent)
{
    return JS_ThrowUnsupportedOperation(ctx);
}

static int invalid_mul_pow10(JSContext *ctx, JSValue *sp)
{
    JS_ThrowUnsupportedOperation(ctx);
    return -1;
}

static void set_dummy_numeric_ops(JSNumericOperations *ops)
{
    ops->to_string = invalid_to_string;
    ops->from_string = invalid_from_string;
    ops->unary_arith = invalid_unary_arith;
    ops->binary_arith = invalid_binary_arith;
    ops->mul_pow10_to_float64 = invalid_mul_pow10_to_float64;
    ops->mul_pow10 = invalid_mul_pow10;
}

#endif /* CONFIG_BIGNUM */

JSRuntime *JS_NewRuntime2(const JSMallocFunctions *mf, void *opaque)
{
    JSRuntime *rt;
    JSMallocState ms;
    bzero(&ms, sizeof(ms));
    ms.opaque = opaque;
    ms.malloc_limit = -1;
    rt = mf->js_malloc(&ms, sizeof(JSRuntime));
    if (!rt)
        return NULL;
    bzero(rt, sizeof(*rt));
    rt->mf = *mf;
    if (!rt->mf.js_malloc_usable_size) {
        /* use dummy function if none provided */
        rt->mf.js_malloc_usable_size = js_malloc_usable_size_unknown;
    }
    rt->malloc_state = ms;
    rt->malloc_gc_threshold = 256 * 1024;
#ifdef CONFIG_BIGNUM
    bf_context_init(&rt->bf_ctx, js_bf_realloc, rt);
    set_dummy_numeric_ops(&rt->bigint_ops);
    set_dummy_numeric_ops(&rt->bigfloat_ops);
    set_dummy_numeric_ops(&rt->bigdecimal_ops);
#endif
    init_list_head(&rt->context_list);
    init_list_head(&rt->gc_obj_list);
    init_list_head(&rt->gc_zero_ref_count_list);
    rt->gc_phase = JS_GC_PHASE_NONE;
#ifdef DUMP_LEAKS
    init_list_head(&rt->string_list);
#endif
    init_list_head(&rt->job_list);
    if (JS_InitAtoms(rt))
        goto fail;
    /* create the object, array and function classes */
    if (init_class_range(rt, js_std_class_def, JS_CLASS_OBJECT,
                         countof(js_std_class_def)) < 0)
        goto fail;
    rt->class_array[JS_CLASS_ARGUMENTS].exotic = &js_arguments_exotic_methods;
    rt->class_array[JS_CLASS_STRING].exotic = &js_string_exotic_methods;
    rt->class_array[JS_CLASS_MODULE_NS].exotic = &js_module_ns_exotic_methods;
    rt->class_array[JS_CLASS_C_FUNCTION].call = js_call_c_function;
    rt->class_array[JS_CLASS_C_FUNCTION_DATA].call = js_c_function_data_call;
    rt->class_array[JS_CLASS_BOUND_FUNCTION].call = js_call_bound_function;
    rt->class_array[JS_CLASS_GENERATOR_FUNCTION].call = js_generator_function_call;
    if (init_shape_hash(rt))
        goto fail;
    rt->stack_size = JS_DEFAULT_STACK_SIZE;
    JS_UpdateStackTop(rt);
    rt->current_exception = JS_NULL;
    return rt;
 fail:
    JS_FreeRuntime(rt);
    return NULL;
}

void *JS_GetRuntimeOpaque(JSRuntime *rt)
{
    return rt->user_opaque;
}

void JS_SetRuntimeOpaque(JSRuntime *rt, void *opaque)
{
    rt->user_opaque = opaque;
}

/* default memory allocation functions with memory limitation */
static inline size_t js_def_malloc_usable_size(void *ptr)
{
#if defined(__APPLE__)
    return malloc_size(ptr);
#elif defined(_WIN32)
    return _msize(ptr);
#elif defined(EMSCRIPTEN)
    return 0;
#elif defined(__linux__)
    return malloc_usable_size(ptr);
#else
    /* change this to `return 0;` if compilation fails */
    return malloc_usable_size(ptr);
#endif
}

static void *js_def_malloc(JSMallocState *s, size_t size)
{
    void *ptr;
    /* Do not allocate zero bytes: behavior is platform dependent */
    assert(size != 0);
    if (UNLIKELY(s->malloc_size + size > s->malloc_limit))
        return NULL;
    ptr = malloc(size);
    if (!ptr)
        return NULL;
    s->malloc_count++;
    s->malloc_size += js_def_malloc_usable_size(ptr) + MALLOC_OVERHEAD;
    return ptr;
}

static void js_def_free(JSMallocState *s, void *ptr)
{
    if (!ptr)
        return;
    s->malloc_count--;
    s->malloc_size -= js_def_malloc_usable_size(ptr) + MALLOC_OVERHEAD;
    free(ptr);
}

static void *js_def_realloc(JSMallocState *s, void *ptr, size_t size)
{
    size_t old_size;

    if (!ptr) {
        if (size == 0)
            return NULL;
        return js_def_malloc(s, size);
    }
    old_size = js_def_malloc_usable_size(ptr);
    if (size == 0) {
        s->malloc_count--;
        s->malloc_size -= old_size + MALLOC_OVERHEAD;
        free(ptr);
        return NULL;
    }
    if (s->malloc_size + size - old_size > s->malloc_limit)
        return NULL;

    ptr = realloc(ptr, size);
    if (!ptr)
        return NULL;

    s->malloc_size += js_def_malloc_usable_size(ptr) - old_size;
    return ptr;
}

static const JSMallocFunctions def_malloc_funcs = {
    js_def_malloc,
    js_def_free,
    js_def_realloc,
#if defined(__APPLE__)
    malloc_size,
#elif defined(_WIN32)
    (size_t (*)(const void *))_msize,
#elif defined(EMSCRIPTEN)
    NULL,
#elif defined(__linux__)
    (size_t (*)(const void *))malloc_usable_size,
#else
    /* change this to `NULL,` if compilation fails */
    malloc_usable_size,
#endif
};

JSRuntime *JS_NewRuntime(void)
{
    return JS_NewRuntime2(&def_malloc_funcs, NULL);
}

void JS_SetMemoryLimit(JSRuntime *rt, size_t limit)
{
    rt->malloc_state.malloc_limit = limit;
}

/* use -1 to disable automatic GC */
void JS_SetGCThreshold(JSRuntime *rt, size_t gc_threshold)
{
    rt->malloc_gc_threshold = gc_threshold;
}

#define malloc(s) malloc_is_forbidden(s)
#define free(p) free_is_forbidden(p)
#define realloc(p,s) realloc_is_forbidden(p,s)

void JS_SetInterruptHandler(JSRuntime *rt, JSInterruptHandler *cb, void *opaque)
{
    rt->interrupt_handler = cb;
    rt->interrupt_opaque = opaque;
}

void JS_SetCanBlock(JSRuntime *rt, BOOL can_block)
{
    rt->can_block = can_block;
}

void JS_SetSharedArrayBufferFunctions(JSRuntime *rt,
                                      const JSSharedArrayBufferFunctions *sf)
{
    rt->sab_funcs = *sf;
}

/* return 0 if OK, < 0 if exception */
int JS_EnqueueJob(JSContext *ctx, JSJobFunc *job_func,
                  int argc, JSValueConst *argv)
{
    JSRuntime *rt = ctx->rt;
    JSJobEntry *e;
    int i;
    e = js_malloc(ctx, sizeof(*e) + argc * sizeof(JSValue));
    if (!e)
        return -1;
    e->ctx = ctx;
    e->job_func = job_func;
    e->argc = argc;
    for(i = 0; i < argc; i++) {
        e->argv[i] = JS_DupValue(ctx, argv[i]);
    }
    list_add_tail(&e->link, &rt->job_list);
    return 0;
}

BOOL JS_IsJobPending(JSRuntime *rt)
{
    return !list_empty(&rt->job_list);
}

/* return < 0 if exception, 0 if no job pending, 1 if a job was
   executed successfully. the context of the job is stored in '*pctx' */
int JS_ExecutePendingJob(JSRuntime *rt, JSContext **pctx)
{
    JSContext *ctx;
    JSJobEntry *e;
    JSValue res;
    int i, ret;
    if (list_empty(&rt->job_list)) {
        *pctx = NULL;
        return 0;
    }
    /* get the first pending job and execute it */
    e = list_entry(rt->job_list.next, JSJobEntry, link);
    list_del(&e->link);
    ctx = e->ctx;
    res = e->job_func(e->ctx, e->argc, (JSValueConst *)e->argv);
    for(i = 0; i < e->argc; i++)
        JS_FreeValue(ctx, e->argv[i]);
    if (JS_IsException(res))
        ret = -1;
    else
        ret = 1;
    JS_FreeValue(ctx, res);
    js_free(ctx, e);
    *pctx = ctx;
    return ret;
}

void JS_SetRuntimeInfo(JSRuntime *rt, const char *s)
{
    if (rt)
        rt->rt_info = s;
}

void JS_FreeRuntime(JSRuntime *rt)
{
    struct list_head *el, *el1;
    int i;
    JS_FreeValueRT(rt, rt->current_exception);
    list_for_each_safe(el, el1, &rt->job_list) {
        JSJobEntry *e = list_entry(el, JSJobEntry, link);
        for(i = 0; i < e->argc; i++)
            JS_FreeValueRT(rt, e->argv[i]);
        js_free_rt(rt, e);
    }
    init_list_head(&rt->job_list);
    JS_RunGC(rt);
#ifdef DUMP_LEAKS
    /* leaking objects */
    {
        BOOL header_done;
        JSGCObjectHeader *p;
        int count;
        /* remove the internal refcounts to display only the object
           referenced externally */
        list_for_each(el, &rt->gc_obj_list) {
            p = list_entry(el, JSGCObjectHeader, link);
            p->mark = 0;
        }
        gc_decref(rt);
        header_done = FALSE;
        list_for_each(el, &rt->gc_obj_list) {
            p = list_entry(el, JSGCObjectHeader, link);
            if (p->ref_count != 0) {
                if (!header_done) {
                    printf("Object leaks:\n");
                    JS_DumpObjectHeader(rt);
                    header_done = TRUE;
                }
                JS_DumpGCObject(rt, p);
            }
        }
        count = 0;
        list_for_each(el, &rt->gc_obj_list) {
            p = list_entry(el, JSGCObjectHeader, link);
            if (p->ref_count == 0) {
                count++;
            }
        }
        if (count != 0)
            printf("Secondary object leaks: %d\n", count);
    }
#endif
    assert(list_empty(&rt->gc_obj_list));
    /* free the classes */
    for(i = 0; i < rt->class_count; i++) {
        JSClass *cl = &rt->class_array[i];
        if (cl->class_id != 0) {
            JS_FreeAtomRT(rt, cl->class_name);
        }
    }
    js_free_rt(rt, rt->class_array);
#ifdef CONFIG_BIGNUM
    bf_context_end(&rt->bf_ctx);
#endif
#ifdef DUMP_LEAKS
    /* only the atoms defined in JS_InitAtoms() should be left */
    {
        BOOL header_done = FALSE;
        for(i = 0; i < rt->atom_size; i++) {
            JSAtomStruct *p = rt->atom_array[i];
            if (!atom_is_free(p) /* && p->str*/) {
                if (i >= JS_ATOM_END || p->header.ref_count != 1) {
                    if (!header_done) {
                        header_done = TRUE;
                        if (rt->rt_info) {
                            printf("%s:1: atom leakage:", rt->rt_info);
                        } else {
                            printf("Atom leaks:\n"
                                   "    %6s %6s %s\n",
                                   "ID", "REFCNT", "NAME");
                        }
                    }
                    if (rt->rt_info) {
                        printf(" ");
                    } else {
                        printf("    %6u %6u ", i, p->header.ref_count);
                    }
                    switch (p->atom_type) {
                    case JS_ATOM_TYPE_STRING:
                        JS_DumpString(rt, p);
                        break;
                    case JS_ATOM_TYPE_GLOBAL_SYMBOL:
                        printf("Symbol.for(");
                        JS_DumpString(rt, p);
                        printf(")");
                        break;
                    case JS_ATOM_TYPE_SYMBOL:
                        if (p->hash == JS_ATOM_HASH_SYMBOL) {
                            printf("Symbol(");
                            JS_DumpString(rt, p);
                            printf(")");
                        } else {
                            printf("Private(");
                            JS_DumpString(rt, p);
                            printf(")");
                        }
                        break;
                    }
                    if (rt->rt_info) {
                        printf(":%u", p->header.ref_count);
                    } else {
                        printf("\n");
                    }
                }
            }
        }
        if (rt->rt_info && header_done)
            printf("\n");
    }
#endif
    /* free the atoms */
    for(i = 0; i < rt->atom_size; i++) {
        JSAtomStruct *p = rt->atom_array[i];
        if (!atom_is_free(p)) {
#ifdef DUMP_LEAKS
            list_del(&p->link);
#endif
            js_free_rt(rt, p);
        }
    }
    js_free_rt(rt, rt->atom_array);
    js_free_rt(rt, rt->atom_hash);
    js_free_rt(rt, rt->shape_hash);
#ifdef DUMP_LEAKS
    if (!list_empty(&rt->string_list)) {
        if (rt->rt_info) {
            printf("%s:1: string leakage:", rt->rt_info);
        } else {
            printf("String leaks:\n"
                   "    %6s %s\n",
                   "REFCNT", "VALUE");
        }
        list_for_each_safe(el, el1, &rt->string_list) {
            JSString *str = list_entry(el, JSString, link);
            if (rt->rt_info) {
                printf(" ");
            } else {
                printf("    %6u ", str->header.ref_count);
            }
            JS_DumpString(rt, str);
            if (rt->rt_info) {
                printf(":%u", str->header.ref_count);
            } else {
                printf("\n");
            }
            list_del(&str->link);
            js_free_rt(rt, str);
        }
        if (rt->rt_info)
            printf("\n");
    }
    {
        JSMallocState *s = &rt->malloc_state;
        if (s->malloc_count > 1) {
            if (rt->rt_info)
                printf("%s:1: ", rt->rt_info);
            printf("Memory leak: %"PRIu64" bytes lost in %"PRIu64" block%s\n",
                   (uint64_t)(s->malloc_size - sizeof(JSRuntime)),
                   (uint64_t)(s->malloc_count - 1), &"s"[s->malloc_count == 2]);
        }
    }
#endif
    {
        JSMallocState ms = rt->malloc_state;
        rt->mf.js_free(&ms, rt);
    }
}

JSContext *JS_NewContextRaw(JSRuntime *rt)
{
    JSContext *ctx;
    int i;
    ctx = js_mallocz_rt(rt, sizeof(JSContext));
    if (!ctx)
        return NULL;
    ctx->header.ref_count = 1;
    add_gc_object(rt, &ctx->header, JS_GC_OBJ_TYPE_JS_CONTEXT);
    ctx->class_proto = js_malloc_rt(rt, sizeof(ctx->class_proto[0]) *
                                    rt->class_count);
    if (!ctx->class_proto) {
        js_free_rt(rt, ctx);
        return NULL;
    }
    ctx->rt = rt;
    list_add_tail(&ctx->link, &rt->context_list);
#ifdef CONFIG_BIGNUM
    ctx->bf_ctx = &rt->bf_ctx;
    ctx->fp_env.prec = 113;
    ctx->fp_env.flags = bf_set_exp_bits(15) | BF_RNDN | BF_FLAG_SUBNORMAL;
#endif
    for(i = 0; i < rt->class_count; i++)
        ctx->class_proto[i] = JS_NULL;
    ctx->array_ctor = JS_NULL;
    ctx->regexp_ctor = JS_NULL;
    ctx->promise_ctor = JS_NULL;
    init_list_head(&ctx->loaded_modules);
    JS_AddIntrinsicBasicObjects(ctx);
    return ctx;
}

JSContext *JS_NewContext(JSRuntime *rt)
{
    JSContext *ctx;
    ctx = JS_NewContextRaw(rt);
    if (!ctx)
        return NULL;
    JS_AddIntrinsicBaseObjects(ctx);
    JS_AddIntrinsicDate(ctx);
    JS_AddIntrinsicEval(ctx);
    JS_AddIntrinsicStringNormalize(ctx);
    JS_AddIntrinsicRegExp(ctx);
    JS_AddIntrinsicJSON(ctx);
    JS_AddIntrinsicProxy(ctx);
    JS_AddIntrinsicMapSet(ctx);
    JS_AddIntrinsicTypedArrays(ctx);
    JS_AddIntrinsicPromise(ctx);
#ifdef CONFIG_BIGNUM
    JS_AddIntrinsicBigInt(ctx);
#endif
    return ctx;
}

void *JS_GetContextOpaque(JSContext *ctx)
{
    return ctx->user_opaque;
}

void JS_SetContextOpaque(JSContext *ctx, void *opaque)
{
    ctx->user_opaque = opaque;
}

void JS_SetClassProto(JSContext *ctx, JSClassID class_id, JSValue obj)
{
    JSRuntime *rt = ctx->rt;
    assert(class_id < rt->class_count);
    set_value(ctx, &ctx->class_proto[class_id], obj);
}

JSValue JS_GetClassProto(JSContext *ctx, JSClassID class_id)
{
    JSRuntime *rt = ctx->rt;
    assert(class_id < rt->class_count);
    return JS_DupValue(ctx, ctx->class_proto[class_id]);
}

typedef enum JSFreeModuleEnum {
    JS_FREE_MODULE_ALL,
    JS_FREE_MODULE_NOT_RESOLVED,
    JS_FREE_MODULE_NOT_EVALUATED,
} JSFreeModuleEnum;

/* XXX: would be more efficient with separate module lists */
static void js_free_modules(JSContext *ctx, JSFreeModuleEnum flag)
{
    struct list_head *el, *el1;
    list_for_each_safe(el, el1, &ctx->loaded_modules) {
        JSModuleDef *m = list_entry(el, JSModuleDef, link);
        if (flag == JS_FREE_MODULE_ALL ||
            (flag == JS_FREE_MODULE_NOT_RESOLVED && !m->resolved) ||
            (flag == JS_FREE_MODULE_NOT_EVALUATED && !m->evaluated)) {
            js_free_module_def(ctx, m);
        }
    }
}

JSContext *JS_DupContext(JSContext *ctx)
{
    ctx->header.ref_count++;
    return ctx;
}

void JS_FreeContext(JSContext *ctx)
{
    JSRuntime *rt = ctx->rt;
    int i;
    if (--ctx->header.ref_count > 0)
        return;
    assert(ctx->header.ref_count == 0);
#ifdef DUMP_ATOMS
    JS_DumpAtoms(ctx->rt);
#endif
#ifdef DUMP_SHAPES
    JS_DumpShapes(ctx->rt);
#endif
#ifdef DUMP_OBJECTS
    {
        struct list_head *el;
        JSGCObjectHeader *p;
        printf("JSObjects: {\n");
        JS_DumpObjectHeader(ctx->rt);
        list_for_each(el, &rt->gc_obj_list) {
            p = list_entry(el, JSGCObjectHeader, link);
            JS_DumpGCObject(rt, p);
        }
        printf("}\n");
    }
#endif
#ifdef DUMP_MEM
    {
        JSMemoryUsage stats;
        JS_ComputeMemoryUsage(rt, &stats);
        JS_DumpMemoryUsage(stdout, &stats, rt);
    }
#endif
    js_free_modules(ctx, JS_FREE_MODULE_ALL);
    JS_FreeValue(ctx, ctx->global_obj);
    JS_FreeValue(ctx, ctx->global_var_obj);
    JS_FreeValue(ctx, ctx->throw_type_error);
    JS_FreeValue(ctx, ctx->eval_obj);
    JS_FreeValue(ctx, ctx->array_proto_values);
    for(i = 0; i < JS_NATIVE_ERROR_COUNT; i++) {
        JS_FreeValue(ctx, ctx->native_error_proto[i]);
    }
    for(i = 0; i < rt->class_count; i++) {
        JS_FreeValue(ctx, ctx->class_proto[i]);
    }
    js_free_rt(rt, ctx->class_proto);
    JS_FreeValue(ctx, ctx->iterator_proto);
    JS_FreeValue(ctx, ctx->async_iterator_proto);
    JS_FreeValue(ctx, ctx->promise_ctor);
    JS_FreeValue(ctx, ctx->array_ctor);
    JS_FreeValue(ctx, ctx->regexp_ctor);
    JS_FreeValue(ctx, ctx->function_ctor);
    JS_FreeValue(ctx, ctx->function_proto);
    js_free_shape_null(ctx->rt, ctx->array_shape);
    list_del(&ctx->link);
    remove_gc_object(&ctx->header);
    js_free_rt(ctx->rt, ctx);
}

JSRuntime *JS_GetRuntime(JSContext *ctx)
{
    return ctx->rt;
}

static void update_stack_limit(JSRuntime *rt)
{
    if (rt->stack_size == 0) {
        rt->stack_limit = 0; /* no limit */
    } else {
        rt->stack_limit = rt->stack_top - rt->stack_size;
    }
}

void JS_SetMaxStackSize(JSRuntime *rt, size_t stack_size)
{
    rt->stack_size = stack_size;
    update_stack_limit(rt);
}

void JS_UpdateStackTop(JSRuntime *rt)
{
    rt->stack_top = js_get_stack_pointer();
    update_stack_limit(rt);
}

/* JSAtom support */

/* XXX: could use faster version ? */
static inline uint32_t hash_string8(const uint8_t *str, size_t len, uint32_t h)
{
    size_t i;
    for(i = 0; i < len; i++)
        h = h * 263 + str[i];
    return h;
}

static inline uint32_t hash_string16(const uint16_t *str,
                                     size_t len, uint32_t h)
{
    size_t i;
    for(i = 0; i < len; i++)
        h = h * 263 + str[i];
    return h;
}

uint32_t hash_string(const JSString *str, uint32_t h)
{
    if (str->is_wide_char)
        h = hash_string16(str->u.str16, str->len, h);
    else
        h = hash_string8(str->u.str8, str->len, h);
    return h;
}

static __maybe_unused void JS_DumpString(JSRuntime *rt, const JSString *p)
{
    int i, c, sep;

    if (p == NULL) {
        printf("<null>");
        return;
    }
    printf("%d", p->header.ref_count);
    sep = (p->header.ref_count == 1) ? '\"' : '\'';
    putchar(sep);
    for(i = 0; i < p->len; i++) {
        if (p->is_wide_char)
            c = p->u.str16[i];
        else
            c = p->u.str8[i];
        if (c == sep || c == '\\') {
            putchar('\\');
            putchar(c);
        } else if (c >= ' ' && c <= 126) {
            putchar(c);
        } else if (c == '\n') {
            putchar('\\');
            putchar('n');
        } else {
            printf("\\u%04x", c);
        }
    }
    putchar(sep);
}

static __maybe_unused void JS_DumpAtoms(JSRuntime *rt)
{
    JSAtomStruct *p;
    int h, i;
    /* This only dumps hashed atoms, not JS_ATOM_TYPE_SYMBOL atoms */
    printf("JSAtom count=%d size=%d hash_size=%d:\n",
           rt->atom_count, rt->atom_size, rt->atom_hash_size);
    printf("JSAtom hash table: {\n");
    for(i = 0; i < rt->atom_hash_size; i++) {
        h = rt->atom_hash[i];
        if (h) {
            printf("  %d:", i);
            while (h) {
                p = rt->atom_array[h];
                printf(" ");
                JS_DumpString(rt, p);
                h = p->hash_next;
            }
            printf("\n");
        }
    }
    printf("}\n");
    printf("JSAtom table: {\n");
    for(i = 0; i < rt->atom_size; i++) {
        p = rt->atom_array[i];
        if (!atom_is_free(p)) {
            printf("  %d: { %d %08x ", i, p->atom_type, p->hash);
            if (!(p->len == 0 && p->is_wide_char != 0))
                JS_DumpString(rt, p);
            printf(" %d }\n", p->hash_next);
        }
    }
    printf("}\n");
}

static int JS_ResizeAtomHash(JSRuntime *rt, int new_hash_size)
{
    JSAtomStruct *p;
    uint32_t new_hash_mask, h, i, hash_next1, j, *new_hash;
    assert((new_hash_size & (new_hash_size - 1)) == 0); /* power of two */
    new_hash_mask = new_hash_size - 1;
    new_hash = js_mallocz_rt(rt, sizeof(rt->atom_hash[0]) * new_hash_size);
    if (!new_hash)
        return -1;
    for(i = 0; i < rt->atom_hash_size; i++) {
        h = rt->atom_hash[i];
        while (h != 0) {
            p = rt->atom_array[h];
            hash_next1 = p->hash_next;
            /* add in new hash table */
            j = p->hash & new_hash_mask;
            p->hash_next = new_hash[j];
            new_hash[j] = h;
            h = hash_next1;
        }
    }
    js_free_rt(rt, rt->atom_hash);
    rt->atom_hash = new_hash;
    rt->atom_hash_size = new_hash_size;
    rt->atom_count_resize = JS_ATOM_COUNT_RESIZE(new_hash_size);
    //    JS_DumpAtoms(rt);
    return 0;
}

static int JS_InitAtoms(JSRuntime *rt)
{
    int i, len, atom_type;
    const char *p;
    rt->atom_hash_size = 0;
    rt->atom_hash = NULL;
    rt->atom_count = 0;
    rt->atom_size = 0;
    rt->atom_free_index = 0;
    if (JS_ResizeAtomHash(rt, 256))     /* there are at least 195 predefined atoms */
        return -1;
    p = js_atom_init;
    for(i = 1; i < JS_ATOM_END; i++) {
        if (i == JS_ATOM_Private_brand)
            atom_type = JS_ATOM_TYPE_PRIVATE;
        else if (i >= JS_ATOM_Symbol_toPrimitive)
            atom_type = JS_ATOM_TYPE_SYMBOL;
        else
            atom_type = JS_ATOM_TYPE_STRING;
        len = strlen(p);
        if (__JS_NewAtomInit(rt, p, len, atom_type) == JS_ATOM_NULL)
            return -1;
        p = p + len + 1;
    }
    return 0;
}

static JSAtom JS_DupAtomRT(JSRuntime *rt, JSAtom v)
{
    JSAtomStruct *p;
    if (!__JS_AtomIsConst(v)) {
        p = rt->atom_array[v];
        p->header.ref_count++;
    }
    return v;
}

JSAtom JS_DupAtom(JSContext *ctx, JSAtom v)
{
    JSRuntime *rt;
    JSAtomStruct *p;
    if (!__JS_AtomIsConst(v)) {
        rt = ctx->rt;
        p = rt->atom_array[v];
        p->header.ref_count++;
    }
    return v;
}

JSAtomKindEnum JS_AtomGetKind(JSContext *ctx, JSAtom v)
{
    JSRuntime *rt;
    JSAtomStruct *p;
    rt = ctx->rt;
    if (__JS_AtomIsTaggedInt(v))
        return JS_ATOM_KIND_STRING;
    p = rt->atom_array[v];
    switch(p->atom_type) {
    case JS_ATOM_TYPE_STRING:
        return JS_ATOM_KIND_STRING;
    case JS_ATOM_TYPE_GLOBAL_SYMBOL:
        return JS_ATOM_KIND_SYMBOL;
    case JS_ATOM_TYPE_SYMBOL:
        switch(p->hash) {
        case JS_ATOM_HASH_SYMBOL:
            return JS_ATOM_KIND_SYMBOL;
        case JS_ATOM_HASH_PRIVATE:
            return JS_ATOM_KIND_PRIVATE;
        default:
            abort();
        }
    default:
        abort();
    }
}

/* string case (internal). Return JS_ATOM_NULL if error. 'str' is
   freed. */
static JSAtom __JS_NewAtom(JSRuntime *rt, JSString *str, int atom_type)
{
    uint32_t h, h1, i;
    JSAtomStruct *p;
    int len;
#if 0
    printf("__JS_NewAtom: ");  JS_DumpString(rt, str); printf("\n");
#endif
    if (atom_type < JS_ATOM_TYPE_SYMBOL) {
        /* str is not NULL */
        if (str->atom_type == atom_type) {
            /* str is the atom, return its index */
            i = js_get_atom_index(rt, str);
            /* reduce string refcount and increase atom's unless constant */
            if (__JS_AtomIsConst(i))
                str->header.ref_count--;
            return i;
        }
        /* try and locate an already registered atom */
        len = str->len;
        h = hash_string(str, atom_type);
        h &= JS_ATOM_HASH_MASK;
        h1 = h & (rt->atom_hash_size - 1);
        i = rt->atom_hash[h1];
        while (i != 0) {
            p = rt->atom_array[i];
            if (p->hash == h &&
                p->atom_type == atom_type &&
                p->len == len &&
                js_string_memcmp(p, str, len) == 0) {
                if (!__JS_AtomIsConst(i))
                    p->header.ref_count++;
                goto done;
            }
            i = p->hash_next;
        }
    } else {
        h1 = 0; /* avoid warning */
        if (atom_type == JS_ATOM_TYPE_SYMBOL) {
            h = JS_ATOM_HASH_SYMBOL;
        } else {
            h = JS_ATOM_HASH_PRIVATE;
            atom_type = JS_ATOM_TYPE_SYMBOL;
        }
    }
    if (rt->atom_free_index == 0) {
        /* allow new atom entries */
        uint32_t new_size, start;
        JSAtomStruct **new_array;
        /* alloc new with size progression 3/2:
           4 6 9 13 19 28 42 63 94 141 211 316 474 711 1066 1599 2398 3597 5395 8092
           preallocating space for predefined atoms (at least 195).
         */
        new_size = max_int(211, rt->atom_size * 3 / 2);
        if (new_size > JS_ATOM_MAX)
            goto fail;
        /* XXX: should use realloc2 to use slack space */
        new_array = js_realloc_rt(rt, rt->atom_array, sizeof(*new_array) * new_size);
        if (!new_array)
            goto fail;
        /* Note: the atom 0 is not used */
        start = rt->atom_size;
        if (start == 0) {
            /* JS_ATOM_NULL entry */
            p = js_mallocz_rt(rt, sizeof(JSAtomStruct));
            if (!p) {
                js_free_rt(rt, new_array);
                goto fail;
            }
            p->header.ref_count = 1;  /* not refcounted */
            p->atom_type = JS_ATOM_TYPE_SYMBOL;
#ifdef DUMP_LEAKS
            list_add_tail(&p->link, &rt->string_list);
#endif
            new_array[0] = p;
            rt->atom_count++;
            start = 1;
        }
        rt->atom_size = new_size;
        rt->atom_array = new_array;
        rt->atom_free_index = start;
        for(i = start; i < new_size; i++) {
            uint32_t next;
            if (i == (new_size - 1))
                next = 0;
            else
                next = i + 1;
            rt->atom_array[i] = atom_set_free(next);
        }
    }
    if (str) {
        if (str->atom_type == 0) {
            p = str;
            p->atom_type = atom_type;
        } else {
            p = js_malloc_rt(rt, sizeof(JSString) +
                             (str->len << str->is_wide_char) +
                             1 - str->is_wide_char);
            if (UNLIKELY(!p))
                goto fail;
            p->header.ref_count = 1;
            p->is_wide_char = str->is_wide_char;
            p->len = str->len;
#ifdef DUMP_LEAKS
            list_add_tail(&p->link, &rt->string_list);
#endif
            memcpy(p->u.str8, str->u.str8, (str->len << str->is_wide_char) +
                   1 - str->is_wide_char);
            js_free_string(rt, str);
        }
    } else {
        p = js_malloc_rt(rt, sizeof(JSAtomStruct)); /* empty wide string */
        if (!p)
            return JS_ATOM_NULL;
        p->header.ref_count = 1;
        p->is_wide_char = 1;    /* Hack to represent NULL as a JSString */
        p->len = 0;
#ifdef DUMP_LEAKS
        list_add_tail(&p->link, &rt->string_list);
#endif
    }
    /* use an already free entry */
    i = rt->atom_free_index;
    rt->atom_free_index = atom_get_free(rt->atom_array[i]);
    rt->atom_array[i] = p;
    p->hash = h;
    p->hash_next = i;   /* atom_index */
    p->atom_type = atom_type;
    rt->atom_count++;
    if (atom_type != JS_ATOM_TYPE_SYMBOL) {
        p->hash_next = rt->atom_hash[h1];
        rt->atom_hash[h1] = i;
        if (UNLIKELY(rt->atom_count >= rt->atom_count_resize))
            JS_ResizeAtomHash(rt, rt->atom_hash_size * 2);
    }
    //    JS_DumpAtoms(rt);
    return i;
 fail:
    i = JS_ATOM_NULL;
 done:
    if (str)
        js_free_string(rt, str);
    return i;
}

/* only works with zero terminated 8 bit strings */
static JSAtom __JS_NewAtomInit(JSRuntime *rt, const char *str, int len,
                               int atom_type)
{
    JSString *p;
    p = js_alloc_string_rt(rt, len, 0);
    if (!p)
        return JS_ATOM_NULL;
    memcpy(p->u.str8, str, len);
    p->u.str8[len] = '\0';
    return __JS_NewAtom(rt, p, atom_type);
}

static JSAtom __JS_FindAtom(JSRuntime *rt, const char *str, size_t len,
                            int atom_type)
{
    uint32_t h, h1, i;
    JSAtomStruct *p;
    h = hash_string8((const uint8_t *)str, len, JS_ATOM_TYPE_STRING);
    h &= JS_ATOM_HASH_MASK;
    h1 = h & (rt->atom_hash_size - 1);
    i = rt->atom_hash[h1];
    while (i != 0) {
        p = rt->atom_array[i];
        if (p->hash == h &&
            p->atom_type == JS_ATOM_TYPE_STRING &&
            p->len == len &&
            p->is_wide_char == 0 &&
            memcmp(p->u.str8, str, len) == 0) {
            if (!__JS_AtomIsConst(i))
                p->header.ref_count++;
            return i;
        }
        i = p->hash_next;
    }
    return JS_ATOM_NULL;
}

static void __JS_FreeAtom(JSRuntime *rt, uint32_t i)
{
    JSAtomStruct *p;
    p = rt->atom_array[i];
    if (--p->header.ref_count > 0)
        return;
    JS_FreeAtomStruct(rt, p);
}

/* Warning: 'p' is freed */
JSAtom JS_NewAtomStr(JSContext *ctx, JSString *p)
{
    JSRuntime *rt = ctx->rt;
    uint32_t n;
    if (is_num_string(&n, p)) {
        if (n <= JS_ATOM_MAX_INT) {
            js_free_string(rt, p);
            return __JS_AtomFromUInt32(n);
        }
    }
    /* XXX: should generate an exception */
    return __JS_NewAtom(rt, p, JS_ATOM_TYPE_STRING);
}

JSAtom JS_NewAtomLen(JSContext *ctx, const char *str, size_t len)
{
    JSValue val;
    if (len == 0 || !isdigit(*str)) {
        JSAtom atom = __JS_FindAtom(ctx->rt, str, len, JS_ATOM_TYPE_STRING);
        if (atom)
            return atom;
    }
    val = JS_NewStringLen(ctx, str, len);
    if (JS_IsException(val))
        return JS_ATOM_NULL;
    return JS_NewAtomStr(ctx, JS_VALUE_GET_STRING(val));
}

JSAtom JS_NewAtom(JSContext *ctx, const char *str)
{
    return JS_NewAtomLen(ctx, str, strlen(str));
}

JSAtom JS_NewAtomUInt32(JSContext *ctx, uint32_t n)
{
    if (n <= JS_ATOM_MAX_INT) {
        return __JS_AtomFromUInt32(n);
    } else {
        char buf[11];
        JSValue val;
        snprintf(buf, sizeof(buf), "%u", n);
        val = JS_NewString(ctx, buf);
        if (JS_IsException(val))
            return JS_ATOM_NULL;
        return __JS_NewAtom(ctx->rt, JS_VALUE_GET_STRING(val),
                            JS_ATOM_TYPE_STRING);
    }
}

JSAtom JS_NewAtomInt64(JSContext *ctx, int64_t n)
{
    if ((uint64_t)n <= JS_ATOM_MAX_INT) {
        return __JS_AtomFromUInt32((uint32_t)n);
    } else {
        char buf[24];
        JSValue val;
        snprintf(buf, sizeof(buf), "%" PRId64 , n);
        val = JS_NewString(ctx, buf);
        if (JS_IsException(val))
            return JS_ATOM_NULL;
        return __JS_NewAtom(ctx->rt, JS_VALUE_GET_STRING(val),
                            JS_ATOM_TYPE_STRING);
    }
}

/* 'p' is freed */
JSValue JS_NewSymbol(JSContext *ctx, JSString *p, int atom_type)
{
    JSRuntime *rt = ctx->rt;
    JSAtom atom;
    atom = __JS_NewAtom(rt, p, atom_type);
    if (atom == JS_ATOM_NULL)
        return JS_ThrowOutOfMemory(ctx);
    return JS_MKPTR(JS_TAG_SYMBOL, rt->atom_array[atom]);
}

/* This test must be fast if atom is not a numeric index (e.g. a
   method name). Return JS_UNDEFINED if not a numeric
   index. JS_EXCEPTION can also be returned. */
static JSValue JS_AtomIsNumericIndex1(JSContext *ctx, JSAtom atom)
{
    JSRuntime *rt = ctx->rt;
    JSAtomStruct *p1;
    JSString *p;
    int c, len, ret;
    JSValue num, str;

    if (__JS_AtomIsTaggedInt(atom))
        return JS_NewInt32(ctx, __JS_AtomToUInt32(atom));
    assert(atom < rt->atom_size);
    p1 = rt->atom_array[atom];
    if (p1->atom_type != JS_ATOM_TYPE_STRING)
        return JS_UNDEFINED;
    p = p1;
    len = p->len;
    if (p->is_wide_char) {
        const uint16_t *r = p->u.str16, *r_end = p->u.str16 + len;
        if (r >= r_end)
            return JS_UNDEFINED;
        c = *r;
        if (c == '-') {
            if (r >= r_end)
                return JS_UNDEFINED;
            r++;
            c = *r;
            /* -0 case is specific */
            if (c == '0' && len == 2)
                goto minus_zero;
        }
        /* XXX: should test NaN, but the tests do not check it */
        if (!isdigit(c)) {
            /* XXX: String should be normalized, therefore 8-bit only */
            const uint16_t nfinity16[7] = { 'n', 'f', 'i', 'n', 'i', 't', 'y' };
            if (!(c =='I' && (r_end - r) == 8 &&
                  !memcmp(r + 1, nfinity16, sizeof(nfinity16))))
                return JS_UNDEFINED;
        }
    } else {
        const uint8_t *r = p->u.str8, *r_end = p->u.str8 + len;
        if (r >= r_end)
            return JS_UNDEFINED;
        c = *r;
        if (c == '-') {
            if (r >= r_end)
                return JS_UNDEFINED;
            r++;
            c = *r;
            /* -0 case is specific */
            if (c == '0' && len == 2) {
            minus_zero:
                return __JS_NewFloat64(ctx, -0.0);
            }
        }
        if (!isdigit(c)) {
            if (!(c =='I' && (r_end - r) == 8 &&
                  !memcmp(r + 1, "nfinity", 7)))
                return JS_UNDEFINED;
        }
    }
    /* XXX: bignum: would be better to only accept integer to avoid
       relying on current floating point precision */
    /* this is ECMA CanonicalNumericIndexString primitive */
    num = JS_ToNumber(ctx, JS_MKPTR(JS_TAG_STRING, p));
    if (JS_IsException(num))
        return num;
    str = JS_ToString(ctx, num);
    if (JS_IsException(str)) {
        JS_FreeValue(ctx, num);
        return str;
    }
    ret = js_string_compare(ctx, p, JS_VALUE_GET_STRING(str));
    JS_FreeValue(ctx, str);
    if (ret == 0) {
        return num;
    } else {
        JS_FreeValue(ctx, num);
        return JS_UNDEFINED;
    }
}

/* return -1 if exception or TRUE/FALSE */
static int JS_AtomIsNumericIndex(JSContext *ctx, JSAtom atom)
{
    JSValue num;
    num = JS_AtomIsNumericIndex1(ctx, atom);
    if (LIKELY(JS_IsUndefined(num)))
        return FALSE;
    if (JS_IsException(num))
        return -1;
    JS_FreeValue(ctx, num);
    return TRUE;
}

void JS_FreeAtom(JSContext *ctx, JSAtom v)
{
    if (!__JS_AtomIsConst(v))
        __JS_FreeAtom(ctx->rt, v);
}

void JS_FreeAtomRT(JSRuntime *rt, JSAtom v)
{
    if (!__JS_AtomIsConst(v))
        __JS_FreeAtom(rt, v);
}

/* return TRUE if 'v' is a symbol with a string description */
static BOOL JS_AtomSymbolHasDescription(JSContext *ctx, JSAtom v)
{
    JSRuntime *rt;
    JSAtomStruct *p;
    rt = ctx->rt;
    if (__JS_AtomIsTaggedInt(v))
        return FALSE;
    p = rt->atom_array[v];
    return (((p->atom_type == JS_ATOM_TYPE_SYMBOL &&
              p->hash == JS_ATOM_HASH_SYMBOL) ||
             p->atom_type == JS_ATOM_TYPE_GLOBAL_SYMBOL) &&
            !(p->len == 0 && p->is_wide_char != 0));
}

static __maybe_unused void print_atom(JSContext *ctx, JSAtom atom)
{
    char buf[ATOM_GET_STR_BUF_SIZE];
    const char *p;
    int i;
    /* XXX: should handle embedded null characters */
    /* XXX: should move encoding code to JS_AtomGetStr */
    p = JS_AtomGetStr(ctx, buf, sizeof(buf), atom);
    for (i = 0; p[i]; i++) {
        int c = (unsigned char)p[i];
        if (!((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') ||
              (c == '_' || c == '$') || (c >= '0' && c <= '9' && i > 0)))
            break;
    }
    if (i > 0 && p[i] == '\0') {
        printf("%s", p);
    } else {
        putchar('"');
        printf("%.*s", i, p);
        for (; p[i]; i++) {
            int c = (unsigned char)p[i];
            if (c == '\"' || c == '\\') {
                putchar('\\');
                putchar(c);
            } else if (c >= ' ' && c <= 126) {
                putchar(c);
            } else if (c == '\n') {
                putchar('\\');
                putchar('n');
            } else {
                printf("\\u%04x", c);
            }
        }
        putchar('\"');
    }
}

/* free with JS_FreeCString() */
const char *JS_AtomToCString(JSContext *ctx, JSAtom atom)
{
    JSValue str;
    const char *cstr;
    str = JS_AtomToString(ctx, atom);
    if (JS_IsException(str))
        return NULL;
    cstr = JS_ToCString(ctx, str);
    JS_FreeValue(ctx, str);
    return cstr;
}

/* JSClass support */

/* a new class ID is allocated if *pclass_id != 0 */
JSClassID JS_NewClassID(JSClassID *pclass_id)
{
    JSClassID class_id;
    /* XXX: make it thread safe */
    class_id = *pclass_id;
    if (class_id == 0) {
        class_id = js_class_id_alloc++;
        *pclass_id = class_id;
    }
    return class_id;
}

BOOL JS_IsRegisteredClass(JSRuntime *rt, JSClassID class_id)
{
    return (class_id < rt->class_count &&
            rt->class_array[class_id].class_id != 0);
}

/* create a new object internal class. Return -1 if error, 0 if
   OK. The finalizer can be NULL if none is needed. */
static int JS_NewClass1(JSRuntime *rt, JSClassID class_id,
                        const JSClassDef *class_def, JSAtom name)
{
    int new_size, i;
    JSClass *cl, *new_class_array;
    struct list_head *el;
    if (class_id >= (1 << 16))
        return -1;
    if (class_id < rt->class_count &&
        rt->class_array[class_id].class_id != 0)
        return -1;
    if (class_id >= rt->class_count) {
        new_size = max_int(JS_CLASS_INIT_COUNT,
                           max_int(class_id + 1, rt->class_count * 3 / 2));
        /* reallocate the context class prototype array, if any */
        list_for_each(el, &rt->context_list) {
            JSContext *ctx = list_entry(el, JSContext, link);
            JSValue *new_tab;
            new_tab = js_realloc_rt(rt, ctx->class_proto,
                                    sizeof(ctx->class_proto[0]) * new_size);
            if (!new_tab)
                return -1;
            for(i = rt->class_count; i < new_size; i++)
                new_tab[i] = JS_NULL;
            ctx->class_proto = new_tab;
        }
        /* reallocate the class array */
        new_class_array = js_realloc_rt(rt, rt->class_array,
                                        sizeof(JSClass) * new_size);
        if (!new_class_array)
            return -1;
        bzero(new_class_array + rt->class_count,
              (new_size - rt->class_count) * sizeof(JSClass));
        rt->class_array = new_class_array;
        rt->class_count = new_size;
    }
    cl = &rt->class_array[class_id];
    cl->class_id = class_id;
    cl->class_name = JS_DupAtomRT(rt, name);
    cl->finalizer = class_def->finalizer;
    cl->gc_mark = class_def->gc_mark;
    cl->call = class_def->call;
    cl->exotic = class_def->exotic;
    return 0;
}

int JS_NewClass(JSRuntime *rt, JSClassID class_id, const JSClassDef *class_def)
{
    int ret, len;
    JSAtom name;
    len = strlen(class_def->class_name);
    name = __JS_FindAtom(rt, class_def->class_name, len, JS_ATOM_TYPE_STRING);
    if (name == JS_ATOM_NULL) {
        name = __JS_NewAtomInit(rt, class_def->class_name, len, JS_ATOM_TYPE_STRING);
        if (name == JS_ATOM_NULL)
            return -1;
    }
    ret = JS_NewClass1(rt, class_id, class_def, name);
    JS_FreeAtomRT(rt, name);
    return ret;
}

/* create a string from a UTF-8 buffer */
JSValue JS_NewStringLen(JSContext *ctx, const char *buf, size_t buf_len)
{
    const uint8_t *p, *p_end, *p_start, *p_next;
    uint32_t c;
    StringBuffer b_s, *b = &b_s;
    size_t len1;
    p_start = (const uint8_t *)buf;
    p_end = p_start + buf_len;
    p = p_start;
    while (p < p_end && *p < 128)
        p++;
    len1 = p - p_start;
    if (len1 > JS_STRING_LEN_MAX)
        return JS_ThrowInternalError(ctx, "string too long");
    if (p == p_end) {
        /* ASCII string */
        return js_new_string8(ctx, (const uint8_t *)buf, buf_len);
    } else {
        if (string_buffer_init(ctx, b, buf_len))
            goto fail;
        string_buffer_write8(b, p_start, len1);
        while (p < p_end) {
            if (*p < 128) {
                string_buffer_putc8(b, *p++);
            } else {
                /* parse utf-8 sequence, return 0xFFFFFFFF for error */
                c = unicode_from_utf8(p, p_end - p, &p_next);
                if (c < 0x10000) {
                    p = p_next;
                } else if (c <= 0x10FFFF) {
                    p = p_next;
                    /* surrogate pair */
                    c -= 0x10000;
                    string_buffer_putc16(b, (c >> 10) + 0xd800);
                    c = (c & 0x3ff) + 0xdc00;
                } else {
                    /* invalid char */
                    c = 0xfffd;
                    /* skip the invalid chars */
                    /* XXX: seems incorrect. Why not just use c = *p++; ? */
                    while (p < p_end && (*p >= 0x80 && *p < 0xc0))
                        p++;
                    if (p < p_end) {
                        p++;
                        while (p < p_end && (*p >= 0x80 && *p < 0xc0))
                            p++;
                    }
                }
                string_buffer_putc16(b, c);
            }
        }
    }
    return string_buffer_end(b);
 fail:
    string_buffer_free(b);
    return JS_EXCEPTION;
}

JSValue JS_ConcatString3(JSContext *ctx, const char *str1,
                         JSValue str2, const char *str3)
{
    StringBuffer b_s, *b = &b_s;
    int len1, len3;
    JSString *p;
    if (UNLIKELY(JS_VALUE_GET_TAG(str2) != JS_TAG_STRING)) {
        str2 = JS_ToStringFree(ctx, str2);
        if (JS_IsException(str2))
            goto fail;
    }
    p = JS_VALUE_GET_STRING(str2);
    len1 = strlen(str1);
    len3 = strlen(str3);
    if (string_buffer_init2(ctx, b, len1 + p->len + len3, p->is_wide_char))
        goto fail;
    string_buffer_write8(b, (const uint8_t *)str1, len1);
    string_buffer_concat(b, p, 0, p->len);
    string_buffer_write8(b, (const uint8_t *)str3, len3);
    JS_FreeValue(ctx, str2);
    return string_buffer_end(b);
 fail:
    JS_FreeValue(ctx, str2);
    return JS_EXCEPTION;
}

JSValue JS_NewString(JSContext *ctx, const char *str)
{
    return JS_NewStringLen(ctx, str, strlen(str));
}

JSValue JS_NewAtomString(JSContext *ctx, const char *str)
{
    JSAtom atom = JS_NewAtom(ctx, str);
    if (atom == JS_ATOM_NULL)
        return JS_EXCEPTION;
    JSValue val = JS_AtomToString(ctx, atom);
    JS_FreeAtom(ctx, atom);
    return val;
}

/* return (NULL, 0) if exception. */
/* return pointer into a JSString with a live ref_count */
/* cesu8 determines if non-BMP1 codepoints are encoded as 1 or 2 utf-8 sequences */
const char *JS_ToCStringLen2(JSContext *ctx, size_t *plen, JSValueConst val1, BOOL cesu8)
{
    JSValue val;
    JSString *str, *str_new;
    int pos, len, c, c1;
    uint8_t *q;
    if (JS_VALUE_GET_TAG(val1) != JS_TAG_STRING) {
        val = JS_ToString(ctx, val1);
        if (JS_IsException(val))
            goto fail;
    } else {
        val = JS_DupValue(ctx, val1);
    }
    str = JS_VALUE_GET_STRING(val);
    len = str->len;
    if (!str->is_wide_char) {
        const uint8_t *src = str->u.str8;
        int count;
        /* count the number of non-ASCII characters */
        /* Scanning the whole string is required for ASCII strings,
           and computing the number of non-ASCII bytes is less expensive
           than testing each byte, hence this method is faster for ASCII
           strings, which is the most common case.
         */
        count = 0;
        for (pos = 0; pos < len; pos++) {
            count += src[pos] >> 7;
        }
        if (count == 0) {
            if (plen)
                *plen = len;
            return (const char *)src;
        }
        str_new = js_alloc_string(ctx, len + count, 0);
        if (!str_new)
            goto fail;
        q = str_new->u.str8;
        for (pos = 0; pos < len; pos++) {
            c = src[pos];
            if (c < 0x80) {
                *q++ = c;
            } else {
                *q++ = (c >> 6) | 0xc0;
                *q++ = (c & 0x3f) | 0x80;
            }
        }
    } else {
        const uint16_t *src = str->u.str16;
        /* Allocate 3 bytes per 16 bit code point. Surrogate pairs may
           produce 4 bytes but use 2 code points.
         */
        str_new = js_alloc_string(ctx, len * 3, 0);
        if (!str_new)
            goto fail;
        q = str_new->u.str8;
        pos = 0;
        while (pos < len) {
            c = src[pos++];
            if (c < 0x80) {
                *q++ = c;
            } else {
                if (c >= 0xd800 && c < 0xdc00) {
                    if (pos < len && !cesu8) {
                        c1 = src[pos];
                        if (c1 >= 0xdc00 && c1 < 0xe000) {
                            pos++;
                            /* surrogate pair */
                            c = (((c & 0x3ff) << 10) | (c1 & 0x3ff)) + 0x10000;
                        } else {
                            /* Keep unmatched surrogate code points */
                            /* c = 0xfffd; */ /* error */
                        }
                    } else {
                        /* Keep unmatched surrogate code points */
                        /* c = 0xfffd; */ /* error */
                    }
                }
                q += unicode_to_utf8(q, c);
            }
        }
    }
    *q = '\0';
    str_new->len = q - str_new->u.str8;
    JS_FreeValue(ctx, val);
    if (plen)
        *plen = str_new->len;
    return (const char *)str_new->u.str8;
 fail:
    if (plen)
        *plen = 0;
    return NULL;
}

void JS_FreeCString(JSContext *ctx, const char *ptr)
{
    JSString *p;
    if (!ptr)
        return;
    /* purposely removing constness */
    p = (JSString *)(void *)(ptr - offsetof(JSString, u));
    JS_FreeValue(ctx, JS_MKPTR(JS_TAG_STRING, p));
}

static void copy_str16(uint16_t *dst, const JSString *p, int offset, int len)
{
    if (p->is_wide_char) {
        memcpy(dst, p->u.str16 + offset, len * 2);
    } else {
        const uint8_t *src1 = p->u.str8 + offset;
        int i;
        for(i = 0; i < len; i++)
            dst[i] = src1[i];
    }
}

static JSValue JS_ConcatString1(JSContext *ctx,
                                const JSString *p1, const JSString *p2)
{
    JSString *p;
    uint32_t len;
    int is_wide_char;

    len = p1->len + p2->len;
    if (len > JS_STRING_LEN_MAX)
        return JS_ThrowInternalError(ctx, "string too long");
    is_wide_char = p1->is_wide_char | p2->is_wide_char;
    p = js_alloc_string(ctx, len, is_wide_char);
    if (!p)
        return JS_EXCEPTION;
    if (!is_wide_char) {
        memcpy(p->u.str8, p1->u.str8, p1->len);
        memcpy(p->u.str8 + p1->len, p2->u.str8, p2->len);
        p->u.str8[len] = '\0';
    } else {
        copy_str16(p->u.str16, p1, 0, p1->len);
        copy_str16(p->u.str16 + p1->len, p2, 0, p2->len);
    }
    return JS_MKPTR(JS_TAG_STRING, p);
}

/* op1 and op2 are converted to strings. For convience, op1 or op2 =
   JS_EXCEPTION are accepted and return JS_EXCEPTION.  */
JSValue JS_ConcatString(JSContext *ctx, JSValue op1, JSValue op2)
{
    JSValue ret;
    JSString *p1, *p2;
    if (UNLIKELY(JS_VALUE_GET_TAG(op1) != JS_TAG_STRING)) {
        op1 = JS_ToStringFree(ctx, op1);
        if (JS_IsException(op1)) {
            JS_FreeValue(ctx, op2);
            return JS_EXCEPTION;
        }
    }
    if (UNLIKELY(JS_VALUE_GET_TAG(op2) != JS_TAG_STRING)) {
        op2 = JS_ToStringFree(ctx, op2);
        if (JS_IsException(op2)) {
            JS_FreeValue(ctx, op1);
            return JS_EXCEPTION;
        }
    }
    p1 = JS_VALUE_GET_STRING(op1);
    p2 = JS_VALUE_GET_STRING(op2);
    /* XXX: could also check if p1 is empty */
    if (p2->len == 0) {
        goto ret_op1;
    }
    if (p1->header.ref_count == 1 && p1->is_wide_char == p2->is_wide_char
    &&  js_malloc_usable_size(ctx, p1) >= sizeof(*p1) + ((p1->len + p2->len) << p2->is_wide_char) + 1 - p1->is_wide_char) {
        /* Concatenate in place in available space at the end of p1 */
        if (p1->is_wide_char) {
            memcpy(p1->u.str16 + p1->len, p2->u.str16, p2->len << 1);
            p1->len += p2->len;
        } else {
            memcpy(p1->u.str8 + p1->len, p2->u.str8, p2->len);
            p1->len += p2->len;
            p1->u.str8[p1->len] = '\0';
        }
    ret_op1:
        JS_FreeValue(ctx, op2);
        return op1;
    }
    ret = JS_ConcatString1(ctx, p1, p2);
    JS_FreeValue(ctx, op1);
    JS_FreeValue(ctx, op2);
    return ret;
}

/* make space to hold at least 'count' properties */
int resize_properties(JSContext *ctx, JSShape **psh, JSObject *p, uint32_t count)
{
    JSShape *sh;
    uint32_t new_size, new_hash_size, new_hash_mask, i;
    JSShapeProperty *pr;
    void *sh_alloc;
    intptr_t h;
    sh = *psh;
    new_size = max_int(count, sh->prop_size * 3 / 2);
    /* Reallocate prop array first to avoid crash or size inconsistency
       in case of memory allocation failure */
    if (p) {
        JSProperty *new_prop;
        new_prop = js_realloc(ctx, p->prop, sizeof(new_prop[0]) * new_size);
        if (UNLIKELY(!new_prop))
            return -1;
        p->prop = new_prop;
    }
    new_hash_size = sh->prop_hash_mask + 1;
    while (new_hash_size < new_size)
        new_hash_size = 2 * new_hash_size;
    if (new_hash_size != (sh->prop_hash_mask + 1)) {
        JSShape *old_sh;
        /* resize the hash table and the properties */
        old_sh = sh;
        sh_alloc = js_malloc(ctx, get_shape_size(new_hash_size, new_size));
        if (!sh_alloc)
            return -1;
        sh = get_shape_from_alloc(sh_alloc, new_hash_size);
        list_del(&old_sh->header.link);
        /* copy all the fields and the properties */
        memcpy(sh, old_sh,
               sizeof(JSShape) + sizeof(sh->prop[0]) * old_sh->prop_count);
        list_add_tail(&sh->header.link, &ctx->rt->gc_obj_list);
        new_hash_mask = new_hash_size - 1;
        sh->prop_hash_mask = new_hash_mask;
        bzero(prop_hash_end(sh) - new_hash_size,
              sizeof(prop_hash_end(sh)[0]) * new_hash_size);
        for(i = 0, pr = sh->prop; i < sh->prop_count; i++, pr++) {
            if (pr->atom != JS_ATOM_NULL) {
                h = ((uintptr_t)pr->atom & new_hash_mask);
                pr->hash_next = prop_hash_end(sh)[-h - 1];
                prop_hash_end(sh)[-h - 1] = i + 1;
            }
        }
        js_free(ctx, get_alloc_from_shape(old_sh));
    } else {
        /* only resize the properties */
        list_del(&sh->header.link);
        sh_alloc = js_realloc(ctx, get_alloc_from_shape(sh),
                              get_shape_size(new_hash_size, new_size));
        if (UNLIKELY(!sh_alloc)) {
            /* insert again in the GC list */
            list_add_tail(&sh->header.link, &ctx->rt->gc_obj_list);
            return -1;
        }
        sh = get_shape_from_alloc(sh_alloc, new_hash_size);
        list_add_tail(&sh->header.link, &ctx->rt->gc_obj_list);
    }
    *psh = sh;
    sh->prop_size = new_size;
    return 0;
}

/* remove the deleted properties. */
static int compact_properties(JSContext *ctx, JSObject *p)
{
    JSShape *sh, *old_sh;
    void *sh_alloc;
    intptr_t h;
    uint32_t new_hash_size, i, j, new_hash_mask, new_size;
    JSShapeProperty *old_pr, *pr;
    JSProperty *prop, *new_prop;
    sh = p->shape;
    assert(!sh->is_hashed);
    new_size = max_int(JS_PROP_INITIAL_SIZE,
                       sh->prop_count - sh->deleted_prop_count);
    assert(new_size <= sh->prop_size);
    new_hash_size = sh->prop_hash_mask + 1;
    while ((new_hash_size / 2) >= new_size)
        new_hash_size = new_hash_size / 2;
    new_hash_mask = new_hash_size - 1;
    /* resize the hash table and the properties */
    old_sh = sh;
    sh_alloc = js_malloc(ctx, get_shape_size(new_hash_size, new_size));
    if (!sh_alloc)
        return -1;
    sh = get_shape_from_alloc(sh_alloc, new_hash_size);
    list_del(&old_sh->header.link);
    memcpy(sh, old_sh, sizeof(JSShape));
    list_add_tail(&sh->header.link, &ctx->rt->gc_obj_list);
    bzero(prop_hash_end(sh) - new_hash_size,
          sizeof(prop_hash_end(sh)[0]) * new_hash_size);
    j = 0;
    old_pr = old_sh->prop;
    pr = sh->prop;
    prop = p->prop;
    for(i = 0; i < sh->prop_count; i++) {
        if (old_pr->atom != JS_ATOM_NULL) {
            pr->atom = old_pr->atom;
            pr->flags = old_pr->flags;
            h = ((uintptr_t)old_pr->atom & new_hash_mask);
            pr->hash_next = prop_hash_end(sh)[-h - 1];
            prop_hash_end(sh)[-h - 1] = j + 1;
            prop[j] = prop[i];
            j++;
            pr++;
        }
        old_pr++;
    }
    assert(j == (sh->prop_count - sh->deleted_prop_count));
    sh->prop_hash_mask = new_hash_mask;
    sh->prop_size = new_size;
    sh->deleted_prop_count = 0;
    sh->prop_count = j;
    p->shape = sh;
    js_free(ctx, get_alloc_from_shape(old_sh));
    /* reduce the size of the object properties */
    new_prop = js_realloc(ctx, p->prop, sizeof(new_prop[0]) * new_size);
    if (new_prop)
        p->prop = new_prop;
    return 0;
}

/* WARNING: proto must be an object or JS_NULL */
JSValue JS_NewObjectProtoClass(JSContext *ctx, JSValueConst proto_val,
                               JSClassID class_id)
{
    JSShape *sh;
    JSObject *proto;
    proto = get_proto_obj(proto_val);
    sh = find_hashed_shape_proto(ctx->rt, proto);
    if (LIKELY(sh)) {
        sh = js_dup_shape(sh);
    } else {
        sh = js_new_shape(ctx, proto);
        if (!sh)
            return JS_EXCEPTION;
    }
    return JS_NewObjectFromShape(ctx, sh, class_id);
}

#if 0
static JSValue JS_GetObjectData(JSContext *ctx, JSValueConst obj)
{
    JSObject *p;
    if (JS_VALUE_GET_TAG(obj) == JS_TAG_OBJECT) {
        p = JS_VALUE_GET_OBJ(obj);
        switch(p->class_id) {
        case JS_CLASS_NUMBER:
        case JS_CLASS_STRING:
        case JS_CLASS_BOOLEAN:
        case JS_CLASS_SYMBOL:
        case JS_CLASS_DATE:
#ifdef CONFIG_BIGNUM
        case JS_CLASS_BIG_INT:
        case JS_CLASS_BIG_FLOAT:
        case JS_CLASS_BIG_DECIMAL:
#endif
            return JS_DupValue(ctx, p->u.object_data);
        }
    }
    return JS_UNDEFINED;
}
#endif

int JS_SetObjectData(JSContext *ctx, JSValueConst obj, JSValue val)
{
    JSObject *p;
    if (JS_VALUE_GET_TAG(obj) == JS_TAG_OBJECT) {
        p = JS_VALUE_GET_OBJ(obj);
        switch(p->class_id) {
        case JS_CLASS_NUMBER:
        case JS_CLASS_STRING:
        case JS_CLASS_BOOLEAN:
        case JS_CLASS_SYMBOL:
        case JS_CLASS_DATE:
#ifdef CONFIG_BIGNUM
        case JS_CLASS_BIG_INT:
        case JS_CLASS_BIG_FLOAT:
        case JS_CLASS_BIG_DECIMAL:
#endif
            JS_FreeValue(ctx, p->u.object_data);
            p->u.object_data = val;
            return 0;
        }
    }
    JS_FreeValue(ctx, val);
    if (!JS_IsException(obj))
        JS_ThrowTypeError(ctx, "invalid object type");
    return -1;
}

JSValue JS_NewObjectClass(JSContext *ctx, int class_id)
{
    return JS_NewObjectProtoClass(ctx, ctx->class_proto[class_id], class_id);
}

JSValue JS_NewObjectProto(JSContext *ctx, JSValueConst proto)
{
    return JS_NewObjectProtoClass(ctx, proto, JS_CLASS_OBJECT);
}

JSValue JS_NewArray(JSContext *ctx)
{
    return JS_NewObjectFromShape(ctx, js_dup_shape(ctx->array_shape),
                                 JS_CLASS_ARRAY);
}

JSValue JS_NewObject(JSContext *ctx)
{
    /* inline JS_NewObjectClass(ctx, JS_CLASS_OBJECT); */
    return JS_NewObjectProtoClass(ctx, ctx->class_proto[JS_CLASS_OBJECT], JS_CLASS_OBJECT);
}

void js_function_set_properties(JSContext *ctx, JSValueConst func_obj, JSAtom name, int len)
{
    /* ES6 feature non compatible with ES5.1: length is configurable */
    JS_DefinePropertyValue(ctx, func_obj, JS_ATOM_length, JS_NewInt32(ctx, len),
                           JS_PROP_CONFIGURABLE);
    JS_DefinePropertyValue(ctx, func_obj, JS_ATOM_name,
                           JS_AtomToString(ctx, name), JS_PROP_CONFIGURABLE);
}

void js_method_set_home_object(JSContext *ctx, JSValueConst func_obj, JSValueConst home_obj)
{
    JSObject *p, *p1;
    JSFunctionBytecode *b;

    if (JS_VALUE_GET_TAG(func_obj) != JS_TAG_OBJECT)
        return;
    p = JS_VALUE_GET_OBJ(func_obj);
    if (!js_class_has_bytecode(p->class_id))
        return;
    b = p->u.func.function_bytecode;
    if (b->need_home_object) {
        p1 = p->u.func.home_object;
        if (p1) {
            JS_FreeValue(ctx, JS_MKPTR(JS_TAG_OBJECT, p1));
        }
        if (JS_VALUE_GET_TAG(home_obj) == JS_TAG_OBJECT)
            p1 = JS_VALUE_GET_OBJ(JS_DupValue(ctx, home_obj));
        else
            p1 = NULL;
        p->u.func.home_object = p1;
    }
}

JSValue js_get_function_name(JSContext *ctx, JSAtom name)
{
    JSValue name_str;
    name_str = JS_AtomToString(ctx, name);
    if (JS_AtomSymbolHasDescription(ctx, name)) {
        name_str = JS_ConcatString3(ctx, "[", name_str, "]");
    }
    return name_str;
}

/* Note: at least 'length' arguments will be readable in 'argv' */
JSValue JS_NewCFunction3(JSContext *ctx, JSCFunction *func,
                         const char *name,
                         int length, JSCFunctionEnum cproto, int magic,
                         JSValueConst proto_val)
{
    JSValue func_obj;
    JSObject *p;
    JSAtom name_atom;
    func_obj = JS_NewObjectProtoClass(ctx, proto_val, JS_CLASS_C_FUNCTION);
    if (JS_IsException(func_obj))
        return func_obj;
    p = JS_VALUE_GET_OBJ(func_obj);
    p->u.cfunc.realm = JS_DupContext(ctx);
    p->u.cfunc.c_function.generic = func;
    p->u.cfunc.length = length;
    p->u.cfunc.cproto = cproto;
    p->u.cfunc.magic = magic;
    p->is_constructor = (cproto == JS_CFUNC_constructor ||
                         cproto == JS_CFUNC_constructor_magic ||
                         cproto == JS_CFUNC_constructor_or_func ||
                         cproto == JS_CFUNC_constructor_or_func_magic);
    if (!name)
        name = "";
    name_atom = JS_NewAtom(ctx, name);
    js_function_set_properties(ctx, func_obj, name_atom, length);
    JS_FreeAtom(ctx, name_atom);
    return func_obj;
}

/* Note: at least 'length' arguments will be readable in 'argv' */
JSValue JS_NewCFunction2(JSContext *ctx, JSCFunction *func,
                         const char *name,
                         int length, JSCFunctionEnum cproto, int magic)
{
    return JS_NewCFunction3(ctx, func, name, length, cproto, magic,
                            ctx->function_proto);
}

static void js_c_function_data_finalizer(JSRuntime *rt, JSValue val)
{
    JSCFunctionDataRecord *s = JS_GetOpaque(val, JS_CLASS_C_FUNCTION_DATA);
    int i;
    if (s) {
        for(i = 0; i < s->data_len; i++) {
            JS_FreeValueRT(rt, s->data[i]);
        }
        js_free_rt(rt, s);
    }
}

static void js_c_function_data_mark(JSRuntime *rt, JSValueConst val,
                                    JS_MarkFunc *mark_func)
{
    JSCFunctionDataRecord *s = JS_GetOpaque(val, JS_CLASS_C_FUNCTION_DATA);
    int i;
    if (s) {
        for(i = 0; i < s->data_len; i++) {
            JS_MarkValue(rt, s->data[i], mark_func);
        }
    }
}

static JSValue js_c_function_data_call(JSContext *ctx, JSValueConst func_obj,
                                       JSValueConst this_val,
                                       int argc, JSValueConst *argv, int flags)
{
    JSCFunctionDataRecord *s = JS_GetOpaque(func_obj, JS_CLASS_C_FUNCTION_DATA);
    JSValueConst *arg_buf;
    int i;

    /* XXX: could add the function on the stack for debug */
    if (UNLIKELY(argc < s->length)) {
        arg_buf = alloca(sizeof(arg_buf[0]) * s->length);
        for(i = 0; i < argc; i++)
            arg_buf[i] = argv[i];
        for(i = argc; i < s->length; i++)
            arg_buf[i] = JS_UNDEFINED;
    } else {
        arg_buf = argv;
    }

    return s->func(ctx, this_val, argc, arg_buf, s->magic, s->data);
}

JSValue JS_NewCFunctionData(JSContext *ctx, JSCFunctionData *func,
                            int length, int magic, int data_len,
                            JSValueConst *data)
{
    JSCFunctionDataRecord *s;
    JSValue func_obj;
    int i;

    func_obj = JS_NewObjectProtoClass(ctx, ctx->function_proto,
                                      JS_CLASS_C_FUNCTION_DATA);
    if (JS_IsException(func_obj))
        return func_obj;
    s = js_malloc(ctx, sizeof(*s) + data_len * sizeof(JSValue));
    if (!s) {
        JS_FreeValue(ctx, func_obj);
        return JS_EXCEPTION;
    }
    s->func = func;
    s->length = length;
    s->data_len = data_len;
    s->magic = magic;
    for(i = 0; i < data_len; i++)
        s->data[i] = JS_DupValue(ctx, data[i]);
    JS_SetOpaque(func_obj, s);
    js_function_set_properties(ctx, func_obj,
                               JS_ATOM_empty_string, length);
    return func_obj;
}

static JSContext *js_autoinit_get_realm(JSProperty *pr)
{
    return (JSContext *)(pr->u.init.realm_and_id & ~3);
}

static JSAutoInitIDEnum js_autoinit_get_id(JSProperty *pr)
{
    return pr->u.init.realm_and_id & 3;
}

static void js_autoinit_free(JSRuntime *rt, JSProperty *pr)
{
    JS_FreeContext(js_autoinit_get_realm(pr));
}

void js_autoinit_mark(JSRuntime *rt, JSProperty *pr, JS_MarkFunc *mark_func)
{
    mark_func(rt, &js_autoinit_get_realm(pr)->header);
}

void free_property(JSRuntime *rt, JSProperty *pr, int prop_flags)
{
    if (UNLIKELY(prop_flags & JS_PROP_TMASK)) {
        if ((prop_flags & JS_PROP_TMASK) == JS_PROP_GETSET) {
            if (pr->u.getset.getter)
                JS_FreeValueRT(rt, JS_MKPTR(JS_TAG_OBJECT, pr->u.getset.getter));
            if (pr->u.getset.setter)
                JS_FreeValueRT(rt, JS_MKPTR(JS_TAG_OBJECT, pr->u.getset.setter));
        } else if ((prop_flags & JS_PROP_TMASK) == JS_PROP_VARREF) {
            free_var_ref(rt, pr->u.var_ref);
        } else if ((prop_flags & JS_PROP_TMASK) == JS_PROP_AUTOINIT) {
            js_autoinit_free(rt, pr);
        }
    } else {
        JS_FreeValueRT(rt, pr->u.value);
    }
}

void free_var_ref(JSRuntime *rt, JSVarRef *var_ref)
{
    if (var_ref) {
        assert(var_ref->header.ref_count > 0);
        if (--var_ref->header.ref_count == 0) {
            if (var_ref->is_detached) {
                JS_FreeValueRT(rt, var_ref->value);
                remove_gc_object(&var_ref->header);
            } else {
                list_del(&var_ref->header.link); /* still on the stack */
            }
            js_free_rt(rt, var_ref);
        }
    }
}

static void js_c_function_finalizer(JSRuntime *rt, JSValue val)
{
    JSObject *p = JS_VALUE_GET_OBJ(val);

    if (p->u.cfunc.realm)
        JS_FreeContext(p->u.cfunc.realm);
}

static void js_c_function_mark(JSRuntime *rt, JSValueConst val,
                               JS_MarkFunc *mark_func)
{
    JSObject *p = JS_VALUE_GET_OBJ(val);

    if (p->u.cfunc.realm)
        mark_func(rt, &p->u.cfunc.realm->header);
}

static void js_bound_function_finalizer(JSRuntime *rt, JSValue val)
{
    JSObject *p = JS_VALUE_GET_OBJ(val);
    JSBoundFunction *bf = p->u.bound_function;
    int i;

    JS_FreeValueRT(rt, bf->func_obj);
    JS_FreeValueRT(rt, bf->this_val);
    for(i = 0; i < bf->argc; i++) {
        JS_FreeValueRT(rt, bf->argv[i]);
    }
    js_free_rt(rt, bf);
}

static void js_bound_function_mark(JSRuntime *rt, JSValueConst val,
                                JS_MarkFunc *mark_func)
{
    JSObject *p = JS_VALUE_GET_OBJ(val);
    JSBoundFunction *bf = p->u.bound_function;
    int i;

    JS_MarkValue(rt, bf->func_obj, mark_func);
    JS_MarkValue(rt, bf->this_val, mark_func);
    for(i = 0; i < bf->argc; i++)
        JS_MarkValue(rt, bf->argv[i], mark_func);
}

static void js_for_in_iterator_finalizer(JSRuntime *rt, JSValue val)
{
    JSObject *p = JS_VALUE_GET_OBJ(val);
    JSForInIterator *it = p->u.for_in_iterator;
    JS_FreeValueRT(rt, it->obj);
    js_free_rt(rt, it);
}

static void js_for_in_iterator_mark(JSRuntime *rt, JSValueConst val,
                                JS_MarkFunc *mark_func)
{
    JSObject *p = JS_VALUE_GET_OBJ(val);
    JSForInIterator *it = p->u.for_in_iterator;
    JS_MarkValue(rt, it->obj, mark_func);
}

/* garbage collection */

JSValue JS_GetGlobalObject(JSContext *ctx)
{
    return JS_DupValue(ctx, ctx->global_obj);
}

/* WARNING: obj is freed */
JSValue JS_Throw(JSContext *ctx, JSValue obj)
{
    JSRuntime *rt = ctx->rt;
    JS_FreeValue(ctx, rt->current_exception);
    rt->current_exception = obj;
    return JS_EXCEPTION;
}

/* return the pending exception (cannot be called twice). */
JSValue JS_GetException(JSContext *ctx)
{
    JSValue val;
    JSRuntime *rt = ctx->rt;
    val = rt->current_exception;
    rt->current_exception = JS_NULL;
    return val;
}

int __js_poll_interrupts(JSContext *ctx)
{
    JSRuntime *rt = ctx->rt;
    ctx->interrupt_counter = JS_INTERRUPT_COUNTER_INIT;
    if (rt->interrupt_handler) {
        if (rt->interrupt_handler(rt, rt->interrupt_opaque)) {
            /* XXX: should set a specific flag to avoid catching */
            JS_ThrowInternalError(ctx, "interrupted");
            JS_SetUncatchableError(ctx, ctx->rt->current_exception, TRUE);
            return -1;
        }
    }
    return 0;
}

/* return -1 (exception) or TRUE/FALSE */
int JS_SetPrototypeInternal(JSContext *ctx, JSValueConst obj, JSValueConst proto_val, BOOL throw_flag)
{
    JSObject *proto, *p, *p1;
    JSShape *sh;
    if (throw_flag) {
        if (JS_VALUE_GET_TAG(obj) == JS_TAG_NULL ||
            JS_VALUE_GET_TAG(obj) == JS_TAG_UNDEFINED)
            goto not_obj;
    } else {
        if (JS_VALUE_GET_TAG(obj) != JS_TAG_OBJECT)
            goto not_obj;
    }
    p = JS_VALUE_GET_OBJ(obj);
    if (JS_VALUE_GET_TAG(proto_val) != JS_TAG_OBJECT) {
        if (JS_VALUE_GET_TAG(proto_val) != JS_TAG_NULL) {
        not_obj:
            JS_ThrowTypeErrorNotAnObject(ctx);
            return -1;
        }
        proto = NULL;
    } else {
        proto = JS_VALUE_GET_OBJ(proto_val);
    }
    if (throw_flag && JS_VALUE_GET_TAG(obj) != JS_TAG_OBJECT)
        return TRUE;
    if (UNLIKELY(p->class_id == JS_CLASS_PROXY))
        return js_proxy_setPrototypeOf(ctx, obj, proto_val, throw_flag);
    sh = p->shape;
    if (sh->proto == proto)
        return TRUE;
    if (!p->extensible) {
        if (throw_flag) {
            JS_ThrowTypeError(ctx, "object is not extensible");
            return -1;
        } else {
            return FALSE;
        }
    }
    if (proto) {
        /* check if there is a cycle */
        p1 = proto;
        do {
            if (p1 == p) {
                if (throw_flag) {
                    JS_ThrowTypeError(ctx, "circular prototype chain");
                    return -1;
                } else {
                    return FALSE;
                }
            }
            /* Note: for Proxy objects, proto is NULL */
            p1 = p1->shape->proto;
        } while (p1 != NULL);
        JS_DupValue(ctx, proto_val);
    }
    if (js_shape_prepare_update(ctx, p, NULL))
        return -1;
    sh = p->shape;
    if (sh->proto)
        JS_FreeValue(ctx, JS_MKPTR(JS_TAG_OBJECT, sh->proto));
    sh->proto = proto;
    return TRUE;
}

/* return -1 (exception) or TRUE/FALSE */
int JS_SetPrototype(JSContext *ctx, JSValueConst obj, JSValueConst proto_val)
{
    return JS_SetPrototypeInternal(ctx, obj, proto_val, TRUE);
}

/* Only works for primitive types, otherwise return JS_NULL. */
static JSValueConst JS_GetPrototypePrimitive(JSContext *ctx, JSValueConst val)
{
    switch(JS_VALUE_GET_NORM_TAG(val)) {
#ifdef CONFIG_BIGNUM
    case JS_TAG_BIG_INT:
        val = ctx->class_proto[JS_CLASS_BIG_INT];
        break;
    case JS_TAG_BIG_FLOAT:
        val = ctx->class_proto[JS_CLASS_BIG_FLOAT];
        break;
    case JS_TAG_BIG_DECIMAL:
        val = ctx->class_proto[JS_CLASS_BIG_DECIMAL];
        break;
#endif
    case JS_TAG_INT:
    case JS_TAG_FLOAT64:
        val = ctx->class_proto[JS_CLASS_NUMBER];
        break;
    case JS_TAG_BOOL:
        val = ctx->class_proto[JS_CLASS_BOOLEAN];
        break;
    case JS_TAG_STRING:
        val = ctx->class_proto[JS_CLASS_STRING];
        break;
    case JS_TAG_SYMBOL:
        val = ctx->class_proto[JS_CLASS_SYMBOL];
        break;
    case JS_TAG_OBJECT:
    case JS_TAG_NULL:
    case JS_TAG_UNDEFINED:
    default:
        val = JS_NULL;
        break;
    }
    return val;
}

/* Return an Object, JS_NULL or JS_EXCEPTION in case of Proxy object. */
JSValue JS_GetPrototype(JSContext *ctx, JSValueConst obj)
{
    JSValue val;
    if (JS_VALUE_GET_TAG(obj) == JS_TAG_OBJECT) {
        JSObject *p;
        p = JS_VALUE_GET_OBJ(obj);
        if (UNLIKELY(p->class_id == JS_CLASS_PROXY)) {
            val = js_proxy_getPrototypeOf(ctx, obj);
        } else {
            p = p->shape->proto;
            if (!p)
                val = JS_NULL;
            else
                val = JS_DupValue(ctx, JS_MKPTR(JS_TAG_OBJECT, p));
        }
    } else {
        val = JS_DupValue(ctx, JS_GetPrototypePrimitive(ctx, obj));
    }
    return val;
}

JSValue JS_GetPrototypeFree(JSContext *ctx, JSValue obj)
{
    JSValue obj1;
    obj1 = JS_GetPrototype(ctx, obj);
    JS_FreeValue(ctx, obj);
    return obj1;
}

/* return TRUE, FALSE or (-1) in case of exception */
static int JS_OrdinaryIsInstanceOf(JSContext *ctx, JSValueConst val,
                                   JSValueConst obj)
{
    JSValue obj_proto;
    JSObject *proto;
    const JSObject *p, *proto1;
    BOOL ret;
    if (!JS_IsFunction(ctx, obj))
        return FALSE;
    p = JS_VALUE_GET_OBJ(obj);
    if (p->class_id == JS_CLASS_BOUND_FUNCTION) {
        JSBoundFunction *s = p->u.bound_function;
        return JS_IsInstanceOf(ctx, val, s->func_obj);
    }
    /* Only explicitly boxed values are instances of constructors */
    if (JS_VALUE_GET_TAG(val) != JS_TAG_OBJECT)
        return FALSE;
    obj_proto = JS_GetProperty(ctx, obj, JS_ATOM_prototype);
    if (JS_VALUE_GET_TAG(obj_proto) != JS_TAG_OBJECT) {
        if (!JS_IsException(obj_proto))
            JS_ThrowTypeError(ctx, "operand 'prototype' property is not an object");
        ret = -1;
        goto done;
    }
    proto = JS_VALUE_GET_OBJ(obj_proto);
    p = JS_VALUE_GET_OBJ(val);
    for(;;) {
        proto1 = p->shape->proto;
        if (!proto1) {
            /* slow case if proxy in the prototype chain */
            if (UNLIKELY(p->class_id == JS_CLASS_PROXY)) {
                JSValue obj1;
                obj1 = JS_DupValue(ctx, JS_MKPTR(JS_TAG_OBJECT, (JSObject *)p));
                for(;;) {
                    obj1 = JS_GetPrototypeFree(ctx, obj1);
                    if (JS_IsException(obj1)) {
                        ret = -1;
                        break;
                    }
                    if (JS_IsNull(obj1)) {
                        ret = FALSE;
                        break;
                    }
                    if (proto == JS_VALUE_GET_OBJ(obj1)) {
                        JS_FreeValue(ctx, obj1);
                        ret = TRUE;
                        break;
                    }
                    /* must check for timeout to avoid infinite loop */
                    if (js_poll_interrupts(ctx)) {
                        JS_FreeValue(ctx, obj1);
                        ret = -1;
                        break;
                    }
                }
            } else {
                ret = FALSE;
            }
            break;
        }
        p = proto1;
        if (proto == p) {
            ret = TRUE;
            break;
        }
    }
done:
    JS_FreeValue(ctx, obj_proto);
    return ret;
}

/* return TRUE, FALSE or (-1) in case of exception */
int JS_IsInstanceOf(JSContext *ctx, JSValueConst val, JSValueConst obj)
{
    JSValue method;
    if (!JS_IsObject(obj))
        goto fail;
    method = JS_GetProperty(ctx, obj, JS_ATOM_Symbol_hasInstance);
    if (JS_IsException(method))
        return -1;
    if (!JS_IsNull(method) && !JS_IsUndefined(method)) {
        JSValue ret;
        ret = JS_CallFree(ctx, method, obj, 1, &val);
        return JS_ToBoolFree(ctx, ret);
    }

    /* legacy case */
    if (!JS_IsFunction(ctx, obj)) {
    fail:
        JS_ThrowTypeError(ctx, "invalid 'instanceof' right operand");
        return -1;
    }
    return JS_OrdinaryIsInstanceOf(ctx, val, obj);
}

/* return the value associated to the autoinit property or an exception */
typedef JSValue JSAutoInitFunc(JSContext *ctx, JSObject *p, JSAtom atom, void *opaque);

static JSAutoInitFunc *js_autoinit_func_table[] = {
    js_instantiate_prototype, /* JS_AUTOINIT_ID_PROTOTYPE */
    js_module_ns_autoinit, /* JS_AUTOINIT_ID_MODULE_NS */
    JS_InstantiateFunctionListItem2, /* JS_AUTOINIT_ID_PROP */
};

/* warning: 'prs' is reallocated after it */
static int JS_AutoInitProperty(JSContext *ctx, JSObject *p, JSAtom prop,
                               JSProperty *pr, JSShapeProperty *prs)
{
    JSValue val;
    JSContext *realm;
    JSAutoInitFunc *func;
    if (js_shape_prepare_update(ctx, p, &prs))
        return -1;
    realm = js_autoinit_get_realm(pr);
    func = js_autoinit_func_table[js_autoinit_get_id(pr)];
    /* 'func' shall not modify the object properties 'pr' */
    val = func(realm, p, prop, pr->u.init.opaque);
    js_autoinit_free(ctx->rt, pr);
    prs->flags &= ~JS_PROP_TMASK;
    pr->u.value = JS_UNDEFINED;
    if (JS_IsException(val))
        return -1;
    pr->u.value = val;
    return 0;
}

JSValue JS_GetPropertyInternal(JSContext *ctx, JSValueConst obj,
                               JSAtom prop, JSValueConst this_obj,
                               BOOL throw_ref_error)
{
    JSObject *p;
    JSProperty *pr;
    JSShapeProperty *prs;
    uint32_t tag;
    tag = JS_VALUE_GET_TAG(obj);
    if (UNLIKELY(tag != JS_TAG_OBJECT)) {
        switch(tag) {
        case JS_TAG_NULL:
            return JS_ThrowTypeErrorAtom(ctx, "cannot read property '%s' of null", prop);
        case JS_TAG_UNDEFINED:
            return JS_ThrowTypeErrorAtom(ctx, "cannot read property '%s' of undefined", prop);
        case JS_TAG_EXCEPTION:
            return JS_EXCEPTION;
        case JS_TAG_STRING:
            {
                JSString *p1 = JS_VALUE_GET_STRING(obj);
                if (__JS_AtomIsTaggedInt(prop)) {
                    uint32_t idx, ch;
                    idx = __JS_AtomToUInt32(prop);
                    if (idx < p1->len) {
                        if (p1->is_wide_char)
                            ch = p1->u.str16[idx];
                        else
                            ch = p1->u.str8[idx];
                        return js_new_string_char(ctx, ch);
                    }
                } else if (prop == JS_ATOM_length) {
                    return JS_NewInt32(ctx, p1->len);
                }
            }
            break;
        default:
            break;
        }
        /* cannot raise an exception */
        p = JS_VALUE_GET_OBJ(JS_GetPrototypePrimitive(ctx, obj));
        if (!p)
            return JS_UNDEFINED;
    } else {
        p = JS_VALUE_GET_OBJ(obj);
    }
    for(;;) {
        prs = find_own_property(&pr, p, prop);
        if (prs) {
            /* found */
            if (UNLIKELY(prs->flags & JS_PROP_TMASK)) {
                if ((prs->flags & JS_PROP_TMASK) == JS_PROP_GETSET) {
                    if (UNLIKELY(!pr->u.getset.getter)) {
                        return JS_UNDEFINED;
                    } else {
                        JSValue func = JS_MKPTR(JS_TAG_OBJECT, pr->u.getset.getter);
                        /* Note: the field could be removed in the getter */
                        func = JS_DupValue(ctx, func);
                        return JS_CallFree(ctx, func, this_obj, 0, NULL);
                    }
                } else if ((prs->flags & JS_PROP_TMASK) == JS_PROP_VARREF) {
                    JSValue val = *pr->u.var_ref->pvalue;
                    if (UNLIKELY(JS_IsUninitialized(val)))
                        return JS_ThrowReferenceErrorUninitialized(ctx, prs->atom);
                    return JS_DupValue(ctx, val);
                } else if ((prs->flags & JS_PROP_TMASK) == JS_PROP_AUTOINIT) {
                    /* Instantiate property and retry */
                    if (JS_AutoInitProperty(ctx, p, prop, pr, prs))
                        return JS_EXCEPTION;
                    continue;
                }
            } else {
                return JS_DupValue(ctx, pr->u.value);
            }
        }
        if (UNLIKELY(p->is_exotic)) {
            /* exotic behaviors */
            if (p->fast_array) {
                if (__JS_AtomIsTaggedInt(prop)) {
                    uint32_t idx = __JS_AtomToUInt32(prop);
                    if (idx < p->u.array.count) {
                        /* we avoid duplicating the code */
                        return JS_GetPropertyUint32(ctx, JS_MKPTR(JS_TAG_OBJECT, p), idx);
                    } else if (p->class_id >= JS_CLASS_UINT8C_ARRAY &&
                               p->class_id <= JS_CLASS_FLOAT64_ARRAY) {
                        return JS_UNDEFINED;
                    }
                } else if (p->class_id >= JS_CLASS_UINT8C_ARRAY &&
                           p->class_id <= JS_CLASS_FLOAT64_ARRAY) {
                    int ret;
                    ret = JS_AtomIsNumericIndex(ctx, prop);
                    if (ret != 0) {
                        if (ret < 0)
                            return JS_EXCEPTION;
                        return JS_UNDEFINED;
                    }
                }
            } else {
                const JSClassExoticMethods *em = ctx->rt->class_array[p->class_id].exotic;
                if (em) {
                    if (em->get_property) {
                        JSValue obj1, retval;
                        /* XXX: should pass throw_ref_error */
                        /* Note: if 'p' is a prototype, it can be
                           freed in the called function */
                        obj1 = JS_DupValue(ctx, JS_MKPTR(JS_TAG_OBJECT, p));
                        retval = em->get_property(ctx, obj1, prop, this_obj);
                        JS_FreeValue(ctx, obj1);
                        return retval;
                    }
                    if (em->get_own_property) {
                        JSPropertyDescriptor desc;
                        int ret;
                        JSValue obj1;
                        /* Note: if 'p' is a prototype, it can be
                           freed in the called function */
                        obj1 = JS_DupValue(ctx, JS_MKPTR(JS_TAG_OBJECT, p));
                        ret = em->get_own_property(ctx, &desc, obj1, prop);
                        JS_FreeValue(ctx, obj1);
                        if (ret < 0)
                            return JS_EXCEPTION;
                        if (ret) {
                            if (desc.flags & JS_PROP_GETSET) {
                                JS_FreeValue(ctx, desc.setter);
                                return JS_CallFree(ctx, desc.getter, this_obj, 0, NULL);
                            } else {
                                return desc.value;
                            }
                        }
                    }
                }
            }
        }
        p = p->shape->proto;
        if (!p)
            break;
    }
    if (UNLIKELY(throw_ref_error)) {
        return JS_ThrowReferenceErrorNotDefined(ctx, prop);
    } else {
        return JS_UNDEFINED;
    }
}

uint32_t js_string_obj_get_length(JSContext *ctx, JSValueConst obj)
{
    JSObject *p;
    JSString *p1;
    uint32_t len = 0;
    /* This is a class exotic method: obj class_id is JS_CLASS_STRING */
    p = JS_VALUE_GET_OBJ(obj);
    if (JS_VALUE_GET_TAG(p->u.object_data) == JS_TAG_STRING) {
        p1 = JS_VALUE_GET_STRING(p->u.object_data);
        len = p1->len;
    }
    return len;
}

static int num_keys_cmp(const void *p1, const void *p2, void *opaque)
{
    JSContext *ctx = opaque;
    JSAtom atom1 = ((const JSPropertyEnum *)p1)->atom;
    JSAtom atom2 = ((const JSPropertyEnum *)p2)->atom;
    uint32_t v1, v2;
    BOOL atom1_is_integer, atom2_is_integer;
    atom1_is_integer = JS_AtomIsArrayIndex(ctx, &v1, atom1);
    atom2_is_integer = JS_AtomIsArrayIndex(ctx, &v2, atom2);
    assert(atom1_is_integer && atom2_is_integer);
    if (v1 < v2)
        return -1;
    else if (v1 == v2)
        return 0;
    else
        return 1;
}

void js_free_prop_enum(JSContext *ctx, JSPropertyEnum *tab, uint32_t len)
{
    uint32_t i;
    if (tab) {
        for(i = 0; i < len; i++)
            JS_FreeAtom(ctx, tab[i].atom);
        js_free(ctx, tab);
    }
}

/* return < 0 in case if exception, 0 if OK. ptab and its atoms must
   be freed by the user. */
int JS_GetOwnPropertyNamesInternal(JSContext *ctx,
                                   JSPropertyEnum **ptab,
                                   uint32_t *plen,
                                   JSObject *p, int flags)
{
    int i, j;
    JSShape *sh;
    JSShapeProperty *prs;
    JSPropertyEnum *tab_atom, *tab_exotic;
    JSAtom atom;
    uint32_t num_keys_count, str_keys_count, sym_keys_count, atom_count;
    uint32_t num_index, str_index, sym_index, exotic_count, exotic_keys_count;
    BOOL is_enumerable, num_sorted;
    uint32_t num_key;
    JSAtomKindEnum kind;
    /* clear pointer for consistency in case of failure */
    *ptab = NULL;
    *plen = 0;
    /* compute the number of returned properties */
    num_keys_count = 0;
    str_keys_count = 0;
    sym_keys_count = 0;
    exotic_keys_count = 0;
    exotic_count = 0;
    tab_exotic = NULL;
    sh = p->shape;
    for(i = 0, prs = get_shape_prop(sh); i < sh->prop_count; i++, prs++) {
        atom = prs->atom;
        if (atom != JS_ATOM_NULL) {
            is_enumerable = ((prs->flags & JS_PROP_ENUMERABLE) != 0);
            kind = JS_AtomGetKind(ctx, atom);
            if ((!(flags & JS_GPN_ENUM_ONLY) || is_enumerable) &&
                ((flags >> kind) & 1) != 0) {
                /* need to raise an exception in case of the module
                   name space (implicit GetOwnProperty) */
                if (UNLIKELY((prs->flags & JS_PROP_TMASK) == JS_PROP_VARREF) &&
                    (flags & (JS_GPN_SET_ENUM | JS_GPN_ENUM_ONLY))) {
                    JSVarRef *var_ref = p->prop[i].u.var_ref;
                    if (UNLIKELY(JS_IsUninitialized(*var_ref->pvalue))) {
                        JS_ThrowReferenceErrorUninitialized(ctx, prs->atom);
                        return -1;
                    }
                }
                if (JS_AtomIsArrayIndex(ctx, &num_key, atom)) {
                    num_keys_count++;
                } else if (kind == JS_ATOM_KIND_STRING) {
                    str_keys_count++;
                } else {
                    sym_keys_count++;
                }
            }
        }
    }
    if (p->is_exotic) {
        if (p->fast_array) {
            if (flags & JS_GPN_STRING_MASK) {
                num_keys_count += p->u.array.count;
            }
        } else if (p->class_id == JS_CLASS_STRING) {
            if (flags & JS_GPN_STRING_MASK) {
                num_keys_count += js_string_obj_get_length(ctx, JS_MKPTR(JS_TAG_OBJECT, p));
            }
        } else {
            const JSClassExoticMethods *em = ctx->rt->class_array[p->class_id].exotic;
            if (em && em->get_own_property_names) {
                if (em->get_own_property_names(ctx, &tab_exotic, &exotic_count,
                                               JS_MKPTR(JS_TAG_OBJECT, p)))
                    return -1;
                for(i = 0; i < exotic_count; i++) {
                    atom = tab_exotic[i].atom;
                    kind = JS_AtomGetKind(ctx, atom);
                    if (((flags >> kind) & 1) != 0) {
                        is_enumerable = FALSE;
                        if (flags & (JS_GPN_SET_ENUM | JS_GPN_ENUM_ONLY)) {
                            JSPropertyDescriptor desc;
                            int res;
                            /* set the "is_enumerable" field if necessary */
                            res = JS_GetOwnPropertyInternal(ctx, &desc, p, atom);
                            if (res < 0) {
                                js_free_prop_enum(ctx, tab_exotic, exotic_count);
                                return -1;
                            }
                            if (res) {
                                is_enumerable =
                                    ((desc.flags & JS_PROP_ENUMERABLE) != 0);
                                js_free_desc(ctx, &desc);
                            }
                            tab_exotic[i].is_enumerable = is_enumerable;
                        }
                        if (!(flags & JS_GPN_ENUM_ONLY) || is_enumerable) {
                            exotic_keys_count++;
                        }
                    }
                }
            }
        }
    }
    /* fill them */
    atom_count = num_keys_count + str_keys_count + sym_keys_count + exotic_keys_count;
    /* avoid allocating 0 bytes */
    tab_atom = js_malloc(ctx, sizeof(tab_atom[0]) * max_int(atom_count, 1));
    if (!tab_atom) {
        js_free_prop_enum(ctx, tab_exotic, exotic_count);
        return -1;
    }
    num_index = 0;
    str_index = num_keys_count;
    sym_index = str_index + str_keys_count;
    num_sorted = TRUE;
    sh = p->shape;
    for(i = 0, prs = get_shape_prop(sh); i < sh->prop_count; i++, prs++) {
        atom = prs->atom;
        if (atom != JS_ATOM_NULL) {
            is_enumerable = ((prs->flags & JS_PROP_ENUMERABLE) != 0);
            kind = JS_AtomGetKind(ctx, atom);
            if ((!(flags & JS_GPN_ENUM_ONLY) || is_enumerable) &&
                ((flags >> kind) & 1) != 0) {
                if (JS_AtomIsArrayIndex(ctx, &num_key, atom)) {
                    j = num_index++;
                    num_sorted = FALSE;
                } else if (kind == JS_ATOM_KIND_STRING) {
                    j = str_index++;
                } else {
                    j = sym_index++;
                }
                tab_atom[j].atom = JS_DupAtom(ctx, atom);
                tab_atom[j].is_enumerable = is_enumerable;
            }
        }
    }
    if (p->is_exotic) {
        int len;
        if (p->fast_array) {
            if (flags & JS_GPN_STRING_MASK) {
                len = p->u.array.count;
                goto add_array_keys;
            }
        } else if (p->class_id == JS_CLASS_STRING) {
            if (flags & JS_GPN_STRING_MASK) {
                len = js_string_obj_get_length(ctx, JS_MKPTR(JS_TAG_OBJECT, p));
            add_array_keys:
                for(i = 0; i < len; i++) {
                    tab_atom[num_index].atom = __JS_AtomFromUInt32(i);
                    if (tab_atom[num_index].atom == JS_ATOM_NULL) {
                        js_free_prop_enum(ctx, tab_atom, num_index);
                        return -1;
                    }
                    tab_atom[num_index].is_enumerable = TRUE;
                    num_index++;
                }
            }
        } else {
            /* Note: exotic keys are not reordered and comes after the object own properties. */
            for(i = 0; i < exotic_count; i++) {
                atom = tab_exotic[i].atom;
                is_enumerable = tab_exotic[i].is_enumerable;
                kind = JS_AtomGetKind(ctx, atom);
                if ((!(flags & JS_GPN_ENUM_ONLY) || is_enumerable) &&
                    ((flags >> kind) & 1) != 0) {
                    tab_atom[sym_index].atom = atom;
                    tab_atom[sym_index].is_enumerable = is_enumerable;
                    sym_index++;
                } else {
                    JS_FreeAtom(ctx, atom);
                }
            }
            js_free(ctx, tab_exotic);
        }
    }
    assert(num_index == num_keys_count);
    assert(str_index == num_keys_count + str_keys_count);
    assert(sym_index == atom_count);
    if (num_keys_count != 0 && !num_sorted) {
        rqsort(tab_atom, num_keys_count, sizeof(tab_atom[0]), num_keys_cmp,
               ctx);
    }
    *ptab = tab_atom;
    *plen = atom_count;
    return 0;
}

int JS_GetOwnPropertyNames(JSContext *ctx, JSPropertyEnum **ptab,
                           uint32_t *plen, JSValueConst obj, int flags)
{
    if (JS_VALUE_GET_TAG(obj) != JS_TAG_OBJECT) {
        JS_ThrowTypeErrorNotAnObject(ctx);
        return -1;
    }
    return JS_GetOwnPropertyNamesInternal(ctx, ptab, plen,
                                          JS_VALUE_GET_OBJ(obj), flags);
}

/* Return -1 if exception,
   FALSE if the property does not exist, TRUE if it exists. If TRUE is
   returned, the property descriptor 'desc' is filled present. */
int JS_GetOwnPropertyInternal(JSContext *ctx, JSPropertyDescriptor *desc,
                              JSObject *p, JSAtom prop)
{
    JSShapeProperty *prs;
    JSProperty *pr;
retry:
    prs = find_own_property(&pr, p, prop);
    if (prs) {
        if (desc) {
            desc->flags = prs->flags & JS_PROP_C_W_E;
            desc->getter = JS_UNDEFINED;
            desc->setter = JS_UNDEFINED;
            desc->value = JS_UNDEFINED;
            if (UNLIKELY(prs->flags & JS_PROP_TMASK)) {
                if ((prs->flags & JS_PROP_TMASK) == JS_PROP_GETSET) {
                    desc->flags |= JS_PROP_GETSET;
                    if (pr->u.getset.getter)
                        desc->getter = JS_DupValue(ctx, JS_MKPTR(JS_TAG_OBJECT, pr->u.getset.getter));
                    if (pr->u.getset.setter)
                        desc->setter = JS_DupValue(ctx, JS_MKPTR(JS_TAG_OBJECT, pr->u.getset.setter));
                } else if ((prs->flags & JS_PROP_TMASK) == JS_PROP_VARREF) {
                    JSValue val = *pr->u.var_ref->pvalue;
                    if (UNLIKELY(JS_IsUninitialized(val))) {
                        JS_ThrowReferenceErrorUninitialized(ctx, prs->atom);
                        return -1;
                    }
                    desc->value = JS_DupValue(ctx, val);
                } else if ((prs->flags & JS_PROP_TMASK) == JS_PROP_AUTOINIT) {
                    /* Instantiate property and retry */
                    if (JS_AutoInitProperty(ctx, p, prop, pr, prs))
                        return -1;
                    goto retry;
                }
            } else {
                desc->value = JS_DupValue(ctx, pr->u.value);
            }
        } else {
            /* for consistency, send the exception even if desc is NULL */
            if (UNLIKELY((prs->flags & JS_PROP_TMASK) == JS_PROP_VARREF)) {
                if (UNLIKELY(JS_IsUninitialized(*pr->u.var_ref->pvalue))) {
                    JS_ThrowReferenceErrorUninitialized(ctx, prs->atom);
                    return -1;
                }
            } else if ((prs->flags & JS_PROP_TMASK) == JS_PROP_AUTOINIT) {
                /* nothing to do: delay instantiation until actual value and/or attributes are read */
            }
        }
        return TRUE;
    }
    if (p->is_exotic) {
        if (p->fast_array) {
            /* specific case for fast arrays */
            if (__JS_AtomIsTaggedInt(prop)) {
                uint32_t idx;
                idx = __JS_AtomToUInt32(prop);
                if (idx < p->u.array.count) {
                    if (desc) {
                        desc->flags = JS_PROP_WRITABLE | JS_PROP_ENUMERABLE |
                            JS_PROP_CONFIGURABLE;
                        desc->getter = JS_UNDEFINED;
                        desc->setter = JS_UNDEFINED;
                        desc->value = JS_GetPropertyUint32(ctx, JS_MKPTR(JS_TAG_OBJECT, p), idx);
                    }
                    return TRUE;
                }
            }
        } else {
            const JSClassExoticMethods *em = ctx->rt->class_array[p->class_id].exotic;
            if (em && em->get_own_property) {
                return em->get_own_property(ctx, desc,
                                            JS_MKPTR(JS_TAG_OBJECT, p), prop);
            }
        }
    }
    return FALSE;
}

int JS_GetOwnProperty(JSContext *ctx, JSPropertyDescriptor *desc,
                      JSValueConst obj, JSAtom prop)
{
    if (JS_VALUE_GET_TAG(obj) != JS_TAG_OBJECT) {
        JS_ThrowTypeErrorNotAnObject(ctx);
        return -1;
    }
    return JS_GetOwnPropertyInternal(ctx, desc, JS_VALUE_GET_OBJ(obj), prop);
}

/* return -1 if exception (Proxy object only) or TRUE/FALSE */
int JS_IsExtensible(JSContext *ctx, JSValueConst obj)
{
    JSObject *p;
    if (UNLIKELY(JS_VALUE_GET_TAG(obj) != JS_TAG_OBJECT))
        return FALSE;
    p = JS_VALUE_GET_OBJ(obj);
    if (UNLIKELY(p->class_id == JS_CLASS_PROXY))
        return js_proxy_isExtensible(ctx, obj);
    else
        return p->extensible;
}

/* return -1 if exception (Proxy object only) or TRUE/FALSE */
int JS_PreventExtensions(JSContext *ctx, JSValueConst obj)
{
    JSObject *p;
    if (UNLIKELY(JS_VALUE_GET_TAG(obj) != JS_TAG_OBJECT))
        return FALSE;
    p = JS_VALUE_GET_OBJ(obj);
    if (UNLIKELY(p->class_id == JS_CLASS_PROXY))
        return js_proxy_preventExtensions(ctx, obj);
    p->extensible = FALSE;
    return TRUE;
}

/* return -1 if exception otherwise TRUE or FALSE */
int JS_HasProperty(JSContext *ctx, JSValueConst obj, JSAtom prop)
{
    JSObject *p;
    int ret;
    JSValue obj1;
    if (UNLIKELY(JS_VALUE_GET_TAG(obj) != JS_TAG_OBJECT))
        return FALSE;
    p = JS_VALUE_GET_OBJ(obj);
    for(;;) {
        if (p->is_exotic) {
            const JSClassExoticMethods *em = ctx->rt->class_array[p->class_id].exotic;
            if (em && em->has_property) {
                /* has_property can free the prototype */
                obj1 = JS_DupValue(ctx, JS_MKPTR(JS_TAG_OBJECT, p));
                ret = em->has_property(ctx, obj1, prop);
                JS_FreeValue(ctx, obj1);
                return ret;
            }
        }
        /* JS_GetOwnPropertyInternal can free the prototype */
        JS_DupValue(ctx, JS_MKPTR(JS_TAG_OBJECT, p));
        ret = JS_GetOwnPropertyInternal(ctx, NULL, p, prop);
        JS_FreeValue(ctx, JS_MKPTR(JS_TAG_OBJECT, p));
        if (ret != 0)
            return ret;
        if (p->class_id >= JS_CLASS_UINT8C_ARRAY &&
            p->class_id <= JS_CLASS_FLOAT64_ARRAY) {
            ret = JS_AtomIsNumericIndex(ctx, prop);
            if (ret != 0) {
                if (ret < 0)
                    return -1;
                return FALSE;
            }
        }
        p = p->shape->proto;
        if (!p)
            break;
    }
    return FALSE;
}

/* return JS_ATOM_NULL in case of exception */
JSAtom JS_ValueToAtom(JSContext *ctx, JSValueConst val)
{
    JSAtom atom;
    uint32_t tag;
    tag = JS_VALUE_GET_TAG(val);
    if (tag == JS_TAG_INT &&
        (uint32_t)JS_VALUE_GET_INT(val) <= JS_ATOM_MAX_INT) {
        /* fast path for integer values */
        atom = __JS_AtomFromUInt32(JS_VALUE_GET_INT(val));
    } else if (tag == JS_TAG_SYMBOL) {
        JSAtomStruct *p = JS_VALUE_GET_PTR(val);
        atom = JS_DupAtom(ctx, js_get_atom_index(ctx->rt, p));
    } else {
        JSValue str;
        str = JS_ToPropertyKey(ctx, val);
        if (JS_IsException(str))
            return JS_ATOM_NULL;
        if (JS_VALUE_GET_TAG(str) == JS_TAG_SYMBOL) {
            atom = js_symbol_to_atom(ctx, str);
        } else {
            atom = JS_NewAtomStr(ctx, JS_VALUE_GET_STRING(str));
        }
    }
    return atom;
}

JSValue JS_GetPropertyValue(JSContext *ctx, JSValueConst this_obj, JSValue prop)
{
    JSAtom atom;
    JSValue ret;
    if (LIKELY(JS_VALUE_GET_TAG(this_obj) == JS_TAG_OBJECT &&
               JS_VALUE_GET_TAG(prop) == JS_TAG_INT)) {
        JSObject *p;
        uint32_t idx, len;
        /* fast path for array access */
        p = JS_VALUE_GET_OBJ(this_obj);
        idx = JS_VALUE_GET_INT(prop);
        len = (uint32_t)p->u.array.count;
        if (UNLIKELY(idx >= len))
            goto slow_path;
        switch(p->class_id) {
        case JS_CLASS_ARRAY:
        case JS_CLASS_ARGUMENTS:
            return JS_DupValue(ctx, p->u.array.u.values[idx]);
        case JS_CLASS_INT8_ARRAY:
            return JS_NewInt32(ctx, p->u.array.u.int8_ptr[idx]);
        case JS_CLASS_UINT8C_ARRAY:
        case JS_CLASS_UINT8_ARRAY:
            return JS_NewInt32(ctx, p->u.array.u.uint8_ptr[idx]);
        case JS_CLASS_INT16_ARRAY:
            return JS_NewInt32(ctx, p->u.array.u.int16_ptr[idx]);
        case JS_CLASS_UINT16_ARRAY:
            return JS_NewInt32(ctx, p->u.array.u.uint16_ptr[idx]);
        case JS_CLASS_INT32_ARRAY:
            return JS_NewInt32(ctx, p->u.array.u.int32_ptr[idx]);
        case JS_CLASS_UINT32_ARRAY:
            return JS_NewUint32(ctx, p->u.array.u.uint32_ptr[idx]);
#ifdef CONFIG_BIGNUM
        case JS_CLASS_BIG_INT64_ARRAY:
            return JS_NewBigInt64(ctx, p->u.array.u.int64_ptr[idx]);
        case JS_CLASS_BIG_UINT64_ARRAY:
            return JS_NewBigUint64(ctx, p->u.array.u.uint64_ptr[idx]);
#endif
        case JS_CLASS_FLOAT32_ARRAY:
            return __JS_NewFloat64(ctx, p->u.array.u.float_ptr[idx]);
        case JS_CLASS_FLOAT64_ARRAY:
            return __JS_NewFloat64(ctx, p->u.array.u.double_ptr[idx]);
        default:
            goto slow_path;
        }
    } else {
    slow_path:
        atom = JS_ValueToAtom(ctx, prop);
        JS_FreeValue(ctx, prop);
        if (UNLIKELY(atom == JS_ATOM_NULL))
            return JS_EXCEPTION;
        ret = JS_GetProperty(ctx, this_obj, atom);
        JS_FreeAtom(ctx, atom);
        return ret;
    }
}

JSValue JS_GetPropertyUint32(JSContext *ctx, JSValueConst this_obj,
                             uint32_t idx)
{
    return JS_GetPropertyValue(ctx, this_obj, JS_NewUint32(ctx, idx));
}

JSValue JS_GetPropertyInt64(JSContext *ctx, JSValueConst obj, int64_t idx)
{
    JSAtom prop;
    JSValue val;
    if ((uint64_t)idx <= INT32_MAX) {
        /* fast path for fast arrays */
        return JS_GetPropertyValue(ctx, obj, JS_NewInt32(ctx, idx));
    }
    prop = JS_NewAtomInt64(ctx, idx);
    if (prop == JS_ATOM_NULL)
        return JS_EXCEPTION;
    val = JS_GetProperty(ctx, obj, prop);
    JS_FreeAtom(ctx, prop);
    return val;
}

JSValue JS_GetPropertyStr(JSContext *ctx, JSValueConst this_obj,
                          const char *prop)
{
    JSAtom atom;
    JSValue ret;
    atom = JS_NewAtom(ctx, prop);
    ret = JS_GetProperty(ctx, this_obj, atom);
    JS_FreeAtom(ctx, atom);
    return ret;
}

/* Note: the property value is not initialized. Return NULL if memory
   error. */
JSProperty *add_property(JSContext *ctx, JSObject *p, JSAtom prop, int prop_flags)
{
    JSShape *sh, *new_sh;
    sh = p->shape;
    if (sh->is_hashed) {
        /* try to find an existing shape */
        new_sh = find_hashed_shape_prop(ctx->rt, sh, prop, prop_flags);
        if (new_sh) {
            /* matching shape found: use it */
            /*  the property array may need to be resized */
            if (new_sh->prop_size != sh->prop_size) {
                JSProperty *new_prop;
                new_prop = js_realloc(ctx, p->prop, sizeof(p->prop[0]) *
                                      new_sh->prop_size);
                if (!new_prop)
                    return NULL;
                p->prop = new_prop;
            }
            p->shape = js_dup_shape(new_sh);
            js_free_shape(ctx->rt, sh);
            return &p->prop[new_sh->prop_count - 1];
        } else if (sh->header.ref_count != 1) {
            /* if the shape is shared, clone it */
            new_sh = js_clone_shape(ctx, sh);
            if (!new_sh)
                return NULL;
            /* hash the cloned shape */
            new_sh->is_hashed = TRUE;
            js_shape_hash_link(ctx->rt, new_sh);
            js_free_shape(ctx->rt, p->shape);
            p->shape = new_sh;
        }
    }
    assert(p->shape->header.ref_count == 1);
    if (add_shape_property(ctx, &p->shape, p, prop, prop_flags))
        return NULL;
    return &p->prop[p->shape->prop_count - 1];
}

/* can be called on Array or Arguments objects. return < 0 if
   memory alloc error. */
static dontinline __exception int convert_fast_array_to_array(JSContext *ctx,
                                                             JSObject *p)
{
    JSProperty *pr;
    JSShape *sh;
    JSValue *tab;
    uint32_t i, len, new_count;
    if (js_shape_prepare_update(ctx, p, NULL))
        return -1;
    len = p->u.array.count;
    /* resize the properties once to simplify the error handling */
    sh = p->shape;
    new_count = sh->prop_count + len;
    if (new_count > sh->prop_size) {
        if (resize_properties(ctx, &p->shape, p, new_count))
            return -1;
    }
    tab = p->u.array.u.values;
    for(i = 0; i < len; i++) {
        /* add_property cannot fail here but
           __JS_AtomFromUInt32(i) fails for i > INT32_MAX */
        pr = add_property(ctx, p, __JS_AtomFromUInt32(i), JS_PROP_C_W_E);
        pr->u.value = *tab++;
    }
    js_free(ctx, p->u.array.u.values);
    p->u.array.count = 0;
    p->u.array.u.values = NULL; /* fail safe */
    p->u.array.u1.size = 0;
    p->fast_array = 0;
    return 0;
}

int delete_property(JSContext *ctx, JSObject *p, JSAtom atom)
{
    JSShape *sh;
    JSShapeProperty *pr, *lpr, *prop;
    JSProperty *pr1;
    uint32_t lpr_idx;
    intptr_t h, h1;
 redo:
    sh = p->shape;
    h1 = atom & sh->prop_hash_mask;
    h = prop_hash_end(sh)[-h1 - 1];
    prop = get_shape_prop(sh);
    lpr = NULL;
    lpr_idx = 0;   /* prevent warning */
    while (h != 0) {
        pr = &prop[h - 1];
        if (LIKELY(pr->atom == atom)) {
            /* found ! */
            if (!(pr->flags & JS_PROP_CONFIGURABLE))
                return FALSE;
            /* realloc the shape if needed */
            if (lpr)
                lpr_idx = lpr - get_shape_prop(sh);
            if (js_shape_prepare_update(ctx, p, &pr))
                return -1;
            sh = p->shape;
            /* remove property */
            if (lpr) {
                lpr = get_shape_prop(sh) + lpr_idx;
                lpr->hash_next = pr->hash_next;
            } else {
                prop_hash_end(sh)[-h1 - 1] = pr->hash_next;
            }
            sh->deleted_prop_count++;
            /* free the entry */
            pr1 = &p->prop[h - 1];
            free_property(ctx->rt, pr1, pr->flags);
            JS_FreeAtom(ctx, pr->atom);
            /* put default values */
            pr->flags = 0;
            pr->atom = JS_ATOM_NULL;
            pr1->u.value = JS_UNDEFINED;
            /* compact the properties if too many deleted properties */
            if (sh->deleted_prop_count >= 8 &&
                sh->deleted_prop_count >= ((unsigned)sh->prop_count / 2)) {
                compact_properties(ctx, p);
            }
            return TRUE;
        }
        lpr = pr;
        h = pr->hash_next;
    }
    if (p->is_exotic) {
        if (p->fast_array) {
            uint32_t idx;
            if (JS_AtomIsArrayIndex(ctx, &idx, atom) &&
                idx < p->u.array.count) {
                if (p->class_id == JS_CLASS_ARRAY ||
                    p->class_id == JS_CLASS_ARGUMENTS) {
                    /* Special case deleting the last element of a fast Array */
                    if (idx == p->u.array.count - 1) {
                        JS_FreeValue(ctx, p->u.array.u.values[idx]);
                        p->u.array.count = idx;
                        return TRUE;
                    }
                    if (convert_fast_array_to_array(ctx, p))
                        return -1;
                    goto redo;
                } else {
                    return FALSE;
                }
            }
        } else {
            const JSClassExoticMethods *em = ctx->rt->class_array[p->class_id].exotic;
            if (em && em->delete_property) {
                return em->delete_property(ctx, JS_MKPTR(JS_TAG_OBJECT, p), atom);
            }
        }
    }
    /* not found */
    return TRUE;
}

static int call_setter(JSContext *ctx, JSObject *setter,
                       JSValueConst this_obj, JSValue val, int flags)
{
    JSValue ret, func;
    if (LIKELY(setter)) {
        func = JS_MKPTR(JS_TAG_OBJECT, setter);
        /* Note: the field could be removed in the setter */
        func = JS_DupValue(ctx, func);
        ret = JS_CallFree(ctx, func, this_obj, 1, (JSValueConst *)&val);
        JS_FreeValue(ctx, val);
        if (JS_IsException(ret))
            return -1;
        JS_FreeValue(ctx, ret);
        return TRUE;
    } else {
        JS_FreeValue(ctx, val);
        if ((flags & JS_PROP_THROW) ||
            ((flags & JS_PROP_THROW_STRICT) && is_strict_mode(ctx))) {
            JS_ThrowTypeError(ctx, "no setter for property");
            return -1;
        }
        return FALSE;
    }
}

void js_free_desc(JSContext *ctx, JSPropertyDescriptor *desc)
{
    JS_FreeValue(ctx, desc->getter);
    JS_FreeValue(ctx, desc->setter);
    JS_FreeValue(ctx, desc->value);
}

/* generic (and slower) version of JS_SetProperty() for
 * Reflect.set(). 'obj' must be an object.  */
int JS_SetPropertyGeneric(JSContext *ctx,
                          JSValueConst obj, JSAtom prop,
                          JSValue val, JSValueConst this_obj,
                          int flags)
{
    int ret;
    JSPropertyDescriptor desc;
    JSValue obj1;
    JSObject *p;
    obj1 = JS_DupValue(ctx, obj);
    for(;;) {
        p = JS_VALUE_GET_OBJ(obj1);
        if (p->is_exotic) {
            const JSClassExoticMethods *em = ctx->rt->class_array[p->class_id].exotic;
            if (em && em->set_property) {
                ret = em->set_property(ctx, obj1, prop,
                                       val, this_obj, flags);
                JS_FreeValue(ctx, obj1);
                JS_FreeValue(ctx, val);
                return ret;
            }
        }
        ret = JS_GetOwnPropertyInternal(ctx, &desc, p, prop);
        if (ret < 0) {
            JS_FreeValue(ctx, obj1);
            JS_FreeValue(ctx, val);
            return ret;
        }
        if (ret) {
            if (desc.flags & JS_PROP_GETSET) {
                JSObject *setter;
                if (JS_IsUndefined(desc.setter))
                    setter = NULL;
                else
                    setter = JS_VALUE_GET_OBJ(desc.setter);
                ret = call_setter(ctx, setter, this_obj, val, flags);
                JS_FreeValue(ctx, desc.getter);
                JS_FreeValue(ctx, desc.setter);
                JS_FreeValue(ctx, obj1);
                return ret;
            } else {
                JS_FreeValue(ctx, desc.value);
                if (!(desc.flags & JS_PROP_WRITABLE)) {
                    JS_FreeValue(ctx, obj1);
                    goto read_only_error;
                }
            }
            break;
        }
        /* Note: at this point 'obj1' cannot be a proxy. XXX: may have
           to check recursion */
        obj1 = JS_GetPrototypeFree(ctx, obj1);
        if (JS_IsNull(obj1))
            break;
    }
    JS_FreeValue(ctx, obj1);
    if (!JS_IsObject(this_obj)) {
        JS_FreeValue(ctx, val);
        return JS_ThrowTypeErrorOrFalse(ctx, flags, "receiver is not an object");
    }
    p = JS_VALUE_GET_OBJ(this_obj);
    /* modify the property in this_obj if it already exists */
    ret = JS_GetOwnPropertyInternal(ctx, &desc, p, prop);
    if (ret < 0) {
        JS_FreeValue(ctx, val);
        return ret;
    }
    if (ret) {
        if (desc.flags & JS_PROP_GETSET) {
            JS_FreeValue(ctx, desc.getter);
            JS_FreeValue(ctx, desc.setter);
            JS_FreeValue(ctx, val);
            return JS_ThrowTypeErrorOrFalse(ctx, flags, "setter is forbidden");
        } else {
            JS_FreeValue(ctx, desc.value);
            if (!(desc.flags & JS_PROP_WRITABLE) ||
                p->class_id == JS_CLASS_MODULE_NS) {
            read_only_error:
                JS_FreeValue(ctx, val);
                return JS_ThrowTypeErrorReadOnly(ctx, flags, prop);
            }
        }
        ret = JS_DefineProperty(ctx, this_obj, prop, val,
                                JS_UNDEFINED, JS_UNDEFINED,
                                JS_PROP_HAS_VALUE);
        JS_FreeValue(ctx, val);
        return ret;
    }
    ret = JS_CreateProperty(ctx, p, prop, val, JS_UNDEFINED, JS_UNDEFINED,
                            flags |
                            JS_PROP_HAS_VALUE |
                            JS_PROP_HAS_ENUMERABLE |
                            JS_PROP_HAS_WRITABLE |
                            JS_PROP_HAS_CONFIGURABLE |
                            JS_PROP_C_W_E);
    JS_FreeValue(ctx, val);
    return ret;
}

/* return -1 in case of exception or TRUE or FALSE. Warning: 'val' is
   freed by the function. 'flags' is a bitmask of JS_PROP_NO_ADD,
   JS_PROP_THROW or JS_PROP_THROW_STRICT. If JS_PROP_NO_ADD is set,
   the new property is not added and an error is raised. */
int JS_SetPropertyInternal(JSContext *ctx, JSValueConst this_obj,
                           JSAtom prop, JSValue val, int flags)
{
    JSObject *p, *p1;
    JSShapeProperty *prs;
    JSProperty *pr;
    uint32_t tag;
    JSPropertyDescriptor desc;
    int ret;
#if 0
    printf("JS_SetPropertyInternal: "); print_atom(ctx, prop); printf("\n");
#endif
    tag = JS_VALUE_GET_TAG(this_obj);
    if (UNLIKELY(tag != JS_TAG_OBJECT)) {
        switch(tag) {
        case JS_TAG_NULL:
            JS_FreeValue(ctx, val);
            JS_ThrowTypeErrorAtom(ctx, "cannot set property '%s' of null", prop);
            return -1;
        case JS_TAG_UNDEFINED:
            JS_FreeValue(ctx, val);
            JS_ThrowTypeErrorAtom(ctx, "cannot set property '%s' of undefined", prop);
            return -1;
        default:
            /* even on a primitive type we can have setters on the prototype */
            p = NULL;
            p1 = JS_VALUE_GET_OBJ(JS_GetPrototypePrimitive(ctx, this_obj));
            goto prototype_lookup;
        }
    }
    p = JS_VALUE_GET_OBJ(this_obj);
retry:
    prs = find_own_property(&pr, p, prop);
    if (prs) {
        if (LIKELY((prs->flags & (JS_PROP_TMASK | JS_PROP_WRITABLE |
                                  JS_PROP_LENGTH)) == JS_PROP_WRITABLE)) {
            /* fast case */
            set_value(ctx, &pr->u.value, val);
            return TRUE;
        } else if (prs->flags & JS_PROP_LENGTH) {
            assert(p->class_id == JS_CLASS_ARRAY);
            assert(prop == JS_ATOM_length);
            return set_array_length(ctx, p, val, flags);
        } else if ((prs->flags & JS_PROP_TMASK) == JS_PROP_GETSET) {
            return call_setter(ctx, pr->u.getset.setter, this_obj, val, flags);
        } else if ((prs->flags & JS_PROP_TMASK) == JS_PROP_VARREF) {
            /* JS_PROP_WRITABLE is always true for variable
               references, but they are write protected in module name
               spaces. */
            if (p->class_id == JS_CLASS_MODULE_NS)
                goto read_only_prop;
            set_value(ctx, pr->u.var_ref->pvalue, val);
            return TRUE;
        } else if ((prs->flags & JS_PROP_TMASK) == JS_PROP_AUTOINIT) {
            /* Instantiate property and retry (potentially useless) */
            if (JS_AutoInitProperty(ctx, p, prop, pr, prs)) {
                JS_FreeValue(ctx, val);
                return -1;
            }
            goto retry;
        } else {
            goto read_only_prop;
        }
    }
    p1 = p;
    for(;;) {
        if (p1->is_exotic) {
            if (p1->fast_array) {
                if (__JS_AtomIsTaggedInt(prop)) {
                    uint32_t idx = __JS_AtomToUInt32(prop);
                    if (idx < p1->u.array.count) {
                        if (UNLIKELY(p == p1))
                            return JS_SetPropertyValue(ctx, this_obj, JS_NewInt32(ctx, idx), val, flags);
                        else
                            break;
                    } else if (p1->class_id >= JS_CLASS_UINT8C_ARRAY &&
                               p1->class_id <= JS_CLASS_FLOAT64_ARRAY) {
                        goto typed_array_oob;
                    }
                } else if (p1->class_id >= JS_CLASS_UINT8C_ARRAY &&
                           p1->class_id <= JS_CLASS_FLOAT64_ARRAY) {
                    ret = JS_AtomIsNumericIndex(ctx, prop);
                    if (ret != 0) {
                        if (ret < 0) {
                            JS_FreeValue(ctx, val);
                            return -1;
                        }
                    typed_array_oob:
                        val = JS_ToNumberFree(ctx, val);
                        JS_FreeValue(ctx, val);
                        if (JS_IsException(val))
                            return -1;
                        return JS_ThrowTypeErrorOrFalse(ctx, flags, "out-of-bound numeric index");
                    }
                }
            } else {
                const JSClassExoticMethods *em = ctx->rt->class_array[p1->class_id].exotic;
                if (em) {
                    JSValue obj1;
                    if (em->set_property) {
                        /* set_property can free the prototype */
                        obj1 = JS_DupValue(ctx, JS_MKPTR(JS_TAG_OBJECT, p1));
                        ret = em->set_property(ctx, obj1, prop,
                                               val, this_obj, flags);
                        JS_FreeValue(ctx, obj1);
                        JS_FreeValue(ctx, val);
                        return ret;
                    }
                    if (em->get_own_property) {
                        /* get_own_property can free the prototype */
                        obj1 = JS_DupValue(ctx, JS_MKPTR(JS_TAG_OBJECT, p1));
                        ret = em->get_own_property(ctx, &desc,
                                                   obj1, prop);
                        JS_FreeValue(ctx, obj1);
                        if (ret < 0) {
                            JS_FreeValue(ctx, val);
                            return ret;
                        }
                        if (ret) {
                            if (desc.flags & JS_PROP_GETSET) {
                                JSObject *setter;
                                if (JS_IsUndefined(desc.setter))
                                    setter = NULL;
                                else
                                    setter = JS_VALUE_GET_OBJ(desc.setter);
                                ret = call_setter(ctx, setter, this_obj, val, flags);
                                JS_FreeValue(ctx, desc.getter);
                                JS_FreeValue(ctx, desc.setter);
                                return ret;
                            } else {
                                JS_FreeValue(ctx, desc.value);
                                if (!(desc.flags & JS_PROP_WRITABLE))
                                    goto read_only_prop;
                                if (LIKELY(p == p1)) {
                                    ret = JS_DefineProperty(ctx, this_obj, prop, val,
                                                            JS_UNDEFINED, JS_UNDEFINED,
                                                            JS_PROP_HAS_VALUE);
                                    JS_FreeValue(ctx, val);
                                    return ret;
                                } else {
                                    break;
                                }
                            }
                        }
                    }
                }
            }
        }
        p1 = p1->shape->proto;
    prototype_lookup:
        if (!p1)
            break;
    retry2:
        prs = find_own_property(&pr, p1, prop);
        if (prs) {
            if ((prs->flags & JS_PROP_TMASK) == JS_PROP_GETSET) {
                return call_setter(ctx, pr->u.getset.setter, this_obj, val, flags);
            } else if ((prs->flags & JS_PROP_TMASK) == JS_PROP_AUTOINIT) {
                /* Instantiate property and retry (potentially useless) */
                if (JS_AutoInitProperty(ctx, p1, prop, pr, prs))
                    return -1;
                goto retry2;
            } else if (!(prs->flags & JS_PROP_WRITABLE)) {
            read_only_prop:
                JS_FreeValue(ctx, val);
                return JS_ThrowTypeErrorReadOnly(ctx, flags, prop);
            }
        }
    }
    if (UNLIKELY(flags & JS_PROP_NO_ADD)) {
        JS_FreeValue(ctx, val);
        JS_ThrowReferenceErrorNotDefined(ctx, prop);
        return -1;
    }
    if (UNLIKELY(!p)) {
        JS_FreeValue(ctx, val);
        return JS_ThrowTypeErrorOrFalse(ctx, flags, "not an object");
    }
    if (UNLIKELY(!p->extensible)) {
        JS_FreeValue(ctx, val);
        return JS_ThrowTypeErrorOrFalse(ctx, flags, "object is not extensible");
    }
    if (p->is_exotic) {
        if (p->class_id == JS_CLASS_ARRAY && p->fast_array &&
            __JS_AtomIsTaggedInt(prop)) {
            uint32_t idx = __JS_AtomToUInt32(prop);
            if (idx == p->u.array.count) {
                /* fast case */
                return add_fast_array_element(ctx, p, val, flags);
            } else {
                goto generic_create_prop;
            }
        } else {
        generic_create_prop:
            ret = JS_CreateProperty(ctx, p, prop, val, JS_UNDEFINED, JS_UNDEFINED,
                                    flags |
                                    JS_PROP_HAS_VALUE |
                                    JS_PROP_HAS_ENUMERABLE |
                                    JS_PROP_HAS_WRITABLE |
                                    JS_PROP_HAS_CONFIGURABLE |
                                    JS_PROP_C_W_E);
            JS_FreeValue(ctx, val);
            return ret;
        }
    }
    pr = add_property(ctx, p, prop, JS_PROP_C_W_E);
    if (UNLIKELY(!pr)) {
        JS_FreeValue(ctx, val);
        return -1;
    }
    pr->u.value = val;
    return TRUE;
}

/* flags can be JS_PROP_THROW or JS_PROP_THROW_STRICT */
int JS_SetPropertyValue(JSContext *ctx, JSValueConst this_obj, JSValue prop, JSValue val, int flags)
{
    if (LIKELY(JS_VALUE_GET_TAG(this_obj) == JS_TAG_OBJECT &&
               JS_VALUE_GET_TAG(prop) == JS_TAG_INT)) {
        JSObject *p;
        uint32_t idx;
        double d;
        int32_t v;
        /* fast path for array access */
        p = JS_VALUE_GET_OBJ(this_obj);
        idx = JS_VALUE_GET_INT(prop);
        switch(p->class_id) {
        case JS_CLASS_ARRAY:
            if (UNLIKELY(idx >= (uint32_t)p->u.array.count)) {
                JSObject *p1;
                JSShape *sh1;
                /* fast path to add an element to the array */
                if (idx != (uint32_t)p->u.array.count ||
                    !p->fast_array || !p->extensible)
                    goto slow_path;
                /* check if prototype chain has a numeric property */
                p1 = p->shape->proto;
                while (p1 != NULL) {
                    sh1 = p1->shape;
                    if (p1->class_id == JS_CLASS_ARRAY) {
                        if (UNLIKELY(!p1->fast_array))
                            goto slow_path;
                    } else if (p1->class_id == JS_CLASS_OBJECT) {
                        if (UNLIKELY(sh1->has_small_array_index))
                            goto slow_path;
                    } else {
                        goto slow_path;
                    }
                    p1 = sh1->proto;
                }
                /* add element */
                return add_fast_array_element(ctx, p, val, flags);
            }
            set_value(ctx, &p->u.array.u.values[idx], val);
            break;
        case JS_CLASS_ARGUMENTS:
            if (UNLIKELY(idx >= (uint32_t)p->u.array.count))
                goto slow_path;
            set_value(ctx, &p->u.array.u.values[idx], val);
            break;
        case JS_CLASS_UINT8C_ARRAY:
            if (JS_ToUint8ClampFree(ctx, &v, val))
                return -1;
            /* Note: the conversion can detach the typed array, so the
               array bound check must be done after */
            if (UNLIKELY(idx >= (uint32_t)p->u.array.count))
                goto ta_out_of_bound;
            p->u.array.u.uint8_ptr[idx] = v;
            break;
        case JS_CLASS_INT8_ARRAY:
        case JS_CLASS_UINT8_ARRAY:
            if (JS_ToInt32Free(ctx, &v, val))
                return -1;
            if (UNLIKELY(idx >= (uint32_t)p->u.array.count))
                goto ta_out_of_bound;
            p->u.array.u.uint8_ptr[idx] = v;
            break;
        case JS_CLASS_INT16_ARRAY:
        case JS_CLASS_UINT16_ARRAY:
            if (JS_ToInt32Free(ctx, &v, val))
                return -1;
            if (UNLIKELY(idx >= (uint32_t)p->u.array.count))
                goto ta_out_of_bound;
            p->u.array.u.uint16_ptr[idx] = v;
            break;
        case JS_CLASS_INT32_ARRAY:
        case JS_CLASS_UINT32_ARRAY:
            if (JS_ToInt32Free(ctx, &v, val))
                return -1;
            if (UNLIKELY(idx >= (uint32_t)p->u.array.count))
                goto ta_out_of_bound;
            p->u.array.u.uint32_ptr[idx] = v;
            break;
#ifdef CONFIG_BIGNUM
        case JS_CLASS_BIG_INT64_ARRAY:
        case JS_CLASS_BIG_UINT64_ARRAY:
            /* XXX: need specific conversion function */
            {
                int64_t v;
                if (JS_ToBigInt64Free(ctx, &v, val))
                    return -1;
                if (UNLIKELY(idx >= (uint32_t)p->u.array.count))
                    goto ta_out_of_bound;
                p->u.array.u.uint64_ptr[idx] = v;
            }
            break;
#endif
        case JS_CLASS_FLOAT32_ARRAY:
            if (JS_ToFloat64Free(ctx, &d, val))
                return -1;
            if (UNLIKELY(idx >= (uint32_t)p->u.array.count))
                goto ta_out_of_bound;
            p->u.array.u.float_ptr[idx] = d;
            break;
        case JS_CLASS_FLOAT64_ARRAY:
            if (JS_ToFloat64Free(ctx, &d, val))
                return -1;
            if (UNLIKELY(idx >= (uint32_t)p->u.array.count)) {
            ta_out_of_bound:
                return JS_ThrowTypeErrorOrFalse(ctx, flags, "out-of-bound numeric index");
            }
            p->u.array.u.double_ptr[idx] = d;
            break;
        default:
            goto slow_path;
        }
        return TRUE;
    } else {
        JSAtom atom;
        int ret;
    slow_path:
        atom = JS_ValueToAtom(ctx, prop);
        JS_FreeValue(ctx, prop);
        if (UNLIKELY(atom == JS_ATOM_NULL)) {
            JS_FreeValue(ctx, val);
            return -1;
        }
        ret = JS_SetPropertyInternal(ctx, this_obj, atom, val, flags);
        JS_FreeAtom(ctx, atom);
        return ret;
    }
}

int JS_SetPropertyUint32(JSContext *ctx, JSValueConst this_obj,
                         uint32_t idx, JSValue val)
{
    return JS_SetPropertyValue(ctx, this_obj, JS_NewUint32(ctx, idx), val,
                               JS_PROP_THROW);
}

int JS_SetPropertyInt64(JSContext *ctx, JSValueConst this_obj,
                        int64_t idx, JSValue val)
{
    JSAtom prop;
    int res;
    if ((uint64_t)idx <= INT32_MAX) {
        /* fast path for fast arrays */
        return JS_SetPropertyValue(ctx, this_obj, JS_NewInt32(ctx, idx), val,
                                   JS_PROP_THROW);
    }
    prop = JS_NewAtomInt64(ctx, idx);
    if (prop == JS_ATOM_NULL) {
        JS_FreeValue(ctx, val);
        return -1;
    }
    res = JS_SetProperty(ctx, this_obj, prop, val);
    JS_FreeAtom(ctx, prop);
    return res;
}

int JS_SetPropertyStr(JSContext *ctx, JSValueConst this_obj,
                      const char *prop, JSValue val)
{
    JSAtom atom;
    int ret;
    atom = JS_NewAtom(ctx, prop);
    ret = JS_SetPropertyInternal(ctx, this_obj, atom, val, JS_PROP_THROW);
    JS_FreeAtom(ctx, atom);
    return ret;
}

/* compute the property flags. For each flag: (JS_PROP_HAS_x forces
   it, otherwise def_flags is used)
   Note: makes assumption about the bit pattern of the flags
*/
static int get_prop_flags(int flags, int def_flags)
{
    int mask;
    mask = (flags >> JS_PROP_HAS_SHIFT) & JS_PROP_C_W_E;
    return (flags & mask) | (def_flags & ~mask);
}

static int JS_CreateProperty(JSContext *ctx, JSObject *p,
                             JSAtom prop, JSValueConst val,
                             JSValueConst getter, JSValueConst setter,
                             int flags)
{
    JSProperty *pr;
    int ret, prop_flags;
    /* add a new property or modify an existing exotic one */
    if (p->is_exotic) {
        if (p->class_id == JS_CLASS_ARRAY) {
            uint32_t idx, len;
            if (p->fast_array) {
                if (__JS_AtomIsTaggedInt(prop)) {
                    idx = __JS_AtomToUInt32(prop);
                    if (idx == p->u.array.count) {
                        if (!p->extensible)
                            goto not_extensible;
                        if (flags & (JS_PROP_HAS_GET | JS_PROP_HAS_SET))
                            goto convert_to_array;
                        prop_flags = get_prop_flags(flags, 0);
                        if (prop_flags != JS_PROP_C_W_E)
                            goto convert_to_array;
                        return add_fast_array_element(ctx, p,
                                                      JS_DupValue(ctx, val), flags);
                    } else {
                        goto convert_to_array;
                    }
                } else if (JS_AtomIsArrayIndex(ctx, &idx, prop)) {
                    /* convert the fast array to normal array */
                convert_to_array:
                    if (convert_fast_array_to_array(ctx, p))
                        return -1;
                    goto generic_array;
                }
            } else if (JS_AtomIsArrayIndex(ctx, &idx, prop)) {
                JSProperty *plen;
                JSShapeProperty *pslen;
            generic_array:
                /* update the length field */
                plen = &p->prop[0];
                JS_ToUint32(ctx, &len, plen->u.value);
                if ((idx + 1) > len) {
                    pslen = get_shape_prop(p->shape);
                    if (UNLIKELY(!(pslen->flags & JS_PROP_WRITABLE)))
                        return JS_ThrowTypeErrorReadOnly(ctx, flags, JS_ATOM_length);
                    /* XXX: should update the length after defining
                       the property */
                    len = idx + 1;
                    set_value(ctx, &plen->u.value, JS_NewUint32(ctx, len));
                }
            }
        } else if (p->class_id >= JS_CLASS_UINT8C_ARRAY &&
                   p->class_id <= JS_CLASS_FLOAT64_ARRAY) {
            ret = JS_AtomIsNumericIndex(ctx, prop);
            if (ret != 0) {
                if (ret < 0)
                    return -1;
                return JS_ThrowTypeErrorOrFalse(ctx, flags, "cannot create numeric index in typed array");
            }
        } else if (!(flags & JS_PROP_NO_EXOTIC)) {
            const JSClassExoticMethods *em = ctx->rt->class_array[p->class_id].exotic;
            if (em) {
                if (em->define_own_property) {
                    return em->define_own_property(ctx, JS_MKPTR(JS_TAG_OBJECT, p),
                                                   prop, val, getter, setter, flags);
                }
                ret = JS_IsExtensible(ctx, JS_MKPTR(JS_TAG_OBJECT, p));
                if (ret < 0)
                    return -1;
                if (!ret)
                    goto not_extensible;
            }
        }
    }
    if (!p->extensible) {
    not_extensible:
        return JS_ThrowTypeErrorOrFalse(ctx, flags, "object is not extensible");
    }
    if (flags & (JS_PROP_HAS_GET | JS_PROP_HAS_SET)) {
        prop_flags = (flags & (JS_PROP_CONFIGURABLE | JS_PROP_ENUMERABLE)) |
            JS_PROP_GETSET;
    } else {
        prop_flags = flags & JS_PROP_C_W_E;
    }
    pr = add_property(ctx, p, prop, prop_flags);
    if (UNLIKELY(!pr))
        return -1;
    if (flags & (JS_PROP_HAS_GET | JS_PROP_HAS_SET)) {
        pr->u.getset.getter = NULL;
        if ((flags & JS_PROP_HAS_GET) && JS_IsFunction(ctx, getter)) {
            pr->u.getset.getter =
                JS_VALUE_GET_OBJ(JS_DupValue(ctx, getter));
        }
        pr->u.getset.setter = NULL;
        if ((flags & JS_PROP_HAS_SET) && JS_IsFunction(ctx, setter)) {
            pr->u.getset.setter =
                JS_VALUE_GET_OBJ(JS_DupValue(ctx, setter));
        }
    } else {
        if (flags & JS_PROP_HAS_VALUE) {
            pr->u.value = JS_DupValue(ctx, val);
        } else {
            pr->u.value = JS_UNDEFINED;
        }
    }
    return TRUE;
}

/* return FALSE if not OK */
BOOL check_define_prop_flags(int prop_flags, int flags)
{
    BOOL has_accessor, is_getset;
    if (!(prop_flags & JS_PROP_CONFIGURABLE)) {
        if ((flags & (JS_PROP_HAS_CONFIGURABLE | JS_PROP_CONFIGURABLE)) ==
            (JS_PROP_HAS_CONFIGURABLE | JS_PROP_CONFIGURABLE)) {
            return FALSE;
        }
        if ((flags & JS_PROP_HAS_ENUMERABLE) &&
            (flags & JS_PROP_ENUMERABLE) != (prop_flags & JS_PROP_ENUMERABLE))
            return FALSE;
    }
    if (flags & (JS_PROP_HAS_VALUE | JS_PROP_HAS_WRITABLE |
                 JS_PROP_HAS_GET | JS_PROP_HAS_SET)) {
        if (!(prop_flags & JS_PROP_CONFIGURABLE)) {
            has_accessor = ((flags & (JS_PROP_HAS_GET | JS_PROP_HAS_SET)) != 0);
            is_getset = ((prop_flags & JS_PROP_TMASK) == JS_PROP_GETSET);
            if (has_accessor != is_getset)
                return FALSE;
            if (!has_accessor && !is_getset && !(prop_flags & JS_PROP_WRITABLE)) {
                /* not writable: cannot set the writable bit */
                if ((flags & (JS_PROP_HAS_WRITABLE | JS_PROP_WRITABLE)) ==
                    (JS_PROP_HAS_WRITABLE | JS_PROP_WRITABLE))
                    return FALSE;
            }
        }
    }
    return TRUE;
}

/* ensure that the shape can be safely modified */
static int js_shape_prepare_update(JSContext *ctx, JSObject *p,
                                   JSShapeProperty **pprs)
{
    JSShape *sh;
    uint32_t idx = 0;    /* prevent warning */
    sh = p->shape;
    if (sh->is_hashed) {
        if (sh->header.ref_count != 1) {
            if (pprs)
                idx = *pprs - get_shape_prop(sh);
            /* clone the shape (the resulting one is no longer hashed) */
            sh = js_clone_shape(ctx, sh);
            if (!sh)
                return -1;
            js_free_shape(ctx->rt, p->shape);
            p->shape = sh;
            if (pprs)
                *pprs = get_shape_prop(sh) + idx;
        } else {
            js_shape_hash_unlink(ctx->rt, sh);
            sh->is_hashed = FALSE;
        }
    }
    return 0;
}

int js_update_property_flags(JSContext *ctx, JSObject *p,
                             JSShapeProperty **pprs, int flags)
{
    if (flags != (*pprs)->flags) {
        if (js_shape_prepare_update(ctx, p, pprs))
            return -1;
        (*pprs)->flags = flags;
    }
    return 0;
}

/* allowed flags:
   JS_PROP_CONFIGURABLE, JS_PROP_WRITABLE, JS_PROP_ENUMERABLE
   JS_PROP_HAS_GET, JS_PROP_HAS_SET, JS_PROP_HAS_VALUE,
   JS_PROP_HAS_CONFIGURABLE, JS_PROP_HAS_WRITABLE, JS_PROP_HAS_ENUMERABLE,
   JS_PROP_THROW, JS_PROP_NO_EXOTIC.
   If JS_PROP_THROW is set, return an exception instead of FALSE.
   if JS_PROP_NO_EXOTIC is set, do not call the exotic
   define_own_property callback.
   return -1 (exception), FALSE or TRUE.
*/
int JS_DefineProperty(JSContext *ctx, JSValueConst this_obj,
                      JSAtom prop, JSValueConst val,
                      JSValueConst getter, JSValueConst setter, int flags)
{
    JSObject *p;
    JSShapeProperty *prs;
    JSProperty *pr;
    int mask, res;
    if (JS_VALUE_GET_TAG(this_obj) != JS_TAG_OBJECT) {
        JS_ThrowTypeErrorNotAnObject(ctx);
        return -1;
    }
    p = JS_VALUE_GET_OBJ(this_obj);
 redo_prop_update:
    prs = find_own_property(&pr, p, prop);
    if (prs) {
        /* the range of the Array length property is always tested before */
        if ((prs->flags & JS_PROP_LENGTH) && (flags & JS_PROP_HAS_VALUE)) {
            uint32_t array_length;
            if (JS_ToArrayLengthFree(ctx, &array_length,
                                     JS_DupValue(ctx, val), FALSE)) {
                return -1;
            }
            /* this code relies on the fact that Uint32 are never allocated */
            val = (JSValueConst)JS_NewUint32(ctx, array_length);
            /* prs may have been modified */
            prs = find_own_property(&pr, p, prop);
            assert(prs != NULL);
        }
        /* property already exists */
        if (!check_define_prop_flags(prs->flags, flags)) {
        not_configurable:
            return JS_ThrowTypeErrorOrFalse(ctx, flags, "property is not configurable");
        }
        if ((prs->flags & JS_PROP_TMASK) == JS_PROP_AUTOINIT) {
            /* Instantiate property and retry */
            if (JS_AutoInitProperty(ctx, p, prop, pr, prs))
                return -1;
            goto redo_prop_update;
        }
        if (flags & (JS_PROP_HAS_VALUE | JS_PROP_HAS_WRITABLE |
                     JS_PROP_HAS_GET | JS_PROP_HAS_SET)) {
            if (flags & (JS_PROP_HAS_GET | JS_PROP_HAS_SET)) {
                JSObject *new_getter, *new_setter;
                if (JS_IsFunction(ctx, getter)) {
                    new_getter = JS_VALUE_GET_OBJ(getter);
                } else {
                    new_getter = NULL;
                }
                if (JS_IsFunction(ctx, setter)) {
                    new_setter = JS_VALUE_GET_OBJ(setter);
                } else {
                    new_setter = NULL;
                }
                if ((prs->flags & JS_PROP_TMASK) != JS_PROP_GETSET) {
                    if (js_shape_prepare_update(ctx, p, &prs))
                        return -1;
                    /* convert to getset */
                    if ((prs->flags & JS_PROP_TMASK) == JS_PROP_VARREF) {
                        free_var_ref(ctx->rt, pr->u.var_ref);
                    } else {
                        JS_FreeValue(ctx, pr->u.value);
                    }
                    prs->flags = (prs->flags &
                                  (JS_PROP_CONFIGURABLE | JS_PROP_ENUMERABLE)) |
                        JS_PROP_GETSET;
                    pr->u.getset.getter = NULL;
                    pr->u.getset.setter = NULL;
                } else {
                    if (!(prs->flags & JS_PROP_CONFIGURABLE)) {
                        if ((flags & JS_PROP_HAS_GET) &&
                            new_getter != pr->u.getset.getter) {
                            goto not_configurable;
                        }
                        if ((flags & JS_PROP_HAS_SET) &&
                            new_setter != pr->u.getset.setter) {
                            goto not_configurable;
                        }
                    }
                }
                if (flags & JS_PROP_HAS_GET) {
                    if (pr->u.getset.getter)
                        JS_FreeValue(ctx, JS_MKPTR(JS_TAG_OBJECT, pr->u.getset.getter));
                    if (new_getter)
                        JS_DupValue(ctx, getter);
                    pr->u.getset.getter = new_getter;
                }
                if (flags & JS_PROP_HAS_SET) {
                    if (pr->u.getset.setter)
                        JS_FreeValue(ctx, JS_MKPTR(JS_TAG_OBJECT, pr->u.getset.setter));
                    if (new_setter)
                        JS_DupValue(ctx, setter);
                    pr->u.getset.setter = new_setter;
                }
            } else {
                if ((prs->flags & JS_PROP_TMASK) == JS_PROP_GETSET) {
                    /* convert to data descriptor */
                    if (js_shape_prepare_update(ctx, p, &prs))
                        return -1;
                    if (pr->u.getset.getter)
                        JS_FreeValue(ctx, JS_MKPTR(JS_TAG_OBJECT, pr->u.getset.getter));
                    if (pr->u.getset.setter)
                        JS_FreeValue(ctx, JS_MKPTR(JS_TAG_OBJECT, pr->u.getset.setter));
                    prs->flags &= ~(JS_PROP_TMASK | JS_PROP_WRITABLE);
                    pr->u.value = JS_UNDEFINED;
                } else if ((prs->flags & JS_PROP_TMASK) == JS_PROP_VARREF) {
                    /* Note: JS_PROP_VARREF is always writable */
                } else {
                    if ((prs->flags & (JS_PROP_CONFIGURABLE | JS_PROP_WRITABLE)) == 0 &&
                        (flags & JS_PROP_HAS_VALUE)) {
                        if (!js_same_value(ctx, val, pr->u.value)) {
                            goto not_configurable;
                        } else {
                            return TRUE;
                        }
                    }
                }
                if ((prs->flags & JS_PROP_TMASK) == JS_PROP_VARREF) {
                    if (flags & JS_PROP_HAS_VALUE) {
                        if (p->class_id == JS_CLASS_MODULE_NS) {
                            /* JS_PROP_WRITABLE is always true for variable
                               references, but they are write protected in module name
                               spaces. */
                            if (!js_same_value(ctx, val, *pr->u.var_ref->pvalue))
                                goto not_configurable;
                        }
                        /* update the reference */
                        set_value(ctx, pr->u.var_ref->pvalue,
                                  JS_DupValue(ctx, val));
                    }
                    /* if writable is set to false, no longer a
                       reference (for mapped arguments) */
                    if ((flags & (JS_PROP_HAS_WRITABLE | JS_PROP_WRITABLE)) == JS_PROP_HAS_WRITABLE) {
                        JSValue val1;
                        if (js_shape_prepare_update(ctx, p, &prs))
                            return -1;
                        val1 = JS_DupValue(ctx, *pr->u.var_ref->pvalue);
                        free_var_ref(ctx->rt, pr->u.var_ref);
                        pr->u.value = val1;
                        prs->flags &= ~(JS_PROP_TMASK | JS_PROP_WRITABLE);
                    }
                } else if (prs->flags & JS_PROP_LENGTH) {
                    if (flags & JS_PROP_HAS_VALUE) {
                        /* Note: no JS code is executable because
                           'val' is guaranted to be a Uint32 */
                        res = set_array_length(ctx, p, JS_DupValue(ctx, val),
                                               flags);
                    } else {
                        res = TRUE;
                    }
                    /* still need to reset the writable flag if
                       needed.  The JS_PROP_LENGTH is kept because the
                       Uint32 test is still done if the length
                       property is read-only. */
                    if ((flags & (JS_PROP_HAS_WRITABLE | JS_PROP_WRITABLE)) ==
                        JS_PROP_HAS_WRITABLE) {
                        prs = get_shape_prop(p->shape);
                        if (js_update_property_flags(ctx, p, &prs,
                                                     prs->flags & ~JS_PROP_WRITABLE))
                            return -1;
                    }
                    return res;
                } else {
                    if (flags & JS_PROP_HAS_VALUE) {
                        JS_FreeValue(ctx, pr->u.value);
                        pr->u.value = JS_DupValue(ctx, val);
                    }
                    if (flags & JS_PROP_HAS_WRITABLE) {
                        if (js_update_property_flags(ctx, p, &prs,
                                                     (prs->flags & ~JS_PROP_WRITABLE) |
                                                     (flags & JS_PROP_WRITABLE)))
                            return -1;
                    }
                }
            }
        }
        mask = 0;
        if (flags & JS_PROP_HAS_CONFIGURABLE)
            mask |= JS_PROP_CONFIGURABLE;
        if (flags & JS_PROP_HAS_ENUMERABLE)
            mask |= JS_PROP_ENUMERABLE;
        if (js_update_property_flags(ctx, p, &prs,
                                     (prs->flags & ~mask) | (flags & mask)))
            return -1;
        return TRUE;
    }
    /* handle modification of fast array elements */
    if (p->fast_array) {
        uint32_t idx;
        uint32_t prop_flags;
        if (p->class_id == JS_CLASS_ARRAY) {
            if (__JS_AtomIsTaggedInt(prop)) {
                idx = __JS_AtomToUInt32(prop);
                if (idx < p->u.array.count) {
                    prop_flags = get_prop_flags(flags, JS_PROP_C_W_E);
                    if (prop_flags != JS_PROP_C_W_E)
                        goto convert_to_slow_array;
                    if (flags & (JS_PROP_HAS_GET | JS_PROP_HAS_SET)) {
                    convert_to_slow_array:
                        if (convert_fast_array_to_array(ctx, p))
                            return -1;
                        else
                            goto redo_prop_update;
                    }
                    if (flags & JS_PROP_HAS_VALUE) {
                        set_value(ctx, &p->u.array.u.values[idx], JS_DupValue(ctx, val));
                    }
                    return TRUE;
                }
            }
        } else if (p->class_id >= JS_CLASS_UINT8C_ARRAY &&
                   p->class_id <= JS_CLASS_FLOAT64_ARRAY) {
            JSValue num;
            int ret;
            if (!__JS_AtomIsTaggedInt(prop)) {
                /* slow path with to handle all numeric indexes */
                num = JS_AtomIsNumericIndex1(ctx, prop);
                if (JS_IsUndefined(num))
                    goto typed_array_done;
                if (JS_IsException(num))
                    return -1;
                ret = JS_NumberIsInteger(ctx, num);
                if (ret < 0) {
                    JS_FreeValue(ctx, num);
                    return -1;
                }
                if (!ret) {
                    JS_FreeValue(ctx, num);
                    return JS_ThrowTypeErrorOrFalse(ctx, flags, "non integer index in typed array");
                }
                ret = JS_NumberIsNegativeOrMinusZero(ctx, num);
                JS_FreeValue(ctx, num);
                if (ret) {
                    return JS_ThrowTypeErrorOrFalse(ctx, flags, "negative index in typed array");
                }
                if (!__JS_AtomIsTaggedInt(prop))
                    goto typed_array_oob;
            }
            idx = __JS_AtomToUInt32(prop);
            /* if the typed array is detached, p->u.array.count = 0 */
            if (idx >= typed_array_get_length(ctx, p)) {
            typed_array_oob:
                return JS_ThrowTypeErrorOrFalse(ctx, flags, "out-of-bound index in typed array");
            }
            prop_flags = get_prop_flags(flags, JS_PROP_ENUMERABLE | JS_PROP_WRITABLE | JS_PROP_CONFIGURABLE);
            if (flags & (JS_PROP_HAS_GET | JS_PROP_HAS_SET) ||
                prop_flags != (JS_PROP_ENUMERABLE | JS_PROP_WRITABLE | JS_PROP_CONFIGURABLE)) {
                return JS_ThrowTypeErrorOrFalse(ctx, flags, "invalid descriptor flags");
            }
            if (flags & JS_PROP_HAS_VALUE) {
                return JS_SetPropertyValue(ctx, this_obj, JS_NewInt32(ctx, idx), JS_DupValue(ctx, val), flags);
            }
            return TRUE;
        typed_array_done: ;
        }
    }
    return JS_CreateProperty(ctx, p, prop, val, getter, setter, flags);
}

int JS_DefineAutoInitProperty(JSContext *ctx, JSValueConst this_obj,
                              JSAtom prop, JSAutoInitIDEnum id,
                              void *opaque, int flags)
{
    JSObject *p;
    JSProperty *pr;

    if (JS_VALUE_GET_TAG(this_obj) != JS_TAG_OBJECT)
        return FALSE;

    p = JS_VALUE_GET_OBJ(this_obj);

    if (find_own_property(&pr, p, prop)) {
        /* property already exists */
        abort();
        return FALSE;
    }

    /* Specialized CreateProperty */
    pr = add_property(ctx, p, prop, (flags & JS_PROP_C_W_E) | JS_PROP_AUTOINIT);
    if (UNLIKELY(!pr))
        return -1;
    pr->u.init.realm_and_id = (uintptr_t)JS_DupContext(ctx);
    assert((pr->u.init.realm_and_id & 3) == 0);
    assert(id <= 3);
    pr->u.init.realm_and_id |= id;
    pr->u.init.opaque = opaque;
    return TRUE;
}

/* shortcut to add or redefine a new property value */
int JS_DefinePropertyValue(JSContext *ctx, JSValueConst this_obj,
                           JSAtom prop, JSValue val, int flags)
{
    int ret;
    ret = JS_DefineProperty(ctx, this_obj, prop, val, JS_UNDEFINED, JS_UNDEFINED,
                            flags | JS_PROP_HAS_VALUE | JS_PROP_HAS_CONFIGURABLE | JS_PROP_HAS_WRITABLE | JS_PROP_HAS_ENUMERABLE);
    JS_FreeValue(ctx, val);
    return ret;
}

int JS_DefinePropertyValueValue(JSContext *ctx, JSValueConst this_obj,
                                JSValue prop, JSValue val, int flags)
{
    JSAtom atom;
    int ret;
    atom = JS_ValueToAtom(ctx, prop);
    JS_FreeValue(ctx, prop);
    if (UNLIKELY(atom == JS_ATOM_NULL)) {
        JS_FreeValue(ctx, val);
        return -1;
    }
    ret = JS_DefinePropertyValue(ctx, this_obj, atom, val, flags);
    JS_FreeAtom(ctx, atom);
    return ret;
}

int JS_DefinePropertyValueUint32(JSContext *ctx, JSValueConst this_obj,
                                 uint32_t idx, JSValue val, int flags)
{
    return JS_DefinePropertyValueValue(ctx, this_obj, JS_NewUint32(ctx, idx),
                                       val, flags);
}

int JS_DefinePropertyValueInt64(JSContext *ctx, JSValueConst this_obj,
                                int64_t idx, JSValue val, int flags)
{
    return JS_DefinePropertyValueValue(ctx, this_obj, JS_NewInt64(ctx, idx),
                                       val, flags);
}

int JS_DefinePropertyValueStr(JSContext *ctx, JSValueConst this_obj,
                              const char *prop, JSValue val, int flags)
{
    JSAtom atom;
    int ret;
    atom = JS_NewAtom(ctx, prop);
    ret = JS_DefinePropertyValue(ctx, this_obj, atom, val, flags);
    JS_FreeAtom(ctx, atom);
    return ret;
}

/* shortcut to add getter & setter */
int JS_DefinePropertyGetSet(JSContext *ctx, JSValueConst this_obj,
                            JSAtom prop, JSValue getter, JSValue setter,
                            int flags)
{
    int ret;
    ret = JS_DefineProperty(ctx, this_obj, prop, JS_UNDEFINED, getter, setter,
                            flags | JS_PROP_HAS_GET | JS_PROP_HAS_SET |
                            JS_PROP_HAS_CONFIGURABLE | JS_PROP_HAS_ENUMERABLE);
    JS_FreeValue(ctx, getter);
    JS_FreeValue(ctx, setter);
    return ret;
}

int JS_CreateDataPropertyUint32(JSContext *ctx, JSValueConst this_obj,
                                int64_t idx, JSValue val, int flags)
{
    return JS_DefinePropertyValueValue(ctx, this_obj, JS_NewInt64(ctx, idx),
                                       val, flags | JS_PROP_CONFIGURABLE |
                                       JS_PROP_ENUMERABLE | JS_PROP_WRITABLE);
}


/* return TRUE if 'obj' has a non empty 'name' string */
static BOOL js_object_has_name(JSContext *ctx, JSValueConst obj)
{
    JSProperty *pr;
    JSShapeProperty *prs;
    JSValueConst val;
    JSString *p;
    prs = find_own_property(&pr, JS_VALUE_GET_OBJ(obj), JS_ATOM_name);
    if (!prs)
        return FALSE;
    if ((prs->flags & JS_PROP_TMASK) != JS_PROP_NORMAL)
        return TRUE;
    val = pr->u.value;
    if (JS_VALUE_GET_TAG(val) != JS_TAG_STRING)
        return TRUE;
    p = JS_VALUE_GET_STRING(val);
    return (p->len != 0);
}

int JS_DefineObjectName(JSContext *ctx, JSValueConst obj, JSAtom name, int flags)
{
    if (name != JS_ATOM_NULL
    &&  JS_IsObject(obj)
    &&  !js_object_has_name(ctx, obj)
    &&  JS_DefinePropertyValue(ctx, obj, JS_ATOM_name, JS_AtomToString(ctx, name), flags) < 0) {
        return -1;
    }
    return 0;
}

int JS_DefineObjectNameComputed(JSContext *ctx, JSValueConst obj, JSValueConst str, int flags)
{
    if (JS_IsObject(obj) &&
        !js_object_has_name(ctx, obj)) {
        JSAtom prop;
        JSValue name_str;
        prop = JS_ValueToAtom(ctx, str);
        if (prop == JS_ATOM_NULL)
            return -1;
        name_str = js_get_function_name(ctx, prop);
        JS_FreeAtom(ctx, prop);
        if (JS_IsException(name_str))
            return -1;
        if (JS_DefinePropertyValue(ctx, obj, JS_ATOM_name, name_str, flags) < 0)
            return -1;
    }
    return 0;
}

/* return -1, FALSE or TRUE. return FALSE if not configurable or
   invalid object. return -1 in case of exception.
   flags can be 0, JS_PROP_THROW or JS_PROP_THROW_STRICT */
int JS_DeleteProperty(JSContext *ctx, JSValueConst obj, JSAtom prop, int flags)
{
    JSValue obj1;
    JSObject *p;
    int res;
    obj1 = JS_ToObject(ctx, obj);
    if (JS_IsException(obj1))
        return -1;
    p = JS_VALUE_GET_OBJ(obj1);
    res = delete_property(ctx, p, prop);
    JS_FreeValue(ctx, obj1);
    if (res != FALSE)
        return res;
    if ((flags & JS_PROP_THROW) ||
        ((flags & JS_PROP_THROW_STRICT) && is_strict_mode(ctx))) {
        JS_ThrowTypeError(ctx, "could not delete property");
        return -1;
    }
    return FALSE;
}

BOOL JS_IsFunction(JSContext *ctx, JSValueConst val)
{
    JSObject *p;
    if (JS_VALUE_GET_TAG(val) != JS_TAG_OBJECT)
        return FALSE;
    p = JS_VALUE_GET_OBJ(val);
    switch(p->class_id) {
    case JS_CLASS_BYTECODE_FUNCTION:
        return TRUE;
    case JS_CLASS_PROXY:
        return p->u.proxy_data->is_func;
    default:
        return (ctx->rt->class_array[p->class_id].call != NULL);
    }
}

BOOL JS_IsCFunction(JSContext *ctx, JSValueConst val, JSCFunction *func, int magic)
{
    JSObject *p;
    if (JS_VALUE_GET_TAG(val) != JS_TAG_OBJECT)
        return FALSE;
    p = JS_VALUE_GET_OBJ(val);
    if (p->class_id == JS_CLASS_C_FUNCTION)
        return (p->u.cfunc.c_function.generic == func && p->u.cfunc.magic == magic);
    else
        return FALSE;
}

BOOL JS_IsConstructor(JSContext *ctx, JSValueConst val)
{
    JSObject *p;
    if (JS_VALUE_GET_TAG(val) != JS_TAG_OBJECT)
        return FALSE;
    p = JS_VALUE_GET_OBJ(val);
    return p->is_constructor;
}

BOOL JS_SetConstructorBit(JSContext *ctx, JSValueConst func_obj, BOOL val)
{
    JSObject *p;
    if (JS_VALUE_GET_TAG(func_obj) != JS_TAG_OBJECT)
        return FALSE;
    p = JS_VALUE_GET_OBJ(func_obj);
    p->is_constructor = val;
    return TRUE;
}

BOOL JS_IsError(JSContext *ctx, JSValueConst val)
{
    JSObject *p;
    if (JS_VALUE_GET_TAG(val) != JS_TAG_OBJECT)
        return FALSE;
    p = JS_VALUE_GET_OBJ(val);
    return (p->class_id == JS_CLASS_ERROR);
}

void JS_SetUncatchableError(JSContext *ctx, JSValueConst val, BOOL flag)
{
    JSObject *p;
    if (JS_VALUE_GET_TAG(val) != JS_TAG_OBJECT)
        return;
    p = JS_VALUE_GET_OBJ(val);
    if (p->class_id == JS_CLASS_ERROR)
        p->is_uncatchable_error = flag;
}

void JS_ResetUncatchableError(JSContext *ctx)
{
    JS_SetUncatchableError(ctx, ctx->rt->current_exception, FALSE);
}

void JS_SetOpaque(JSValue obj, void *opaque)
{
   JSObject *p;
    if (JS_VALUE_GET_TAG(obj) == JS_TAG_OBJECT) {
        p = JS_VALUE_GET_OBJ(obj);
        p->u.opaque = opaque;
    }
}

/* return NULL if not an object of class class_id */
void *JS_GetOpaque(JSValueConst obj, JSClassID class_id)
{
    JSObject *p;
    if (JS_VALUE_GET_TAG(obj) != JS_TAG_OBJECT)
        return NULL;
    p = JS_VALUE_GET_OBJ(obj);
    if (p->class_id != class_id)
        return NULL;
    return p->u.opaque;
}

void *JS_GetOpaque2(JSContext *ctx, JSValueConst obj, JSClassID class_id)
{
    void *p = JS_GetOpaque(obj, class_id);
    if (UNLIKELY(!p)) {
        JS_ThrowTypeErrorInvalidClass(ctx, class_id);
    }
    return p;
}

void JS_SetIsHTMLDDA(JSContext *ctx, JSValueConst obj)
{
    JSObject *p;
    if (JS_VALUE_GET_TAG(obj) != JS_TAG_OBJECT)
        return;
    p = JS_VALUE_GET_OBJ(obj);
    p->is_HTMLDDA = TRUE;
}

static inline BOOL JS_IsHTMLDDA(JSContext *ctx, JSValueConst obj)
{
    JSObject *p;
    if (JS_VALUE_GET_TAG(obj) != JS_TAG_OBJECT)
        return FALSE;
    p = JS_VALUE_GET_OBJ(obj);
    return p->is_HTMLDDA;
}

int skip_spaces(const char *pc)
{
    const uint8_t *p, *p_next, *p_start;
    uint32_t c;
    p = p_start = (const uint8_t *)pc;
    for (;;) {
        c = *p;
        if (c < 128) {
            if (!((c >= 0x09 && c <= 0x0d) || (c == 0x20)))
                break;
            p++;
        } else {
            c = unicode_from_utf8(p, UTF8_CHAR_LEN_MAX, &p_next);
            if (!lre_is_space(c))
                break;
            p = p_next;
        }
    }
    return p - p_start;
}

typedef enum JSToNumberHintEnum {
    TON_FLAG_NUMBER,
    TON_FLAG_NUMERIC,
} JSToNumberHintEnum;

static JSValue JS_ToNumberHintFree(JSContext *ctx, JSValue val,
                                   JSToNumberHintEnum flag)
{
    uint32_t tag;
    JSValue ret;
 redo:
    tag = JS_VALUE_GET_NORM_TAG(val);
    switch(tag) {
#ifdef CONFIG_BIGNUM
    case JS_TAG_BIG_DECIMAL:
        if (flag != TON_FLAG_NUMERIC) {
            JS_FreeValue(ctx, val);
            return JS_ThrowTypeError(ctx, "cannot convert bigdecimal to number");
        }
        ret = val;
        break;
    case JS_TAG_BIG_INT:
        if (flag != TON_FLAG_NUMERIC) {
            JS_FreeValue(ctx, val);
            return JS_ThrowTypeError(ctx, "cannot convert bigint to number");
        }
        ret = val;
        break;
    case JS_TAG_BIG_FLOAT:
        if (flag != TON_FLAG_NUMERIC) {
            JS_FreeValue(ctx, val);
            return JS_ThrowTypeError(ctx, "cannot convert bigfloat to number");
        }
        ret = val;
        break;
#endif
    case JS_TAG_FLOAT64:
    case JS_TAG_INT:
    case JS_TAG_EXCEPTION:
        ret = val;
        break;
    case JS_TAG_BOOL:
    case JS_TAG_NULL:
        ret = JS_NewInt32(ctx, JS_VALUE_GET_INT(val));
        break;
    case JS_TAG_UNDEFINED:
        ret = JS_NAN;
        break;
    case JS_TAG_OBJECT:
        val = JS_ToPrimitiveFree(ctx, val, HINT_NUMBER);
        if (JS_IsException(val))
            return JS_EXCEPTION;
        goto redo;
    case JS_TAG_STRING:
        {
            const char *str;
            const char *p;
            size_t len;
            str = JS_ToCStringLen(ctx, &len, val);
            JS_FreeValue(ctx, val);
            if (!str)
                return JS_EXCEPTION;
            p = str;
            p += skip_spaces(p);
            if ((p - str) == len) {
                ret = JS_NewInt32(ctx, 0);
            } else {
                int flags = ATOD_ACCEPT_BIN_OCT;
                ret = js_atof(ctx, p, &p, 0, flags);
                if (!JS_IsException(ret)) {
                    p += skip_spaces(p);
                    if ((p - str) != len) {
                        JS_FreeValue(ctx, ret);
                        ret = JS_NAN;
                    }
                }
            }
            JS_FreeCString(ctx, str);
        }
        break;
    case JS_TAG_SYMBOL:
        JS_FreeValue(ctx, val);
        return JS_ThrowTypeError(ctx, "cannot convert symbol to number");
    default:
        JS_FreeValue(ctx, val);
        ret = JS_NAN;
        break;
    }
    return ret;
}

JSValue JS_ToNumberFree(JSContext *ctx, JSValue val)
{
    return JS_ToNumberHintFree(ctx, val, TON_FLAG_NUMBER);
}

JSValue JS_ToNumericFree(JSContext *ctx, JSValue val)
{
    return JS_ToNumberHintFree(ctx, val, TON_FLAG_NUMERIC);
}

JSValue JS_ToNumeric(JSContext *ctx, JSValueConst val)
{
    return JS_ToNumericFree(ctx, JS_DupValue(ctx, val));
}

__exception int __JS_ToFloat64Free(JSContext *ctx, double *pres, JSValue val)
{
    double d;
    uint32_t tag;
    val = JS_ToNumberFree(ctx, val);
    if (JS_IsException(val)) {
        *pres = JS_FLOAT64_NAN;
        return -1;
    }
    tag = JS_VALUE_GET_NORM_TAG(val);
    switch(tag) {
    case JS_TAG_INT:
        d = JS_VALUE_GET_INT(val);
        break;
    case JS_TAG_FLOAT64:
        d = JS_VALUE_GET_FLOAT64(val);
        break;
#ifdef CONFIG_BIGNUM
    case JS_TAG_BIG_INT:
    case JS_TAG_BIG_FLOAT:
        {
            JSBigFloat *p = JS_VALUE_GET_PTR(val);
            /* XXX: there can be a double rounding issue with some
               primitives (such as JS_ToUint8ClampFree()), but it is
               not critical to fix it. */
            bf_get_float64(&p->num, &d, BF_RNDN);
            JS_FreeValue(ctx, val);
        }
        break;
#endif
    default:
        abort();
    }
    *pres = d;
    return 0;
}

int JS_ToFloat64(JSContext *ctx, double *pres, JSValueConst val)
{
    return JS_ToFloat64Free(ctx, pres, JS_DupValue(ctx, val));
}

static JSValue JS_ToNumber(JSContext *ctx, JSValueConst val)
{
    return JS_ToNumberFree(ctx, JS_DupValue(ctx, val));
}

/* same as JS_ToNumber() but return 0 in case of NaN/Undefined */
static __maybe_unused JSValue JS_ToIntegerFree(JSContext *ctx, JSValue val)
{
    uint32_t tag;
    JSValue ret;
 redo:
    tag = JS_VALUE_GET_NORM_TAG(val);
    switch(tag) {
    case JS_TAG_INT:
    case JS_TAG_BOOL:
    case JS_TAG_NULL:
    case JS_TAG_UNDEFINED:
        ret = JS_NewInt32(ctx, JS_VALUE_GET_INT(val));
        break;
    case JS_TAG_FLOAT64:
        {
            double d = JS_VALUE_GET_FLOAT64(val);
            if (isnan(d)) {
                ret = JS_NewInt32(ctx, 0);
            } else {
                /* convert -0 to +0 */
                d = trunc(d) + 0.0;
                ret = JS_NewFloat64(ctx, d);
            }
        }
        break;
#ifdef CONFIG_BIGNUM
    case JS_TAG_BIG_FLOAT:
        {
            bf_t a_s, *a, r_s, *r = &r_s;
            BOOL is_nan;
            a = JS_ToBigFloat(ctx, &a_s, val);
            if (!bf_is_finite(a)) {
                is_nan = bf_is_nan(a);
                if (is_nan)
                    ret = JS_NewInt32(ctx, 0);
                else
                    ret = JS_DupValue(ctx, val);
            } else {
                ret = JS_NewBigInt(ctx);
                if (!JS_IsException(ret)) {
                    r = JS_GetBigInt(ret);
                    bf_set(r, a);
                    bf_rint(r, BF_RNDZ);
                    ret = JS_CompactBigInt(ctx, ret);
                }
            }
            if (a == &a_s)
                bf_delete(a);
            JS_FreeValue(ctx, val);
        }
        break;
#endif
    default:
        val = JS_ToNumberFree(ctx, val);
        if (JS_IsException(val))
            return val;
        goto redo;
    }
    return ret;
}

/* Note: the integer value is satured to 32 bits */
static int JS_ToInt32SatFree(JSContext *ctx, int *pres, JSValue val)
{
    uint32_t tag;
    int ret;

 redo:
    tag = JS_VALUE_GET_NORM_TAG(val);
    switch(tag) {
    case JS_TAG_INT:
    case JS_TAG_BOOL:
    case JS_TAG_NULL:
    case JS_TAG_UNDEFINED:
        ret = JS_VALUE_GET_INT(val);
        break;
    case JS_TAG_EXCEPTION:
        *pres = 0;
        return -1;
    case JS_TAG_FLOAT64:
        {
            double d = JS_VALUE_GET_FLOAT64(val);
            if (isnan(d)) {
                ret = 0;
            } else {
                if (d < INT32_MIN)
                    ret = INT32_MIN;
                else if (d > INT32_MAX)
                    ret = INT32_MAX;
                else
                    ret = (int)d;
            }
        }
        break;
#ifdef CONFIG_BIGNUM
    case JS_TAG_BIG_FLOAT:
        {
            JSBigFloat *p = JS_VALUE_GET_PTR(val);
            bf_get_int32(&ret, &p->num, 0);
            JS_FreeValue(ctx, val);
        }
        break;
#endif
    default:
        val = JS_ToNumberFree(ctx, val);
        if (JS_IsException(val)) {
            *pres = 0;
            return -1;
        }
        goto redo;
    }
    *pres = ret;
    return 0;
}

int JS_ToInt32Sat(JSContext *ctx, int *pres, JSValueConst val)
{
    return JS_ToInt32SatFree(ctx, pres, JS_DupValue(ctx, val));
}

int JS_ToInt32Clamp(JSContext *ctx, int *pres, JSValueConst val,
                    int min, int max, int min_offset)
{
    int res = JS_ToInt32SatFree(ctx, pres, JS_DupValue(ctx, val));
    if (res == 0) {
        if (*pres < min) {
            *pres += min_offset;
            if (*pres < min)
                *pres = min;
        } else {
            if (*pres > max)
                *pres = max;
        }
    }
    return res;
}

int JS_ToInt64SatFree(JSContext *ctx, int64_t *pres, JSValue val)
{
    uint32_t tag;
 redo:
    tag = JS_VALUE_GET_NORM_TAG(val);
    switch(tag) {
    case JS_TAG_INT:
    case JS_TAG_BOOL:
    case JS_TAG_NULL:
    case JS_TAG_UNDEFINED:
        *pres = JS_VALUE_GET_INT(val);
        return 0;
    case JS_TAG_EXCEPTION:
        *pres = 0;
        return -1;
    case JS_TAG_FLOAT64:
        {
            double d = JS_VALUE_GET_FLOAT64(val);
            if (isnan(d)) {
                *pres = 0;
            } else {
                if (d < INT64_MIN)
                    *pres = INT64_MIN;
                else if (d > (double)INT64_MAX)
                    *pres = INT64_MAX;
                else
                    *pres = (int64_t)d;
            }
        }
        return 0;
#ifdef CONFIG_BIGNUM
    case JS_TAG_BIG_FLOAT:
        {
            JSBigFloat *p = JS_VALUE_GET_PTR(val);
            bf_get_int64(pres, &p->num, 0);
            JS_FreeValue(ctx, val);
        }
        return 0;
#endif
    default:
        val = JS_ToNumberFree(ctx, val);
        if (JS_IsException(val)) {
            *pres = 0;
            return -1;
        }
        goto redo;
    }
}

int JS_ToInt64Sat(JSContext *ctx, int64_t *pres, JSValueConst val)
{
    return JS_ToInt64SatFree(ctx, pres, JS_DupValue(ctx, val));
}

int JS_ToInt64Clamp(JSContext *ctx, int64_t *pres, JSValueConst val,
                    int64_t min, int64_t max, int64_t neg_offset)
{
    int res = JS_ToInt64SatFree(ctx, pres, JS_DupValue(ctx, val));
    if (res == 0) {
        if (*pres < 0)
            *pres += neg_offset;
        if (*pres < min)
            *pres = min;
        else if (*pres > max)
            *pres = max;
    }
    return res;
}

/* Same as JS_ToInt32Free() but with a 64 bit result. Return (<0, 0)
   in case of exception */
static int JS_ToInt64Free(JSContext *ctx, int64_t *pres, JSValue val)
{
    uint32_t tag;
    int64_t ret;

 redo:
    tag = JS_VALUE_GET_NORM_TAG(val);
    switch(tag) {
    case JS_TAG_INT:
    case JS_TAG_BOOL:
    case JS_TAG_NULL:
    case JS_TAG_UNDEFINED:
        ret = JS_VALUE_GET_INT(val);
        break;
    case JS_TAG_FLOAT64:
        {
            JSFloat64Union u;
            double d;
            int e;
            d = JS_VALUE_GET_FLOAT64(val);
            u.d = d;
            /* we avoid doing fmod(x, 2^64) */
            e = (u.u64 >> 52) & 0x7ff;
            if (LIKELY(e <= (1023 + 62))) {
                /* fast case */
                ret = (int64_t)d;
            } else if (e <= (1023 + 62 + 53)) {
                uint64_t v;
                /* remainder modulo 2^64 */
                v = (u.u64 & (((uint64_t)1 << 52) - 1)) | ((uint64_t)1 << 52);
                ret = v << ((e - 1023) - 52);
                /* take the sign into account */
                if (u.u64 >> 63)
                    ret = -ret;
            } else {
                ret = 0; /* also handles NaN and +inf */
            }
        }
        break;
#ifdef CONFIG_BIGNUM
    case JS_TAG_BIG_FLOAT:
        {
            JSBigFloat *p = JS_VALUE_GET_PTR(val);
            bf_get_int64(&ret, &p->num, BF_GET_INT_MOD);
            JS_FreeValue(ctx, val);
        }
        break;
#endif
    default:
        val = JS_ToNumberFree(ctx, val);
        if (JS_IsException(val)) {
            *pres = 0;
            return -1;
        }
        goto redo;
    }
    *pres = ret;
    return 0;
}

int JS_ToInt64(JSContext *ctx, int64_t *pres, JSValueConst val)
{
    return JS_ToInt64Free(ctx, pres, JS_DupValue(ctx, val));
}

int JS_ToInt64Ext(JSContext *ctx, int64_t *pres, JSValueConst val)
{
    if (JS_IsBigInt(ctx, val))
        return JS_ToBigInt64(ctx, pres, val);
    else
        return JS_ToInt64(ctx, pres, val);
}

/* return (<0, 0) in case of exception */
int JS_ToInt32Free(JSContext *ctx, int32_t *pres, JSValue val)
{
    uint32_t tag;
    int32_t ret;
 redo:
    tag = JS_VALUE_GET_NORM_TAG(val);
    switch(tag) {
    case JS_TAG_INT:
    case JS_TAG_BOOL:
    case JS_TAG_NULL:
    case JS_TAG_UNDEFINED:
        ret = JS_VALUE_GET_INT(val);
        break;
    case JS_TAG_FLOAT64:
        {
            JSFloat64Union u;
            double d;
            int e;
            d = JS_VALUE_GET_FLOAT64(val);
            u.d = d;
            /* we avoid doing fmod(x, 2^32) */
            e = (u.u64 >> 52) & 0x7ff;
            if (LIKELY(e <= (1023 + 30))) {
                /* fast case */
                ret = (int32_t)d;
            } else if (e <= (1023 + 30 + 53)) {
                uint64_t v;
                /* remainder modulo 2^32 */
                v = (u.u64 & (((uint64_t)1 << 52) - 1)) | ((uint64_t)1 << 52);
                v = v << ((e - 1023) - 52 + 32);
                ret = v >> 32;
                /* take the sign into account */
                if (u.u64 >> 63)
                    ret = -ret;
            } else {
                ret = 0; /* also handles NaN and +inf */
            }
        }
        break;
#ifdef CONFIG_BIGNUM
    case JS_TAG_BIG_FLOAT:
        {
            JSBigFloat *p = JS_VALUE_GET_PTR(val);
            bf_get_int32(&ret, &p->num, BF_GET_INT_MOD);
            JS_FreeValue(ctx, val);
        }
        break;
#endif
    default:
        val = JS_ToNumberFree(ctx, val);
        if (JS_IsException(val)) {
            *pres = 0;
            return -1;
        }
        goto redo;
    }
    *pres = ret;
    return 0;
}

int JS_ToInt32(JSContext *ctx, int32_t *pres, JSValueConst val)
{
    return JS_ToInt32Free(ctx, pres, JS_DupValue(ctx, val));
}

static inline int JS_ToUint32Free(JSContext *ctx, uint32_t *pres, JSValue val)
{
    return JS_ToInt32Free(ctx, (int32_t *)pres, val);
}

int JS_ToUint8ClampFree(JSContext *ctx, int32_t *pres, JSValue val)
{
    uint32_t tag;
    int res;
 redo:
    tag = JS_VALUE_GET_NORM_TAG(val);
    switch(tag) {
    case JS_TAG_INT:
    case JS_TAG_BOOL:
    case JS_TAG_NULL:
    case JS_TAG_UNDEFINED:
        res = JS_VALUE_GET_INT(val);
#ifdef CONFIG_BIGNUM
    int_clamp:
#endif
        res = max_int(0, min_int(255, res));
        break;
    case JS_TAG_FLOAT64:
        {
            double d = JS_VALUE_GET_FLOAT64(val);
            if (isnan(d)) {
                res = 0;
            } else {
                if (d < 0)
                    res = 0;
                else if (d > 255)
                    res = 255;
                else
                    res = lrint(d);
            }
        }
        break;
#ifdef CONFIG_BIGNUM
    case JS_TAG_BIG_FLOAT:
        {
            JSBigFloat *p = JS_VALUE_GET_PTR(val);
            bf_t r_s, *r = &r_s;
            bf_init(ctx->bf_ctx, r);
            bf_set(r, &p->num);
            bf_rint(r, BF_RNDN);
            bf_get_int32(&res, r, 0);
            bf_delete(r);
            JS_FreeValue(ctx, val);
        }
        goto int_clamp;
#endif
    default:
        val = JS_ToNumberFree(ctx, val);
        if (JS_IsException(val)) {
            *pres = 0;
            return -1;
        }
        goto redo;
    }
    *pres = res;
    return 0;
}

static BOOL is_safe_integer(double d)
{
    return isfinite(d) && floor(d) == d &&
        fabs(d) <= (double)MAX_SAFE_INTEGER;
}

int JS_ToIndex(JSContext *ctx, uint64_t *plen, JSValueConst val)
{
    int64_t v;
    if (JS_ToInt64Sat(ctx, &v, val))
        return -1;
    if (v < 0 || v > MAX_SAFE_INTEGER) {
        JS_ThrowRangeError(ctx, "invalid array index");
        *plen = 0;
        return -1;
    }
    *plen = v;
    return 0;
}

/* convert a value to a length between 0 and MAX_SAFE_INTEGER.
   return -1 for exception */
int JS_ToLengthFree(JSContext *ctx, int64_t *plen, JSValue val)
{
    int res = JS_ToInt64Clamp(ctx, plen, val, 0, MAX_SAFE_INTEGER, 0);
    JS_FreeValue(ctx, val);
    return res;
}

/* Note: can return an exception */
static int JS_NumberIsInteger(JSContext *ctx, JSValueConst val)
{
    double d;
    if (!JS_IsNumber(val))
        return FALSE;
    if (UNLIKELY(JS_ToFloat64(ctx, &d, val)))
        return -1;
    return isfinite(d) && floor(d) == d;
}

static BOOL JS_NumberIsNegativeOrMinusZero(JSContext *ctx, JSValueConst val)
{
    uint32_t tag;

    tag = JS_VALUE_GET_NORM_TAG(val);
    switch(tag) {
    case JS_TAG_INT:
        {
            int v;
            v = JS_VALUE_GET_INT(val);
            return (v < 0);
        }
    case JS_TAG_FLOAT64:
        {
            JSFloat64Union u;
            u.d = JS_VALUE_GET_FLOAT64(val);
            return (u.u64 >> 63);
        }
#ifdef CONFIG_BIGNUM
    case JS_TAG_BIG_INT:
        {
            JSBigFloat *p = JS_VALUE_GET_PTR(val);
            /* Note: integer zeros are not necessarily positive */
            return p->num.sign && !bf_is_zero(&p->num);
        }
    case JS_TAG_BIG_FLOAT:
        {
            JSBigFloat *p = JS_VALUE_GET_PTR(val);
            return p->num.sign;
        }
        break;
    case JS_TAG_BIG_DECIMAL:
        {
            JSBigDecimal *p = JS_VALUE_GET_PTR(val);
            return p->num.sign;
        }
        break;
#endif
    default:
        return FALSE;
    }
}

JSValue JS_ToStringCheckObject(JSContext *ctx, JSValueConst val)
{
    uint32_t tag = JS_VALUE_GET_TAG(val);
    if (tag == JS_TAG_NULL || tag == JS_TAG_UNDEFINED)
        return JS_ThrowTypeError(ctx, "null or undefined are forbidden");
    return JS_ToString(ctx, val);
}

static __maybe_unused void JS_DumpObjectHeader(JSRuntime *rt)
{
    printf("%14s %4s %4s %14s %10s %s\n",
           "ADDRESS", "REFS", "SHRF", "PROTO", "CLASS", "PROPS");
}

/* for debug only: dump an object without side effect */
static __maybe_unused void JS_DumpObject(JSRuntime *rt, JSObject *p)
{
    uint32_t i;
    char atom_buf[ATOM_GET_STR_BUF_SIZE];
    JSShape *sh;
    JSShapeProperty *prs;
    JSProperty *pr;
    BOOL is_first = TRUE;
    /* XXX: should encode atoms with special characters */
    sh = p->shape; /* the shape can be NULL while freeing an object */
    printf("%14p %4d ",
           (void *)p,
           p->header.ref_count);
    if (sh) {
        printf("%3d%c %14p ",
               sh->header.ref_count,
               " *"[sh->is_hashed],
               (void *)sh->proto);
    } else {
        printf("%3s  %14s ", "-", "-");
    }
    printf("%10s ",
           JS_AtomGetStrRT(rt, atom_buf, sizeof(atom_buf), rt->class_array[p->class_id].class_name));
    if (p->is_exotic && p->fast_array) {
        printf("[ ");
        for(i = 0; i < p->u.array.count; i++) {
            if (i != 0)
                printf(", ");
            switch (p->class_id) {
            case JS_CLASS_ARRAY:
            case JS_CLASS_ARGUMENTS:
                JS_DumpValueShort(rt, p->u.array.u.values[i]);
                break;
            case JS_CLASS_UINT8C_ARRAY:
            case JS_CLASS_INT8_ARRAY:
            case JS_CLASS_UINT8_ARRAY:
            case JS_CLASS_INT16_ARRAY:
            case JS_CLASS_UINT16_ARRAY:
            case JS_CLASS_INT32_ARRAY:
            case JS_CLASS_UINT32_ARRAY:
#ifdef CONFIG_BIGNUM
            case JS_CLASS_BIG_INT64_ARRAY:
            case JS_CLASS_BIG_UINT64_ARRAY:
#endif
            case JS_CLASS_FLOAT32_ARRAY:
            case JS_CLASS_FLOAT64_ARRAY:
                {
                    int size = 1 << typed_array_size_log2(p->class_id);
                    const uint8_t *b = p->u.array.u.uint8_ptr + i * size;
                    while (size-- > 0)
                        printf("%02X", *b++);
                }
                break;
            }
        }
        printf(" ] ");
    }
    if (sh) {
        printf("{ ");
        for(i = 0, prs = get_shape_prop(sh); i < sh->prop_count; i++, prs++) {
            if (prs->atom != JS_ATOM_NULL) {
                pr = &p->prop[i];
                if (!is_first)
                    printf(", ");
                printf("%s: ",
                       JS_AtomGetStrRT(rt, atom_buf, sizeof(atom_buf), prs->atom));
                if ((prs->flags & JS_PROP_TMASK) == JS_PROP_GETSET) {
                    printf("[getset %p %p]", (void *)pr->u.getset.getter,
                           (void *)pr->u.getset.setter);
                } else if ((prs->flags & JS_PROP_TMASK) == JS_PROP_VARREF) {
                    printf("[varref %p]", (void *)pr->u.var_ref);
                } else if ((prs->flags & JS_PROP_TMASK) == JS_PROP_AUTOINIT) {
                    printf("[autoinit %p %d %p]",
                           (void *)js_autoinit_get_realm(pr),
                           js_autoinit_get_id(pr),
                           (void *)pr->u.init.opaque);
                } else {
                    JS_DumpValueShort(rt, pr->u.value);
                }
                is_first = FALSE;
            }
        }
        printf(" }");
    }
    if (js_class_has_bytecode(p->class_id)) {
        JSFunctionBytecode *b = p->u.func.function_bytecode;
        JSVarRef **var_refs;
        if (b->closure_var_count) {
            var_refs = p->u.func.var_refs;
            printf(" Closure:");
            for(i = 0; i < b->closure_var_count; i++) {
                printf(" ");
                JS_DumpValueShort(rt, var_refs[i]->value);
            }
            if (p->u.func.home_object) {
                printf(" HomeObject: ");
                JS_DumpValueShort(rt, JS_MKPTR(JS_TAG_OBJECT, p->u.func.home_object));
            }
        }
    }
    printf("\n");
}

static __maybe_unused void JS_DumpGCObject(JSRuntime *rt, JSGCObjectHeader *p)
{
    if (p->gc_obj_type == JS_GC_OBJ_TYPE_JS_OBJECT) {
        JS_DumpObject(rt, (JSObject *)p);
    } else {
        printf("%14p %4d ",
               (void *)p,
               p->ref_count);
        switch(p->gc_obj_type) {
        case JS_GC_OBJ_TYPE_FUNCTION_BYTECODE:
            printf("[function bytecode]");
            break;
        case JS_GC_OBJ_TYPE_SHAPE:
            printf("[shape]");
            break;
        case JS_GC_OBJ_TYPE_VAR_REF:
            printf("[var_ref]");
            break;
        case JS_GC_OBJ_TYPE_ASYNC_FUNCTION:
            printf("[async_function]");
            break;
        case JS_GC_OBJ_TYPE_JS_CONTEXT:
            printf("[js_context]");
            break;
        default:
            printf("[unknown %d]", p->gc_obj_type);
            break;
        }
        printf("\n");
    }
}

static __maybe_unused void JS_DumpValueShort(JSRuntime *rt,
                                             JSValueConst val)
{
    uint32_t tag = JS_VALUE_GET_NORM_TAG(val);
    const char *str;
    switch(tag) {
    case JS_TAG_INT:
        printf("%d", JS_VALUE_GET_INT(val));
        break;
    case JS_TAG_BOOL:
        if (JS_VALUE_GET_BOOL(val))
            str = "true";
        else
            str = "false";
        goto print_str;
    case JS_TAG_NULL:
        str = "null";
        goto print_str;
    case JS_TAG_EXCEPTION:
        str = "exception";
        goto print_str;
    case JS_TAG_UNINITIALIZED:
        str = "uninitialized";
        goto print_str;
    case JS_TAG_UNDEFINED:
        str = "undefined";
    print_str:
        printf("%s", str);
        break;
    case JS_TAG_FLOAT64:
        printf("%.14g", JS_VALUE_GET_FLOAT64(val));
        break;
#ifdef CONFIG_BIGNUM
    case JS_TAG_BIG_INT:
        {
            JSBigFloat *p = JS_VALUE_GET_PTR(val);
            char *str;
            str = bf_ftoa(NULL, &p->num, 10, 0,
                          BF_RNDZ | BF_FTOA_FORMAT_FRAC);
            printf("%sn", str);
            bf_realloc(&rt->bf_ctx, str, 0);
        }
        break;
    case JS_TAG_BIG_FLOAT:
        {
            JSBigFloat *p = JS_VALUE_GET_PTR(val);
            char *str;
            str = bf_ftoa(NULL, &p->num, 16, BF_PREC_INF,
                          BF_RNDZ | BF_FTOA_FORMAT_FREE | BF_FTOA_ADD_PREFIX);
            printf("%sl", str);
            bf_free(&rt->bf_ctx, str);
        }
        break;
    case JS_TAG_BIG_DECIMAL:
        {
            JSBigDecimal *p = JS_VALUE_GET_PTR(val);
            char *str;
            str = bfdec_ftoa(NULL, &p->num, BF_PREC_INF,
                             BF_RNDZ | BF_FTOA_FORMAT_FREE);
            printf("%sm", str);
            bf_free(&rt->bf_ctx, str);
        }
        break;
#endif
    case JS_TAG_STRING:
        {
            JSString *p;
            p = JS_VALUE_GET_STRING(val);
            JS_DumpString(rt, p);
        }
        break;
    case JS_TAG_FUNCTION_BYTECODE:
        {
            JSFunctionBytecode *b = JS_VALUE_GET_PTR(val);
            char buf[ATOM_GET_STR_BUF_SIZE];
            printf("[bytecode %s]", JS_AtomGetStrRT(rt, buf, sizeof(buf), b->func_name));
        }
        break;
    case JS_TAG_OBJECT:
        {
            JSObject *p = JS_VALUE_GET_OBJ(val);
            JSAtom atom = rt->class_array[p->class_id].class_name;
            char atom_buf[ATOM_GET_STR_BUF_SIZE];
            printf("[%s %p]",
                   JS_AtomGetStrRT(rt, atom_buf, sizeof(atom_buf), atom), (void *)p);
        }
        break;
    case JS_TAG_SYMBOL:
        {
            JSAtomStruct *p = JS_VALUE_GET_PTR(val);
            char atom_buf[ATOM_GET_STR_BUF_SIZE];
            printf("Symbol(%s)",
                   JS_AtomGetStrRT(rt, atom_buf, sizeof(atom_buf), js_get_atom_index(rt, p)));
        }
        break;
    case JS_TAG_MODULE:
        printf("[module]");
        break;
    default:
        printf("[unknown tag %d]", tag);
        break;
    }
}

static __maybe_unused void JS_DumpValue(JSContext *ctx,
                                                 JSValueConst val)
{
    JS_DumpValueShort(ctx->rt, val);
}

static __maybe_unused void JS_PrintValue(JSContext *ctx,
                                                  const char *str,
                                                  JSValueConst val)
{
    printf("%s=", str);
    JS_DumpValueShort(ctx->rt, val);
    printf("\n");
}

/* return -1 if exception (proxy case) or TRUE/FALSE */
int JS_IsArray(JSContext *ctx, JSValueConst val)
{
    JSObject *p;
    if (JS_VALUE_GET_TAG(val) == JS_TAG_OBJECT) {
        p = JS_VALUE_GET_OBJ(val);
        if (UNLIKELY(p->class_id == JS_CLASS_PROXY))
            return js_proxy_isArray(ctx, val);
        else
            return p->class_id == JS_CLASS_ARRAY;
    } else {
        return FALSE;
    }
}

#ifdef CONFIG_BIGNUM

JSValue JS_NewBigInt64_1(JSContext *ctx, int64_t v)
{
    JSValue val;
    bf_t *a;
    val = JS_NewBigInt(ctx);
    if (JS_IsException(val))
        return val;
    a = JS_GetBigInt(val);
    if (bf_set_si(a, v)) {
        JS_FreeValue(ctx, val);
        return JS_ThrowOutOfMemory(ctx);
    }
    return val;
}

JSValue JS_NewBigInt64(JSContext *ctx, int64_t v)
{
    if (is_math_mode(ctx) &&
        v >= -MAX_SAFE_INTEGER && v <= MAX_SAFE_INTEGER) {
        return JS_NewInt64(ctx, v);
    } else {
        return JS_NewBigInt64_1(ctx, v);
    }
}

JSValue JS_NewBigUint64(JSContext *ctx, uint64_t v)
{
    JSValue val;
    if (is_math_mode(ctx) && v <= MAX_SAFE_INTEGER) {
        val = JS_NewInt64(ctx, v);
    } else {
        bf_t *a;
        val = JS_NewBigInt(ctx);
        if (JS_IsException(val))
            return val;
        a = JS_GetBigInt(val);
        if (bf_set_ui(a, v)) {
            JS_FreeValue(ctx, val);
            return JS_ThrowOutOfMemory(ctx);
        }
    }
    return val;
}

/* must be kept in sync with JSOverloadableOperatorEnum */
/* XXX: use atoms ? */
static const char js_overloadable_operator_names[JS_OVOP_COUNT][4] = {
    "+",
    "-",
    "*",
    "/",
    "%",
    "**",
    "|",
    "&",
    "^",
    "<<",
    ">>",
    ">>>",
    "==",
    "<",
    "pos",
    "neg",
    "++",
    "--",
    "~",
};

int get_ovop_from_opcode(OPCodeEnum op)
{
    switch(op) {
    case OP_add:
        return JS_OVOP_ADD;
    case OP_sub:
        return JS_OVOP_SUB;
    case OP_mul:
        return JS_OVOP_MUL;
    case OP_div:
        return JS_OVOP_DIV;
    case OP_mod:
    case OP_math_mod:
        return JS_OVOP_MOD;
    case OP_pow:
        return JS_OVOP_POW;
    case OP_or:
        return JS_OVOP_OR;
    case OP_and:
        return JS_OVOP_AND;
    case OP_xor:
        return JS_OVOP_XOR;
    case OP_shl:
        return JS_OVOP_SHL;
    case OP_sar:
        return JS_OVOP_SAR;
    case OP_shr:
        return JS_OVOP_SHR;
    case OP_eq:
    case OP_neq:
        return JS_OVOP_EQ;
    case OP_lt:
    case OP_lte:
    case OP_gt:
    case OP_gte:
        return JS_OVOP_LESS;
    case OP_plus:
        return JS_OVOP_POS;
    case OP_neg:
        return JS_OVOP_NEG;
    case OP_inc:
        return JS_OVOP_INC;
    case OP_dec:
        return JS_OVOP_DEC;
    default:
        abort();
    }
}

/* return NULL if not present */
static JSObject *find_binary_op(JSBinaryOperatorDef *def,
                                uint32_t operator_index,
                                JSOverloadableOperatorEnum op)
{
    JSBinaryOperatorDefEntry *ent;
    int i;
    for(i = 0; i < def->count; i++) {
        ent = &def->tab[i];
        if (ent->operator_index == operator_index)
            return ent->ops[op];
    }
    return NULL;
}

/* return -1 if exception, 0 if no operator overloading, 1 if
   overloaded operator called */
int js_call_binary_op_fallback(JSContext *ctx,
                               JSValue *pret,
                               JSValueConst op1,
                               JSValueConst op2,
                               OPCodeEnum op,
                               BOOL is_numeric,
                               int hint)
{
    JSValue opset1_obj, opset2_obj, method, ret, new_op1, new_op2;
    JSOperatorSetData *opset1, *opset2;
    JSOverloadableOperatorEnum ovop;
    JSObject *p;
    JSValueConst args[2];
    if (!ctx->allow_operator_overloading)
        return 0;
    opset2_obj = JS_UNDEFINED;
    opset1_obj = JS_GetProperty(ctx, op1, JS_ATOM_Symbol_operatorSet);
    if (JS_IsException(opset1_obj))
        goto exception;
    if (JS_IsUndefined(opset1_obj))
        return 0;
    opset1 = JS_GetOpaque2(ctx, opset1_obj, JS_CLASS_OPERATOR_SET);
    if (!opset1)
        goto exception;
    opset2_obj = JS_GetProperty(ctx, op2, JS_ATOM_Symbol_operatorSet);
    if (JS_IsException(opset2_obj))
        goto exception;
    if (JS_IsUndefined(opset2_obj)) {
        JS_FreeValue(ctx, opset1_obj);
        return 0;
    }
    opset2 = JS_GetOpaque2(ctx, opset2_obj, JS_CLASS_OPERATOR_SET);
    if (!opset2)
        goto exception;
    if (opset1->is_primitive && opset2->is_primitive) {
        JS_FreeValue(ctx, opset1_obj);
        JS_FreeValue(ctx, opset2_obj);
        return 0;
    }
    ovop = get_ovop_from_opcode(op);
    if (opset1->operator_counter == opset2->operator_counter) {
        p = opset1->self_ops[ovop];
    } else if (opset1->operator_counter > opset2->operator_counter) {
        p = find_binary_op(&opset1->left, opset2->operator_counter, ovop);
    } else {
        p = find_binary_op(&opset2->right, opset1->operator_counter, ovop);
    }
    if (!p) {
        JS_ThrowTypeError(ctx, "operator %s: no function defined",
                          js_overloadable_operator_names[ovop]);
        goto exception;
    }
    if (opset1->is_primitive) {
        if (is_numeric) {
            new_op1 = JS_ToNumeric(ctx, op1);
        } else {
            new_op1 = JS_ToPrimitive(ctx, op1, hint);
        }
        if (JS_IsException(new_op1))
            goto exception;
    } else {
        new_op1 = JS_DupValue(ctx, op1);
    }
    if (opset2->is_primitive) {
        if (is_numeric) {
            new_op2 = JS_ToNumeric(ctx, op2);
        } else {
            new_op2 = JS_ToPrimitive(ctx, op2, hint);
        }
        if (JS_IsException(new_op2)) {
            JS_FreeValue(ctx, new_op1);
            goto exception;
        }
    } else {
        new_op2 = JS_DupValue(ctx, op2);
    }
    /* XXX: could apply JS_ToPrimitive() if primitive type so that the
       operator function does not get a value object */
    method = JS_DupValue(ctx, JS_MKPTR(JS_TAG_OBJECT, p));
    if (ovop == JS_OVOP_LESS && (op == OP_lte || op == OP_gt)) {
        args[0] = new_op2;
        args[1] = new_op1;
    } else {
        args[0] = new_op1;
        args[1] = new_op2;
    }
    ret = JS_CallFree(ctx, method, JS_UNDEFINED, 2, args);
    JS_FreeValue(ctx, new_op1);
    JS_FreeValue(ctx, new_op2);
    if (JS_IsException(ret))
        goto exception;
    if (ovop == JS_OVOP_EQ) {
        BOOL res = JS_ToBoolFree(ctx, ret);
        if (op == OP_neq)
            res ^= 1;
        ret = JS_NewBool(ctx, res);
    } else if (ovop == JS_OVOP_LESS) {
        if (JS_IsUndefined(ret)) {
            ret = JS_FALSE;
        } else {
            BOOL res = JS_ToBoolFree(ctx, ret);
            if (op == OP_lte || op == OP_gte)
                res ^= 1;
            ret = JS_NewBool(ctx, res);
        }
    }
    JS_FreeValue(ctx, opset1_obj);
    JS_FreeValue(ctx, opset2_obj);
    *pret = ret;
    return 1;
 exception:
    JS_FreeValue(ctx, opset1_obj);
    JS_FreeValue(ctx, opset2_obj);
    *pret = JS_UNDEFINED;
    return -1;
}

/* return -1 if exception, 0 if no operator overloading, 1 if
   overloaded operator called */
static __exception int js_call_unary_op_fallback(JSContext *ctx,
                                                 JSValue *pret,
                                                 JSValueConst op1,
                                                 OPCodeEnum op)
{
    JSValue opset1_obj, method, ret;
    JSOperatorSetData *opset1;
    JSOverloadableOperatorEnum ovop;
    JSObject *p;
    if (!ctx->allow_operator_overloading)
        return 0;
    opset1_obj = JS_GetProperty(ctx, op1, JS_ATOM_Symbol_operatorSet);
    if (JS_IsException(opset1_obj))
        goto exception;
    if (JS_IsUndefined(opset1_obj))
        return 0;
    opset1 = JS_GetOpaque2(ctx, opset1_obj, JS_CLASS_OPERATOR_SET);
    if (!opset1)
        goto exception;
    if (opset1->is_primitive) {
        JS_FreeValue(ctx, opset1_obj);
        return 0;
    }
    ovop = get_ovop_from_opcode(op);
    p = opset1->self_ops[ovop];
    if (!p) {
        JS_ThrowTypeError(ctx, "no overloaded operator %s",
                          js_overloadable_operator_names[ovop]);
        goto exception;
    }
    method = JS_DupValue(ctx, JS_MKPTR(JS_TAG_OBJECT, p));
    ret = JS_CallFree(ctx, method, JS_UNDEFINED, 1, &op1);
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

int js_unary_arith_slow(JSContext *ctx, JSValue *sp, OPCodeEnum op)
{
    JSValue op1, val;
    int v, ret;
    uint32_t tag;
    op1 = sp[-1];
    /* fast path for float64 */
    if (JS_TAG_IS_FLOAT64(JS_VALUE_GET_TAG(op1)))
        goto handle_float64;
    if (JS_IsObject(op1)) {
        ret = js_call_unary_op_fallback(ctx, &val, op1, op);
        if (ret < 0)
            return -1;
        if (ret) {
            JS_FreeValue(ctx, op1);
            sp[-1] = val;
            return 0;
        }
    }
    op1 = JS_ToNumericFree(ctx, op1);
    if (JS_IsException(op1))
        goto exception;
    tag = JS_VALUE_GET_TAG(op1);
    switch(tag) {
    case JS_TAG_INT:
        {
            int64_t v64;
            v64 = JS_VALUE_GET_INT(op1);
            switch(op) {
            case OP_inc:
            case OP_dec:
                v = 2 * (op - OP_dec) - 1;
                v64 += v;
                break;
            case OP_plus:
                break;
            case OP_neg:
                if (v64 == 0) {
                    sp[-1] = __JS_NewFloat64(ctx, -0.0);
                    return 0;
                } else {
                    v64 = -v64;
                }
                break;
            default:
                abort();
            }
            sp[-1] = JS_NewInt64(ctx, v64);
        }
        break;
    case JS_TAG_BIG_INT:
    handle_bigint:
        if (ctx->rt->bigint_ops.unary_arith(ctx, sp - 1, op, op1))
            goto exception;
        break;
    case JS_TAG_BIG_FLOAT:
        if (ctx->rt->bigfloat_ops.unary_arith(ctx, sp - 1, op, op1))
            goto exception;
        break;
    case JS_TAG_BIG_DECIMAL:
        if (ctx->rt->bigdecimal_ops.unary_arith(ctx, sp - 1, op, op1))
            goto exception;
        break;
    default:
    handle_float64:
        {
            double d;
            if (is_math_mode(ctx))
                goto handle_bigint;
            d = JS_VALUE_GET_FLOAT64(op1);
            switch(op) {
            case OP_inc:
            case OP_dec:
                v = 2 * (op - OP_dec) - 1;
                d += v;
                break;
            case OP_plus:
                break;
            case OP_neg:
                d = -d;
                break;
            default:
                abort();
            }
            sp[-1] = __JS_NewFloat64(ctx, d);
        }
        break;
    }
    return 0;
 exception:
    sp[-1] = JS_UNDEFINED;
    return -1;
}

int js_post_inc_slow(JSContext *ctx, JSValue *sp, OPCodeEnum op)
{
    JSValue op1;
    /* XXX: allow custom operators */
    op1 = sp[-1];
    op1 = JS_ToNumericFree(ctx, op1);
    if (JS_IsException(op1)) {
        sp[-1] = JS_UNDEFINED;
        return -1;
    }
    sp[-1] = op1;
    sp[0] = JS_DupValue(ctx, op1);
    return js_unary_arith_slow(ctx, sp + 1, op - OP_post_dec + OP_dec);
}

int js_not_slow(JSContext *ctx, JSValue *sp)
{
    JSValue op1, val;
    int ret;
    op1 = sp[-1];
    if (JS_IsObject(op1)) {
        ret = js_call_unary_op_fallback(ctx, &val, op1, OP_not);
        if (ret < 0)
            return -1;
        if (ret) {
            JS_FreeValue(ctx, op1);
            sp[-1] = val;
            return 0;
        }
    }
    op1 = JS_ToNumericFree(ctx, op1);
    if (JS_IsException(op1))
        goto exception;
    if (is_math_mode(ctx) || JS_VALUE_GET_TAG(op1) == JS_TAG_BIG_INT) {
        if (ctx->rt->bigint_ops.unary_arith(ctx, sp - 1, OP_not, op1))
            goto exception;
    } else {
        int32_t v1;
        if (UNLIKELY(JS_ToInt32Free(ctx, &v1, op1)))
            goto exception;
        sp[-1] = JS_NewInt32(ctx, ~v1);
    }
    return 0;
 exception:
    sp[-1] = JS_UNDEFINED;
    return -1;
}

int js_binary_arith_slow(JSContext *ctx, JSValue *sp, OPCodeEnum op)
{
    JSValue op1, op2, res;
    uint32_t tag1, tag2;
    int ret;
    double d1, d2;
    op1 = sp[-2];
    op2 = sp[-1];
    tag1 = JS_VALUE_GET_NORM_TAG(op1);
    tag2 = JS_VALUE_GET_NORM_TAG(op2);
    /* fast path for float operations */
    if (tag1 == JS_TAG_FLOAT64 && tag2 == JS_TAG_FLOAT64) {
        d1 = JS_VALUE_GET_FLOAT64(op1);
        d2 = JS_VALUE_GET_FLOAT64(op2);
        goto handle_float64;
    }
    /* try to call an overloaded operator */
    if ((tag1 == JS_TAG_OBJECT &&
         (tag2 != JS_TAG_NULL && tag2 != JS_TAG_UNDEFINED)) ||
        (tag2 == JS_TAG_OBJECT &&
         (tag1 != JS_TAG_NULL && tag1 != JS_TAG_UNDEFINED))) {
        ret = js_call_binary_op_fallback(ctx, &res, op1, op2, op, TRUE, 0);
        if (ret != 0) {
            JS_FreeValue(ctx, op1);
            JS_FreeValue(ctx, op2);
            if (ret < 0) {
                goto exception;
            } else {
                sp[-2] = res;
                return 0;
            }
        }
    }
    op1 = JS_ToNumericFree(ctx, op1);
    if (JS_IsException(op1)) {
        JS_FreeValue(ctx, op2);
        goto exception;
    }
    op2 = JS_ToNumericFree(ctx, op2);
    if (JS_IsException(op2)) {
        JS_FreeValue(ctx, op1);
        goto exception;
    }
    tag1 = JS_VALUE_GET_NORM_TAG(op1);
    tag2 = JS_VALUE_GET_NORM_TAG(op2);
    if (tag1 == JS_TAG_INT && tag2 == JS_TAG_INT) {
        int32_t v1, v2;
        int64_t v;
        v1 = JS_VALUE_GET_INT(op1);
        v2 = JS_VALUE_GET_INT(op2);
        switch(op) {
        case OP_sub:
            v = (int64_t)v1 - (int64_t)v2;
            break;
        case OP_mul:
            v = (int64_t)v1 * (int64_t)v2;
            if (is_math_mode(ctx) &&
                (v < -MAX_SAFE_INTEGER || v > MAX_SAFE_INTEGER))
                goto handle_bigint;
            if (v == 0 && (v1 | v2) < 0) {
                sp[-2] = __JS_NewFloat64(ctx, -0.0);
                return 0;
            }
            break;
        case OP_div:
            if (is_math_mode(ctx))
                goto handle_bigint;
            sp[-2] = __JS_NewFloat64(ctx, (double)v1 / (double)v2);
            return 0;
        case OP_math_mod:
            if (UNLIKELY(v2 == 0)) {
                throw_bf_exception(ctx, BF_ST_DIVIDE_ZERO);
                goto exception;
            }
            v = (int64_t)v1 % (int64_t)v2;
            if (v < 0) {
                if (v2 < 0)
                    v -= v2;
                else
                    v += v2;
            }
            break;
        case OP_mod:
            if (v1 < 0 || v2 <= 0) {
                sp[-2] = JS_NewFloat64(ctx, fmod(v1, v2));
                return 0;
            } else {
                v = (int64_t)v1 % (int64_t)v2;
            }
            break;
        case OP_pow:
            if (!is_math_mode(ctx)) {
                sp[-2] = JS_NewFloat64(ctx, js_pow(v1, v2));
                return 0;
            } else {
                goto handle_bigint;
            }
            break;
        default:
            abort();
        }
        sp[-2] = JS_NewInt64(ctx, v);
    } else if (tag1 == JS_TAG_BIG_DECIMAL || tag2 == JS_TAG_BIG_DECIMAL) {
        if (ctx->rt->bigdecimal_ops.binary_arith(ctx, op, sp - 2, op1, op2))
            goto exception;
    } else if (tag1 == JS_TAG_BIG_FLOAT || tag2 == JS_TAG_BIG_FLOAT) {
        if (ctx->rt->bigfloat_ops.binary_arith(ctx, op, sp - 2, op1, op2))
            goto exception;
    } else if (tag1 == JS_TAG_BIG_INT || tag2 == JS_TAG_BIG_INT) {
    handle_bigint:
        if (ctx->rt->bigint_ops.binary_arith(ctx, op, sp - 2, op1, op2))
            goto exception;
    } else {
        double dr;
        /* float64 result */
        if (JS_ToFloat64Free(ctx, &d1, op1)) {
            JS_FreeValue(ctx, op2);
            goto exception;
        }
        if (JS_ToFloat64Free(ctx, &d2, op2))
            goto exception;
    handle_float64:
        if (is_math_mode(ctx) && is_safe_integer(d1) && is_safe_integer(d2))
            goto handle_bigint;
        switch(op) {
        case OP_sub:
            dr = d1 - d2;
            break;
        case OP_mul:
            dr = d1 * d2;
            break;
        case OP_div:
            dr = d1 / d2;
            break;
        case OP_mod:
            dr = fmod(d1, d2);
            break;
        case OP_math_mod:
            d2 = fabs(d2);
            dr = fmod(d1, d2);
            /* XXX: loss of accuracy if dr < 0 */
            if (dr < 0)
                dr += d2;
            break;
        case OP_pow:
            dr = js_pow(d1, d2);
            break;
        default:
            abort();
        }
        sp[-2] = __JS_NewFloat64(ctx, dr);
    }
    return 0;
 exception:
    sp[-2] = JS_UNDEFINED;
    sp[-1] = JS_UNDEFINED;
    return -1;
}

int js_add_slow(JSContext *ctx, JSValue *sp)
{
    JSValue op1, op2, res;
    uint32_t tag1, tag2;
    int ret;
    op1 = sp[-2];
    op2 = sp[-1];
    tag1 = JS_VALUE_GET_NORM_TAG(op1);
    tag2 = JS_VALUE_GET_NORM_TAG(op2);
    /* fast path for float64 */
    if (tag1 == JS_TAG_FLOAT64 && tag2 == JS_TAG_FLOAT64) {
        double d1, d2;
        d1 = JS_VALUE_GET_FLOAT64(op1);
        d2 = JS_VALUE_GET_FLOAT64(op2);
        sp[-2] = __JS_NewFloat64(ctx, d1 + d2);
        return 0;
    }
    if (tag1 == JS_TAG_OBJECT || tag2 == JS_TAG_OBJECT) {
        /* try to call an overloaded operator */
        if ((tag1 == JS_TAG_OBJECT &&
             (tag2 != JS_TAG_NULL && tag2 != JS_TAG_UNDEFINED &&
              tag2 != JS_TAG_STRING)) ||
            (tag2 == JS_TAG_OBJECT &&
             (tag1 != JS_TAG_NULL && tag1 != JS_TAG_UNDEFINED &&
              tag1 != JS_TAG_STRING))) {
            ret = js_call_binary_op_fallback(ctx, &res, op1, op2, OP_add,
                                             FALSE, HINT_NONE);
            if (ret != 0) {
                JS_FreeValue(ctx, op1);
                JS_FreeValue(ctx, op2);
                if (ret < 0) {
                    goto exception;
                } else {
                    sp[-2] = res;
                    return 0;
                }
            }
        }
        op1 = JS_ToPrimitiveFree(ctx, op1, HINT_NONE);
        if (JS_IsException(op1)) {
            JS_FreeValue(ctx, op2);
            goto exception;
        }
        op2 = JS_ToPrimitiveFree(ctx, op2, HINT_NONE);
        if (JS_IsException(op2)) {
            JS_FreeValue(ctx, op1);
            goto exception;
        }
        tag1 = JS_VALUE_GET_NORM_TAG(op1);
        tag2 = JS_VALUE_GET_NORM_TAG(op2);
    }
    if (tag1 == JS_TAG_STRING || tag2 == JS_TAG_STRING) {
        sp[-2] = JS_ConcatString(ctx, op1, op2);
        if (JS_IsException(sp[-2]))
            goto exception;
        return 0;
    }
    op1 = JS_ToNumericFree(ctx, op1);
    if (JS_IsException(op1)) {
        JS_FreeValue(ctx, op2);
        goto exception;
    }
    op2 = JS_ToNumericFree(ctx, op2);
    if (JS_IsException(op2)) {
        JS_FreeValue(ctx, op1);
        goto exception;
    }
    tag1 = JS_VALUE_GET_NORM_TAG(op1);
    tag2 = JS_VALUE_GET_NORM_TAG(op2);
    if (tag1 == JS_TAG_INT && tag2 == JS_TAG_INT) {
        int32_t v1, v2;
        int64_t v;
        v1 = JS_VALUE_GET_INT(op1);
        v2 = JS_VALUE_GET_INT(op2);
        v = (int64_t)v1 + (int64_t)v2;
        sp[-2] = JS_NewInt64(ctx, v);
    } else if (tag1 == JS_TAG_BIG_DECIMAL || tag2 == JS_TAG_BIG_DECIMAL) {
        if (ctx->rt->bigdecimal_ops.binary_arith(ctx, OP_add, sp - 2, op1, op2))
            goto exception;
    } else if (tag1 == JS_TAG_BIG_FLOAT || tag2 == JS_TAG_BIG_FLOAT) {
        if (ctx->rt->bigfloat_ops.binary_arith(ctx, OP_add, sp - 2, op1, op2))
            goto exception;
    } else if (tag1 == JS_TAG_BIG_INT || tag2 == JS_TAG_BIG_INT) {
    handle_bigint:
        if (ctx->rt->bigint_ops.binary_arith(ctx, OP_add, sp - 2, op1, op2))
            goto exception;
    } else {
        double d1, d2;
        /* float64 result */
        if (JS_ToFloat64Free(ctx, &d1, op1)) {
            JS_FreeValue(ctx, op2);
            goto exception;
        }
        if (JS_ToFloat64Free(ctx, &d2, op2))
            goto exception;
        if (is_math_mode(ctx) && is_safe_integer(d1) && is_safe_integer(d2))
            goto handle_bigint;
        sp[-2] = __JS_NewFloat64(ctx, d1 + d2);
    }
    return 0;
 exception:
    sp[-2] = JS_UNDEFINED;
    sp[-1] = JS_UNDEFINED;
    return -1;
}

int js_binary_logic_slow(JSContext *ctx, JSValue *sp, OPCodeEnum op)
{
    JSValue op1, op2, res;
    int ret;
    uint32_t tag1, tag2;
    uint32_t v1, v2, r;
    op1 = sp[-2];
    op2 = sp[-1];
    tag1 = JS_VALUE_GET_NORM_TAG(op1);
    tag2 = JS_VALUE_GET_NORM_TAG(op2);
    /* try to call an overloaded operator */
    if ((tag1 == JS_TAG_OBJECT &&
         (tag2 != JS_TAG_NULL && tag2 != JS_TAG_UNDEFINED)) ||
        (tag2 == JS_TAG_OBJECT &&
         (tag1 != JS_TAG_NULL && tag1 != JS_TAG_UNDEFINED))) {
        ret = js_call_binary_op_fallback(ctx, &res, op1, op2, op, TRUE, 0);
        if (ret != 0) {
            JS_FreeValue(ctx, op1);
            JS_FreeValue(ctx, op2);
            if (ret < 0) {
                goto exception;
            } else {
                sp[-2] = res;
                return 0;
            }
        }
    }
    op1 = JS_ToNumericFree(ctx, op1);
    if (JS_IsException(op1)) {
        JS_FreeValue(ctx, op2);
        goto exception;
    }
    op2 = JS_ToNumericFree(ctx, op2);
    if (JS_IsException(op2)) {
        JS_FreeValue(ctx, op1);
        goto exception;
    }
    if (is_math_mode(ctx))
        goto bigint_op;
    tag1 = JS_VALUE_GET_TAG(op1);
    tag2 = JS_VALUE_GET_TAG(op2);
    if (tag1 == JS_TAG_BIG_INT || tag2 == JS_TAG_BIG_INT) {
        if (tag1 != tag2) {
            JS_FreeValue(ctx, op1);
            JS_FreeValue(ctx, op2);
            JS_ThrowTypeError(ctx, "both operands must be bigint");
            goto exception;
        } else {
        bigint_op:
            if (ctx->rt->bigint_ops.binary_arith(ctx, op, sp - 2, op1, op2))
                goto exception;
        }
    } else {
        if (UNLIKELY(JS_ToInt32Free(ctx, (int32_t *)&v1, op1))) {
            JS_FreeValue(ctx, op2);
            goto exception;
        }
        if (UNLIKELY(JS_ToInt32Free(ctx, (int32_t *)&v2, op2)))
            goto exception;
        switch(op) {
        case OP_shl:
            r = v1 << (v2 & 0x1f);
            break;
        case OP_sar:
            r = (int)v1 >> (v2 & 0x1f);
            break;
        case OP_and:
            r = v1 & v2;
            break;
        case OP_or:
            r = v1 | v2;
            break;
        case OP_xor:
            r = v1 ^ v2;
            break;
        default:
            abort();
        }
        sp[-2] = JS_NewInt32(ctx, r);
    }
    return 0;
 exception:
    sp[-2] = JS_UNDEFINED;
    sp[-1] = JS_UNDEFINED;
    return -1;
}

int js_relational_slow(JSContext *ctx, JSValue *sp, OPCodeEnum op)
{
    JSValue op1, op2, ret;
    int res;
    uint32_t tag1, tag2;
    op1 = sp[-2];
    op2 = sp[-1];
    tag1 = JS_VALUE_GET_NORM_TAG(op1);
    tag2 = JS_VALUE_GET_NORM_TAG(op2);
    /* try to call an overloaded operator */
    if ((tag1 == JS_TAG_OBJECT &&
         (tag2 != JS_TAG_NULL && tag2 != JS_TAG_UNDEFINED)) ||
        (tag2 == JS_TAG_OBJECT &&
         (tag1 != JS_TAG_NULL && tag1 != JS_TAG_UNDEFINED))) {
        res = js_call_binary_op_fallback(ctx, &ret, op1, op2, op,
                                         FALSE, HINT_NUMBER);
        if (res != 0) {
            JS_FreeValue(ctx, op1);
            JS_FreeValue(ctx, op2);
            if (res < 0) {
                goto exception;
            } else {
                sp[-2] = ret;
                return 0;
            }
        }
    }
    op1 = JS_ToPrimitiveFree(ctx, op1, HINT_NUMBER);
    if (JS_IsException(op1)) {
        JS_FreeValue(ctx, op2);
        goto exception;
    }
    op2 = JS_ToPrimitiveFree(ctx, op2, HINT_NUMBER);
    if (JS_IsException(op2)) {
        JS_FreeValue(ctx, op1);
        goto exception;
    }
    tag1 = JS_VALUE_GET_NORM_TAG(op1);
    tag2 = JS_VALUE_GET_NORM_TAG(op2);
    if (tag1 == JS_TAG_STRING && tag2 == JS_TAG_STRING) {
        JSString *p1, *p2;
        p1 = JS_VALUE_GET_STRING(op1);
        p2 = JS_VALUE_GET_STRING(op2);
        res = js_string_compare(ctx, p1, p2);
        switch(op) {
        case OP_lt:
            res = (res < 0);
            break;
        case OP_lte:
            res = (res <= 0);
            break;
        case OP_gt:
            res = (res > 0);
            break;
        default:
        case OP_gte:
            res = (res >= 0);
            break;
        }
        JS_FreeValue(ctx, op1);
        JS_FreeValue(ctx, op2);
    } else if ((tag1 <= JS_TAG_NULL || tag1 == JS_TAG_FLOAT64) &&
               (tag2 <= JS_TAG_NULL || tag2 == JS_TAG_FLOAT64)) {
        /* fast path for float64/int */
        goto float64_compare;
    } else {
        if (((tag1 == JS_TAG_BIG_INT && tag2 == JS_TAG_STRING) ||
             (tag2 == JS_TAG_BIG_INT && tag1 == JS_TAG_STRING)) &&
            !is_math_mode(ctx)) {
            if (tag1 == JS_TAG_STRING) {
                op1 = JS_StringToBigInt(ctx, op1);
                if (JS_VALUE_GET_TAG(op1) != JS_TAG_BIG_INT)
                    goto invalid_bigint_string;
            }
            if (tag2 == JS_TAG_STRING) {
                op2 = JS_StringToBigInt(ctx, op2);
                if (JS_VALUE_GET_TAG(op2) != JS_TAG_BIG_INT) {
                invalid_bigint_string:
                    JS_FreeValue(ctx, op1);
                    JS_FreeValue(ctx, op2);
                    res = FALSE;
                    goto done;
                }
            }
        } else {
            op1 = JS_ToNumericFree(ctx, op1);
            if (JS_IsException(op1)) {
                JS_FreeValue(ctx, op2);
                goto exception;
            }
            op2 = JS_ToNumericFree(ctx, op2);
            if (JS_IsException(op2)) {
                JS_FreeValue(ctx, op1);
                goto exception;
            }
        }
        tag1 = JS_VALUE_GET_NORM_TAG(op1);
        tag2 = JS_VALUE_GET_NORM_TAG(op2);
        if (tag1 == JS_TAG_BIG_DECIMAL || tag2 == JS_TAG_BIG_DECIMAL) {
            res = ctx->rt->bigdecimal_ops.compare(ctx, op, op1, op2);
            if (res < 0)
                goto exception;
        } else if (tag1 == JS_TAG_BIG_FLOAT || tag2 == JS_TAG_BIG_FLOAT) {
            res = ctx->rt->bigfloat_ops.compare(ctx, op, op1, op2);
            if (res < 0)
                goto exception;
        } else if (tag1 == JS_TAG_BIG_INT || tag2 == JS_TAG_BIG_INT) {
            res = ctx->rt->bigint_ops.compare(ctx, op, op1, op2);
            if (res < 0)
                goto exception;
        } else {
            double d1, d2;
        float64_compare:
            /* can use floating point comparison */
            if (tag1 == JS_TAG_FLOAT64) {
                d1 = JS_VALUE_GET_FLOAT64(op1);
            } else {
                d1 = JS_VALUE_GET_INT(op1);
            }
            if (tag2 == JS_TAG_FLOAT64) {
                d2 = JS_VALUE_GET_FLOAT64(op2);
            } else {
                d2 = JS_VALUE_GET_INT(op2);
            }
            switch(op) {
            case OP_lt:
                res = (d1 < d2); /* if NaN return false */
                break;
            case OP_lte:
                res = (d1 <= d2); /* if NaN return false */
                break;
            case OP_gt:
                res = (d1 > d2); /* if NaN return false */
                break;
            default:
            case OP_gte:
                res = (d1 >= d2); /* if NaN return false */
                break;
            }
        }
    }
 done:
    sp[-2] = JS_NewBool(ctx, res);
    return 0;
 exception:
    sp[-2] = JS_UNDEFINED;
    sp[-1] = JS_UNDEFINED;
    return -1;
}

int js_shr_slow(JSContext *ctx, JSValue *sp)
{
    JSValue op1, op2;
    uint32_t v1, v2, r;

    op1 = sp[-2];
    op2 = sp[-1];
    op1 = JS_ToNumericFree(ctx, op1);
    if (JS_IsException(op1)) {
        JS_FreeValue(ctx, op2);
        goto exception;
    }
    op2 = JS_ToNumericFree(ctx, op2);
    if (JS_IsException(op2)) {
        JS_FreeValue(ctx, op1);
        goto exception;
    }
    /* XXX: could forbid >>> in bignum mode */
    if (!is_math_mode(ctx) &&
        (JS_VALUE_GET_TAG(op1) == JS_TAG_BIG_INT ||
         JS_VALUE_GET_TAG(op2) == JS_TAG_BIG_INT)) {
        JS_ThrowTypeError(ctx, "bigint operands are forbidden for >>>");
        JS_FreeValue(ctx, op1);
        JS_FreeValue(ctx, op2);
        goto exception;
    }
    /* cannot give an exception */
    JS_ToUint32Free(ctx, &v1, op1);
    JS_ToUint32Free(ctx, &v2, op2);
    r = v1 >> (v2 & 0x1f);
    sp[-2] = JS_NewUint32(ctx, r);
    return 0;
 exception:
    sp[-2] = JS_UNDEFINED;
    sp[-1] = JS_UNDEFINED;
    return -1;
}

#else /* !CONFIG_BIGNUM */

static JSValue JS_ThrowUnsupportedBigint(JSContext *ctx)
{
    return JS_ThrowTypeError(ctx, "bigint is not supported");
}

JSValue JS_NewBigInt64(JSContext *ctx, int64_t v)
{
    return JS_ThrowUnsupportedBigint(ctx);
}

JSValue JS_NewBigUint64(JSContext *ctx, uint64_t v)
{
    return JS_ThrowUnsupportedBigint(ctx);
}

int JS_ToBigInt64(JSContext *ctx, int64_t *pres, JSValueConst val)
{
    JS_ThrowUnsupportedBigint(ctx);
    *pres = 0;
    return -1;
}

int js_unary_arith_slow(JSContext *ctx, JSValue *sp, OPCodeEnum op)
{
    JSValue op1;
    double d;
    op1 = sp[-1];
    if (UNLIKELY(JS_ToFloat64Free(ctx, &d, op1))) {
        sp[-1] = JS_UNDEFINED;
        return -1;
    }
    switch(op) {
    case OP_inc:
        d++;
        break;
    case OP_dec:
        d--;
        break;
    case OP_plus:
        break;
    case OP_neg:
        d = -d;
        break;
    default:
        abort();
    }
    sp[-1] = JS_NewFloat64(ctx, d);
    return 0;
}

/* specific case necessary for correct return value semantics */
int js_post_inc_slow(JSContext *ctx, JSValue *sp, OPCodeEnum op)
{
    JSValue op1;
    double d, r;
    op1 = sp[-1];
    if (UNLIKELY(JS_ToFloat64Free(ctx, &d, op1))) {
        sp[-1] = JS_UNDEFINED;
        return -1;
    }
    r = d + 2 * (op - OP_post_dec) - 1;
    sp[0] = JS_NewFloat64(ctx, r);
    sp[-1] = JS_NewFloat64(ctx, d);
    return 0;
}

int js_binary_arith_slow(JSContext *ctx, JSValue *sp, OPCodeEnum op)
{
    JSValue op1, op2;
    double d1, d2, r;
    op1 = sp[-2];
    op2 = sp[-1];
    if (UNLIKELY(JS_ToFloat64Free(ctx, &d1, op1))) {
        JS_FreeValue(ctx, op2);
        goto exception;
    }
    if (UNLIKELY(JS_ToFloat64Free(ctx, &d2, op2))) {
        goto exception;
    }
    switch(op) {
    case OP_sub:
        r = d1 - d2;
        break;
    case OP_mul:
        r = d1 * d2;
        break;
    case OP_div:
        r = d1 / d2;
        break;
    case OP_mod:
        r = fmod(d1, d2);
        break;
    case OP_pow:
        r = js_pow(d1, d2);
        break;
    default:
        abort();
    }
    sp[-2] = JS_NewFloat64(ctx, r);
    return 0;
 exception:
    sp[-2] = JS_UNDEFINED;
    sp[-1] = JS_UNDEFINED;
    return -1;
}

int js_add_slow(JSContext *ctx, JSValue *sp)
{
    JSValue op1, op2;
    uint32_t tag1, tag2;
    op1 = sp[-2];
    op2 = sp[-1];
    tag1 = JS_VALUE_GET_TAG(op1);
    tag2 = JS_VALUE_GET_TAG(op2);
    if ((tag1 == JS_TAG_INT || JS_TAG_IS_FLOAT64(tag1)) &&
        (tag2 == JS_TAG_INT || JS_TAG_IS_FLOAT64(tag2))) {
        goto add_numbers;
    } else {
        op1 = JS_ToPrimitiveFree(ctx, op1, HINT_NONE);
        if (JS_IsException(op1)) {
            JS_FreeValue(ctx, op2);
            goto exception;
        }
        op2 = JS_ToPrimitiveFree(ctx, op2, HINT_NONE);
        if (JS_IsException(op2)) {
            JS_FreeValue(ctx, op1);
            goto exception;
        }
        tag1 = JS_VALUE_GET_TAG(op1);
        tag2 = JS_VALUE_GET_TAG(op2);
        if (tag1 == JS_TAG_STRING || tag2 == JS_TAG_STRING) {
            sp[-2] = JS_ConcatString(ctx, op1, op2);
            if (JS_IsException(sp[-2]))
                goto exception;
        } else {
            double d1, d2;
        add_numbers:
            if (JS_ToFloat64Free(ctx, &d1, op1)) {
                JS_FreeValue(ctx, op2);
                goto exception;
            }
            if (JS_ToFloat64Free(ctx, &d2, op2))
                goto exception;
            sp[-2] = JS_NewFloat64(ctx, d1 + d2);
        }
    }
    return 0;
 exception:
    sp[-2] = JS_UNDEFINED;
    sp[-1] = JS_UNDEFINED;
    return -1;
}

int js_binary_logic_slow(JSContext *ctx, JSValue *sp, OPCodeEnum op)
{
    JSValue op1, op2;
    uint32_t v1, v2, r;
    op1 = sp[-2];
    op2 = sp[-1];
    if (UNLIKELY(JS_ToInt32Free(ctx, (int32_t *)&v1, op1))) {
        JS_FreeValue(ctx, op2);
        goto exception;
    }
    if (UNLIKELY(JS_ToInt32Free(ctx, (int32_t *)&v2, op2)))
        goto exception;
    switch(op) {
    case OP_shl:
        r = v1 << (v2 & 0x1f);
        break;
    case OP_sar:
        r = (int)v1 >> (v2 & 0x1f);
        break;
    case OP_and:
        r = v1 & v2;
        break;
    case OP_or:
        r = v1 | v2;
        break;
    case OP_xor:
        r = v1 ^ v2;
        break;
    default:
        abort();
    }
    sp[-2] = JS_NewInt32(ctx, r);
    return 0;
 exception:
    sp[-2] = JS_UNDEFINED;
    sp[-1] = JS_UNDEFINED;
    return -1;
}

int js_not_slow(JSContext *ctx, JSValue *sp)
{
    int32_t v1;
    if (UNLIKELY(JS_ToInt32Free(ctx, &v1, sp[-1]))) {
        sp[-1] = JS_UNDEFINED;
        return -1;
    }
    sp[-1] = JS_NewInt32(ctx, ~v1);
    return 0;
}

int js_relational_slow(JSContext *ctx, JSValue *sp, OPCodeEnum op)
{
    JSValue op1, op2;
    int res;
    op1 = sp[-2];
    op2 = sp[-1];
    op1 = JS_ToPrimitiveFree(ctx, op1, HINT_NUMBER);
    if (JS_IsException(op1)) {
        JS_FreeValue(ctx, op2);
        goto exception;
    }
    op2 = JS_ToPrimitiveFree(ctx, op2, HINT_NUMBER);
    if (JS_IsException(op2)) {
        JS_FreeValue(ctx, op1);
        goto exception;
    }
    if (JS_VALUE_GET_TAG(op1) == JS_TAG_STRING &&
        JS_VALUE_GET_TAG(op2) == JS_TAG_STRING) {
        JSString *p1, *p2;
        p1 = JS_VALUE_GET_STRING(op1);
        p2 = JS_VALUE_GET_STRING(op2);
        res = js_string_compare(ctx, p1, p2);
        JS_FreeValue(ctx, op1);
        JS_FreeValue(ctx, op2);
        switch(op) {
        case OP_lt:
            res = (res < 0);
            break;
        case OP_lte:
            res = (res <= 0);
            break;
        case OP_gt:
            res = (res > 0);
            break;
        default:
        case OP_gte:
            res = (res >= 0);
            break;
        }
    } else {
        double d1, d2;
        if (JS_ToFloat64Free(ctx, &d1, op1)) {
            JS_FreeValue(ctx, op2);
            goto exception;
        }
        if (JS_ToFloat64Free(ctx, &d2, op2))
            goto exception;
        switch(op) {
        case OP_lt:
            res = (d1 < d2); /* if NaN return false */
            break;
        case OP_lte:
            res = (d1 <= d2); /* if NaN return false */
            break;
        case OP_gt:
            res = (d1 > d2); /* if NaN return false */
            break;
        default:
        case OP_gte:
            res = (d1 >= d2); /* if NaN return false */
            break;
        }
    }
    sp[-2] = JS_NewBool(ctx, res);
    return 0;
 exception:
    sp[-2] = JS_UNDEFINED;
    sp[-1] = JS_UNDEFINED;
    return -1;
}

int js_shr_slow(JSContext *ctx, JSValue *sp)
{
    JSValue op1, op2;
    uint32_t v1, v2, r;
    op1 = sp[-2];
    op2 = sp[-1];
    if (UNLIKELY(JS_ToUint32Free(ctx, &v1, op1))) {
        JS_FreeValue(ctx, op2);
        goto exception;
    }
    if (UNLIKELY(JS_ToUint32Free(ctx, &v2, op2)))
        goto exception;
    r = v1 >> (v2 & 0x1f);
    sp[-2] = JS_NewUint32(ctx, r);
    return 0;
 exception:
    sp[-2] = JS_UNDEFINED;
    sp[-1] = JS_UNDEFINED;
    return -1;
}

#endif /* !CONFIG_BIGNUM */

int js_operator_in(JSContext *ctx, JSValue *sp)
{
    JSValue op1, op2;
    JSAtom atom;
    int ret;
    op1 = sp[-2];
    op2 = sp[-1];
    if (JS_VALUE_GET_TAG(op2) != JS_TAG_OBJECT) {
        JS_ThrowTypeError(ctx, "invalid 'in' operand");
        return -1;
    }
    atom = JS_ValueToAtom(ctx, op1);
    if (UNLIKELY(atom == JS_ATOM_NULL))
        return -1;
    ret = JS_HasProperty(ctx, op2, atom);
    JS_FreeAtom(ctx, atom);
    if (ret < 0)
        return -1;
    JS_FreeValue(ctx, op1);
    JS_FreeValue(ctx, op2);
    sp[-2] = JS_NewBool(ctx, ret);
    return 0;
}

static JSValue js_throw_type_error(JSContext *ctx, JSValueConst this_val,
                                   int argc, JSValueConst *argv)
{
    return JS_ThrowTypeError(ctx, "invalid property access");
}

/* XXX: not 100% compatible, but mozilla seems to use a similar
   implementation to ensure that caller in non strict mode does not
   throw (ES5 compatibility) */
static JSValue js_function_proto_caller(JSContext *ctx, JSValueConst this_val,
                                        int argc, JSValueConst *argv)
{
    JSFunctionBytecode *b = JS_GetFunctionBytecode(this_val);
    if (!b || (b->js_mode & JS_MODE_STRICT) || !b->has_prototype) {
        return js_throw_type_error(ctx, this_val, 0, NULL);
    }
    return JS_UNDEFINED;
}

static JSValue js_function_proto_fileName(JSContext *ctx,
                                          JSValueConst this_val)
{
    JSFunctionBytecode *b = JS_GetFunctionBytecode(this_val);
    if (b && b->has_debug) {
        return JS_AtomToString(ctx, b->debug.filename);
    }
    return JS_UNDEFINED;
}

static JSValue js_function_proto_lineNumber(JSContext *ctx,
                                            JSValueConst this_val)
{
    JSFunctionBytecode *b = JS_GetFunctionBytecode(this_val);
    if (b && b->has_debug) {
        return JS_NewInt32(ctx, b->debug.line_num);
    }
    return JS_UNDEFINED;
}

static int js_arguments_define_own_property(JSContext *ctx,
                                            JSValueConst this_obj,
                                            JSAtom prop, JSValueConst val,
                                            JSValueConst getter, JSValueConst setter, int flags)
{
    JSObject *p;
    uint32_t idx;
    p = JS_VALUE_GET_OBJ(this_obj);
    /* convert to normal array when redefining an existing numeric field */
    if (p->fast_array && JS_AtomIsArrayIndex(ctx, &idx, prop) &&
        idx < p->u.array.count) {
        if (convert_fast_array_to_array(ctx, p))
            return -1;
    }
    /* run the default define own property */
    return JS_DefineProperty(ctx, this_obj, prop, val, getter, setter,
                             flags | JS_PROP_NO_EXOTIC);
}

static const JSClassExoticMethods js_arguments_exotic_methods = {
    .define_own_property = js_arguments_define_own_property,
};

JSValue JS_GetIterator2(JSContext *ctx, JSValueConst obj, JSValueConst method)
{
    JSValue enum_obj;

    enum_obj = JS_Call(ctx, method, obj, 0, NULL);
    if (JS_IsException(enum_obj))
        return enum_obj;
    if (!JS_IsObject(enum_obj)) {
        JS_FreeValue(ctx, enum_obj);
        return JS_ThrowTypeErrorNotAnObject(ctx);
    }
    return enum_obj;
}

JSValue JS_GetIterator(JSContext *ctx, JSValueConst obj, BOOL is_async)
{
    JSValue method, ret, sync_iter;
    if (is_async) {
        method = JS_GetProperty(ctx, obj, JS_ATOM_Symbol_asyncIterator);
        if (JS_IsException(method))
            return method;
        if (JS_IsUndefined(method) || JS_IsNull(method)) {
            method = JS_GetProperty(ctx, obj, JS_ATOM_Symbol_iterator);
            if (JS_IsException(method))
                return method;
            sync_iter = JS_GetIterator2(ctx, obj, method);
            JS_FreeValue(ctx, method);
            if (JS_IsException(sync_iter))
                return sync_iter;
            ret = JS_CreateAsyncFromSyncIterator(ctx, sync_iter);
            JS_FreeValue(ctx, sync_iter);
            return ret;
        }
    } else {
        method = JS_GetProperty(ctx, obj, JS_ATOM_Symbol_iterator);
        if (JS_IsException(method))
            return method;
    }
    if (!JS_IsFunction(ctx, method)) {
        JS_FreeValue(ctx, method);
        return JS_ThrowTypeError(ctx, "value is not iterable");
    }
    ret = JS_GetIterator2(ctx, obj, method);
    JS_FreeValue(ctx, method);
    return ret;
}

/* return *pdone = 2 if the iterator object is not parsed */
JSValue JS_IteratorNext2(JSContext *ctx, JSValueConst enum_obj,
                         JSValueConst method,
                         int argc, JSValueConst *argv, int *pdone)
{
    JSValue obj;

    /* fast path for the built-in iterators (avoid creating the
       intermediate result object) */
    if (JS_IsObject(method)) {
        JSObject *p = JS_VALUE_GET_OBJ(method);
        if (p->class_id == JS_CLASS_C_FUNCTION &&
            p->u.cfunc.cproto == JS_CFUNC_iterator_next) {
            JSCFunctionType func;
            JSValueConst args[1];

            /* in case the function expects one argument */
            if (argc == 0) {
                args[0] = JS_UNDEFINED;
                argv = args;
            }
            func = p->u.cfunc.c_function;
            return func.iterator_next(ctx, enum_obj, argc, argv,
                                      pdone, p->u.cfunc.magic);
        }
    }
    obj = JS_Call(ctx, method, enum_obj, argc, argv);
    if (JS_IsException(obj))
        goto fail;
    if (!JS_IsObject(obj)) {
        JS_FreeValue(ctx, obj);
        JS_ThrowTypeError(ctx, "iterator must return an object");
        goto fail;
    }
    *pdone = 2;
    return obj;
 fail:
    *pdone = FALSE;
    return JS_EXCEPTION;
}

JSValue JS_IteratorNext(JSContext *ctx, JSValueConst enum_obj,
                        JSValueConst method,
                        int argc, JSValueConst *argv, BOOL *pdone)
{
    JSValue obj, value, done_val;
    int done;
    obj = JS_IteratorNext2(ctx, enum_obj, method, argc, argv, &done);
    if (JS_IsException(obj))
        goto fail;
    if (done != 2) {
        *pdone = done;
        return obj;
    } else {
        done_val = JS_GetProperty(ctx, obj, JS_ATOM_done);
        if (JS_IsException(done_val))
            goto fail;
        *pdone = JS_ToBoolFree(ctx, done_val);
        value = JS_UNDEFINED;
        if (!*pdone) {
            value = JS_GetProperty(ctx, obj, JS_ATOM_value);
        }
        JS_FreeValue(ctx, obj);
        return value;
    }
 fail:
    JS_FreeValue(ctx, obj);
    *pdone = FALSE;
    return JS_EXCEPTION;
}

/* return < 0 in case of exception */
int JS_IteratorClose(JSContext *ctx, JSValueConst enum_obj, BOOL is_exception_pending)
{
    JSValue method, ret, ex_obj;
    int res;
    if (is_exception_pending) {
        ex_obj = ctx->rt->current_exception;
        ctx->rt->current_exception = JS_NULL;
        res = -1;
    } else {
        ex_obj = JS_UNDEFINED;
        res = 0;
    }
    method = JS_GetProperty(ctx, enum_obj, JS_ATOM_return);
    if (JS_IsException(method)) {
        res = -1;
        goto done;
    }
    if (JS_IsUndefined(method) || JS_IsNull(method)) {
        goto done;
    }
    ret = JS_CallFree(ctx, method, enum_obj, 0, NULL);
    if (!is_exception_pending) {
        if (JS_IsException(ret)) {
            res = -1;
        } else if (!JS_IsObject(ret)) {
            JS_ThrowTypeErrorNotAnObject(ctx);
            res = -1;
        }
    }
    JS_FreeValue(ctx, ret);
 done:
    if (is_exception_pending) {
        JS_Throw(ctx, ex_obj);
    }
    return res;
}

/* obj -> enum_rec (3 slots) */
int js_for_of_start(JSContext *ctx, JSValue *sp, BOOL is_async)
{
    JSValue op1, obj, method;
    op1 = sp[-1];
    obj = JS_GetIterator(ctx, op1, is_async);
    if (JS_IsException(obj))
        return -1;
    JS_FreeValue(ctx, op1);
    sp[-1] = obj;
    method = JS_GetProperty(ctx, obj, JS_ATOM_next);
    if (JS_IsException(method))
        return -1;
    sp[0] = method;
    return 0;
}

JSValue JS_IteratorGetCompleteValue(JSContext *ctx, JSValueConst obj, BOOL *pdone)
{
    JSValue done_val, value;
    BOOL done;
    done_val = JS_GetProperty(ctx, obj, JS_ATOM_done);
    if (JS_IsException(done_val))
        goto fail;
    done = JS_ToBoolFree(ctx, done_val);
    value = JS_GetProperty(ctx, obj, JS_ATOM_value);
    if (JS_IsException(value))
        goto fail;
    *pdone = done;
    return value;
 fail:
    *pdone = FALSE;
    return JS_EXCEPTION;
}

JSValue js_create_iterator_result(JSContext *ctx, JSValue val, BOOL done)
{
    JSValue obj;
    obj = JS_NewObject(ctx);
    if (JS_IsException(obj)) {
        JS_FreeValue(ctx, val);
        return obj;
    }
    if (JS_DefinePropertyValue(ctx, obj, JS_ATOM_value,
                               val, JS_PROP_C_W_E) < 0) {
        goto fail;
    }
    if (JS_DefinePropertyValue(ctx, obj, JS_ATOM_done,
                               JS_NewBool(ctx, done), JS_PROP_C_W_E) < 0) {
    fail:
        JS_FreeValue(ctx, obj);
        return JS_EXCEPTION;
    }
    return obj;
}

int JS_CopyDataProperties(JSContext *ctx,
                          JSValueConst target,
                          JSValueConst source,
                          JSValueConst excluded,
                          BOOL setprop)
{
    JSPropertyEnum *tab_atom;
    JSValue val;
    uint32_t i, tab_atom_count;
    JSObject *p;
    JSObject *pexcl = NULL;
    int ret, gpn_flags;
    JSPropertyDescriptor desc;
    BOOL is_enumerable;
    if (JS_VALUE_GET_TAG(source) != JS_TAG_OBJECT)
        return 0;
    if (JS_VALUE_GET_TAG(excluded) == JS_TAG_OBJECT)
        pexcl = JS_VALUE_GET_OBJ(excluded);
    p = JS_VALUE_GET_OBJ(source);
    gpn_flags = JS_GPN_STRING_MASK | JS_GPN_SYMBOL_MASK | JS_GPN_ENUM_ONLY;
    if (p->is_exotic) {
        const JSClassExoticMethods *em = ctx->rt->class_array[p->class_id].exotic;
        /* cannot use JS_GPN_ENUM_ONLY with e.g. proxies because it
           introduces a visible change */
        if (em && em->get_own_property_names) {
            gpn_flags &= ~JS_GPN_ENUM_ONLY;
        }
    }
    if (JS_GetOwnPropertyNamesInternal(ctx, &tab_atom, &tab_atom_count, p,
                                       gpn_flags))
        return -1;
    for (i = 0; i < tab_atom_count; i++) {
        if (pexcl) {
            ret = JS_GetOwnPropertyInternal(ctx, NULL, pexcl, tab_atom[i].atom);
            if (ret) {
                if (ret < 0)
                    goto exception;
                continue;
            }
        }
        if (!(gpn_flags & JS_GPN_ENUM_ONLY)) {
            /* test if the property is enumerable */
            ret = JS_GetOwnPropertyInternal(ctx, &desc, p, tab_atom[i].atom);
            if (ret < 0)
                goto exception;
            if (!ret)
                continue;
            is_enumerable = (desc.flags & JS_PROP_ENUMERABLE) != 0;
            js_free_desc(ctx, &desc);
            if (!is_enumerable)
                continue;
        }
        val = JS_GetProperty(ctx, source, tab_atom[i].atom);
        if (JS_IsException(val))
            goto exception;
        if (setprop)
            ret = JS_SetProperty(ctx, target, tab_atom[i].atom, val);
        else
            ret = JS_DefinePropertyValue(ctx, target, tab_atom[i].atom, val,
                                         JS_PROP_C_W_E);
        if (ret < 0)
            goto exception;
    }
    js_free_prop_enum(ctx, tab_atom, tab_atom_count);
    return 0;
 exception:
    js_free_prop_enum(ctx, tab_atom, tab_atom_count);
    return -1;
}

JSVarRef *get_var_ref(JSContext *ctx, JSStackFrame *sf, int var_idx, BOOL is_arg)
{
    JSVarRef *var_ref;
    struct list_head *el;

    list_for_each(el, &sf->var_ref_list) {
        var_ref = list_entry(el, JSVarRef, header.link);
        if (var_ref->var_idx == var_idx && var_ref->is_arg == is_arg) {
            var_ref->header.ref_count++;
            return var_ref;
        }
    }
    /* create a new one */
    var_ref = js_malloc(ctx, sizeof(JSVarRef));
    if (!var_ref)
        return NULL;
    var_ref->header.ref_count = 1;
    var_ref->is_detached = FALSE;
    var_ref->is_arg = is_arg;
    var_ref->var_idx = var_idx;
    list_add_tail(&var_ref->header.link, &sf->var_ref_list);
    if (is_arg)
        var_ref->pvalue = &sf->arg_buf[var_idx];
    else
        var_ref->pvalue = &sf->var_buf[var_idx];
    var_ref->value = JS_UNDEFINED;
    return var_ref;
}

static JSValue js_instantiate_prototype(JSContext *ctx, JSObject *p, JSAtom atom, void *opaque)
{
    JSValue obj, this_val;
    int ret;
    this_val = JS_MKPTR(JS_TAG_OBJECT, p);
    obj = JS_NewObject(ctx);
    if (JS_IsException(obj))
        return JS_EXCEPTION;
    set_cycle_flag(ctx, obj);
    set_cycle_flag(ctx, this_val);
    ret = JS_DefinePropertyValue(ctx, obj, JS_ATOM_constructor,
                                 JS_DupValue(ctx, this_val),
                                 JS_PROP_WRITABLE | JS_PROP_CONFIGURABLE);
    if (ret < 0) {
        JS_FreeValue(ctx, obj);
        return JS_EXCEPTION;
    }
    return obj;
}

/* AsyncFunction */

/* AsyncGenerator */

/* JS parser */

const JSOpCode opcode_info[OP_COUNT + (OP_TEMP_END - OP_TEMP_START)] = {
#define FMT(f)
#ifdef DUMP_BYTECODE
#define DEF(id, size, n_pop, n_push, f) { #id, size, n_pop, n_push, OP_FMT_ ## f },
#else
#define DEF(id, size, n_pop, n_push, f) { size, n_pop, n_push, OP_FMT_ ## f },
#endif
#include "third_party/quickjs/quickjs-opcode.inc"
#undef DEF
#undef FMT
};

static void __attribute((unused)) dump_token(JSParseState *s,
                                             const JSToken *token)
{
    switch(token->val) {
    case TOK_NUMBER:
        {
            double d;
            JS_ToFloat64(s->ctx, &d, token->u.num.val);  /* no exception possible */
            printf("number: %.14g\n", d);
        }
        break;
    case TOK_IDENT:
    dump_atom:
        {
            char buf[ATOM_GET_STR_BUF_SIZE];
            printf("ident: '%s'\n",
                   JS_AtomGetStr(s->ctx, buf, sizeof(buf), token->u.ident.atom));
        }
        break;
    case TOK_STRING:
        {
            const char *str;
            /* XXX: quote the string */
            str = JS_ToCString(s->ctx, token->u.str.str);
            printf("string: '%s'\n", str);
            JS_FreeCString(s->ctx, str);
        }
        break;
    case TOK_TEMPLATE:
        {
            const char *str;
            str = JS_ToCString(s->ctx, token->u.str.str);
            printf("template: `%s`\n", str);
            JS_FreeCString(s->ctx, str);
        }
        break;
    case TOK_REGEXP:
        {
            const char *str, *str2;
            str = JS_ToCString(s->ctx, token->u.regexp.body);
            str2 = JS_ToCString(s->ctx, token->u.regexp.flags);
            printf("regexp: '%s' '%s'\n", str, str2);
            JS_FreeCString(s->ctx, str);
            JS_FreeCString(s->ctx, str2);
        }
        break;
    case TOK_EOF:
        printf("eof\n");
        break;
    default:
        if (s->token.val >= TOK_NULL && s->token.val <= TOK_LAST_KEYWORD) {
            goto dump_atom;
        } else if (s->token.val >= 256) {
            printf("token: %d\n", token->val);
        } else {
            printf("token: '%c'\n", token->val);
        }
        break;
    }
}

int ident_realloc(JSContext *ctx, char **pbuf, size_t *psize, char *static_buf)
{
    char *buf, *new_buf;
    size_t size, new_size;
    buf = *pbuf;
    size = *psize;
    if (size >= (SIZE_MAX / 3) * 2)
        new_size = SIZE_MAX;
    else
        new_size = size + (size >> 1);
    if (buf == static_buf) {
        new_buf = js_malloc(ctx, new_size);
        if (!new_buf)
            return -1;
        memcpy(new_buf, buf, size);
    } else {
        new_buf = js_realloc(ctx, buf, new_size);
        if (!new_buf)
            return -1;
    }
    *pbuf = new_buf;
    *psize = new_size;
    return 0;
}

/**
 * Returns true if 'input' contains the source of a module
 * (heuristic). 'input' must be a zero terminated.
 *
 * Heuristic: skip comments and expect 'import' keyword not followed
 * by '(' or '.' or export keyword.
 */
BOOL JS_DetectModule(const char *input, size_t input_len)
{
    const uint8_t *p = (const uint8_t *)input;
    int tok;
    switch(simple_next_token(&p, FALSE)) {
    case TOK_IMPORT:
        tok = simple_next_token(&p, FALSE);
        return (tok != '.' && tok != '(');
    case TOK_EXPORT:
        return TRUE;
    default:
        return FALSE;
    }
}

void emit_op(JSParseState *s, uint8_t val)
{
    JSFunctionDef *fd = s->cur_func;
    DynBuf *bc = &fd->byte_code;
    /* Use the line number of the last token used, not the next token,
       nor the current offset in the source file.
     */
    if (UNLIKELY(fd->last_opcode_line_num != s->last_line_num)) {
        dbuf_putc(bc, OP_line_num);
        dbuf_put_u32(bc, s->last_line_num);
        fd->last_opcode_line_num = s->last_line_num;
    }
    fd->last_opcode_pos = bc->size;
    dbuf_putc(bc, val);
}

void emit_atom(JSParseState *s, JSAtom name)
{
    emit_u32(s, JS_DupAtom(s->ctx, name));
}

int update_label(JSFunctionDef *s, int label, int delta)
{
    LabelSlot *ls;
    assert(label >= 0 && label < s->label_count);
    ls = &s->label_slots[label];
    ls->ref_count += delta;
    assert(ls->ref_count >= 0);
    return ls->ref_count;
}

static int new_label_fd(JSFunctionDef *fd, int label)
{
    LabelSlot *ls;

    if (label < 0) {
        if (js_resize_array(fd->ctx, (void *)&fd->label_slots,
                            sizeof(fd->label_slots[0]),
                            &fd->label_size, fd->label_count + 1))
            return -1;
        label = fd->label_count++;
        ls = &fd->label_slots[label];
        ls->ref_count = 0;
        ls->pos = -1;
        ls->pos2 = -1;
        ls->addr = -1;
        ls->first_reloc = NULL;
    }
    return label;
}

int new_label(JSParseState *s)
{
    return new_label_fd(s->cur_func, -1);
}

/* return the label ID offset */
int emit_label(JSParseState *s, int label)
{
    if (label >= 0) {
        emit_op(s, OP_label);
        emit_u32(s, label);
        s->cur_func->label_slots[label].pos = s->cur_func->byte_code.size;
        return s->cur_func->byte_code.size - 4;
    } else {
        return -1;
    }
}

/* return the constant pool index. 'val' is not duplicated. */
int cpool_add(JSParseState *s, JSValue val)
{
    JSFunctionDef *fd = s->cur_func;
    if (js_resize_array(s->ctx, (void *)&fd->cpool, sizeof(fd->cpool[0]),
                        &fd->cpool_size, fd->cpool_count + 1))
        return -1;
    fd->cpool[fd->cpool_count++] = val;
    return fd->cpool_count - 1;
}

/* return the variable index or -1 if not found,
   add ARGUMENT_VAR_OFFSET for argument variables */
static int find_arg(JSContext *ctx, JSFunctionDef *fd, JSAtom name)
{
    int i;
    for(i = fd->arg_count; i-- > 0;) {
        if (fd->args[i].var_name == name)
            return i | ARGUMENT_VAR_OFFSET;
    }
    return -1;
}

int find_var(JSContext *ctx, JSFunctionDef *fd, JSAtom name)
{
    int i;
    for(i = fd->var_count; i-- > 0;) {
        if (fd->vars[i].var_name == name && fd->vars[i].scope_level == 0)
            return i;
    }
    return find_arg(ctx, fd, name);
}

/* find a variable declaration in a given scope */
static int find_var_in_scope(JSContext *ctx, JSFunctionDef *fd,
                             JSAtom name, int scope_level)
{
    int scope_idx;
    for(scope_idx = fd->scopes[scope_level].first; scope_idx >= 0;
        scope_idx = fd->vars[scope_idx].scope_next) {
        if (fd->vars[scope_idx].scope_level != scope_level)
            break;
        if (fd->vars[scope_idx].var_name == name)
            return scope_idx;
    }
    return -1;
}

/* return true if scope == parent_scope or if scope is a child of
   parent_scope */
static BOOL is_child_scope(JSContext *ctx, JSFunctionDef *fd,
                           int scope, int parent_scope)
{
    while (scope >= 0) {
        if (scope == parent_scope)
            return TRUE;
        scope = fd->scopes[scope].parent;
    }
    return FALSE;
}

/* find a 'var' declaration in the same scope or a child scope */
static int find_var_in_child_scope(JSContext *ctx, JSFunctionDef *fd,
                                   JSAtom name, int scope_level)
{
    int i;
    for(i = 0; i < fd->var_count; i++) {
        JSVarDef *vd = &fd->vars[i];
        if (vd->var_name == name && vd->scope_level == 0) {
            if (is_child_scope(ctx, fd, vd->scope_next,
                               scope_level))
                return i;
        }
    }
    return -1;
}


JSGlobalVar *find_global_var(JSFunctionDef *fd, JSAtom name)
{
    int i;
    for(i = 0; i < fd->global_var_count; i++) {
        JSGlobalVar *hf = &fd->global_vars[i];
        if (hf->var_name == name)
            return hf;
    }
    return NULL;

}

static JSGlobalVar *find_lexical_global_var(JSFunctionDef *fd, JSAtom name)
{
    JSGlobalVar *hf = find_global_var(fd, name);
    if (hf && hf->is_lexical)
        return hf;
    else
        return NULL;
}

int find_lexical_decl(JSContext *ctx, JSFunctionDef *fd, JSAtom name,
                      int scope_idx, BOOL check_catch_var)
{
    while (scope_idx >= 0) {
        JSVarDef *vd = &fd->vars[scope_idx];
        if (vd->var_name == name &&
            (vd->is_lexical || (vd->var_kind == JS_VAR_CATCH &&
                                check_catch_var)))
            return scope_idx;
        scope_idx = vd->scope_next;
    }
    if (fd->is_eval && fd->eval_type == JS_EVAL_TYPE_GLOBAL) {
        if (find_lexical_global_var(fd, name))
            return GLOBAL_VAR_OFFSET;
    }
    return -1;
}

int push_scope(JSParseState *s) {
    if (s->cur_func) {
        JSFunctionDef *fd = s->cur_func;
        int scope = fd->scope_count;
        /* XXX: should check for scope overflow */
        if ((fd->scope_count + 1) > fd->scope_size) {
            int new_size;
            size_t slack;
            JSVarScope *new_buf;
            /* XXX: potential arithmetic overflow */
            new_size = max_int(fd->scope_count + 1, fd->scope_size * 3 / 2);
            if (fd->scopes == fd->def_scope_array) {
                new_buf = js_realloc2(s->ctx, NULL, new_size * sizeof(*fd->scopes), &slack);
                if (!new_buf)
                    return -1;
                memcpy(new_buf, fd->scopes, fd->scope_count * sizeof(*fd->scopes));
            } else {
                new_buf = js_realloc2(s->ctx, fd->scopes, new_size * sizeof(*fd->scopes), &slack);
                if (!new_buf)
                    return -1;
            }
            new_size += slack / sizeof(*new_buf);
            fd->scopes = new_buf;
            fd->scope_size = new_size;
        }
        fd->scope_count++;
        fd->scopes[scope].parent = fd->scope_level;
        fd->scopes[scope].first = fd->scope_first;
        emit_op(s, OP_enter_scope);
        emit_u16(s, scope);
        return fd->scope_level = scope;
    }
    return 0;
}

void close_scopes(JSParseState *s, int scope, int scope_stop)
{
    while (scope > scope_stop) {
        emit_op(s, OP_leave_scope);
        emit_u16(s, scope);
        scope = s->cur_func->scopes[scope].parent;
    }
}

/* return the variable index or -1 if error */
int add_var(JSContext *ctx, JSFunctionDef *fd, JSAtom name)
{
    JSVarDef *vd;
    /* the local variable indexes are currently stored on 16 bits */
    if (fd->var_count >= JS_MAX_LOCAL_VARS) {
        JS_ThrowInternalError(ctx, "too many local variables");
        return -1;
    }
    if (js_resize_array(ctx, (void **)&fd->vars, sizeof(fd->vars[0]),
                        &fd->var_size, fd->var_count + 1))
        return -1;
    vd = &fd->vars[fd->var_count++];
    bzero(vd, sizeof(*vd));
    vd->var_name = JS_DupAtom(ctx, name);
    vd->func_pool_idx = -1;
    return fd->var_count - 1;
}

int add_scope_var(JSContext *ctx, JSFunctionDef *fd, JSAtom name, JSVarKindEnum var_kind)
{
    int idx = add_var(ctx, fd, name);
    if (idx >= 0) {
        JSVarDef *vd = &fd->vars[idx];
        vd->var_kind = var_kind;
        vd->scope_level = fd->scope_level;
        vd->scope_next = fd->scope_first;
        fd->scopes[fd->scope_level].first = idx;
        fd->scope_first = idx;
    }
    return idx;
}

static int add_func_var(JSContext *ctx, JSFunctionDef *fd, JSAtom name)
{
    int idx = fd->func_var_idx;
    if (idx < 0 && (idx = add_var(ctx, fd, name)) >= 0) {
        fd->func_var_idx = idx;
        fd->vars[idx].var_kind = JS_VAR_FUNCTION_NAME;
        if (fd->js_mode & JS_MODE_STRICT)
            fd->vars[idx].is_const = TRUE;
    }
    return idx;
}

static int add_arguments_var(JSContext *ctx, JSFunctionDef *fd)
{
    int idx = fd->arguments_var_idx;
    if (idx < 0 && (idx = add_var(ctx, fd, JS_ATOM_arguments)) >= 0) {
        fd->arguments_var_idx = idx;
    }
    return idx;
}

/* add an argument definition in the argument scope. Only needed when
   "eval()" may be called in the argument scope. Return 0 if OK. */
static int add_arguments_arg(JSContext *ctx, JSFunctionDef *fd)
{
    int idx;
    if (fd->arguments_arg_idx < 0) {
        idx = find_var_in_scope(ctx, fd, JS_ATOM_arguments, ARG_SCOPE_INDEX);
        if (idx < 0) {
            /* XXX: the scope links are not fully updated. May be an
               issue if there are child scopes of the argument
               scope */
            idx = add_var(ctx, fd, JS_ATOM_arguments);
            if (idx < 0)
                return -1;
            fd->vars[idx].scope_next = fd->scopes[ARG_SCOPE_INDEX].first;
            fd->scopes[ARG_SCOPE_INDEX].first = idx;
            fd->vars[idx].scope_level = ARG_SCOPE_INDEX;
            fd->vars[idx].is_lexical = TRUE;

            fd->arguments_arg_idx = idx;
        }
    }
    return 0;
}

/* add a global variable definition */
JSGlobalVar *add_global_var(JSContext *ctx, JSFunctionDef *s, JSAtom name)
{
    JSGlobalVar *hf;
    if (js_resize_array(ctx, (void **)&s->global_vars,
                        sizeof(s->global_vars[0]),
                        &s->global_var_size, s->global_var_count + 1))
        return NULL;
    hf = &s->global_vars[s->global_var_count++];
    hf->cpool_idx = -1;
    hf->force_init = FALSE;
    hf->is_lexical = FALSE;
    hf->is_const = FALSE;
    hf->scope_level = s->scope_level;
    hf->var_name = JS_DupAtom(ctx, name);
    return hf;
}

int define_var(JSParseState *s, JSFunctionDef *fd, JSAtom name, JSVarDefEnum var_def_type)
{
    JSContext *ctx = s->ctx;
    JSVarDef *vd;
    int idx;
    switch (var_def_type) {
    case JS_VAR_DEF_WITH:
        idx = add_scope_var(ctx, fd, name, JS_VAR_NORMAL);
        break;
    case JS_VAR_DEF_LET:
    case JS_VAR_DEF_CONST:
    case JS_VAR_DEF_FUNCTION_DECL:
    case JS_VAR_DEF_NEW_FUNCTION_DECL:
        idx = find_lexical_decl(ctx, fd, name, fd->scope_first, TRUE);
        if (idx >= 0) {
            if (idx < GLOBAL_VAR_OFFSET) {
                if (fd->vars[idx].scope_level == fd->scope_level) {
                    /* same scope: in non strict mode, functions
                       can be redefined (annex B.3.3.4). */
                    if (!(!(fd->js_mode & JS_MODE_STRICT) &&
                          var_def_type == JS_VAR_DEF_FUNCTION_DECL &&
                          fd->vars[idx].var_kind == JS_VAR_FUNCTION_DECL)) {
                        goto redef_lex_error;
                    }
                } else if (fd->vars[idx].var_kind == JS_VAR_CATCH && (fd->vars[idx].scope_level + 2) == fd->scope_level) {
                    goto redef_lex_error;
                }
            } else {
                if (fd->scope_level == fd->body_scope) {
                redef_lex_error:
                    /* redefining a scoped var in the same scope: error */
                    return js_parse_error(s, "invalid redefinition of lexical identifier");
                }
            }
        }
        if (var_def_type != JS_VAR_DEF_FUNCTION_DECL &&
            var_def_type != JS_VAR_DEF_NEW_FUNCTION_DECL &&
            fd->scope_level == fd->body_scope &&
            find_arg(ctx, fd, name) >= 0) {
            /* lexical variable redefines a parameter name */
            return js_parse_error(s, "invalid redefinition of parameter name");
        }
        if (find_var_in_child_scope(ctx, fd, name, fd->scope_level) >= 0) {
            return js_parse_error(s, "invalid redefinition of a variable");
        }
        if (fd->is_global_var) {
            JSGlobalVar *hf;
            hf = find_global_var(fd, name);
            if (hf && is_child_scope(ctx, fd, hf->scope_level,
                                     fd->scope_level)) {
                return js_parse_error(s, "invalid redefinition of global identifier");
            }
        }
        if (fd->is_eval &&
            (fd->eval_type == JS_EVAL_TYPE_GLOBAL ||
             fd->eval_type == JS_EVAL_TYPE_MODULE) &&
            fd->scope_level == fd->body_scope) {
            JSGlobalVar *hf;
            hf = add_global_var(s->ctx, fd, name);
            if (!hf)
                return -1;
            hf->is_lexical = TRUE;
            hf->is_const = (var_def_type == JS_VAR_DEF_CONST);
            idx = GLOBAL_VAR_OFFSET;
        } else {
            JSVarKindEnum var_kind;
            if (var_def_type == JS_VAR_DEF_FUNCTION_DECL)
                var_kind = JS_VAR_FUNCTION_DECL;
            else if (var_def_type == JS_VAR_DEF_NEW_FUNCTION_DECL)
                var_kind = JS_VAR_NEW_FUNCTION_DECL;
            else
                var_kind = JS_VAR_NORMAL;
            idx = add_scope_var(ctx, fd, name, var_kind);
            if (idx >= 0) {
                vd = &fd->vars[idx];
                vd->is_lexical = 1;
                vd->is_const = (var_def_type == JS_VAR_DEF_CONST);
            }
        }
        break;
    case JS_VAR_DEF_CATCH:
        idx = add_scope_var(ctx, fd, name, JS_VAR_CATCH);
        break;
    case JS_VAR_DEF_VAR:
        if (find_lexical_decl(ctx, fd, name, fd->scope_first,
                              FALSE) >= 0) {
       invalid_lexical_redefinition:
            /* error to redefine a var that inside a lexical scope */
            return js_parse_error(s, "invalid redefinition of lexical identifier");
        }
        if (fd->is_global_var) {
            JSGlobalVar *hf;
            hf = find_global_var(fd, name);
            if (hf && hf->is_lexical && hf->scope_level == fd->scope_level &&
                fd->eval_type == JS_EVAL_TYPE_MODULE) {
                goto invalid_lexical_redefinition;
            }
            hf = add_global_var(s->ctx, fd, name);
            if (!hf)
                return -1;
            idx = GLOBAL_VAR_OFFSET;
        } else {
            /* if the variable already exists, don't add it again  */
            idx = find_var(ctx, fd, name);
            if (idx >= 0)
                break;
            idx = add_var(ctx, fd, name);
            if (idx >= 0) {
                if (name == JS_ATOM_arguments && fd->has_arguments_binding)
                    fd->arguments_var_idx = idx;
                fd->vars[idx].scope_next = fd->scope_level;
            }
        }
        break;
    default:
        abort();
    }
    return idx;
}

/* find field in the current scope */
int find_private_class_field(JSContext *ctx, JSFunctionDef *fd, JSAtom name, int scope_level)
{
    int idx;
    idx = fd->scopes[scope_level].first;
    while (idx != -1) {
        if (fd->vars[idx].scope_level != scope_level)
            break;
        if (fd->vars[idx].var_name == name)
            return idx;
        idx = fd->vars[idx].scope_next;
    }
    return -1;
}

/* build a private setter function name from the private getter name */
JSAtom get_private_setter_name(JSContext *ctx, JSAtom name)
{
    return js_atom_concat_str(ctx, name, "<set>");
}

/* 'name' is freed */
JSModuleDef *js_new_module_def(JSContext *ctx, JSAtom name)
{
    JSModuleDef *m;
    m = js_mallocz(ctx, sizeof(*m));
    if (!m) {
        JS_FreeAtom(ctx, name);
        return NULL;
    }
    m->header.ref_count = 1;
    m->module_name = name;
    m->module_ns = JS_UNDEFINED;
    m->func_obj = JS_UNDEFINED;
    m->eval_exception = JS_UNDEFINED;
    m->meta_obj = JS_UNDEFINED;
    list_add_tail(&m->link, &ctx->loaded_modules);
    return m;
}

void js_free_module_def(JSContext *ctx, JSModuleDef *m)
{
    int i;
    JS_FreeAtom(ctx, m->module_name);
    for(i = 0; i < m->req_module_entries_count; i++) {
        JSReqModuleEntry *rme = &m->req_module_entries[i];
        JS_FreeAtom(ctx, rme->module_name);
    }
    js_free(ctx, m->req_module_entries);
    for(i = 0; i < m->export_entries_count; i++) {
        JSExportEntry *me = &m->export_entries[i];
        if (me->export_type == JS_EXPORT_TYPE_LOCAL)
            free_var_ref(ctx->rt, me->u.local.var_ref);
        JS_FreeAtom(ctx, me->export_name);
        JS_FreeAtom(ctx, me->local_name);
    }
    js_free(ctx, m->export_entries);
    js_free(ctx, m->star_export_entries);
    for(i = 0; i < m->import_entries_count; i++) {
        JSImportEntry *mi = &m->import_entries[i];
        JS_FreeAtom(ctx, mi->import_name);
    }
    js_free(ctx, m->import_entries);
    JS_FreeValue(ctx, m->module_ns);
    JS_FreeValue(ctx, m->func_obj);
    JS_FreeValue(ctx, m->eval_exception);
    JS_FreeValue(ctx, m->meta_obj);
    list_del(&m->link);
    js_free(ctx, m);
}

static JSExportEntry *find_export_entry(JSContext *ctx, JSModuleDef *m,
                                        JSAtom export_name)
{
    JSExportEntry *me;
    int i;
    for(i = 0; i < m->export_entries_count; i++) {
        me = &m->export_entries[i];
        if (me->export_name == export_name)
            return me;
    }
    return NULL;
}

static JSExportEntry *add_export_entry2(JSContext *ctx,
                                        JSParseState *s, JSModuleDef *m,
                                       JSAtom local_name, JSAtom export_name,
                                       JSExportTypeEnum export_type)
{
    JSExportEntry *me;
    if (find_export_entry(ctx, m, export_name)) {
        char buf1[ATOM_GET_STR_BUF_SIZE];
        if (s) {
            js_parse_error(s, "duplicate exported name '%s'",
                           JS_AtomGetStr(ctx, buf1, sizeof(buf1), export_name));
        } else {
            JS_ThrowSyntaxErrorAtom(ctx, "duplicate exported name '%s'", export_name);
        }
        return NULL;
    }
    if (js_resize_array(ctx, (void **)&m->export_entries,
                        sizeof(JSExportEntry),
                        &m->export_entries_size,
                        m->export_entries_count + 1))
        return NULL;
    me = &m->export_entries[m->export_entries_count++];
    bzero(me, sizeof(*me));
    me->local_name = JS_DupAtom(ctx, local_name);
    me->export_name = JS_DupAtom(ctx, export_name);
    me->export_type = export_type;
    return me;
}

JSExportEntry *add_export_entry(JSParseState *s, JSModuleDef *m,
                                JSAtom local_name, JSAtom export_name,
                                JSExportTypeEnum export_type)
{
    return add_export_entry2(s->ctx, s, m, local_name, export_name,
                             export_type);
}

/* create a C module */
JSModuleDef *JS_NewCModule(JSContext *ctx, const char *name_str,
                           JSModuleInitFunc *func)
{
    JSModuleDef *m;
    JSAtom name;
    name = JS_NewAtom(ctx, name_str);
    if (name == JS_ATOM_NULL)
        return NULL;
    m = js_new_module_def(ctx, name);
    m->init_func = func;
    return m;
}

int JS_AddModuleExport(JSContext *ctx, JSModuleDef *m, const char *export_name)
{
    JSExportEntry *me;
    JSAtom name;
    name = JS_NewAtom(ctx, export_name);
    if (name == JS_ATOM_NULL)
        return -1;
    me = add_export_entry2(ctx, NULL, m, JS_ATOM_NULL, name,
                           JS_EXPORT_TYPE_LOCAL);
    JS_FreeAtom(ctx, name);
    if (!me)
        return -1;
    else
        return 0;
}

int JS_SetModuleExport(JSContext *ctx, JSModuleDef *m, const char *export_name,
                       JSValue val)
{
    JSExportEntry *me;
    JSAtom name;
    name = JS_NewAtom(ctx, export_name);
    if (name == JS_ATOM_NULL)
        goto fail;
    me = find_export_entry(ctx, m, name);
    JS_FreeAtom(ctx, name);
    if (!me)
        goto fail;
    set_value(ctx, me->u.local.var_ref->pvalue, val);
    return 0;
 fail:
    JS_FreeValue(ctx, val);
    return -1;
}

void JS_SetModuleLoaderFunc(JSRuntime *rt,
                            JSModuleNormalizeFunc *module_normalize,
                            JSModuleLoaderFunc *module_loader, void *opaque)
{
    rt->module_normalize_func = module_normalize;
    rt->module_loader_func = module_loader;
    rt->module_loader_opaque = opaque;
}

/* default module filename normalizer */
static char *js_default_module_normalize_name(JSContext *ctx,
                                              const char *base_name,
                                              const char *name)
{
    char *filename, *p;
    const char *r;
    int len;
    if (name[0] != '.') {
        /* if no initial dot, the module name is not modified */
        return js_strdup(ctx, name);
    }
    p = strrchr(base_name, '/');
    if (p)
        len = p - base_name;
    else
        len = 0;
    filename = js_malloc(ctx, len + strlen(name) + 1 + 1);
    if (!filename)
        return NULL;
    memcpy(filename, base_name, len);
    filename[len] = '\0';
    /* we only normalize the leading '..' or '.' */
    r = name;
    for(;;) {
        if (r[0] == '.' && r[1] == '/') {
            r += 2;
        } else if (r[0] == '.' && r[1] == '.' && r[2] == '/') {
            /* remove the last path element of filename, except if "."
               or ".." */
            if (filename[0] == '\0')
                break;
            p = strrchr(filename, '/');
            if (!p)
                p = filename;
            else
                p++;
            if (!strcmp(p, ".") || !strcmp(p, ".."))
                break;
            if (p > filename)
                p--;
            *p = '\0';
            r += 3;
        } else {
            break;
        }
    }
    if (filename[0] != '\0')
        strcat(filename, "/");
    strcat(filename, r);
    //    printf("normalize: %s %s -> %s\n", base_name, name, filename);
    return filename;
}

JSModuleDef *js_find_loaded_module(JSContext *ctx, JSAtom name)
{
    struct list_head *el;
    JSModuleDef *m;
    /* first look at the loaded modules */
    list_for_each(el, &ctx->loaded_modules) {
        m = list_entry(el, JSModuleDef, link);
        if (m->module_name == name)
            return m;
    }
    return NULL;
}

/* return NULL in case of exception (e.g. module could not be loaded) */
static JSModuleDef *js_host_resolve_imported_module(JSContext *ctx,
                                                    const char *base_cname,
                                                    const char *cname1)
{
    JSRuntime *rt = ctx->rt;
    JSModuleDef *m;
    char *cname;
    JSAtom module_name;
    if (!rt->module_normalize_func) {
        cname = js_default_module_normalize_name(ctx, base_cname, cname1);
    } else {
        cname = rt->module_normalize_func(ctx, base_cname, cname1,
                                          rt->module_loader_opaque);
    }
    if (!cname)
        return NULL;
    module_name = JS_NewAtom(ctx, cname);
    if (module_name == JS_ATOM_NULL) {
        js_free(ctx, cname);
        return NULL;
    }
    /* first look at the loaded modules */
    m = js_find_loaded_module(ctx, module_name);
    if (m) {
        js_free(ctx, cname);
        JS_FreeAtom(ctx, module_name);
        return m;
    }
    JS_FreeAtom(ctx, module_name);
    /* load the module */
    if (!rt->module_loader_func) {
        /* XXX: use a syntax error ? */
        JS_ThrowReferenceError(ctx, "could not load module '%s'",
                               cname);
        js_free(ctx, cname);
        return NULL;
    }
    m = rt->module_loader_func(ctx, cname, rt->module_loader_opaque);
    js_free(ctx, cname);
    return m;
}

static JSModuleDef *js_host_resolve_imported_module_atom(JSContext *ctx,
                                                    JSAtom base_module_name,
                                                    JSAtom module_name1)
{
    const char *base_cname, *cname;
    JSModuleDef *m;
    base_cname = JS_AtomToCString(ctx, base_module_name);
    if (!base_cname)
        return NULL;
    cname = JS_AtomToCString(ctx, module_name1);
    if (!cname) {
        JS_FreeCString(ctx, base_cname);
        return NULL;
    }
    m = js_host_resolve_imported_module(ctx, base_cname, cname);
    JS_FreeCString(ctx, base_cname);
    JS_FreeCString(ctx, cname);
    return m;
}

typedef struct JSResolveEntry {
    JSModuleDef *module;
    JSAtom name;
} JSResolveEntry;

typedef struct JSResolveState {
    JSResolveEntry *array;
    int size;
    int count;
} JSResolveState;

static int find_resolve_entry(JSResolveState *s,
                              JSModuleDef *m, JSAtom name)
{
    int i;
    for(i = 0; i < s->count; i++) {
        JSResolveEntry *re = &s->array[i];
        if (re->module == m && re->name == name)
            return i;
    }
    return -1;
}

static int add_resolve_entry(JSContext *ctx, JSResolveState *s,
                             JSModuleDef *m, JSAtom name)
{
    JSResolveEntry *re;
    if (js_resize_array(ctx, (void **)&s->array,
                        sizeof(JSResolveEntry),
                        &s->size, s->count + 1))
        return -1;
    re = &s->array[s->count++];
    re->module = m;
    re->name = JS_DupAtom(ctx, name);
    return 0;
}

typedef enum JSResolveResultEnum {
    JS_RESOLVE_RES_EXCEPTION = -1, /* memory alloc error */
    JS_RESOLVE_RES_FOUND = 0,
    JS_RESOLVE_RES_NOT_FOUND,
    JS_RESOLVE_RES_CIRCULAR,
    JS_RESOLVE_RES_AMBIGUOUS,
} JSResolveResultEnum;

static JSResolveResultEnum js_resolve_export1(JSContext *ctx,
                                              JSModuleDef **pmodule,
                                              JSExportEntry **pme,
                                              JSModuleDef *m,
                                              JSAtom export_name,
                                              JSResolveState *s)
{
    JSExportEntry *me;
    *pmodule = NULL;
    *pme = NULL;
    if (find_resolve_entry(s, m, export_name) >= 0)
        return JS_RESOLVE_RES_CIRCULAR;
    if (add_resolve_entry(ctx, s, m, export_name) < 0)
        return JS_RESOLVE_RES_EXCEPTION;
    me = find_export_entry(ctx, m, export_name);
    if (me) {
        if (me->export_type == JS_EXPORT_TYPE_LOCAL) {
            /* local export */
            *pmodule = m;
            *pme = me;
            return JS_RESOLVE_RES_FOUND;
        } else {
            /* indirect export */
            JSModuleDef *m1;
            m1 = m->req_module_entries[me->u.req_module_idx].module;
            if (me->local_name == JS_ATOM__star_) {
                /* export ns from */
                *pmodule = m;
                *pme = me;
                return JS_RESOLVE_RES_FOUND;
            } else {
                return js_resolve_export1(ctx, pmodule, pme, m1,
                                          me->local_name, s);
            }
        }
    } else {
        if (export_name != JS_ATOM_default) {
            /* not found in direct or indirect exports: try star exports */
            int i;
            for(i = 0; i < m->star_export_entries_count; i++) {
                JSStarExportEntry *se = &m->star_export_entries[i];
                JSModuleDef *m1, *res_m;
                JSExportEntry *res_me;
                JSResolveResultEnum ret;
                m1 = m->req_module_entries[se->req_module_idx].module;
                ret = js_resolve_export1(ctx, &res_m, &res_me, m1,
                                         export_name, s);
                if (ret == JS_RESOLVE_RES_AMBIGUOUS ||
                    ret == JS_RESOLVE_RES_EXCEPTION) {
                    return ret;
                } else if (ret == JS_RESOLVE_RES_FOUND) {
                    if (*pme != NULL) {
                        if (*pmodule != res_m ||
                            res_me->local_name != (*pme)->local_name) {
                            *pmodule = NULL;
                            *pme = NULL;
                            return JS_RESOLVE_RES_AMBIGUOUS;
                        }
                    } else {
                        *pmodule = res_m;
                        *pme = res_me;
                    }
                }
            }
            if (*pme != NULL)
                return JS_RESOLVE_RES_FOUND;
        }
        return JS_RESOLVE_RES_NOT_FOUND;
    }
}

/* If the return value is JS_RESOLVE_RES_FOUND, return the module
  (*pmodule) and the corresponding local export entry
  (*pme). Otherwise return (NULL, NULL) */
static JSResolveResultEnum js_resolve_export(JSContext *ctx,
                                             JSModuleDef **pmodule,
                                             JSExportEntry **pme,
                                             JSModuleDef *m,
                                             JSAtom export_name)
{
    JSResolveState ss, *s = &ss;
    int i;
    JSResolveResultEnum ret;
    s->array = NULL;
    s->size = 0;
    s->count = 0;
    ret = js_resolve_export1(ctx, pmodule, pme, m, export_name, s);
    for(i = 0; i < s->count; i++)
        JS_FreeAtom(ctx, s->array[i].name);
    js_free(ctx, s->array);
    return ret;
}

static void js_resolve_export_throw_error(JSContext *ctx,
                                          JSResolveResultEnum res,
                                          JSModuleDef *m, JSAtom export_name)
{
    char buf1[ATOM_GET_STR_BUF_SIZE];
    char buf2[ATOM_GET_STR_BUF_SIZE];
    switch(res) {
    case JS_RESOLVE_RES_EXCEPTION:
        break;
    default:
    case JS_RESOLVE_RES_NOT_FOUND:
        JS_ThrowSyntaxError(ctx, "Could not find export '%s' in module '%s'",
                            JS_AtomGetStr(ctx, buf1, sizeof(buf1), export_name),
                            JS_AtomGetStr(ctx, buf2, sizeof(buf2), m->module_name));
        break;
    case JS_RESOLVE_RES_CIRCULAR:
        JS_ThrowSyntaxError(ctx, "circular reference when looking for export '%s' in module '%s'",
                            JS_AtomGetStr(ctx, buf1, sizeof(buf1), export_name),
                            JS_AtomGetStr(ctx, buf2, sizeof(buf2), m->module_name));
        break;
    case JS_RESOLVE_RES_AMBIGUOUS:
        JS_ThrowSyntaxError(ctx, "export '%s' in module '%s' is ambiguous",
                            JS_AtomGetStr(ctx, buf1, sizeof(buf1), export_name),
                            JS_AtomGetStr(ctx, buf2, sizeof(buf2), m->module_name));
        break;
    }
}

typedef enum {
    EXPORTED_NAME_AMBIGUOUS,
    EXPORTED_NAME_NORMAL,
    EXPORTED_NAME_NS,
} ExportedNameEntryEnum;

typedef struct ExportedNameEntry {
    JSAtom export_name;
    ExportedNameEntryEnum export_type;
    union {
        JSExportEntry *me; /* using when the list is built */
        JSVarRef *var_ref; /* EXPORTED_NAME_NORMAL */
        JSModuleDef *module; /* for EXPORTED_NAME_NS */
    } u;
} ExportedNameEntry;

typedef struct GetExportNamesState {
    JSModuleDef **modules;
    int modules_size;
    int modules_count;
    ExportedNameEntry *exported_names;
    int exported_names_size;
    int exported_names_count;
} GetExportNamesState;

static int find_exported_name(GetExportNamesState *s, JSAtom name)
{
    int i;
    for(i = 0; i < s->exported_names_count; i++) {
        if (s->exported_names[i].export_name == name)
            return i;
    }
    return -1;
}

static __exception int get_exported_names(JSContext *ctx,
                                          GetExportNamesState *s,
                                          JSModuleDef *m, BOOL from_star)
{
    ExportedNameEntry *en;
    int i, j;
    /* check circular reference */
    for(i = 0; i < s->modules_count; i++) {
        if (s->modules[i] == m)
            return 0;
    }
    if (js_resize_array(ctx, (void **)&s->modules, sizeof(s->modules[0]),
                        &s->modules_size, s->modules_count + 1))
        return -1;
    s->modules[s->modules_count++] = m;
    for(i = 0; i < m->export_entries_count; i++) {
        JSExportEntry *me = &m->export_entries[i];
        if (from_star && me->export_name == JS_ATOM_default)
            continue;
        j = find_exported_name(s, me->export_name);
        if (j < 0) {
            if (js_resize_array(ctx, (void **)&s->exported_names, sizeof(s->exported_names[0]),
                                &s->exported_names_size,
                                s->exported_names_count + 1))
                return -1;
            en = &s->exported_names[s->exported_names_count++];
            en->export_name = me->export_name;
            /* avoid a second lookup for simple module exports */
            if (from_star || me->export_type != JS_EXPORT_TYPE_LOCAL)
                en->u.me = NULL;
            else
                en->u.me = me;
        } else {
            en = &s->exported_names[j];
            en->u.me = NULL;
        }
    }
    for(i = 0; i < m->star_export_entries_count; i++) {
        JSStarExportEntry *se = &m->star_export_entries[i];
        JSModuleDef *m1;
        m1 = m->req_module_entries[se->req_module_idx].module;
        if (get_exported_names(ctx, s, m1, TRUE))
            return -1;
    }
    return 0;
}

/* Unfortunately, the spec gives a different behavior from GetOwnProperty ! */
static int js_module_ns_has(JSContext *ctx, JSValueConst obj, JSAtom atom)
{
    return (find_own_property1(JS_VALUE_GET_OBJ(obj), atom) != NULL);
}

static const JSClassExoticMethods js_module_ns_exotic_methods = {
    .has_property = js_module_ns_has,
};

static int exported_names_cmp(const void *p1, const void *p2, void *opaque)
{
    JSContext *ctx = opaque;
    const ExportedNameEntry *me1 = p1;
    const ExportedNameEntry *me2 = p2;
    JSValue str1, str2;
    int ret;
    /* XXX: should avoid allocation memory in atom comparison */
    str1 = JS_AtomToString(ctx, me1->export_name);
    str2 = JS_AtomToString(ctx, me2->export_name);
    if (JS_IsException(str1) || JS_IsException(str2)) {
        /* XXX: raise an error ? */
        ret = 0;
    } else {
        ret = js_string_compare(ctx, JS_VALUE_GET_STRING(str1),
                                JS_VALUE_GET_STRING(str2));
    }
    JS_FreeValue(ctx, str1);
    JS_FreeValue(ctx, str2);
    return ret;
}

static JSValue js_module_ns_autoinit(JSContext *ctx, JSObject *p, JSAtom atom,
                                     void *opaque)
{
    JSModuleDef *m = opaque;
    return js_get_module_ns(ctx, m);
}

static JSValue js_build_module_ns(JSContext *ctx, JSModuleDef *m)
{
    JSValue obj;
    JSObject *p;
    GetExportNamesState s_s, *s = &s_s;
    int i, ret;
    JSProperty *pr;
    obj = JS_NewObjectClass(ctx, JS_CLASS_MODULE_NS);
    if (JS_IsException(obj))
        return obj;
    p = JS_VALUE_GET_OBJ(obj);
    bzero(s, sizeof(*s));
    ret = get_exported_names(ctx, s, m, FALSE);
    js_free(ctx, s->modules);
    if (ret)
        goto fail;
    /* Resolve the exported names. The ambiguous exports are removed */
    for(i = 0; i < s->exported_names_count; i++) {
        ExportedNameEntry *en = &s->exported_names[i];
        JSResolveResultEnum res;
        JSExportEntry *res_me;
        JSModuleDef *res_m;
        if (en->u.me) {
            res_me = en->u.me; /* fast case: no resolution needed */
            res_m = m;
            res = JS_RESOLVE_RES_FOUND;
        } else {
            res = js_resolve_export(ctx, &res_m, &res_me, m,
                                    en->export_name);
        }
        if (res != JS_RESOLVE_RES_FOUND) {
            if (res != JS_RESOLVE_RES_AMBIGUOUS) {
                js_resolve_export_throw_error(ctx, res, m, en->export_name);
                goto fail;
            }
            en->export_type = EXPORTED_NAME_AMBIGUOUS;
        } else {
            if (res_me->local_name == JS_ATOM__star_) {
                en->export_type = EXPORTED_NAME_NS;
                en->u.module = res_m->req_module_entries[res_me->u.req_module_idx].module;
            } else {
                en->export_type = EXPORTED_NAME_NORMAL;
                if (res_me->u.local.var_ref) {
                    en->u.var_ref = res_me->u.local.var_ref;
                } else {
                    JSObject *p1 = JS_VALUE_GET_OBJ(res_m->func_obj);
                    p1 = JS_VALUE_GET_OBJ(res_m->func_obj);
                    en->u.var_ref = p1->u.func.var_refs[res_me->u.local.var_idx];
                }
            }
        }
    }
    /* sort the exported names */
    rqsort(s->exported_names, s->exported_names_count,
           sizeof(s->exported_names[0]), exported_names_cmp, ctx);
    for(i = 0; i < s->exported_names_count; i++) {
        ExportedNameEntry *en = &s->exported_names[i];
        switch(en->export_type) {
        case EXPORTED_NAME_NORMAL:
            {
                JSVarRef *var_ref = en->u.var_ref;
                pr = add_property(ctx, p, en->export_name,
                                  JS_PROP_ENUMERABLE | JS_PROP_WRITABLE |
                                  JS_PROP_VARREF);
                if (!pr)
                    goto fail;
                var_ref->header.ref_count++;
                pr->u.var_ref = var_ref;
            }
            break;
        case EXPORTED_NAME_NS:
            /* the exported namespace must be created on demand */
            if (JS_DefineAutoInitProperty(ctx, obj,
                                          en->export_name,
                                          JS_AUTOINIT_ID_MODULE_NS,
                                          en->u.module, JS_PROP_ENUMERABLE | JS_PROP_WRITABLE) < 0)
                goto fail;
            break;
        default:
            break;
        }
    }
    js_free(ctx, s->exported_names);
    JS_DefinePropertyValue(ctx, obj, JS_ATOM_Symbol_toStringTag,
                           JS_AtomToString(ctx, JS_ATOM_Module),
                           0);
    p->extensible = FALSE;
    return obj;
 fail:
    js_free(ctx, s->exported_names);
    JS_FreeValue(ctx, obj);
    return JS_EXCEPTION;
}

JSValue js_get_module_ns(JSContext *ctx, JSModuleDef *m)
{
    if (JS_IsUndefined(m->module_ns)) {
        JSValue val;
        val = js_build_module_ns(ctx, m);
        if (JS_IsException(val))
            return JS_EXCEPTION;
        m->module_ns = val;
    }
    return JS_DupValue(ctx, m->module_ns);
}

/* Load all the required modules for module 'm' */
static int js_resolve_module(JSContext *ctx, JSModuleDef *m)
{
    int i;
    JSModuleDef *m1;
    if (m->resolved)
        return 0;
#ifdef DUMP_MODULE_RESOLVE
    {
        char buf1[ATOM_GET_STR_BUF_SIZE];
        printf("resolving module '%s':\n", JS_AtomGetStr(ctx, buf1, sizeof(buf1), m->module_name));
    }
#endif
    m->resolved = TRUE;
    /* resolve each requested module */
    for(i = 0; i < m->req_module_entries_count; i++) {
        JSReqModuleEntry *rme = &m->req_module_entries[i];
        m1 = js_host_resolve_imported_module_atom(ctx, m->module_name,
                                                  rme->module_name);
        if (!m1)
            return -1;
        rme->module = m1;
        /* already done in js_host_resolve_imported_module() except if
           the module was loaded with JS_EvalBinary() */
        if (js_resolve_module(ctx, m1) < 0)
            return -1;
    }
    return 0;
}

static JSVarRef *js_create_module_var(JSContext *ctx, BOOL is_lexical)
{
    JSVarRef *var_ref;
    var_ref = js_malloc(ctx, sizeof(JSVarRef));
    if (!var_ref)
        return NULL;
    var_ref->header.ref_count = 1;
    if (is_lexical)
        var_ref->value = JS_UNINITIALIZED;
    else
        var_ref->value = JS_UNDEFINED;
    var_ref->pvalue = &var_ref->value;
    var_ref->is_detached = TRUE;
    add_gc_object(ctx->rt, &var_ref->header, JS_GC_OBJ_TYPE_VAR_REF);
    return var_ref;
}

/* Create the <eval> function associated with the module */
static int js_create_module_bytecode_function(JSContext *ctx, JSModuleDef *m)
{
    JSFunctionBytecode *b;
    int i;
    JSVarRef **var_refs;
    JSValue func_obj, bfunc;
    JSObject *p;
    bfunc = m->func_obj;
    func_obj = JS_NewObjectProtoClass(ctx, ctx->function_proto,
                                      JS_CLASS_BYTECODE_FUNCTION);
    if (JS_IsException(func_obj))
        return -1;
    b = JS_VALUE_GET_PTR(bfunc);
    p = JS_VALUE_GET_OBJ(func_obj);
    p->u.func.function_bytecode = b;
    b->header.ref_count++;
    p->u.func.home_object = NULL;
    p->u.func.var_refs = NULL;
    if (b->closure_var_count) {
        var_refs = js_mallocz(ctx, sizeof(var_refs[0]) * b->closure_var_count);
        if (!var_refs)
            goto fail;
        p->u.func.var_refs = var_refs;
        /* create the global variables. The other variables are
           imported from other modules */
        for(i = 0; i < b->closure_var_count; i++) {
            JSClosureVar *cv = &b->closure_var[i];
            JSVarRef *var_ref;
            if (cv->is_local) {
                var_ref = js_create_module_var(ctx, cv->is_lexical);
                if (!var_ref)
                    goto fail;
#ifdef DUMP_MODULE_RESOLVE
                printf("local %d: %p\n", i, var_ref);
#endif
                var_refs[i] = var_ref;
            }
        }
    }
    m->func_obj = func_obj;
    JS_FreeValue(ctx, bfunc);
    return 0;
 fail:
    JS_FreeValue(ctx, func_obj);
    return -1;
}

/* must be done before js_link_module() because of cyclic references */
static int js_create_module_function(JSContext *ctx, JSModuleDef *m)
{
    BOOL is_c_module;
    int i;
    JSVarRef *var_ref;
    if (m->func_created)
        return 0;
    is_c_module = (m->init_func != NULL);
    if (is_c_module) {
        /* initialize the exported variables */
        for(i = 0; i < m->export_entries_count; i++) {
            JSExportEntry *me = &m->export_entries[i];
            if (me->export_type == JS_EXPORT_TYPE_LOCAL) {
                var_ref = js_create_module_var(ctx, FALSE);
                if (!var_ref)
                    return -1;
                me->u.local.var_ref = var_ref;
            }
        }
    } else {
        if (js_create_module_bytecode_function(ctx, m))
            return -1;
    }
    m->func_created = TRUE;
    /* do it on the dependencies */
    for(i = 0; i < m->req_module_entries_count; i++) {
        JSReqModuleEntry *rme = &m->req_module_entries[i];
        if (js_create_module_function(ctx, rme->module) < 0)
            return -1;
    }
    return 0;
}

/* Prepare a module to be executed by resolving all the imported
   variables. */
static int js_link_module(JSContext *ctx, JSModuleDef *m)
{
    int i;
    JSImportEntry *mi;
    JSModuleDef *m1;
    JSVarRef **var_refs, *var_ref;
    JSObject *p;
    BOOL is_c_module;
    JSValue ret_val;
    if (m->instantiated)
        return 0;
    m->instantiated = TRUE;
#ifdef DUMP_MODULE_RESOLVE
    {
        char buf1[ATOM_GET_STR_BUF_SIZE];
        printf("start instantiating module '%s':\n", JS_AtomGetStr(ctx, buf1, sizeof(buf1), m->module_name));
    }
#endif
    for(i = 0; i < m->req_module_entries_count; i++) {
        JSReqModuleEntry *rme = &m->req_module_entries[i];
        if (js_link_module(ctx, rme->module) < 0)
            goto fail;
    }
#ifdef DUMP_MODULE_RESOLVE
    {
        char buf1[ATOM_GET_STR_BUF_SIZE];
        printf("instantiating module '%s':\n", JS_AtomGetStr(ctx, buf1, sizeof(buf1), m->module_name));
    }
#endif
    /* check the indirect exports */
    for(i = 0; i < m->export_entries_count; i++) {
        JSExportEntry *me = &m->export_entries[i];
        if (me->export_type == JS_EXPORT_TYPE_INDIRECT &&
            me->local_name != JS_ATOM__star_) {
            JSResolveResultEnum ret;
            JSExportEntry *res_me;
            JSModuleDef *res_m, *m1;
            m1 = m->req_module_entries[me->u.req_module_idx].module;
            ret = js_resolve_export(ctx, &res_m, &res_me, m1, me->local_name);
            if (ret != JS_RESOLVE_RES_FOUND) {
                js_resolve_export_throw_error(ctx, ret, m, me->export_name);
                goto fail;
            }
        }
    }
#ifdef DUMP_MODULE_RESOLVE
    {
        printf("exported bindings:\n");
        for(i = 0; i < m->export_entries_count; i++) {
            JSExportEntry *me = &m->export_entries[i];
            printf(" name="); print_atom(ctx, me->export_name);
            printf(" local="); print_atom(ctx, me->local_name);
            printf(" type=%d idx=%d\n", me->export_type, me->u.local.var_idx);
        }
    }
#endif
    is_c_module = (m->init_func != NULL);
    if (!is_c_module) {
        p = JS_VALUE_GET_OBJ(m->func_obj);
        var_refs = p->u.func.var_refs;
        for(i = 0; i < m->import_entries_count; i++) {
            mi = &m->import_entries[i];
#ifdef DUMP_MODULE_RESOLVE
            printf("import var_idx=%d name=", mi->var_idx);
            print_atom(ctx, mi->import_name);
            printf(": ");
#endif
            m1 = m->req_module_entries[mi->req_module_idx].module;
            if (mi->import_name == JS_ATOM__star_) {
                JSValue val;
                /* name space import */
                val = js_get_module_ns(ctx, m1);
                if (JS_IsException(val))
                    goto fail;
                set_value(ctx, &var_refs[mi->var_idx]->value, val);
#ifdef DUMP_MODULE_RESOLVE
                printf("namespace\n");
#endif
            } else {
                JSResolveResultEnum ret;
                JSExportEntry *res_me;
                JSModuleDef *res_m;
                JSObject *p1;
                ret = js_resolve_export(ctx, &res_m,
                                        &res_me, m1, mi->import_name);
                if (ret != JS_RESOLVE_RES_FOUND) {
                    js_resolve_export_throw_error(ctx, ret, m1, mi->import_name);
                    goto fail;
                }
                if (res_me->local_name == JS_ATOM__star_) {
                    JSValue val;
                    JSModuleDef *m2;
                    /* name space import from */
                    m2 = res_m->req_module_entries[res_me->u.req_module_idx].module;
                    val = js_get_module_ns(ctx, m2);
                    if (JS_IsException(val))
                        goto fail;
                    var_ref = js_create_module_var(ctx, TRUE);
                    if (!var_ref) {
                        JS_FreeValue(ctx, val);
                        goto fail;
                    }
                    set_value(ctx, &var_ref->value, val);
                    var_refs[mi->var_idx] = var_ref;
#ifdef DUMP_MODULE_RESOLVE
                    printf("namespace from\n");
#endif
                } else {
                    var_ref = res_me->u.local.var_ref;
                    if (!var_ref) {
                        p1 = JS_VALUE_GET_OBJ(res_m->func_obj);
                        var_ref = p1->u.func.var_refs[res_me->u.local.var_idx];
                    }
                    var_ref->header.ref_count++;
                    var_refs[mi->var_idx] = var_ref;
#ifdef DUMP_MODULE_RESOLVE
                    printf("local export (var_ref=%p)\n", var_ref);
#endif
                }
            }
        }
        /* keep the exported variables in the module export entries (they
           are used when the eval function is deleted and cannot be
           initialized before in case imports are exported) */
        for(i = 0; i < m->export_entries_count; i++) {
            JSExportEntry *me = &m->export_entries[i];
            if (me->export_type == JS_EXPORT_TYPE_LOCAL) {
                var_ref = var_refs[me->u.local.var_idx];
                var_ref->header.ref_count++;
                me->u.local.var_ref = var_ref;
            }
        }
        /* initialize the global variables */
        ret_val = JS_Call(ctx, m->func_obj, JS_TRUE, 0, NULL);
        if (JS_IsException(ret_val))
            goto fail;
        JS_FreeValue(ctx, ret_val);
    }
#ifdef DUMP_MODULE_RESOLVE
    printf("done instantiate\n");
#endif
    return 0;
 fail:
    return -1;
}

/* return JS_ATOM_NULL if the name cannot be found. Only works with
   not striped bytecode functions. */
JSAtom JS_GetScriptOrModuleName(JSContext *ctx, int n_stack_levels)
{
    JSStackFrame *sf;
    JSFunctionBytecode *b;
    JSObject *p;
    /* XXX: currently we just use the filename of the englobing
       function. It does not work for eval(). Need to add a
       ScriptOrModule info in JSFunctionBytecode */
    sf = ctx->rt->current_stack_frame;
    if (!sf)
        return JS_ATOM_NULL;
    while (n_stack_levels-- > 0) {
        sf = sf->prev_frame;
        if (!sf)
            return JS_ATOM_NULL;
    }
    if (JS_VALUE_GET_TAG(sf->cur_func) != JS_TAG_OBJECT)
        return JS_ATOM_NULL;
    p = JS_VALUE_GET_OBJ(sf->cur_func);
    if (!js_class_has_bytecode(p->class_id))
        return JS_ATOM_NULL;
    b = p->u.func.function_bytecode;
    if (!b->has_debug)
        return JS_ATOM_NULL;
    return JS_DupAtom(ctx, b->debug.filename);
}

JSAtom JS_GetModuleName(JSContext *ctx, JSModuleDef *m)
{
    return JS_DupAtom(ctx, m->module_name);
}

JSValue JS_GetImportMeta(JSContext *ctx, JSModuleDef *m)
{
    JSValue obj;
    /* allocate meta_obj only if requested to save memory */
    obj = m->meta_obj;
    if (JS_IsUndefined(obj)) {
        obj = JS_NewObjectProto(ctx, JS_NULL);
        if (JS_IsException(obj))
            return JS_EXCEPTION;
        m->meta_obj = obj;
    }
    return JS_DupValue(ctx, obj);
}

/* used by os.Worker() and import() */
JSModuleDef *JS_RunModule(JSContext *ctx, const char *basename,
                          const char *filename)
{
    JSModuleDef *m;
    JSValue ret, func_obj;
    m = js_host_resolve_imported_module(ctx, basename, filename);
    if (!m)
        return NULL;
    if (js_resolve_module(ctx, m) < 0) {
        js_free_modules(ctx, JS_FREE_MODULE_NOT_RESOLVED);
        return NULL;
    }
    /* Evaluate the module code */
    func_obj = JS_DupValue(ctx, JS_MKPTR(JS_TAG_MODULE, m));
    ret = JS_EvalFunction(ctx, func_obj);
    if (JS_IsException(ret))
        return NULL;
    JS_FreeValue(ctx, ret);
    return m;
}

/* Run the <eval> function of the module and of all its requested
   modules. */
static JSValue js_evaluate_module(JSContext *ctx, JSModuleDef *m)
{
    JSModuleDef *m1;
    int i;
    JSValue ret_val;
    if (m->eval_mark)
        return JS_UNDEFINED; /* avoid cycles */
    if (m->evaluated) {
        /* if the module was already evaluated, rethrow the exception
           it raised */
        if (m->eval_has_exception) {
            return JS_Throw(ctx, JS_DupValue(ctx, m->eval_exception));
        } else {
            return JS_UNDEFINED;
        }
    }
    m->eval_mark = TRUE;
    for(i = 0; i < m->req_module_entries_count; i++) {
        JSReqModuleEntry *rme = &m->req_module_entries[i];
        m1 = rme->module;
        if (!m1->eval_mark) {
            ret_val = js_evaluate_module(ctx, m1);
            if (JS_IsException(ret_val)) {
                m->eval_mark = FALSE;
                return ret_val;
            }
            JS_FreeValue(ctx, ret_val);
        }
    }
    if (m->init_func) {
        /* C module init */
        if (m->init_func(ctx, m) < 0)
            ret_val = JS_EXCEPTION;
        else
            ret_val = JS_UNDEFINED;
    } else {
        ret_val = JS_CallFree(ctx, m->func_obj, JS_UNDEFINED, 0, NULL);
        m->func_obj = JS_UNDEFINED;
    }
    if (JS_IsException(ret_val)) {
        /* save the thrown exception value */
        m->eval_has_exception = TRUE;
        m->eval_exception = JS_DupValue(ctx, ctx->rt->current_exception);
    }
    m->eval_mark = FALSE;
    m->evaluated = TRUE;
    return ret_val;
}

JSFunctionDef *js_new_function_def(JSContext *ctx,
                                   JSFunctionDef *parent,
                                   BOOL is_eval,
                                   BOOL is_func_expr,
                                   const char *filename, int line_num)
{
    JSFunctionDef *fd;
    fd = js_mallocz(ctx, sizeof(*fd));
    if (!fd)
        return NULL;
    fd->ctx = ctx;
    init_list_head(&fd->child_list);
    /* insert in parent list */
    fd->parent = parent;
    fd->parent_cpool_idx = -1;
    if (parent) {
        list_add_tail(&fd->link, &parent->child_list);
        fd->js_mode = parent->js_mode;
        fd->parent_scope_level = parent->scope_level;
    }
    fd->is_eval = is_eval;
    fd->is_func_expr = is_func_expr;
    js_dbuf_init(ctx, &fd->byte_code);
    fd->last_opcode_pos = -1;
    fd->func_name = JS_ATOM_NULL;
    fd->var_object_idx = -1;
    fd->arg_var_object_idx = -1;
    fd->arguments_var_idx = -1;
    fd->arguments_arg_idx = -1;
    fd->func_var_idx = -1;
    fd->eval_ret_idx = -1;
    fd->this_var_idx = -1;
    fd->new_target_var_idx = -1;
    fd->this_active_func_var_idx = -1;
    fd->home_object_var_idx = -1;
    /* XXX: should distinguish arg, var and var object and body scopes */
    fd->scopes = fd->def_scope_array;
    fd->scope_size = countof(fd->def_scope_array);
    fd->scope_count = 1;
    fd->scopes[0].first = -1;
    fd->scopes[0].parent = -1;
    fd->scope_level = 0;  /* 0: var/arg scope */
    fd->scope_first = -1;
    fd->body_scope = -1;
    fd->filename = JS_NewAtom(ctx, filename);
    fd->line_num = line_num;
    js_dbuf_init(ctx, &fd->pc2line);
    //fd->pc2line_last_line_num = line_num;
    //fd->pc2line_last_pc = 0;
    fd->last_opcode_line_num = line_num;
    return fd;
}

void free_bytecode_atoms(JSRuntime *rt,
                         const uint8_t *bc_buf, int bc_len,
                         BOOL use_short_opcodes)
{
    int pos, len, op;
    JSAtom atom;
    const JSOpCode *oi;
    pos = 0;
    while (pos < bc_len) {
        op = bc_buf[pos];
        if (use_short_opcodes)
            oi = &short_opcode_info(op);
        else
            oi = &opcode_info[op];
        len = oi->size;
        switch(oi->fmt) {
        case OP_FMT_atom:
        case OP_FMT_atom_u8:
        case OP_FMT_atom_u16:
        case OP_FMT_atom_label_u8:
        case OP_FMT_atom_label_u16:
            atom = get_u32(bc_buf + pos + 1);
            JS_FreeAtomRT(rt, atom);
            break;
        default:
            break;
        }
        pos += len;
    }
}

void js_free_function_def(JSContext *ctx, JSFunctionDef *fd)
{
    int i;
    struct list_head *el, *el1;
    /* free the child functions */
    list_for_each_safe(el, el1, &fd->child_list) {
        JSFunctionDef *fd1;
        fd1 = list_entry(el, JSFunctionDef, link);
        js_free_function_def(ctx, fd1);
    }
    free_bytecode_atoms(ctx->rt, fd->byte_code.buf, fd->byte_code.size,
                        fd->use_short_opcodes);
    dbuf_free(&fd->byte_code);
    js_free(ctx, fd->jump_slots);
    js_free(ctx, fd->label_slots);
    js_free(ctx, fd->line_number_slots);
    for(i = 0; i < fd->cpool_count; i++) {
        JS_FreeValue(ctx, fd->cpool[i]);
    }
    js_free(ctx, fd->cpool);
    JS_FreeAtom(ctx, fd->func_name);
    for(i = 0; i < fd->var_count; i++) {
        JS_FreeAtom(ctx, fd->vars[i].var_name);
    }
    js_free(ctx, fd->vars);
    for(i = 0; i < fd->arg_count; i++) {
        JS_FreeAtom(ctx, fd->args[i].var_name);
    }
    js_free(ctx, fd->args);
    for(i = 0; i < fd->global_var_count; i++) {
        JS_FreeAtom(ctx, fd->global_vars[i].var_name);
    }
    js_free(ctx, fd->global_vars);
    for(i = 0; i < fd->closure_var_count; i++) {
        JSClosureVar *cv = &fd->closure_var[i];
        JS_FreeAtom(ctx, cv->var_name);
    }
    js_free(ctx, fd->closure_var);
    if (fd->scopes != fd->def_scope_array)
        js_free(ctx, fd->scopes);
    JS_FreeAtom(ctx, fd->filename);
    dbuf_free(&fd->pc2line);
    js_free(ctx, fd->source);
    if (fd->parent) {
        /* remove in parent list */
        list_del(&fd->link);
    }
    js_free(ctx, fd);
}

#ifdef DUMP_BYTECODE
static const char *skip_lines(const char *p, int n) {
    while (n-- > 0 && *p) {
        while (*p && *p++ != '\n')
            continue;
    }
    return p;
}

static void print_lines(const char *source, int line, int line1) {
    const char *s = source;
    const char *p = skip_lines(s, line);
    if (*p) {
        while (line++ < line1) {
            p = skip_lines(s = p, 1);
            printf(";; %.*s", (int)(p - s), s);
            if (!*p) {
                if (p[-1] != '\n')
                    printf("\n");
                break;
            }
        }
    }
}

static void dump_byte_code(JSContext *ctx, int pass,
                           const uint8_t *tab, int len,
                           const JSVarDef *args, int arg_count,
                           const JSVarDef *vars, int var_count,
                           const JSClosureVar *closure_var, int closure_var_count,
                           const JSValue *cpool, uint32_t cpool_count,
                           const char *source, int line_num,
                           const LabelSlot *label_slots, JSFunctionBytecode *b)
{
    const JSOpCode *oi;
    int pos, pos_next, op, size, idx, addr, line, line1, in_source;
    uint8_t *bits = js_mallocz(ctx, len * sizeof(*bits));
    BOOL use_short_opcodes = (b != NULL);
    /* scan for jump targets */
    for (pos = 0; pos < len; pos = pos_next) {
        op = tab[pos];
        if (use_short_opcodes)
            oi = &short_opcode_info(op);
        else
            oi = &opcode_info[op];
        pos_next = pos + oi->size;
        if (op < OP_COUNT) {
            switch (oi->fmt) {
#if SHORT_OPCODES
            case OP_FMT_label8:
                pos++;
                addr = (int8_t)tab[pos];
                goto has_addr;
            case OP_FMT_label16:
                pos++;
                addr = (int16_t)get_u16(tab + pos);
                goto has_addr;
#endif
            case OP_FMT_atom_label_u8:
            case OP_FMT_atom_label_u16:
                pos += 4;
                /* fall thru */
            case OP_FMT_label:
            case OP_FMT_label_u16:
                pos++;
                addr = get_u32(tab + pos);
                goto has_addr;
            has_addr:
                if (pass == 1)
                    addr = label_slots[addr].pos;
                if (pass == 2)
                    addr = label_slots[addr].pos2;
                if (pass == 3)
                    addr += pos;
                if (addr >= 0 && addr < len)
                    bits[addr] |= 1;
                break;
            }
        }
    }
    in_source = 0;
    if (source) {
        /* Always print first line: needed if single line */
        print_lines(source, 0, 1);
        in_source = 1;
    }
    line1 = line = 1;
    pos = 0;
    while (pos < len) {
        op = tab[pos];
        if (source) {
            if (b) {
                line1 = find_line_num(ctx, b, pos) - line_num + 1;
            } else if (op == OP_line_num) {
                line1 = get_u32(tab + pos + 1) - line_num + 1;
            }
            if (line1 > line) {
                if (!in_source)
                    printf("\n");
                in_source = 1;
                print_lines(source, line, line1);
                line = line1;
                //bits[pos] |= 2;
            }
        }
        if (in_source)
            printf("\n");
        in_source = 0;
        if (op >= OP_COUNT) {
            printf("invalid opcode (0x%02x)\n", op);
            pos++;
            continue;
        }
        if (use_short_opcodes)
            oi = &short_opcode_info(op);
        else
            oi = &opcode_info[op];
        size = oi->size;
        if (pos + size > len) {
            printf("truncated opcode (0x%02x)\n", op);
            break;
        }
#if defined(DUMP_BYTECODE) && (DUMP_BYTECODE & 16)
        {
            int i, x, x0;
            x = x0 = printf("%5d ", pos);
            for (i = 0; i < size; i++) {
                if (i == 6) {
                    printf("\n%*s", x = x0, "");
                }
                x += printf(" %02X", tab[pos + i]);
            }
            printf("%*s", x0 + 20 - x, "");
        }
#endif
        if (bits[pos]) {
            printf("%5d:  ", pos);
        } else {
            printf("        ");
        }
        printf("%s", oi->name);
        pos++;
        switch(oi->fmt) {
        case OP_FMT_none_int:
            printf(" %d", op - OP_push_0);
            break;
        case OP_FMT_npopx:
            printf(" %d", op - OP_call0);
            break;
        case OP_FMT_u8:
            printf(" %u", get_u8(tab + pos));
            break;
        case OP_FMT_i8:
            printf(" %d", get_i8(tab + pos));
            break;
        case OP_FMT_u16:
        case OP_FMT_npop:
            printf(" %u", get_u16(tab + pos));
            break;
        case OP_FMT_npop_u16:
            printf(" %u,%u", get_u16(tab + pos), get_u16(tab + pos + 2));
            break;
        case OP_FMT_i16:
            printf(" %d", get_i16(tab + pos));
            break;
        case OP_FMT_i32:
            printf(" %d", get_i32(tab + pos));
            break;
        case OP_FMT_u32:
            printf(" %u", get_u32(tab + pos));
            break;
#if SHORT_OPCODES
        case OP_FMT_label8:
            addr = get_i8(tab + pos);
            goto has_addr1;
        case OP_FMT_label16:
            addr = get_i16(tab + pos);
            goto has_addr1;
#endif
        case OP_FMT_label:
            addr = get_u32(tab + pos);
            goto has_addr1;
        has_addr1:
            if (pass == 1)
                printf(" %u:%u", addr, label_slots[addr].pos);
            if (pass == 2)
                printf(" %u:%u", addr, label_slots[addr].pos2);
            if (pass == 3)
                printf(" %u", addr + pos);
            break;
        case OP_FMT_label_u16:
            addr = get_u32(tab + pos);
            if (pass == 1)
                printf(" %u:%u", addr, label_slots[addr].pos);
            if (pass == 2)
                printf(" %u:%u", addr, label_slots[addr].pos2);
            if (pass == 3)
                printf(" %u", addr + pos);
            printf(",%u", get_u16(tab + pos + 4));
            break;
#if SHORT_OPCODES
        case OP_FMT_const8:
            idx = get_u8(tab + pos);
            goto has_pool_idx;
#endif
        case OP_FMT_const:
            idx = get_u32(tab + pos);
            goto has_pool_idx;
        has_pool_idx:
            printf(" %u: ", idx);
            if (idx < cpool_count) {
                JS_DumpValue(ctx, cpool[idx]);
            }
            break;
        case OP_FMT_atom:
            printf(" ");
            print_atom(ctx, get_u32(tab + pos));
            break;
        case OP_FMT_atom_u8:
            printf(" ");
            print_atom(ctx, get_u32(tab + pos));
            printf(",%d", get_u8(tab + pos + 4));
            break;
        case OP_FMT_atom_u16:
            printf(" ");
            print_atom(ctx, get_u32(tab + pos));
            printf(",%d", get_u16(tab + pos + 4));
            break;
        case OP_FMT_atom_label_u8:
        case OP_FMT_atom_label_u16:
            printf(" ");
            print_atom(ctx, get_u32(tab + pos));
            addr = get_u32(tab + pos + 4);
            if (pass == 1)
                printf(",%u:%u", addr, label_slots[addr].pos);
            if (pass == 2)
                printf(",%u:%u", addr, label_slots[addr].pos2);
            if (pass == 3)
                printf(",%u", addr + pos + 4);
            if (oi->fmt == OP_FMT_atom_label_u8)
                printf(",%u", get_u8(tab + pos + 8));
            else
                printf(",%u", get_u16(tab + pos + 8));
            break;
        case OP_FMT_none_loc:
            idx = (op - OP_get_loc0) % 4;
            goto has_loc;
        case OP_FMT_loc8:
            idx = get_u8(tab + pos);
            goto has_loc;
        case OP_FMT_loc:
            idx = get_u16(tab + pos);
        has_loc:
            printf(" %d: ", idx);
            if (idx < var_count) {
                print_atom(ctx, vars[idx].var_name);
            }
            break;
        case OP_FMT_none_arg:
            idx = (op - OP_get_arg0) % 4;
            goto has_arg;
        case OP_FMT_arg:
            idx = get_u16(tab + pos);
        has_arg:
            printf(" %d: ", idx);
            if (idx < arg_count) {
                print_atom(ctx, args[idx].var_name);
            }
            break;
        case OP_FMT_none_var_ref:
            idx = (op - OP_get_var_ref0) % 4;
            goto has_var_ref;
        case OP_FMT_var_ref:
            idx = get_u16(tab + pos);
        has_var_ref:
            printf(" %d: ", idx);
            if (idx < closure_var_count) {
                print_atom(ctx, closure_var[idx].var_name);
            }
            break;
        default:
            break;
        }
        printf("\n");
        pos += oi->size - 1;
    }
    if (source) {
        if (!in_source)
            printf("\n");
        print_lines(source, line, INT32_MAX);
    }
    js_free(ctx, bits);
}

static __maybe_unused void dump_pc2line(JSContext *ctx, const uint8_t *buf, int len,
                                                 int line_num)
{
    const uint8_t *p_end, *p_next, *p;
    int pc, v;
    unsigned int op;
    if (len <= 0)
        return;
    printf("%5s %5s\n", "PC", "LINE");
    p = buf;
    p_end = buf + len;
    pc = 0;
    while (p < p_end) {
        op = *p++;
        if (op == 0) {
            v = unicode_from_utf8(p, p_end - p, &p_next);
            if (v < 0)
                goto fail;
            pc += v;
            p = p_next;
            v = unicode_from_utf8(p, p_end - p, &p_next);
            if (v < 0) {
            fail:
                printf("invalid pc2line encode pos=%d\n", (int)(p - buf));
                return;
            }
            if (!(v & 1)) {
                v = v >> 1;
            } else {
                v = -(v >> 1) - 1;
            }
            line_num += v;
            p = p_next;
        } else {
            op -= PC2LINE_OP_FIRST;
            pc += (op / PC2LINE_RANGE);
            line_num += (op % PC2LINE_RANGE) + PC2LINE_BASE;
        }
        printf("%5d %5d\n", pc, line_num);
    }
}

void js_dump_function_bytecode(JSContext *ctx, JSFunctionBytecode *b)
{
    int i;
    char atom_buf[ATOM_GET_STR_BUF_SIZE];
    const char *str;
    if (b->has_debug && b->debug.filename != JS_ATOM_NULL) {
        str = JS_AtomGetStr(ctx, atom_buf, sizeof(atom_buf), b->debug.filename);
        printf("%s:%d: ", str, b->debug.line_num);
    }
    str = JS_AtomGetStr(ctx, atom_buf, sizeof(atom_buf), b->func_name);
    printf("function: %s%s\n", &"*"[b->func_kind != JS_FUNC_GENERATOR], str);
    if (b->js_mode) {
        printf("  mode:");
        if (b->js_mode & JS_MODE_STRICT)
            printf(" strict");
#ifdef CONFIG_BIGNUM
        if (b->js_mode & JS_MODE_MATH)
            printf(" math");
#endif
        printf("\n");
    }
    if (b->arg_count && b->vardefs) {
        printf("  args:");
        for(i = 0; i < b->arg_count; i++) {
            printf(" %s", JS_AtomGetStr(ctx, atom_buf, sizeof(atom_buf),
                                        b->vardefs[i].var_name));
        }
        printf("\n");
    }
    if (b->var_count && b->vardefs) {
        printf("  locals:\n");
        for(i = 0; i < b->var_count; i++) {
            JSVarDef *vd = &b->vardefs[b->arg_count + i];
            printf("%5d: %s %s", i,
                   vd->var_kind == JS_VAR_CATCH ? "catch" :
                   (vd->var_kind == JS_VAR_FUNCTION_DECL ||
                    vd->var_kind == JS_VAR_NEW_FUNCTION_DECL) ? "function" :
                   vd->is_const ? "const" :
                   vd->is_lexical ? "let" : "var",
                   JS_AtomGetStr(ctx, atom_buf, sizeof(atom_buf), vd->var_name));
            if (vd->scope_level)
                printf(" [level:%d next:%d]", vd->scope_level, vd->scope_next);
            printf("\n");
        }
    }
    if (b->closure_var_count) {
        printf("  closure vars:\n");
        for(i = 0; i < b->closure_var_count; i++) {
            JSClosureVar *cv = &b->closure_var[i];
            printf("%5d: %s %s:%s%d %s\n", i,
                   JS_AtomGetStr(ctx, atom_buf, sizeof(atom_buf), cv->var_name),
                   cv->is_local ? "local" : "parent",
                   cv->is_arg ? "arg" : "loc", cv->var_idx,
                   cv->is_const ? "const" :
                   cv->is_lexical ? "let" : "var");
        }
    }
    printf("  stack_size: %d\n", b->stack_size);
    printf("  opcodes:\n");
    dump_byte_code(ctx, 3, b->byte_code_buf, b->byte_code_len,
                   b->vardefs, b->arg_count,
                   b->vardefs ? b->vardefs + b->arg_count : NULL, b->var_count,
                   b->closure_var, b->closure_var_count,
                   b->cpool, b->cpool_count,
                   b->has_debug ? b->debug.source : NULL,
                   b->has_debug ? b->debug.line_num : -1, NULL, b);
#if defined(DUMP_BYTECODE) && (DUMP_BYTECODE & 32)
    if (b->has_debug)
        dump_pc2line(ctx, b->debug.pc2line_buf, b->debug.pc2line_len, b->debug.line_num);
#endif
    printf("\n");
}
#endif

int add_closure_var(JSContext *ctx, JSFunctionDef *s,
                    BOOL is_local, BOOL is_arg,
                    int var_idx, JSAtom var_name,
                    BOOL is_const, BOOL is_lexical,
                    JSVarKindEnum var_kind)
{
    JSClosureVar *cv;
    /* the closure variable indexes are currently stored on 16 bits */
    if (s->closure_var_count >= JS_MAX_LOCAL_VARS) {
        JS_ThrowInternalError(ctx, "too many closure variables");
        return -1;
    }
    if (js_resize_array(ctx, (void **)&s->closure_var,
                        sizeof(s->closure_var[0]),
                        &s->closure_var_size, s->closure_var_count + 1))
        return -1;
    cv = &s->closure_var[s->closure_var_count++];
    cv->is_local = is_local;
    cv->is_arg = is_arg;
    cv->is_const = is_const;
    cv->is_lexical = is_lexical;
    cv->var_kind = var_kind;
    cv->var_idx = var_idx;
    cv->var_name = JS_DupAtom(ctx, var_name);
    return s->closure_var_count - 1;
}

static int find_closure_var(JSContext *ctx, JSFunctionDef *s,
                            JSAtom var_name)
{
    int i;
    for(i = 0; i < s->closure_var_count; i++) {
        JSClosureVar *cv = &s->closure_var[i];
        if (cv->var_name == var_name)
            return i;
    }
    return -1;
}

/* 'fd' must be a parent of 's'. Create in 's' a closure referencing a
   local variable (is_local = TRUE) or a closure (is_local = FALSE) in
   'fd' */
static int get_closure_var2(JSContext *ctx, JSFunctionDef *s,
                            JSFunctionDef *fd, BOOL is_local,
                            BOOL is_arg, int var_idx, JSAtom var_name,
                            BOOL is_const, BOOL is_lexical,
                            JSVarKindEnum var_kind)
{
    int i;
    if (fd != s->parent) {
        var_idx = get_closure_var2(ctx, s->parent, fd, is_local,
                                   is_arg, var_idx, var_name,
                                   is_const, is_lexical, var_kind);
        if (var_idx < 0)
            return -1;
        is_local = FALSE;
    }
    for(i = 0; i < s->closure_var_count; i++) {
        JSClosureVar *cv = &s->closure_var[i];
        if (cv->var_idx == var_idx && cv->is_arg == is_arg &&
            cv->is_local == is_local)
            return i;
    }
    return add_closure_var(ctx, s, is_local, is_arg, var_idx, var_name,
                           is_const, is_lexical, var_kind);
}

static int get_closure_var(JSContext *ctx, JSFunctionDef *s,
                           JSFunctionDef *fd, BOOL is_arg,
                           int var_idx, JSAtom var_name,
                           BOOL is_const, BOOL is_lexical,
                           JSVarKindEnum var_kind)
{
    return get_closure_var2(ctx, s, fd, TRUE, is_arg,
                            var_idx, var_name, is_const, is_lexical,
                            var_kind);
}

static int get_with_scope_opcode(int op)
{
    if (op == OP_scope_get_var_undef)
        return OP_with_get_var;
    else
        return OP_with_get_var + (op - OP_scope_get_var);
}

static BOOL can_opt_put_ref_value(const uint8_t *bc_buf, int pos)
{
    int opcode = bc_buf[pos];
    return (bc_buf[pos + 1] == OP_put_ref_value &&
            (opcode == OP_insert3 ||
             opcode == OP_perm4 ||
             opcode == OP_nop ||
             opcode == OP_rot3l));
}

static BOOL can_opt_put_global_ref_value(const uint8_t *bc_buf, int pos)
{
    int opcode = bc_buf[pos];
    return (bc_buf[pos + 1] == OP_put_ref_value &&
            (opcode == OP_insert3 ||
             opcode == OP_perm4 ||
             opcode == OP_nop ||
             opcode == OP_rot3l));
}

static int optimize_scope_make_ref(JSContext *ctx, JSFunctionDef *s,
                                   DynBuf *bc, uint8_t *bc_buf,
                                   LabelSlot *ls, int pos_next,
                                   int get_op, int var_idx)
{
    int label_pos, end_pos, pos;
    /* XXX: should optimize `loc(a) += expr` as `expr add_loc(a)`
       but only if expr does not modify `a`.
       should scan the code between pos_next and label_pos
       for operations that can potentially change `a`:
       OP_scope_make_ref(a), function calls, jumps and gosub.
     */
    /* replace the reference get/put with normal variable
       accesses */
    if (bc_buf[pos_next] == OP_get_ref_value) {
        dbuf_putc(bc, get_op);
        dbuf_put_u16(bc, var_idx);
        pos_next++;
    }
    /* remove the OP_label to make room for replacement */
    /* label should have a refcount of 0 anyway */
    /* XXX: should avoid this patch by inserting nops in phase 1 */
    label_pos = ls->pos;
    pos = label_pos - 5;
    assert(bc_buf[pos] == OP_label);
    /* label points to an instruction pair:
       - insert3 / put_ref_value
       - perm4 / put_ref_value
       - rot3l / put_ref_value
       - nop / put_ref_value
     */
    end_pos = label_pos + 2;
    if (bc_buf[label_pos] == OP_insert3)
        bc_buf[pos++] = OP_dup;
    bc_buf[pos] = get_op + 1;
    put_u16(bc_buf + pos + 1, var_idx);
    pos += 3;
    /* pad with OP_nop */
    while (pos < end_pos)
        bc_buf[pos++] = OP_nop;
    return pos_next;
}

static int optimize_scope_make_global_ref(JSContext *ctx, JSFunctionDef *s,
                                          DynBuf *bc, uint8_t *bc_buf,
                                          LabelSlot *ls, int pos_next,
                                          JSAtom var_name)
{
    int label_pos, end_pos, pos, op;
    BOOL is_strict;
    is_strict = ((s->js_mode & JS_MODE_STRICT) != 0);
    /* replace the reference get/put with normal variable
       accesses */
    if (is_strict) {
        /* need to check if the variable exists before evaluating the right
           expression */
        /* XXX: need an extra OP_true if destructuring an array */
        dbuf_putc(bc, OP_check_var);
        dbuf_put_u32(bc, JS_DupAtom(ctx, var_name));
    } else {
        /* XXX: need 2 extra OP_true if destructuring an array */
    }
    if (bc_buf[pos_next] == OP_get_ref_value) {
        dbuf_putc(bc, OP_get_var);
        dbuf_put_u32(bc, JS_DupAtom(ctx, var_name));
        pos_next++;
    }
    /* remove the OP_label to make room for replacement */
    /* label should have a refcount of 0 anyway */
    /* XXX: should have emitted several OP_nop to avoid this kludge */
    label_pos = ls->pos;
    pos = label_pos - 5;
    assert(bc_buf[pos] == OP_label);
    end_pos = label_pos + 2;
    op = bc_buf[label_pos];
    if (is_strict) {
        if (op != OP_nop) {
            switch(op) {
            case OP_insert3:
                op = OP_insert2;
                break;
            case OP_perm4:
                op = OP_perm3;
                break;
            case OP_rot3l:
                op = OP_swap;
                break;
            default:
                abort();
            }
            bc_buf[pos++] = op;
        }
    } else {
        if (op == OP_insert3)
            bc_buf[pos++] = OP_dup;
    }
    if (is_strict) {
        bc_buf[pos] = OP_put_var_strict;
        /* XXX: need 1 extra OP_drop if destructuring an array */
    } else {
        bc_buf[pos] = OP_put_var;
        /* XXX: need 2 extra OP_drop if destructuring an array */
    }
    put_u32(bc_buf + pos + 1, JS_DupAtom(ctx, var_name));
    pos += 5;
    /* pad with OP_nop */
    while (pos < end_pos)
        bc_buf[pos++] = OP_nop;
    return pos_next;
}

static int add_var_this(JSContext *ctx, JSFunctionDef *fd)
{
    int idx;
    idx = add_var(ctx, fd, JS_ATOM_this);
    if (idx >= 0 && fd->is_derived_class_constructor) {
        JSVarDef *vd = &fd->vars[idx];
        /* XXX: should have is_this flag or var type */
        vd->is_lexical = 1; /* used to trigger 'uninitialized' checks
                               in a derived class constructor */
    }
    return idx;
}

static int resolve_pseudo_var(JSContext *ctx, JSFunctionDef *s,
                               JSAtom var_name)
{
    int var_idx;
    if (!s->has_this_binding)
        return -1;
    switch(var_name) {
    case JS_ATOM_home_object:
        /* 'home_object' pseudo variable */
        if (s->home_object_var_idx < 0)
            s->home_object_var_idx = add_var(ctx, s, var_name);
        var_idx = s->home_object_var_idx;
        break;
    case JS_ATOM_this_active_func:
        /* 'this.active_func' pseudo variable */
        if (s->this_active_func_var_idx < 0)
            s->this_active_func_var_idx = add_var(ctx, s, var_name);
        var_idx = s->this_active_func_var_idx;
        break;
    case JS_ATOM_new_target:
        /* 'new.target' pseudo variable */
        if (s->new_target_var_idx < 0)
            s->new_target_var_idx = add_var(ctx, s, var_name);
        var_idx = s->new_target_var_idx;
        break;
    case JS_ATOM_this:
        /* 'this' pseudo variable */
        if (s->this_var_idx < 0)
            s->this_var_idx = add_var_this(ctx, s);
        var_idx = s->this_var_idx;
        break;
    default:
        var_idx = -1;
        break;
    }
    return var_idx;
}

/* test if 'var_name' is in the variable object on the stack. If is it
   the case, handle it and jump to 'label_done' */
static void var_object_test(JSContext *ctx, JSFunctionDef *s,
                            JSAtom var_name, int op, DynBuf *bc,
                            int *plabel_done, BOOL is_with)
{
    dbuf_putc(bc, get_with_scope_opcode(op));
    dbuf_put_u32(bc, JS_DupAtom(ctx, var_name));
    *plabel_done = new_label_fd(s, *plabel_done);
    dbuf_put_u32(bc, *plabel_done);
    dbuf_putc(bc, is_with);
    update_label(s, *plabel_done, 1);
    s->jump_size++;
}

/* return the position of the next opcode */
static int resolve_scope_var(JSContext *ctx, JSFunctionDef *s,
                             JSAtom var_name, int scope_level, int op,
                             DynBuf *bc, uint8_t *bc_buf,
                             LabelSlot *ls, int pos_next)
{
    int idx, var_idx, is_put;
    int label_done;
    JSFunctionDef *fd;
    JSVarDef *vd;
    BOOL is_pseudo_var, is_arg_scope;
    label_done = -1;
    /* XXX: could be simpler to use a specific function to
       resolve the pseudo variables */
    is_pseudo_var = (var_name == JS_ATOM_home_object ||
                     var_name == JS_ATOM_this_active_func ||
                     var_name == JS_ATOM_new_target ||
                     var_name == JS_ATOM_this);
    /* resolve local scoped variables */
    var_idx = -1;
    for (idx = s->scopes[scope_level].first; idx >= 0;) {
        vd = &s->vars[idx];
        if (vd->var_name == var_name) {
            if (op == OP_scope_put_var || op == OP_scope_make_ref) {
                if (vd->is_const) {
                    dbuf_putc(bc, OP_throw_error);
                    dbuf_put_u32(bc, JS_DupAtom(ctx, var_name));
                    dbuf_putc(bc, JS_THROW_VAR_RO);
                    goto done;
                }
            }
            var_idx = idx;
            break;
        } else
        if (vd->var_name == JS_ATOM__with_ && !is_pseudo_var) {
            dbuf_putc(bc, OP_get_loc);
            dbuf_put_u16(bc, idx);
            var_object_test(ctx, s, var_name, op, bc, &label_done, 1);
        }
        idx = vd->scope_next;
    }
    is_arg_scope = (idx == ARG_SCOPE_END);
    if (var_idx < 0) {
        /* argument scope: variables are not visible but pseudo
           variables are visible */
        if (!is_arg_scope) {
            var_idx = find_var(ctx, s, var_name);
        }
        if (var_idx < 0 && is_pseudo_var)
            var_idx = resolve_pseudo_var(ctx, s, var_name);
        if (var_idx < 0 && var_name == JS_ATOM_arguments &&
            s->has_arguments_binding) {
            /* 'arguments' pseudo variable */
            var_idx = add_arguments_var(ctx, s);
        }
        if (var_idx < 0 && s->is_func_expr && var_name == s->func_name) {
            /* add a new variable with the function name */
            var_idx = add_func_var(ctx, s, var_name);
        }
    }
    if (var_idx >= 0) {
        if ((op == OP_scope_put_var || op == OP_scope_make_ref) &&
            !(var_idx & ARGUMENT_VAR_OFFSET) &&
            s->vars[var_idx].is_const) {
            /* only happens when assigning a function expression name
               in strict mode */
            dbuf_putc(bc, OP_throw_error);
            dbuf_put_u32(bc, JS_DupAtom(ctx, var_name));
            dbuf_putc(bc, JS_THROW_VAR_RO);
            goto done;
        }
        /* OP_scope_put_var_init is only used to initialize a
           lexical variable, so it is never used in a with or var object. It
           can be used with a closure (module global variable case). */
        switch (op) {
        case OP_scope_make_ref:
            if (!(var_idx & ARGUMENT_VAR_OFFSET) &&
                s->vars[var_idx].var_kind == JS_VAR_FUNCTION_NAME) {
                /* Create a dummy object reference for the func_var */
                dbuf_putc(bc, OP_object);
                dbuf_putc(bc, OP_get_loc);
                dbuf_put_u16(bc, var_idx);
                dbuf_putc(bc, OP_define_field);
                dbuf_put_u32(bc, JS_DupAtom(ctx, var_name));
                dbuf_putc(bc, OP_push_atom_value);
                dbuf_put_u32(bc, JS_DupAtom(ctx, var_name));
            } else
            if (label_done == -1 && can_opt_put_ref_value(bc_buf, ls->pos)) {
                int get_op;
                if (var_idx & ARGUMENT_VAR_OFFSET) {
                    get_op = OP_get_arg;
                    var_idx -= ARGUMENT_VAR_OFFSET;
                } else {
                    if (s->vars[var_idx].is_lexical)
                        get_op = OP_get_loc_check;
                    else
                        get_op = OP_get_loc;
                }
                pos_next = optimize_scope_make_ref(ctx, s, bc, bc_buf, ls,
                                                   pos_next, get_op, var_idx);
            } else {
                /* Create a dummy object with a named slot that is
                   a reference to the local variable */
                if (var_idx & ARGUMENT_VAR_OFFSET) {
                    dbuf_putc(bc, OP_make_arg_ref);
                    dbuf_put_u32(bc, JS_DupAtom(ctx, var_name));
                    dbuf_put_u16(bc, var_idx - ARGUMENT_VAR_OFFSET);
                } else {
                    dbuf_putc(bc, OP_make_loc_ref);
                    dbuf_put_u32(bc, JS_DupAtom(ctx, var_name));
                    dbuf_put_u16(bc, var_idx);
                }
            }
            break;
        case OP_scope_get_ref:
            dbuf_putc(bc, OP_undefined);
            /* fall thru */
        case OP_scope_get_var_undef:
        case OP_scope_get_var:
        case OP_scope_put_var:
        case OP_scope_put_var_init:
            is_put = (op == OP_scope_put_var || op == OP_scope_put_var_init);
            if (var_idx & ARGUMENT_VAR_OFFSET) {
                dbuf_putc(bc, OP_get_arg + is_put);
                dbuf_put_u16(bc, var_idx - ARGUMENT_VAR_OFFSET);
            } else {
                if (is_put) {
                    if (s->vars[var_idx].is_lexical) {
                        if (op == OP_scope_put_var_init) {
                            /* 'this' can only be initialized once */
                            if (var_name == JS_ATOM_this)
                                dbuf_putc(bc, OP_put_loc_check_init);
                            else
                                dbuf_putc(bc, OP_put_loc);
                        } else {
                            dbuf_putc(bc, OP_put_loc_check);
                        }
                    } else {
                        dbuf_putc(bc, OP_put_loc);
                    }
                } else {
                    if (s->vars[var_idx].is_lexical) {
                        dbuf_putc(bc, OP_get_loc_check);
                    } else {
                        dbuf_putc(bc, OP_get_loc);
                    }
                }
                dbuf_put_u16(bc, var_idx);
            }
            break;
        case OP_scope_delete_var:
            dbuf_putc(bc, OP_push_false);
            break;
        }
        goto done;
    }
    /* check eval object */
    if (!is_arg_scope && s->var_object_idx >= 0 && !is_pseudo_var) {
        dbuf_putc(bc, OP_get_loc);
        dbuf_put_u16(bc, s->var_object_idx);
        var_object_test(ctx, s, var_name, op, bc, &label_done, 0);
    }
    /* check eval object in argument scope */
    if (s->arg_var_object_idx >= 0 && !is_pseudo_var) {
        dbuf_putc(bc, OP_get_loc);
        dbuf_put_u16(bc, s->arg_var_object_idx);
        var_object_test(ctx, s, var_name, op, bc, &label_done, 0);
    }
    /* check parent scopes */
    for (fd = s; fd->parent;) {
        scope_level = fd->parent_scope_level;
        fd = fd->parent;
        for (idx = fd->scopes[scope_level].first; idx >= 0;) {
            vd = &fd->vars[idx];
            if (vd->var_name == var_name) {
                if (op == OP_scope_put_var || op == OP_scope_make_ref) {
                    if (vd->is_const) {
                        dbuf_putc(bc, OP_throw_error);
                        dbuf_put_u32(bc, JS_DupAtom(ctx, var_name));
                        dbuf_putc(bc, JS_THROW_VAR_RO);
                        goto done;
                    }
                }
                var_idx = idx;
                break;
            } else if (vd->var_name == JS_ATOM__with_ && !is_pseudo_var) {
                vd->is_captured = 1;
                idx = get_closure_var(ctx, s, fd, FALSE, idx, vd->var_name, FALSE, FALSE, JS_VAR_NORMAL);
                if (idx >= 0) {
                    dbuf_putc(bc, OP_get_var_ref);
                    dbuf_put_u16(bc, idx);
                    var_object_test(ctx, s, var_name, op, bc, &label_done, 1);
                }
            }
            idx = vd->scope_next;
        }
        is_arg_scope = (idx == ARG_SCOPE_END);
        if (var_idx >= 0)
            break;
        if (!is_arg_scope) {
            var_idx = find_var(ctx, fd, var_name);
            if (var_idx >= 0)
                break;
        }
        if (is_pseudo_var) {
            var_idx = resolve_pseudo_var(ctx, fd, var_name);
            if (var_idx >= 0)
                break;
        }
        if (var_name == JS_ATOM_arguments && fd->has_arguments_binding) {
            var_idx = add_arguments_var(ctx, fd);
            break;
        }
        if (fd->is_func_expr && fd->func_name == var_name) {
            /* add a new variable with the function name */
            var_idx = add_func_var(ctx, fd, var_name);
            break;
        }
        /* check eval object */
        if (!is_arg_scope && fd->var_object_idx >= 0 && !is_pseudo_var) {
            vd = &fd->vars[fd->var_object_idx];
            vd->is_captured = 1;
            idx = get_closure_var(ctx, s, fd, FALSE,
                                  fd->var_object_idx, vd->var_name,
                                  FALSE, FALSE, JS_VAR_NORMAL);
            dbuf_putc(bc, OP_get_var_ref);
            dbuf_put_u16(bc, idx);
            var_object_test(ctx, s, var_name, op, bc, &label_done, 0);
        }
        /* check eval object in argument scope */
        if (fd->arg_var_object_idx >= 0 && !is_pseudo_var) {
            vd = &fd->vars[fd->arg_var_object_idx];
            vd->is_captured = 1;
            idx = get_closure_var(ctx, s, fd, FALSE,
                                  fd->arg_var_object_idx, vd->var_name,
                                  FALSE, FALSE, JS_VAR_NORMAL);
            dbuf_putc(bc, OP_get_var_ref);
            dbuf_put_u16(bc, idx);
            var_object_test(ctx, s, var_name, op, bc, &label_done, 0);
        }
        if (fd->is_eval)
            break; /* it it necessarily the top level function */
    }
    /* check direct eval scope (in the closure of the eval function
       which is necessarily at the top level) */
    if (!fd)
        fd = s;
    if (var_idx < 0 && fd->is_eval) {
        int idx1;
        for (idx1 = 0; idx1 < fd->closure_var_count; idx1++) {
            JSClosureVar *cv = &fd->closure_var[idx1];
            if (var_name == cv->var_name) {
                if (fd != s) {
                    idx = get_closure_var2(ctx, s, fd,
                                           FALSE,
                                           cv->is_arg, idx1,
                                           cv->var_name, cv->is_const,
                                           cv->is_lexical, cv->var_kind);
                } else {
                    idx = idx1;
                }
                goto has_idx;
            } else if ((cv->var_name == JS_ATOM__var_ ||
                        cv->var_name == JS_ATOM__arg_var_ ||
                        cv->var_name == JS_ATOM__with_) && !is_pseudo_var) {
                int is_with = (cv->var_name == JS_ATOM__with_);
                if (fd != s) {
                    idx = get_closure_var2(ctx, s, fd,
                                           FALSE,
                                           cv->is_arg, idx1,
                                           cv->var_name, FALSE, FALSE,
                                           JS_VAR_NORMAL);
                } else {
                    idx = idx1;
                }
                dbuf_putc(bc, OP_get_var_ref);
                dbuf_put_u16(bc, idx);
                var_object_test(ctx, s, var_name, op, bc, &label_done, is_with);
            }
        }
    }
    if (var_idx >= 0) {
        /* find the corresponding closure variable */
        if (var_idx & ARGUMENT_VAR_OFFSET) {
            fd->args[var_idx - ARGUMENT_VAR_OFFSET].is_captured = 1;
            idx = get_closure_var(ctx, s, fd,
                                  TRUE, var_idx - ARGUMENT_VAR_OFFSET,
                                  var_name, FALSE, FALSE, JS_VAR_NORMAL);
        } else {
            fd->vars[var_idx].is_captured = 1;
            idx = get_closure_var(ctx, s, fd,
                                  FALSE, var_idx,
                                  var_name,
                                  fd->vars[var_idx].is_const,
                                  fd->vars[var_idx].is_lexical,
                                  fd->vars[var_idx].var_kind);
        }
        if (idx >= 0) {
        has_idx:
            if ((op == OP_scope_put_var || op == OP_scope_make_ref) &&
                s->closure_var[idx].is_const) {
                dbuf_putc(bc, OP_throw_error);
                dbuf_put_u32(bc, JS_DupAtom(ctx, var_name));
                dbuf_putc(bc, JS_THROW_VAR_RO);
                goto done;
            }
            switch (op) {
            case OP_scope_make_ref:
                if (s->closure_var[idx].var_kind == JS_VAR_FUNCTION_NAME) {
                    /* Create a dummy object reference for the func_var */
                    dbuf_putc(bc, OP_object);
                    dbuf_putc(bc, OP_get_var_ref);
                    dbuf_put_u16(bc, idx);
                    dbuf_putc(bc, OP_define_field);
                    dbuf_put_u32(bc, JS_DupAtom(ctx, var_name));
                    dbuf_putc(bc, OP_push_atom_value);
                    dbuf_put_u32(bc, JS_DupAtom(ctx, var_name));
                } else
                if (label_done == -1 &&
                    can_opt_put_ref_value(bc_buf, ls->pos)) {
                    int get_op;
                    if (s->closure_var[idx].is_lexical)
                        get_op = OP_get_var_ref_check;
                    else
                        get_op = OP_get_var_ref;
                    pos_next = optimize_scope_make_ref(ctx, s, bc, bc_buf, ls,
                                                       pos_next,
                                                       get_op, idx);
                } else {
                    /* Create a dummy object with a named slot that is
                       a reference to the closure variable */
                    dbuf_putc(bc, OP_make_var_ref_ref);
                    dbuf_put_u32(bc, JS_DupAtom(ctx, var_name));
                    dbuf_put_u16(bc, idx);
                }
                break;
            case OP_scope_get_ref:
                /* XXX: should create a dummy object with a named slot that is
                   a reference to the closure variable */
                dbuf_putc(bc, OP_undefined);
                /* fall thru */
            case OP_scope_get_var_undef:
            case OP_scope_get_var:
            case OP_scope_put_var:
            case OP_scope_put_var_init:
                is_put = (op == OP_scope_put_var ||
                          op == OP_scope_put_var_init);
                if (is_put) {
                    if (s->closure_var[idx].is_lexical) {
                        if (op == OP_scope_put_var_init) {
                            /* 'this' can only be initialized once */
                            if (var_name == JS_ATOM_this)
                                dbuf_putc(bc, OP_put_var_ref_check_init);
                            else
                                dbuf_putc(bc, OP_put_var_ref);
                        } else {
                            dbuf_putc(bc, OP_put_var_ref_check);
                        }
                    } else {
                        dbuf_putc(bc, OP_put_var_ref);
                    }
                } else {
                    if (s->closure_var[idx].is_lexical) {
                        dbuf_putc(bc, OP_get_var_ref_check);
                    } else {
                        dbuf_putc(bc, OP_get_var_ref);
                    }
                }
                dbuf_put_u16(bc, idx);
                break;
            case OP_scope_delete_var:
                dbuf_putc(bc, OP_push_false);
                break;
            }
            goto done;
        }
    }
    /* global variable access */
    switch (op) {
    case OP_scope_make_ref:
        if (label_done == -1 && can_opt_put_global_ref_value(bc_buf, ls->pos)) {
            pos_next = optimize_scope_make_global_ref(ctx, s, bc, bc_buf, ls,
                                                      pos_next, var_name);
        } else {
            dbuf_putc(bc, OP_make_var_ref);
            dbuf_put_u32(bc, JS_DupAtom(ctx, var_name));
        }
        break;
    case OP_scope_get_ref:
        /* XXX: should create a dummy object with a named slot that is
           a reference to the global variable */
        dbuf_putc(bc, OP_undefined);
        dbuf_putc(bc, OP_get_var);
        dbuf_put_u32(bc, JS_DupAtom(ctx, var_name));
        break;
    case OP_scope_get_var_undef:
    case OP_scope_get_var:
    case OP_scope_put_var:
        dbuf_putc(bc, OP_get_var_undef + (op - OP_scope_get_var_undef));
        dbuf_put_u32(bc, JS_DupAtom(ctx, var_name));
        break;
    case OP_scope_put_var_init:
        dbuf_putc(bc, OP_put_var_init);
        dbuf_put_u32(bc, JS_DupAtom(ctx, var_name));
        break;
    case OP_scope_delete_var:
        dbuf_putc(bc, OP_delete_var);
        dbuf_put_u32(bc, JS_DupAtom(ctx, var_name));
        break;
    }
done:
    if (label_done >= 0) {
        dbuf_putc(bc, OP_label);
        dbuf_put_u32(bc, label_done);
        s->label_slots[label_done].pos2 = bc->size;
    }
    return pos_next;
}

/* search in all scopes */
static int find_private_class_field_all(JSContext *ctx, JSFunctionDef *fd,
                                        JSAtom name, int scope_level)
{
    int idx;
    idx = fd->scopes[scope_level].first;
    while (idx >= 0) {
        if (fd->vars[idx].var_name == name)
            return idx;
        idx = fd->vars[idx].scope_next;
    }
    return -1;
}

static void get_loc_or_ref(DynBuf *bc, BOOL is_ref, int idx)
{
    /* if the field is not initialized, the error is catched when
       accessing it */
    if (is_ref)
        dbuf_putc(bc, OP_get_var_ref);
    else
        dbuf_putc(bc, OP_get_loc);
    dbuf_put_u16(bc, idx);
}

static int resolve_scope_private_field1(JSContext *ctx,
                                        BOOL *pis_ref, int *pvar_kind,
                                        JSFunctionDef *s,
                                        JSAtom var_name, int scope_level)
{
    int idx, var_kind;
    JSFunctionDef *fd;
    BOOL is_ref;
    fd = s;
    is_ref = FALSE;
    for(;;) {
        idx = find_private_class_field_all(ctx, fd, var_name, scope_level);
        if (idx >= 0) {
            var_kind = fd->vars[idx].var_kind;
            if (is_ref) {
                idx = get_closure_var(ctx, s, fd, FALSE, idx, var_name,
                                      TRUE, TRUE, JS_VAR_NORMAL);
                if (idx < 0)
                    return -1;
            }
            break;
        }
        scope_level = fd->parent_scope_level;
        if (!fd->parent) {
            if (fd->is_eval) {
                /* closure of the eval function (top level) */
                for (idx = 0; idx < fd->closure_var_count; idx++) {
                    JSClosureVar *cv = &fd->closure_var[idx];
                    if (cv->var_name == var_name) {
                        var_kind = cv->var_kind;
                        is_ref = TRUE;
                        if (fd != s) {
                            idx = get_closure_var2(ctx, s, fd,
                                                   FALSE,
                                                   cv->is_arg, idx,
                                                   cv->var_name, cv->is_const,
                                                   cv->is_lexical,
                                                   cv->var_kind);
                            if (idx < 0)
                                return -1;
                        }
                        goto done;
                    }
                }
            }
            /* XXX: no line number info */
            JS_ThrowSyntaxErrorAtom(ctx, "undefined private field '%s'",
                                    var_name);
            return -1;
        } else {
            fd = fd->parent;
        }
        is_ref = TRUE;
    }
 done:
    *pis_ref = is_ref;
    *pvar_kind = var_kind;
    return idx;
}

/* return 0 if OK or -1 if the private field could not be resolved */
static int resolve_scope_private_field(JSContext *ctx, JSFunctionDef *s,
                                       JSAtom var_name, int scope_level, int op,
                                       DynBuf *bc)
{
    int idx, var_kind;
    BOOL is_ref;
    idx = resolve_scope_private_field1(ctx, &is_ref, &var_kind, s,
                                       var_name, scope_level);
    if (idx < 0)
        return -1;
    assert(var_kind != JS_VAR_NORMAL);
    switch (op) {
    case OP_scope_get_private_field:
    case OP_scope_get_private_field2:
        switch(var_kind) {
        case JS_VAR_PRIVATE_FIELD:
            if (op == OP_scope_get_private_field2)
                dbuf_putc(bc, OP_dup);
            get_loc_or_ref(bc, is_ref, idx);
            dbuf_putc(bc, OP_get_private_field);
            break;
        case JS_VAR_PRIVATE_METHOD:
            get_loc_or_ref(bc, is_ref, idx);
            dbuf_putc(bc, OP_check_brand);
            if (op != OP_scope_get_private_field2)
                dbuf_putc(bc, OP_nip);
            break;
        case JS_VAR_PRIVATE_GETTER:
        case JS_VAR_PRIVATE_GETTER_SETTER:
            if (op == OP_scope_get_private_field2)
                dbuf_putc(bc, OP_dup);
            get_loc_or_ref(bc, is_ref, idx);
            dbuf_putc(bc, OP_check_brand);
            dbuf_putc(bc, OP_call_method);
            dbuf_put_u16(bc, 0);
            break;
        case JS_VAR_PRIVATE_SETTER:
            /* XXX: add clearer error message */
            dbuf_putc(bc, OP_throw_error);
            dbuf_put_u32(bc, JS_DupAtom(ctx, var_name));
            dbuf_putc(bc, JS_THROW_VAR_RO);
            break;
        default:
            abort();
        }
        break;
    case OP_scope_put_private_field:
        switch(var_kind) {
        case JS_VAR_PRIVATE_FIELD:
            get_loc_or_ref(bc, is_ref, idx);
            dbuf_putc(bc, OP_put_private_field);
            break;
        case JS_VAR_PRIVATE_METHOD:
        case JS_VAR_PRIVATE_GETTER:
            /* XXX: add clearer error message */
            dbuf_putc(bc, OP_throw_error);
            dbuf_put_u32(bc, JS_DupAtom(ctx, var_name));
            dbuf_putc(bc, JS_THROW_VAR_RO);
            break;
        case JS_VAR_PRIVATE_SETTER:
        case JS_VAR_PRIVATE_GETTER_SETTER:
            {
                JSAtom setter_name = get_private_setter_name(ctx, var_name);
                if (setter_name == JS_ATOM_NULL)
                    return -1;
                idx = resolve_scope_private_field1(ctx, &is_ref,
                                                   &var_kind, s,
                                                   setter_name, scope_level);
                JS_FreeAtom(ctx, setter_name);
                if (idx < 0)
                    return -1;
                assert(var_kind == JS_VAR_PRIVATE_SETTER);
                get_loc_or_ref(bc, is_ref, idx);
                dbuf_putc(bc, OP_swap);
                /* obj func value */
                dbuf_putc(bc, OP_rot3r);
                /* value obj func */
                dbuf_putc(bc, OP_check_brand);
                dbuf_putc(bc, OP_rot3l);
                /* obj func value */
                dbuf_putc(bc, OP_call_method);
                dbuf_put_u16(bc, 1);
            }
            break;
        default:
            abort();
        }
        break;
    default:
        abort();
    }
    return 0;
}

static void mark_eval_captured_variables(JSContext *ctx, JSFunctionDef *s,
                                         int scope_level)
{
    int idx;
    JSVarDef *vd;
    for (idx = s->scopes[scope_level].first; idx >= 0;) {
        vd = &s->vars[idx];
        vd->is_captured = 1;
        idx = vd->scope_next;
    }
}

/* XXX: should handle the argument scope generically */
static BOOL is_var_in_arg_scope(const JSVarDef *vd)
{
    return (vd->var_name == JS_ATOM_home_object ||
            vd->var_name == JS_ATOM_this_active_func ||
            vd->var_name == JS_ATOM_new_target ||
            vd->var_name == JS_ATOM_this ||
            vd->var_name == JS_ATOM__arg_var_ ||
            vd->var_kind == JS_VAR_FUNCTION_NAME);
}

static void add_eval_variables(JSContext *ctx, JSFunctionDef *s)
{
    JSFunctionDef *fd;
    JSVarDef *vd;
    int i, scope_level, scope_idx;
    BOOL has_arguments_binding, has_this_binding, is_arg_scope;
    /* in non strict mode, variables are created in the caller's
       environment object */
    if (!s->is_eval && !(s->js_mode & JS_MODE_STRICT)) {
        s->var_object_idx = add_var(ctx, s, JS_ATOM__var_);
        if (s->has_parameter_expressions) {
            /* an additional variable object is needed for the
               argument scope */
            s->arg_var_object_idx = add_var(ctx, s, JS_ATOM__arg_var_);
        }
    }
    /* eval can potentially use 'arguments' so we must define it */
    has_this_binding = s->has_this_binding;
    if (has_this_binding) {
        if (s->this_var_idx < 0)
            s->this_var_idx = add_var_this(ctx, s);
        if (s->new_target_var_idx < 0)
            s->new_target_var_idx = add_var(ctx, s, JS_ATOM_new_target);
        if (s->is_derived_class_constructor && s->this_active_func_var_idx < 0)
            s->this_active_func_var_idx = add_var(ctx, s, JS_ATOM_this_active_func);
        if (s->has_home_object && s->home_object_var_idx < 0)
            s->home_object_var_idx = add_var(ctx, s, JS_ATOM_home_object);
    }
    has_arguments_binding = s->has_arguments_binding;
    if (has_arguments_binding) {
        add_arguments_var(ctx, s);
        /* also add an arguments binding in the argument scope to
           raise an error if a direct eval in the argument scope tries
           to redefine it */
        if (s->has_parameter_expressions && !(s->js_mode & JS_MODE_STRICT))
            add_arguments_arg(ctx, s);
    }
    if (s->is_func_expr && s->func_name != JS_ATOM_NULL)
        add_func_var(ctx, s, s->func_name);
    /* eval can use all the variables of the enclosing functions, so
       they must be all put in the closure. The closure variables are
       ordered by scope. It works only because no closure are created
       before. */
    assert(s->is_eval || s->closure_var_count == 0);
    /* XXX: inefficient, but eval performance is less critical */
    fd = s;
    for(;;) {
        scope_level = fd->parent_scope_level;
        fd = fd->parent;
        if (!fd)
            break;
        /* add 'this' if it was not previously added */
        if (!has_this_binding && fd->has_this_binding) {
            if (fd->this_var_idx < 0)
                fd->this_var_idx = add_var_this(ctx, fd);
            if (fd->new_target_var_idx < 0)
                fd->new_target_var_idx = add_var(ctx, fd, JS_ATOM_new_target);
            if (fd->is_derived_class_constructor && fd->this_active_func_var_idx < 0)
                fd->this_active_func_var_idx = add_var(ctx, fd, JS_ATOM_this_active_func);
            if (fd->has_home_object && fd->home_object_var_idx < 0)
                fd->home_object_var_idx = add_var(ctx, fd, JS_ATOM_home_object);
            has_this_binding = TRUE;
        }
        /* add 'arguments' if it was not previously added */
        if (!has_arguments_binding && fd->has_arguments_binding) {
            add_arguments_var(ctx, fd);
            has_arguments_binding = TRUE;
        }
        /* add function name */
        if (fd->is_func_expr && fd->func_name != JS_ATOM_NULL)
            add_func_var(ctx, fd, fd->func_name);
        /* add lexical variables */
        scope_idx = fd->scopes[scope_level].first;
        while (scope_idx >= 0) {
            vd = &fd->vars[scope_idx];
            vd->is_captured = 1;
            get_closure_var(ctx, s, fd, FALSE, scope_idx,
                            vd->var_name, vd->is_const, vd->is_lexical, vd->var_kind);
            scope_idx = vd->scope_next;
        }
        is_arg_scope = (scope_idx == ARG_SCOPE_END);
        if (!is_arg_scope) {
            /* add unscoped variables */
            for(i = 0; i < fd->arg_count; i++) {
                vd = &fd->args[i];
                if (vd->var_name != JS_ATOM_NULL) {
                    get_closure_var(ctx, s, fd,
                                    TRUE, i, vd->var_name, FALSE, FALSE,
                                    JS_VAR_NORMAL);
                }
            }
            for(i = 0; i < fd->var_count; i++) {
                vd = &fd->vars[i];
                /* do not close top level last result */
                if (vd->scope_level == 0 &&
                    vd->var_name != JS_ATOM__ret_ &&
                    vd->var_name != JS_ATOM_NULL) {
                    get_closure_var(ctx, s, fd,
                                    FALSE, i, vd->var_name, FALSE, FALSE,
                                    JS_VAR_NORMAL);
                }
            }
        } else {
            for(i = 0; i < fd->var_count; i++) {
                vd = &fd->vars[i];
                /* do not close top level last result */
                if (vd->scope_level == 0 && is_var_in_arg_scope(vd)) {
                    get_closure_var(ctx, s, fd,
                                    FALSE, i, vd->var_name, FALSE, FALSE,
                                    JS_VAR_NORMAL);
                }
            }
        }
        if (fd->is_eval) {
            int idx;
            /* add direct eval variables (we are necessarily at the
               top level) */
            for (idx = 0; idx < fd->closure_var_count; idx++) {
                JSClosureVar *cv = &fd->closure_var[idx];
                get_closure_var2(ctx, s, fd,
                                 FALSE, cv->is_arg,
                                 idx, cv->var_name, cv->is_const,
                                 cv->is_lexical, cv->var_kind);
            }
        }
    }
}

static void set_closure_from_var(JSContext *ctx, JSClosureVar *cv,
                                 JSVarDef *vd, int var_idx)
{
    cv->is_local = TRUE;
    cv->is_arg = FALSE;
    cv->is_const = vd->is_const;
    cv->is_lexical = vd->is_lexical;
    cv->var_kind = vd->var_kind;
    cv->var_idx = var_idx;
    cv->var_name = JS_DupAtom(ctx, vd->var_name);
}

/* for direct eval compilation: add references to the variables of the
   calling function */
static __exception int add_closure_variables(JSContext *ctx, JSFunctionDef *s,
                                             JSFunctionBytecode *b, int scope_idx)
{
    int i, count;
    JSVarDef *vd;
    BOOL is_arg_scope;
    count = b->arg_count + b->var_count + b->closure_var_count;
    s->closure_var = NULL;
    s->closure_var_count = 0;
    s->closure_var_size = count;
    if (count == 0)
        return 0;
    s->closure_var = js_malloc(ctx, sizeof(s->closure_var[0]) * count);
    if (!s->closure_var)
        return -1;
    /* Add lexical variables in scope at the point of evaluation */
    for (i = scope_idx; i >= 0;) {
        vd = &b->vardefs[b->arg_count + i];
        if (vd->scope_level > 0) {
            JSClosureVar *cv = &s->closure_var[s->closure_var_count++];
            set_closure_from_var(ctx, cv, vd, i);
        }
        i = vd->scope_next;
    }
    is_arg_scope = (i == ARG_SCOPE_END);
    if (!is_arg_scope) {
        /* Add argument variables */
        for(i = 0; i < b->arg_count; i++) {
            JSClosureVar *cv = &s->closure_var[s->closure_var_count++];
            vd = &b->vardefs[i];
            cv->is_local = TRUE;
            cv->is_arg = TRUE;
            cv->is_const = FALSE;
            cv->is_lexical = FALSE;
            cv->var_kind = JS_VAR_NORMAL;
            cv->var_idx = i;
            cv->var_name = JS_DupAtom(ctx, vd->var_name);
        }
        /* Add local non lexical variables */
        for(i = 0; i < b->var_count; i++) {
            vd = &b->vardefs[b->arg_count + i];
            if (vd->scope_level == 0 && vd->var_name != JS_ATOM__ret_) {
                JSClosureVar *cv = &s->closure_var[s->closure_var_count++];
                set_closure_from_var(ctx, cv, vd, i);
            }
        }
    } else {
        /* only add pseudo variables */
        for(i = 0; i < b->var_count; i++) {
            vd = &b->vardefs[b->arg_count + i];
            if (vd->scope_level == 0 && is_var_in_arg_scope(vd)) {
                JSClosureVar *cv = &s->closure_var[s->closure_var_count++];
                set_closure_from_var(ctx, cv, vd, i);
            }
        }
    }
    for(i = 0; i < b->closure_var_count; i++) {
        JSClosureVar *cv0 = &b->closure_var[i];
        JSClosureVar *cv = &s->closure_var[s->closure_var_count++];
        cv->is_local = FALSE;
        cv->is_arg = cv0->is_arg;
        cv->is_const = cv0->is_const;
        cv->is_lexical = cv0->is_lexical;
        cv->var_kind = cv0->var_kind;
        cv->var_idx = i;
        cv->var_name = JS_DupAtom(ctx, cv0->var_name);
    }
    return 0;
}

typedef struct CodeContext {
    const uint8_t *bc_buf; /* code buffer */
    int bc_len;   /* length of the code buffer */
    int pos;      /* position past the matched code pattern */
    int line_num; /* last visited OP_line_num parameter or -1 */
    int op;
    int idx;
    int label;
    int val;
    JSAtom atom;
} CodeContext;

#define M2(op1, op2)            ((op1) | ((op2) << 8))
#define M3(op1, op2, op3)       ((op1) | ((op2) << 8) | ((op3) << 16))
#define M4(op1, op2, op3, op4)  ((op1) | ((op2) << 8) | ((op3) << 16) | ((uint32_t)(op4) << 24))

static BOOL code_match(CodeContext *s, int pos, ...)
{
    const uint8_t *tab = s->bc_buf;
    int op, len, op1, line_num, pos_next;
    va_list ap;
    BOOL ret = FALSE;
    line_num = -1;
    va_start(ap, pos);
    for(;;) {
        op1 = va_arg(ap, int);
        if (op1 == -1) {
            s->pos = pos;
            s->line_num = line_num;
            ret = TRUE;
            break;
        }
        for (;;) {
            if (pos >= s->bc_len)
                goto done;
            op = tab[pos];
            len = opcode_info[op].size;
            pos_next = pos + len;
            if (pos_next > s->bc_len)
                goto done;
            if (op == OP_line_num) {
                line_num = get_u32(tab + pos + 1);
                pos = pos_next;
            } else {
                break;
            }
        }
        if (op != op1) {
            if (op1 == (uint8_t)op1 || !op)
                break;
            if (op != (uint8_t)op1
            &&  op != (uint8_t)(op1 >> 8)
            &&  op != (uint8_t)(op1 >> 16)
            &&  op != (uint8_t)(op1 >> 24)) {
                break;
            }
            s->op = op;
        }
        pos++;
        switch(opcode_info[op].fmt) {
        case OP_FMT_loc8:
        case OP_FMT_u8:
            {
                int idx = tab[pos];
                int arg = va_arg(ap, int);
                if (arg == -1) {
                    s->idx = idx;
                } else {
                    if (arg != idx)
                        goto done;
                }
                break;
            }
        case OP_FMT_u16:
        case OP_FMT_npop:
        case OP_FMT_loc:
        case OP_FMT_arg:
        case OP_FMT_var_ref:
            {
                int idx = get_u16(tab + pos);
                int arg = va_arg(ap, int);
                if (arg == -1) {
                    s->idx = idx;
                } else {
                    if (arg != idx)
                        goto done;
                }
                break;
            }
        case OP_FMT_i32:
        case OP_FMT_u32:
        case OP_FMT_label:
        case OP_FMT_const:
            {
                s->label = get_u32(tab + pos);
                break;
            }
        case OP_FMT_label_u16:
            {
                s->label = get_u32(tab + pos);
                s->val = get_u16(tab + pos + 4);
                break;
            }
        case OP_FMT_atom:
            {
                s->atom = get_u32(tab + pos);
                break;
            }
        case OP_FMT_atom_u8:
            {
                s->atom = get_u32(tab + pos);
                s->val = get_u8(tab + pos + 4);
                break;
            }
        case OP_FMT_atom_u16:
            {
                s->atom = get_u32(tab + pos);
                s->val = get_u16(tab + pos + 4);
                break;
            }
        case OP_FMT_atom_label_u8:
            {
                s->atom = get_u32(tab + pos);
                s->label = get_u32(tab + pos + 4);
                s->val = get_u8(tab + pos + 8);
                break;
            }
        default:
            break;
        }
        pos = pos_next;
    }
 done:
    va_end(ap);
    return ret;
}

static void instantiate_hoisted_definitions(JSContext *ctx, JSFunctionDef *s, DynBuf *bc)
{
    int i, idx, label_next = -1;
    /* add the hoisted functions in arguments and local variables */
    for(i = 0; i < s->arg_count; i++) {
        JSVarDef *vd = &s->args[i];
        if (vd->func_pool_idx >= 0) {
            dbuf_putc(bc, OP_fclosure);
            dbuf_put_u32(bc, vd->func_pool_idx);
            dbuf_putc(bc, OP_put_arg);
            dbuf_put_u16(bc, i);
        }
    }
    for(i = 0; i < s->var_count; i++) {
        JSVarDef *vd = &s->vars[i];
        if (vd->scope_level == 0 && vd->func_pool_idx >= 0) {
            dbuf_putc(bc, OP_fclosure);
            dbuf_put_u32(bc, vd->func_pool_idx);
            dbuf_putc(bc, OP_put_loc);
            dbuf_put_u16(bc, i);
        }
    }
    /* the module global variables must be initialized before
       evaluating the module so that the exported functions are
       visible if there are cyclic module references */
    if (s->module) {
        label_next = new_label_fd(s, -1);
        /* if 'this' is true, initialize the global variables and return */
        dbuf_putc(bc, OP_push_this);
        dbuf_putc(bc, OP_if_false);
        dbuf_put_u32(bc, label_next);
        update_label(s, label_next, 1);
        s->jump_size++;
    }
    /* add the global variables (only happens if s->is_global_var is
       true) */
    for(i = 0; i < s->global_var_count; i++) {
        JSGlobalVar *hf = &s->global_vars[i];
        int has_closure = 0;
        BOOL force_init = hf->force_init;
        /* we are in an eval, so the closure contains all the
           enclosing variables */
        /* If the outer function has a variable environment,
           create a property for the variable there */
        for(idx = 0; idx < s->closure_var_count; idx++) {
            JSClosureVar *cv = &s->closure_var[idx];
            if (cv->var_name == hf->var_name) {
                has_closure = 2;
                force_init = FALSE;
                break;
            }
            if (cv->var_name == JS_ATOM__var_ ||
                cv->var_name == JS_ATOM__arg_var_) {
                dbuf_putc(bc, OP_get_var_ref);
                dbuf_put_u16(bc, idx);
                has_closure = 1;
                force_init = TRUE;
                break;
            }
        }
        if (!has_closure) {
            int flags;
            flags = 0;
            if (s->eval_type != JS_EVAL_TYPE_GLOBAL)
                flags |= JS_PROP_CONFIGURABLE;
            if (hf->cpool_idx >= 0 && !hf->is_lexical) {
                /* global function definitions need a specific handling */
                dbuf_putc(bc, OP_fclosure);
                dbuf_put_u32(bc, hf->cpool_idx);
                dbuf_putc(bc, OP_define_func);
                dbuf_put_u32(bc, JS_DupAtom(ctx, hf->var_name));
                dbuf_putc(bc, flags);
                goto done_global_var;
            } else {
                if (hf->is_lexical) {
                    flags |= DEFINE_GLOBAL_LEX_VAR;
                    if (!hf->is_const)
                        flags |= JS_PROP_WRITABLE;
                }
                dbuf_putc(bc, OP_define_var);
                dbuf_put_u32(bc, JS_DupAtom(ctx, hf->var_name));
                dbuf_putc(bc, flags);
            }
        }
        if (hf->cpool_idx >= 0 || force_init) {
            if (hf->cpool_idx >= 0) {
                dbuf_putc(bc, OP_fclosure);
                dbuf_put_u32(bc, hf->cpool_idx);
                if (hf->var_name == JS_ATOM__default_) {
                    /* set default export function name */
                    dbuf_putc(bc, OP_set_name);
                    dbuf_put_u32(bc, JS_DupAtom(ctx, JS_ATOM_default));
                }
            } else {
                dbuf_putc(bc, OP_undefined);
            }
            if (has_closure == 2) {
                dbuf_putc(bc, OP_put_var_ref);
                dbuf_put_u16(bc, idx);
            } else if (has_closure == 1) {
                dbuf_putc(bc, OP_define_field);
                dbuf_put_u32(bc, JS_DupAtom(ctx, hf->var_name));
                dbuf_putc(bc, OP_drop);
            } else {
                /* XXX: Check if variable is writable and enumerable */
                dbuf_putc(bc, OP_put_var);
                dbuf_put_u32(bc, JS_DupAtom(ctx, hf->var_name));
            }
        }
    done_global_var:
        JS_FreeAtom(ctx, hf->var_name);
    }
    if (s->module) {
        dbuf_putc(bc, OP_return_undef);
        dbuf_putc(bc, OP_label);
        dbuf_put_u32(bc, label_next);
        s->label_slots[label_next].pos2 = bc->size;
    }
    js_free(ctx, s->global_vars);
    s->global_vars = NULL;
    s->global_var_count = 0;
    s->global_var_size = 0;
}

static int skip_dead_code(JSFunctionDef *s, const uint8_t *bc_buf, int bc_len,
                          int pos, int *linep)
{
    int op, len, label;
    for (; pos < bc_len; pos += len) {
        op = bc_buf[pos];
        len = opcode_info[op].size;
        if (op == OP_line_num) {
            *linep = get_u32(bc_buf + pos + 1);
        } else
        if (op == OP_label) {
            label = get_u32(bc_buf + pos + 1);
            if (update_label(s, label, 0) > 0)
                break;
#if 0
            if (s->label_slots[label].first_reloc) {
                printf("line %d: unreferenced label %d:%d has relocations\n",
                       *linep, label, s->label_slots[label].pos2);
            }
#endif
            assert(s->label_slots[label].first_reloc == NULL);
        } else {
            /* XXX: output a warning for unreachable code? */
            JSAtom atom;
            switch(opcode_info[op].fmt) {
            case OP_FMT_label:
            case OP_FMT_label_u16:
                label = get_u32(bc_buf + pos + 1);
                update_label(s, label, -1);
                break;
            case OP_FMT_atom_label_u8:
            case OP_FMT_atom_label_u16:
                label = get_u32(bc_buf + pos + 5);
                update_label(s, label, -1);
                /* fall thru */
            case OP_FMT_atom:
            case OP_FMT_atom_u8:
            case OP_FMT_atom_u16:
                atom = get_u32(bc_buf + pos + 1);
                JS_FreeAtom(s->ctx, atom);
                break;
            default:
                break;
            }
        }
    }
    return pos;
}

static int get_label_pos(JSFunctionDef *s, int label)
{
    int i, pos;
    for (i = 0; i < 20; i++) {
        pos = s->label_slots[label].pos;
        for (;;) {
            switch (s->byte_code.buf[pos]) {
            case OP_line_num:
            case OP_label:
                pos += 5;
                continue;
            case OP_goto:
                label = get_u32(s->byte_code.buf + pos + 1);
                break;
            default:
                return pos;
            }
            break;
        }
    }
    return pos;
}

/* convert global variable accesses to local variables or closure
   variables when necessary */
static __exception int resolve_variables(JSContext *ctx, JSFunctionDef *s)
{
    int pos, pos_next, bc_len, op, len, i, idx, line_num;
    uint8_t *bc_buf;
    JSAtom var_name;
    DynBuf bc_out;
    CodeContext cc;
    int scope;
    cc.bc_buf = bc_buf = s->byte_code.buf;
    cc.bc_len = bc_len = s->byte_code.size;
    js_dbuf_init(ctx, &bc_out);
    /* first pass for runtime checks (must be done before the
       variables are created) */
    for(i = 0; i < s->global_var_count; i++) {
        JSGlobalVar *hf = &s->global_vars[i];
        int flags;
        /* check if global variable (XXX: simplify) */
        for(idx = 0; idx < s->closure_var_count; idx++) {
            JSClosureVar *cv = &s->closure_var[idx];
            if (cv->var_name == hf->var_name) {
                if (s->eval_type == JS_EVAL_TYPE_DIRECT &&
                    cv->is_lexical) {
                    /* Check if a lexical variable is
                       redefined as 'var'. XXX: Could abort
                       compilation here, but for consistency
                       with the other checks, we delay the
                       error generation. */
                    dbuf_putc(&bc_out, OP_throw_error);
                    dbuf_put_u32(&bc_out, JS_DupAtom(ctx, hf->var_name));
                    dbuf_putc(&bc_out, JS_THROW_VAR_REDECL);
                }
                goto next;
            }
            if (cv->var_name == JS_ATOM__var_ ||
                cv->var_name == JS_ATOM__arg_var_)
                goto next;
        }
        dbuf_putc(&bc_out, OP_check_define_var);
        dbuf_put_u32(&bc_out, JS_DupAtom(ctx, hf->var_name));
        flags = 0;
        if (hf->is_lexical)
            flags |= DEFINE_GLOBAL_LEX_VAR;
        if (hf->cpool_idx >= 0)
            flags |= DEFINE_GLOBAL_FUNC_VAR;
        dbuf_putc(&bc_out, flags);
    next: ;
    }
    line_num = 0; /* avoid warning */
    for (pos = 0; pos < bc_len; pos = pos_next) {
        op = bc_buf[pos];
        len = opcode_info[op].size;
        pos_next = pos + len;
        switch(op) {
        case OP_line_num:
            line_num = get_u32(bc_buf + pos + 1);
            s->line_number_size++;
            goto no_change;
        case OP_eval: /* convert scope index to adjusted variable index */
            {
                int call_argc = get_u16(bc_buf + pos + 1);
                scope = get_u16(bc_buf + pos + 1 + 2);
                mark_eval_captured_variables(ctx, s, scope);
                dbuf_putc(&bc_out, op);
                dbuf_put_u16(&bc_out, call_argc);
                dbuf_put_u16(&bc_out, s->scopes[scope].first + 1);
            }
            break;
        case OP_apply_eval: /* convert scope index to adjusted variable index */
            scope = get_u16(bc_buf + pos + 1);
            mark_eval_captured_variables(ctx, s, scope);
            dbuf_putc(&bc_out, op);
            dbuf_put_u16(&bc_out, s->scopes[scope].first + 1);
            break;
        case OP_scope_get_var_undef:
        case OP_scope_get_var:
        case OP_scope_put_var:
        case OP_scope_delete_var:
        case OP_scope_get_ref:
        case OP_scope_put_var_init:
            var_name = get_u32(bc_buf + pos + 1);
            scope = get_u16(bc_buf + pos + 5);
            pos_next = resolve_scope_var(ctx, s, var_name, scope, op, &bc_out,
                                         NULL, NULL, pos_next);
            JS_FreeAtom(ctx, var_name);
            break;
        case OP_scope_make_ref:
            {
                int label;
                LabelSlot *ls;
                var_name = get_u32(bc_buf + pos + 1);
                label = get_u32(bc_buf + pos + 5);
                scope = get_u16(bc_buf + pos + 9);
                ls = &s->label_slots[label];
                ls->ref_count--;  /* always remove label reference */
                pos_next = resolve_scope_var(ctx, s, var_name, scope, op, &bc_out,
                                             bc_buf, ls, pos_next);
                JS_FreeAtom(ctx, var_name);
            }
            break;
        case OP_scope_get_private_field:
        case OP_scope_get_private_field2:
        case OP_scope_put_private_field:
            {
                int ret;
                var_name = get_u32(bc_buf + pos + 1);
                scope = get_u16(bc_buf + pos + 5);
                ret = resolve_scope_private_field(ctx, s, var_name, scope, op, &bc_out);
                if (ret < 0)
                    goto fail;
                JS_FreeAtom(ctx, var_name);
            }
            break;
        case OP_gosub:
            s->jump_size++;
            if (OPTIMIZE) {
                /* remove calls to empty finalizers  */
                int label;
                LabelSlot *ls;
                label = get_u32(bc_buf + pos + 1);
                assert(label >= 0 && label < s->label_count);
                ls = &s->label_slots[label];
                if (code_match(&cc, ls->pos, OP_ret, -1)) {
                    ls->ref_count--;
                    break;
                }
            }
            goto no_change;
        case OP_drop:
            if (0) {
                /* remove drops before return_undef */
                /* do not perform this optimization in pass2 because
                   it breaks patterns recognised in resolve_labels */
                int pos1 = pos_next;
                int line1 = line_num;
                while (code_match(&cc, pos1, OP_drop, -1)) {
                    if (cc.line_num >= 0) line1 = cc.line_num;
                    pos1 = cc.pos;
                }
                if (code_match(&cc, pos1, OP_return_undef, -1)) {
                    pos_next = pos1;
                    if (line1 != -1 && line1 != line_num) {
                        line_num = line1;
                        s->line_number_size++;
                        dbuf_putc(&bc_out, OP_line_num);
                        dbuf_put_u32(&bc_out, line_num);
                    }
                    break;
                }
            }
            goto no_change;
        case OP_insert3:
            if (OPTIMIZE) {
                /* Transformation: insert3 put_array_el|put_ref_value drop -> put_array_el|put_ref_value */
                if (code_match(&cc, pos_next, M2(OP_put_array_el, OP_put_ref_value), OP_drop, -1)) {
                    dbuf_putc(&bc_out, cc.op);
                    pos_next = cc.pos;
                    if (cc.line_num != -1 && cc.line_num != line_num) {
                        line_num = cc.line_num;
                        s->line_number_size++;
                        dbuf_putc(&bc_out, OP_line_num);
                        dbuf_put_u32(&bc_out, line_num);
                    }
                    break;
                }
            }
            goto no_change;
        case OP_goto:
            s->jump_size++;
            /* fall thru */
        case OP_tail_call:
        case OP_tail_call_method:
        case OP_return:
        case OP_return_undef:
        case OP_throw:
        case OP_throw_error:
        case OP_ret:
            if (OPTIMIZE) {
                /* remove dead code */
                int line = -1;
                dbuf_put(&bc_out, bc_buf + pos, len);
                pos = skip_dead_code(s, bc_buf, bc_len, pos + len, &line);
                pos_next = pos;
                if (pos < bc_len && line >= 0 && line_num != line) {
                    line_num = line;
                    s->line_number_size++;
                    dbuf_putc(&bc_out, OP_line_num);
                    dbuf_put_u32(&bc_out, line_num);
                }
                break;
            }
            goto no_change;
        case OP_label:
            {
                int label;
                LabelSlot *ls;
                label = get_u32(bc_buf + pos + 1);
                assert(label >= 0 && label < s->label_count);
                ls = &s->label_slots[label];
                ls->pos2 = bc_out.size + opcode_info[op].size;
            }
            goto no_change;
        case OP_enter_scope:
            {
                int scope_idx, scope = get_u16(bc_buf + pos + 1);
                if (scope == s->body_scope) {
                    instantiate_hoisted_definitions(ctx, s, &bc_out);
                }
                for(scope_idx = s->scopes[scope].first; scope_idx >= 0;) {
                    JSVarDef *vd = &s->vars[scope_idx];
                    if (vd->scope_level == scope) {
                        if (scope_idx != s->arguments_arg_idx) {
                            if (vd->var_kind == JS_VAR_FUNCTION_DECL ||
                                vd->var_kind == JS_VAR_NEW_FUNCTION_DECL) {
                                /* Initialize lexical variable upon entering scope */
                                dbuf_putc(&bc_out, OP_fclosure);
                                dbuf_put_u32(&bc_out, vd->func_pool_idx);
                                dbuf_putc(&bc_out, OP_put_loc);
                                dbuf_put_u16(&bc_out, scope_idx);
                            } else {
                                /* XXX: should check if variable can be used
                                   before initialization */
                                dbuf_putc(&bc_out, OP_set_loc_uninitialized);
                                dbuf_put_u16(&bc_out, scope_idx);
                            }
                        }
                        scope_idx = vd->scope_next;
                    } else {
                        break;
                    }
                }
            }
            break;
        case OP_leave_scope:
            {
                int scope_idx, scope = get_u16(bc_buf + pos + 1);
                for(scope_idx = s->scopes[scope].first; scope_idx >= 0;) {
                    JSVarDef *vd = &s->vars[scope_idx];
                    if (vd->scope_level == scope) {
                        if (vd->is_captured) {
                            dbuf_putc(&bc_out, OP_close_loc);
                            dbuf_put_u16(&bc_out, scope_idx);
                        }
                        scope_idx = vd->scope_next;
                    } else {
                        break;
                    }
                }
            }
            break;
        case OP_set_name:
            {
                /* remove dummy set_name opcodes */
                JSAtom name = get_u32(bc_buf + pos + 1);
                if (name == JS_ATOM_NULL)
                    break;
            }
            goto no_change;
        case OP_if_false:
        case OP_if_true:
        case OP_catch:
            s->jump_size++;
            goto no_change;
        case OP_dup:
            if (OPTIMIZE) {
                /* Transformation: dup if_false(l1) drop, l1: if_false(l2) -> if_false(l2) */
                /* Transformation: dup if_true(l1) drop, l1: if_true(l2) -> if_true(l2) */
                if (code_match(&cc, pos_next, M2(OP_if_false, OP_if_true), OP_drop, -1)) {
                    int lab0, lab1, op1, pos1, line1, pos2;
                    lab0 = lab1 = cc.label;
                    assert(lab1 >= 0 && lab1 < s->label_count);
                    op1 = cc.op;
                    pos1 = cc.pos;
                    line1 = cc.line_num;
                    while (code_match(&cc, (pos2 = get_label_pos(s, lab1)), OP_dup, op1, OP_drop, -1)) {
                        lab1 = cc.label;
                    }
                    if (code_match(&cc, pos2, op1, -1)) {
                        s->jump_size++;
                        update_label(s, lab0, -1);
                        update_label(s, cc.label, +1);
                        dbuf_putc(&bc_out, op1);
                        dbuf_put_u32(&bc_out, cc.label);
                        pos_next = pos1;
                        if (line1 != -1 && line1 != line_num) {
                            line_num = line1;
                            s->line_number_size++;
                            dbuf_putc(&bc_out, OP_line_num);
                            dbuf_put_u32(&bc_out, line_num);
                        }
                        break;
                    }
                }
            }
            goto no_change;
        case OP_nop:
            /* remove erased code */
            break;
        case OP_set_class_name:
            /* only used during parsing */
            break;
        default:
        no_change:
            dbuf_put(&bc_out, bc_buf + pos, len);
            break;
        }
    }
    /* set the new byte code */
    dbuf_free(&s->byte_code);
    s->byte_code = bc_out;
    if (dbuf_error(&s->byte_code)) {
        JS_ThrowOutOfMemory(ctx);
        return -1;
    }
    return 0;
 fail:
    /* continue the copy to keep the atom refcounts consistent */
    /* XXX: find a better solution ? */
    for (; pos < bc_len; pos = pos_next) {
        op = bc_buf[pos];
        len = opcode_info[op].size;
        pos_next = pos + len;
        dbuf_put(&bc_out, bc_buf + pos, len);
    }
    dbuf_free(&s->byte_code);
    s->byte_code = bc_out;
    return -1;
}

/* the pc2line table gives a line number for each PC value */
static void add_pc2line_info(JSFunctionDef *s, uint32_t pc, int line_num)
{
    if (s->line_number_slots != NULL
    &&  s->line_number_count < s->line_number_size
    &&  pc >= s->line_number_last_pc
    &&  line_num != s->line_number_last) {
        s->line_number_slots[s->line_number_count].pc = pc;
        s->line_number_slots[s->line_number_count].line_num = line_num;
        s->line_number_count++;
        s->line_number_last_pc = pc;
        s->line_number_last = line_num;
    }
}

static void compute_pc2line_info(JSFunctionDef *s)
{
    if (!(s->js_mode & JS_MODE_STRIP) && s->line_number_slots) {
        int last_line_num = s->line_num;
        uint32_t last_pc = 0;
        int i;
        js_dbuf_init(s->ctx, &s->pc2line);
        for (i = 0; i < s->line_number_count; i++) {
            uint32_t pc = s->line_number_slots[i].pc;
            int line_num = s->line_number_slots[i].line_num;
            int diff_pc, diff_line;

            if (line_num < 0)
                continue;

            diff_pc = pc - last_pc;
            diff_line = line_num - last_line_num;
            if (diff_line == 0 || diff_pc < 0)
                continue;

            if (diff_line >= PC2LINE_BASE &&
                diff_line < PC2LINE_BASE + PC2LINE_RANGE &&
                diff_pc <= PC2LINE_DIFF_PC_MAX) {
                dbuf_putc(&s->pc2line, (diff_line - PC2LINE_BASE) +
                          diff_pc * PC2LINE_RANGE + PC2LINE_OP_FIRST);
            } else {
                /* longer encoding */
                dbuf_putc(&s->pc2line, 0);
                dbuf_put_leb128(&s->pc2line, diff_pc);
                dbuf_put_sleb128(&s->pc2line, diff_line);
            }
            last_pc = pc;
            last_line_num = line_num;
        }
    }
}

static RelocEntry *add_reloc(JSContext *ctx, LabelSlot *ls, uint32_t addr, int size)
{
    RelocEntry *re;
    re = js_malloc(ctx, sizeof(*re));
    if (!re)
        return NULL;
    re->addr = addr;
    re->size = size;
    re->next = ls->first_reloc;
    ls->first_reloc = re;
    return re;
}

static BOOL code_has_label(CodeContext *s, int pos, int label)
{
    while (pos < s->bc_len) {
        int op = s->bc_buf[pos];
        if (op == OP_line_num) {
            pos += 5;
            continue;
        }
        if (op == OP_label) {
            int lab = get_u32(s->bc_buf + pos + 1);
            if (lab == label)
                return TRUE;
            pos += 5;
            continue;
        }
        if (op == OP_goto) {
            int lab = get_u32(s->bc_buf + pos + 1);
            if (lab == label)
                return TRUE;
        }
        break;
    }
    return FALSE;
}

/* return the target label, following the OP_goto jumps
   the first opcode at destination is stored in *pop
 */
static int find_jump_target(JSFunctionDef *s, int label, int *pop, int *pline)
{
    int i, pos, op;
    update_label(s, label, -1);
    for (i = 0; i < 10; i++) {
        assert(label >= 0 && label < s->label_count);
        pos = s->label_slots[label].pos2;
        for (;;) {
            switch(op = s->byte_code.buf[pos]) {
            case OP_line_num:
                if (pline)
                    *pline = get_u32(s->byte_code.buf + pos + 1);
                /* fall thru */
            case OP_label:
                pos += opcode_info[op].size;
                continue;
            case OP_goto:
                label = get_u32(s->byte_code.buf + pos + 1);
                break;
            case OP_drop:
                /* ignore drop opcodes if followed by OP_return_undef */
                while (s->byte_code.buf[++pos] == OP_drop)
                    continue;
                if (s->byte_code.buf[pos] == OP_return_undef)
                    op = OP_return_undef;
                /* fall thru */
            default:
                goto done;
            }
            break;
        }
    }
    /* cycle detected, could issue a warning */
 done:
    *pop = op;
    update_label(s, label, +1);
    return label;
}

static void push_short_int(DynBuf *bc_out, int val)
{
#if SHORT_OPCODES
    if (val >= -1 && val <= 7) {
        dbuf_putc(bc_out, OP_push_0 + val);
        return;
    }
    if (val == (int8_t)val) {
        dbuf_putc(bc_out, OP_push_i8);
        dbuf_putc(bc_out, val);
        return;
    }
    if (val == (int16_t)val) {
        dbuf_putc(bc_out, OP_push_i16);
        dbuf_put_u16(bc_out, val);
        return;
    }
#endif
    dbuf_putc(bc_out, OP_push_i32);
    dbuf_put_u32(bc_out, val);
}

static void put_short_code(DynBuf *bc_out, int op, int idx)
{
#if SHORT_OPCODES
    if (idx < 4) {
        switch (op) {
        case OP_get_loc:
            dbuf_putc(bc_out, OP_get_loc0 + idx);
            return;
        case OP_put_loc:
            dbuf_putc(bc_out, OP_put_loc0 + idx);
            return;
        case OP_set_loc:
            dbuf_putc(bc_out, OP_set_loc0 + idx);
            return;
        case OP_get_arg:
            dbuf_putc(bc_out, OP_get_arg0 + idx);
            return;
        case OP_put_arg:
            dbuf_putc(bc_out, OP_put_arg0 + idx);
            return;
        case OP_set_arg:
            dbuf_putc(bc_out, OP_set_arg0 + idx);
            return;
        case OP_get_var_ref:
            dbuf_putc(bc_out, OP_get_var_ref0 + idx);
            return;
        case OP_put_var_ref:
            dbuf_putc(bc_out, OP_put_var_ref0 + idx);
            return;
        case OP_set_var_ref:
            dbuf_putc(bc_out, OP_set_var_ref0 + idx);
            return;
        case OP_call:
            dbuf_putc(bc_out, OP_call0 + idx);
            return;
        }
    }
    if (idx < 256) {
        switch (op) {
        case OP_get_loc:
            dbuf_putc(bc_out, OP_get_loc8);
            dbuf_putc(bc_out, idx);
            return;
        case OP_put_loc:
            dbuf_putc(bc_out, OP_put_loc8);
            dbuf_putc(bc_out, idx);
            return;
        case OP_set_loc:
            dbuf_putc(bc_out, OP_set_loc8);
            dbuf_putc(bc_out, idx);
            return;
        }
    }
#endif
    dbuf_putc(bc_out, op);
    dbuf_put_u16(bc_out, idx);
}

/* peephole optimizations and resolve goto/labels */
static __exception int resolve_labels(JSContext *ctx, JSFunctionDef *s)
{
    int pos, pos_next, bc_len, op, op1, len, i, line_num;
    const uint8_t *bc_buf;
    DynBuf bc_out;
    LabelSlot *label_slots, *ls;
    RelocEntry *re, *re_next;
    CodeContext cc;
    int label;
#if SHORT_OPCODES
    JumpSlot *jp;
#endif
    label_slots = s->label_slots;
    line_num = s->line_num;
    cc.bc_buf = bc_buf = s->byte_code.buf;
    cc.bc_len = bc_len = s->byte_code.size;
    js_dbuf_init(ctx, &bc_out);
#if SHORT_OPCODES
    if (s->jump_size) {
        s->jump_slots = js_mallocz(s->ctx, sizeof(*s->jump_slots) * s->jump_size);
        if (s->jump_slots == NULL)
            return -1;
    }
#endif
    /* XXX: Should skip this phase if not generating SHORT_OPCODES */
    if (s->line_number_size && !(s->js_mode & JS_MODE_STRIP)) {
        s->line_number_slots = js_mallocz(s->ctx, sizeof(*s->line_number_slots) * s->line_number_size);
        if (s->line_number_slots == NULL)
            return -1;
        s->line_number_last = s->line_num;
        s->line_number_last_pc = 0;
    }
    /* initialize the 'home_object' variable if needed */
    if (s->home_object_var_idx >= 0) {
        dbuf_putc(&bc_out, OP_special_object);
        dbuf_putc(&bc_out, OP_SPECIAL_OBJECT_HOME_OBJECT);
        put_short_code(&bc_out, OP_put_loc, s->home_object_var_idx);
    }
    /* initialize the 'this.active_func' variable if needed */
    if (s->this_active_func_var_idx >= 0) {
        dbuf_putc(&bc_out, OP_special_object);
        dbuf_putc(&bc_out, OP_SPECIAL_OBJECT_THIS_FUNC);
        put_short_code(&bc_out, OP_put_loc, s->this_active_func_var_idx);
    }
    /* initialize the 'new.target' variable if needed */
    if (s->new_target_var_idx >= 0) {
        dbuf_putc(&bc_out, OP_special_object);
        dbuf_putc(&bc_out, OP_SPECIAL_OBJECT_NEW_TARGET);
        put_short_code(&bc_out, OP_put_loc, s->new_target_var_idx);
    }
    /* initialize the 'this' variable if needed. In a derived class
       constructor, this is initially uninitialized. */
    if (s->this_var_idx >= 0) {
        if (s->is_derived_class_constructor) {
            dbuf_putc(&bc_out, OP_set_loc_uninitialized);
            dbuf_put_u16(&bc_out, s->this_var_idx);
        } else {
            dbuf_putc(&bc_out, OP_push_this);
            put_short_code(&bc_out, OP_put_loc, s->this_var_idx);
        }
    }
    /* initialize the 'arguments' variable if needed */
    if (s->arguments_var_idx >= 0) {
        if ((s->js_mode & JS_MODE_STRICT) || !s->has_simple_parameter_list) {
            dbuf_putc(&bc_out, OP_special_object);
            dbuf_putc(&bc_out, OP_SPECIAL_OBJECT_ARGUMENTS);
        } else {
            dbuf_putc(&bc_out, OP_special_object);
            dbuf_putc(&bc_out, OP_SPECIAL_OBJECT_MAPPED_ARGUMENTS);
        }
        if (s->arguments_arg_idx >= 0)
            put_short_code(&bc_out, OP_set_loc, s->arguments_arg_idx);
        put_short_code(&bc_out, OP_put_loc, s->arguments_var_idx);
    }
    /* initialize a reference to the current function if needed */
    if (s->func_var_idx >= 0) {
        dbuf_putc(&bc_out, OP_special_object);
        dbuf_putc(&bc_out, OP_SPECIAL_OBJECT_THIS_FUNC);
        put_short_code(&bc_out, OP_put_loc, s->func_var_idx);
    }
    /* initialize the variable environment object if needed */
    if (s->var_object_idx >= 0) {
        dbuf_putc(&bc_out, OP_special_object);
        dbuf_putc(&bc_out, OP_SPECIAL_OBJECT_VAR_OBJECT);
        put_short_code(&bc_out, OP_put_loc, s->var_object_idx);
    }
    if (s->arg_var_object_idx >= 0) {
        dbuf_putc(&bc_out, OP_special_object);
        dbuf_putc(&bc_out, OP_SPECIAL_OBJECT_VAR_OBJECT);
        put_short_code(&bc_out, OP_put_loc, s->arg_var_object_idx);
    }
    for (pos = 0; pos < bc_len; pos = pos_next) {
        int val;
        op = bc_buf[pos];
        len = opcode_info[op].size;
        pos_next = pos + len;
        switch(op) {
        case OP_line_num:
            /* line number info (for debug). We put it in a separate
               compressed table to reduce memory usage and get better
               performance */
            line_num = get_u32(bc_buf + pos + 1);
            break;
        case OP_label:
            {
                label = get_u32(bc_buf + pos + 1);
                assert(label >= 0 && label < s->label_count);
                ls = &label_slots[label];
                assert(ls->addr == -1);
                ls->addr = bc_out.size;
                /* resolve the relocation entries */
                for(re = ls->first_reloc; re != NULL; re = re_next) {
                    int diff = ls->addr - re->addr;
                    re_next = re->next;
                    switch (re->size) {
                    case 4:
                        put_u32(bc_out.buf + re->addr, diff);
                        break;
                    case 2:
                        assert(diff == (int16_t)diff);
                        put_u16(bc_out.buf + re->addr, diff);
                        break;
                    case 1:
                        assert(diff == (int8_t)diff);
                        put_u8(bc_out.buf + re->addr, diff);
                        break;
                    }
                    js_free(ctx, re);
                }
                ls->first_reloc = NULL;
            }
            break;
        case OP_call:
        case OP_call_method:
            {
                /* detect and transform tail calls */
                int argc;
                argc = get_u16(bc_buf + pos + 1);
                if (code_match(&cc, pos_next, OP_return, -1)) {
                    if (cc.line_num >= 0) line_num = cc.line_num;
                    add_pc2line_info(s, bc_out.size, line_num);
                    put_short_code(&bc_out, op + 1, argc);
                    pos_next = skip_dead_code(s, bc_buf, bc_len, cc.pos, &line_num);
                    break;
                }
                add_pc2line_info(s, bc_out.size, line_num);
                put_short_code(&bc_out, op, argc);
                break;
            }
            goto no_change;
        case OP_return:
        case OP_return_undef:
        case OP_return_async:
        case OP_throw:
        case OP_throw_error:
            pos_next = skip_dead_code(s, bc_buf, bc_len, pos_next, &line_num);
            goto no_change;
        case OP_goto:
            label = get_u32(bc_buf + pos + 1);
        has_goto:
            if (OPTIMIZE) {
                int line1 = -1;
                /* Use custom matcher because multiple labels can follow */
                label = find_jump_target(s, label, &op1, &line1);
                if (code_has_label(&cc, pos_next, label)) {
                    /* jump to next instruction: remove jump */
                    update_label(s, label, -1);
                    break;
                }
                if (op1 == OP_return || op1 == OP_return_undef || op1 == OP_throw) {
                    /* jump to return/throw: remove jump, append return/throw */
                    /* updating the line number obfuscates assembly listing */
                    //if (line1 >= 0) line_num = line1;
                    update_label(s, label, -1);
                    add_pc2line_info(s, bc_out.size, line_num);
                    dbuf_putc(&bc_out, op1);
                    pos_next = skip_dead_code(s, bc_buf, bc_len, pos_next, &line_num);
                    break;
                }
                /* XXX: should duplicate single instructions followed by goto or return */
                /* For example, can match one of these followed by return:
                   push_i32 / push_const / push_atom_value / get_var /
                   undefined / null / push_false / push_true / get_ref_value /
                   get_loc / get_arg / get_var_ref
                 */
            }
            goto has_label;
        case OP_gosub:
            label = get_u32(bc_buf + pos + 1);
            if (0 && OPTIMIZE) {
                label = find_jump_target(s, label, &op1, NULL);
                if (op1 == OP_ret) {
                    update_label(s, label, -1);
                    /* empty finally clause: remove gosub */
                    break;
                }
            }
            goto has_label;
        case OP_catch:
            label = get_u32(bc_buf + pos + 1);
            goto has_label;
        case OP_if_true:
        case OP_if_false:
            label = get_u32(bc_buf + pos + 1);
            if (OPTIMIZE) {
                label = find_jump_target(s, label, &op1, NULL);
                /* transform if_false/if_true(l1) label(l1) -> drop label(l1) */
                if (code_has_label(&cc, pos_next, label)) {
                    update_label(s, label, -1);
                    dbuf_putc(&bc_out, OP_drop);
                    break;
                }
                /* transform if_false(l1) goto(l2) label(l1) -> if_false(l2) label(l1) */
                if (code_match(&cc, pos_next, OP_goto, -1)) {
                    int pos1 = cc.pos;
                    int line1 = cc.line_num;
                    if (code_has_label(&cc, pos1, label)) {
                        if (line1 >= 0) line_num = line1;
                        pos_next = pos1;
                        update_label(s, label, -1);
                        label = cc.label;
                        op ^= OP_if_true ^ OP_if_false;
                    }
                }
            }
        has_label:
            add_pc2line_info(s, bc_out.size, line_num);
            if (op == OP_goto) {
                pos_next = skip_dead_code(s, bc_buf, bc_len, pos_next, &line_num);
            }
            assert(label >= 0 && label < s->label_count);
            ls = &label_slots[label];
#if SHORT_OPCODES
            jp = &s->jump_slots[s->jump_count++];
            jp->op = op;
            jp->size = 4;
            jp->pos = bc_out.size + 1;
            jp->label = label;
            if (ls->addr == -1) {
                int diff = ls->pos2 - pos - 1;
                if (diff < 128 && (op == OP_if_false || op == OP_if_true || op == OP_goto)) {
                    jp->size = 1;
                    jp->op = OP_if_false8 + (op - OP_if_false);
                    dbuf_putc(&bc_out, OP_if_false8 + (op - OP_if_false));
                    dbuf_putc(&bc_out, 0);
                    if (!add_reloc(ctx, ls, bc_out.size - 1, 1))
                        goto fail;
                    break;
                }
                if (diff < 32768 && op == OP_goto) {
                    jp->size = 2;
                    jp->op = OP_goto16;
                    dbuf_putc(&bc_out, OP_goto16);
                    dbuf_put_u16(&bc_out, 0);
                    if (!add_reloc(ctx, ls, bc_out.size - 2, 2))
                        goto fail;
                    break;
                }
            } else {
                int diff = ls->addr - bc_out.size - 1;
                if (diff == (int8_t)diff && (op == OP_if_false || op == OP_if_true || op == OP_goto)) {
                    jp->size = 1;
                    jp->op = OP_if_false8 + (op - OP_if_false);
                    dbuf_putc(&bc_out, OP_if_false8 + (op - OP_if_false));
                    dbuf_putc(&bc_out, diff);
                    break;
                }
                if (diff == (int16_t)diff && op == OP_goto) {
                    jp->size = 2;
                    jp->op = OP_goto16;
                    dbuf_putc(&bc_out, OP_goto16);
                    dbuf_put_u16(&bc_out, diff);
                    break;
                }
            }
#endif
            dbuf_putc(&bc_out, op);
            dbuf_put_u32(&bc_out, ls->addr - bc_out.size);
            if (ls->addr == -1) {
                /* unresolved yet: create a new relocation entry */
                if (!add_reloc(ctx, ls, bc_out.size - 4, 4))
                    goto fail;
            }
            break;
        case OP_with_get_var:
        case OP_with_put_var:
        case OP_with_delete_var:
        case OP_with_make_ref:
        case OP_with_get_ref:
        case OP_with_get_ref_undef:
            {
                JSAtom atom;
                int is_with;
                atom = get_u32(bc_buf + pos + 1);
                label = get_u32(bc_buf + pos + 5);
                is_with = bc_buf[pos + 9];
                if (OPTIMIZE) {
                    label = find_jump_target(s, label, &op1, NULL);
                }
                assert(label >= 0 && label < s->label_count);
                ls = &label_slots[label];
                add_pc2line_info(s, bc_out.size, line_num);
#if SHORT_OPCODES
                jp = &s->jump_slots[s->jump_count++];
                jp->op = op;
                jp->size = 4;
                jp->pos = bc_out.size + 5;
                jp->label = label;
#endif
                dbuf_putc(&bc_out, op);
                dbuf_put_u32(&bc_out, atom);
                dbuf_put_u32(&bc_out, ls->addr - bc_out.size);
                if (ls->addr == -1) {
                    /* unresolved yet: create a new relocation entry */
                    if (!add_reloc(ctx, ls, bc_out.size - 4, 4))
                        goto fail;
                }
                dbuf_putc(&bc_out, is_with);
            }
            break;
        case OP_drop:
            if (OPTIMIZE) {
                /* remove useless drops before return */
                if (code_match(&cc, pos_next, OP_return_undef, -1)) {
                    if (cc.line_num >= 0) line_num = cc.line_num;
                    break;
                }
            }
            goto no_change;
        case OP_null:
#if SHORT_OPCODES
            if (OPTIMIZE) {
                /* transform null strict_eq into is_null */
                if (code_match(&cc, pos_next, OP_strict_eq, -1)) {
                    if (cc.line_num >= 0) line_num = cc.line_num;
                    add_pc2line_info(s, bc_out.size, line_num);
                    dbuf_putc(&bc_out, OP_is_null);
                    pos_next = cc.pos;
                    break;
                }
                /* transform null strict_neq if_false/if_true -> is_null if_true/if_false */
                if (code_match(&cc, pos_next, OP_strict_neq, M2(OP_if_false, OP_if_true), -1)) {
                    if (cc.line_num >= 0) line_num = cc.line_num;
                    add_pc2line_info(s, bc_out.size, line_num);
                    dbuf_putc(&bc_out, OP_is_null);
                    pos_next = cc.pos;
                    label = cc.label;
                    op = cc.op ^ OP_if_false ^ OP_if_true;
                    goto has_label;
                }
            }
#endif
            /* fall thru */
        case OP_push_false:
        case OP_push_true:
            if (OPTIMIZE) {
                val = (op == OP_push_true);
                if (code_match(&cc, pos_next, M2(OP_if_false, OP_if_true), -1)) {
                has_constant_test:
                    if (cc.line_num >= 0) line_num = cc.line_num;
                    if (val == cc.op - OP_if_false) {
                        /* transform null if_false(l1) -> goto l1 */
                        /* transform false if_false(l1) -> goto l1 */
                        /* transform true if_true(l1) -> goto l1 */
                        pos_next = cc.pos;
                        op = OP_goto;
                        label = cc.label;
                        goto has_goto;
                    } else {
                        /* transform null if_true(l1) -> nop */
                        /* transform false if_true(l1) -> nop */
                        /* transform true if_false(l1) -> nop */
                        pos_next = cc.pos;
                        update_label(s, cc.label, -1);
                        break;
                    }
                }
            }
            goto no_change;
        case OP_push_i32:
            if (OPTIMIZE) {
                /* transform i32(val) neg -> i32(-val) */
                val = get_i32(bc_buf + pos + 1);
                if ((val != INT32_MIN && val != 0)
                &&  code_match(&cc, pos_next, OP_neg, -1)) {
                    if (cc.line_num >= 0) line_num = cc.line_num;
                    if (code_match(&cc, cc.pos, OP_drop, -1)) {
                        if (cc.line_num >= 0) line_num = cc.line_num;
                    } else {
                        add_pc2line_info(s, bc_out.size, line_num);
                        push_short_int(&bc_out, -val);
                    }
                    pos_next = cc.pos;
                    break;
                }
                /* remove push/drop pairs generated by the parser */
                if (code_match(&cc, pos_next, OP_drop, -1)) {
                    if (cc.line_num >= 0) line_num = cc.line_num;
                    pos_next = cc.pos;
                    break;
                }
                /* Optimize constant tests: `if (0)`, `if (1)`, `if (!0)`... */
                if (code_match(&cc, pos_next, M2(OP_if_false, OP_if_true), -1)) {
                    val = (val != 0);
                    goto has_constant_test;
                }
                add_pc2line_info(s, bc_out.size, line_num);
                push_short_int(&bc_out, val);
                break;
            }
            goto no_change;
#if SHORT_OPCODES
        case OP_push_const:
        case OP_fclosure:
            if (OPTIMIZE) {
                int idx = get_u32(bc_buf + pos + 1);
                if (idx < 256) {
                    add_pc2line_info(s, bc_out.size, line_num);
                    dbuf_putc(&bc_out, OP_push_const8 + op - OP_push_const);
                    dbuf_putc(&bc_out, idx);
                    break;
                }
            }
            goto no_change;
        case OP_get_field:
            if (OPTIMIZE) {
                JSAtom atom = get_u32(bc_buf + pos + 1);
                if (atom == JS_ATOM_length) {
                    JS_FreeAtom(ctx, atom);
                    add_pc2line_info(s, bc_out.size, line_num);
                    dbuf_putc(&bc_out, OP_get_length);
                    break;
                }
            }
            goto no_change;
#endif
        case OP_push_atom_value:
            if (OPTIMIZE) {
                JSAtom atom = get_u32(bc_buf + pos + 1);
                /* remove push/drop pairs generated by the parser */
                if (code_match(&cc, pos_next, OP_drop, -1)) {
                    JS_FreeAtom(ctx, atom);
                    if (cc.line_num >= 0) line_num = cc.line_num;
                    pos_next = cc.pos;
                    break;
                }
#if SHORT_OPCODES
                if (atom == JS_ATOM_empty_string) {
                    JS_FreeAtom(ctx, atom);
                    add_pc2line_info(s, bc_out.size, line_num);
                    dbuf_putc(&bc_out, OP_push_empty_string);
                    break;
                }
#endif
            }
            goto no_change;
        case OP_to_propkey:
        case OP_to_propkey2:
            if (OPTIMIZE) {
                /* remove redundant to_propkey/to_propkey2 opcodes when storing simple data */
                if (code_match(&cc, pos_next, M3(OP_get_loc, OP_get_arg, OP_get_var_ref), -1, OP_put_array_el, -1)
                ||  code_match(&cc, pos_next, M3(OP_push_i32, OP_push_const, OP_push_atom_value), OP_put_array_el, -1)
                ||  code_match(&cc, pos_next, M4(OP_undefined, OP_null, OP_push_true, OP_push_false), OP_put_array_el, -1)) {
                    break;
                }
            }
            goto no_change;
        case OP_undefined:
            if (OPTIMIZE) {
                /* remove push/drop pairs generated by the parser */
                if (code_match(&cc, pos_next, OP_drop, -1)) {
                    if (cc.line_num >= 0) line_num = cc.line_num;
                    pos_next = cc.pos;
                    break;
                }
                /* transform undefined return -> return_undefined */
                if (code_match(&cc, pos_next, OP_return, -1)) {
                    if (cc.line_num >= 0) line_num = cc.line_num;
                    add_pc2line_info(s, bc_out.size, line_num);
                    dbuf_putc(&bc_out, OP_return_undef);
                    pos_next = cc.pos;
                    break;
                }
                /* transform undefined if_true(l1)/if_false(l1) -> nop/goto(l1) */
                if (code_match(&cc, pos_next, M2(OP_if_false, OP_if_true), -1)) {
                    val = 0;
                    goto has_constant_test;
                }
#if SHORT_OPCODES
                /* transform undefined strict_eq -> is_undefined */
                if (code_match(&cc, pos_next, OP_strict_eq, -1)) {
                    if (cc.line_num >= 0) line_num = cc.line_num;
                    add_pc2line_info(s, bc_out.size, line_num);
                    dbuf_putc(&bc_out, OP_is_undefined);
                    pos_next = cc.pos;
                    break;
                }
                /* transform undefined strict_neq if_false/if_true -> is_undefined if_true/if_false */
                if (code_match(&cc, pos_next, OP_strict_neq, M2(OP_if_false, OP_if_true), -1)) {
                    if (cc.line_num >= 0) line_num = cc.line_num;
                    add_pc2line_info(s, bc_out.size, line_num);
                    dbuf_putc(&bc_out, OP_is_undefined);
                    pos_next = cc.pos;
                    label = cc.label;
                    op = cc.op ^ OP_if_false ^ OP_if_true;
                    goto has_label;
                }
#endif
            }
            goto no_change;
        case OP_insert2:
            if (OPTIMIZE) {
                /* Transformation:
                   insert2 put_field(a) drop -> put_field(a)
                   insert2 put_var_strict(a) drop -> put_var_strict(a)
                */
                if (code_match(&cc, pos_next, M2(OP_put_field, OP_put_var_strict), OP_drop, -1)) {
                    if (cc.line_num >= 0) line_num = cc.line_num;
                    add_pc2line_info(s, bc_out.size, line_num);
                    dbuf_putc(&bc_out, cc.op);
                    dbuf_put_u32(&bc_out, cc.atom);
                    pos_next = cc.pos;
                    break;
                }
            }
            goto no_change;
        case OP_dup:
            if (OPTIMIZE) {
                /* Transformation: dup put_x(n) drop -> put_x(n) */
                int op1, line2 = -1;
                /* Transformation: dup put_x(n) -> set_x(n) */
                if (code_match(&cc, pos_next, M3(OP_put_loc, OP_put_arg, OP_put_var_ref), -1, -1)) {
                    if (cc.line_num >= 0) line_num = cc.line_num;
                    op1 = cc.op + 1;  /* put_x -> set_x */
                    pos_next = cc.pos;
                    if (code_match(&cc, cc.pos, OP_drop, -1)) {
                        if (cc.line_num >= 0) line_num = cc.line_num;
                        op1 -= 1; /* set_x drop -> put_x */
                        pos_next = cc.pos;
                        if (code_match(&cc, cc.pos, op1 - 1, cc.idx, -1)) {
                            line2 = cc.line_num; /* delay line number update */
                            op1 += 1;   /* put_x(n) get_x(n) -> set_x(n) */
                            pos_next = cc.pos;
                        }
                    }
                    add_pc2line_info(s, bc_out.size, line_num);
                    put_short_code(&bc_out, op1, cc.idx);
                    if (line2 >= 0) line_num = line2;
                    break;
                }
            }
            goto no_change;
        case OP_get_loc:
            if (OPTIMIZE) {
                /* transformation:
                   get_loc(n) post_dec put_loc(n) drop -> dec_loc(n)
                   get_loc(n) post_inc put_loc(n) drop -> inc_loc(n)
                   get_loc(n) dec dup put_loc(n) drop -> dec_loc(n)
                   get_loc(n) inc dup put_loc(n) drop -> inc_loc(n)
                 */
                int idx;
                idx = get_u16(bc_buf + pos + 1);
                if (idx >= 256)
                    goto no_change;
                if (code_match(&cc, pos_next, M2(OP_post_dec, OP_post_inc), OP_put_loc, idx, OP_drop, -1) ||
                    code_match(&cc, pos_next, M2(OP_dec, OP_inc), OP_dup, OP_put_loc, idx, OP_drop, -1)) {
                    if (cc.line_num >= 0) line_num = cc.line_num;
                    add_pc2line_info(s, bc_out.size, line_num);
                    dbuf_putc(&bc_out, (cc.op == OP_inc || cc.op == OP_post_inc) ? OP_inc_loc : OP_dec_loc);
                    dbuf_putc(&bc_out, idx);
                    pos_next = cc.pos;
                    break;
                }
                /* transformation:
                   get_loc(n) push_atom_value(x) add dup put_loc(n) drop -> push_atom_value(x) add_loc(n)
                 */
                if (code_match(&cc, pos_next, OP_push_atom_value, OP_add, OP_dup, OP_put_loc, idx, OP_drop, -1)) {
                    if (cc.line_num >= 0) line_num = cc.line_num;
                    add_pc2line_info(s, bc_out.size, line_num);
#if SHORT_OPCODES
                    if (cc.atom == JS_ATOM_empty_string) {
                        JS_FreeAtom(ctx, cc.atom);
                        dbuf_putc(&bc_out, OP_push_empty_string);
                    } else
#endif
                    {
                        dbuf_putc(&bc_out, OP_push_atom_value);
                        dbuf_put_u32(&bc_out, cc.atom);
                    }
                    dbuf_putc(&bc_out, OP_add_loc);
                    dbuf_putc(&bc_out, idx);
                    pos_next = cc.pos;
                    break;
                }
                /* transformation:
                   get_loc(n) push_i32(x) add dup put_loc(n) drop -> push_i32(x) add_loc(n)
                 */
                if (code_match(&cc, pos_next, OP_push_i32, OP_add, OP_dup, OP_put_loc, idx, OP_drop, -1)) {
                    if (cc.line_num >= 0) line_num = cc.line_num;
                    add_pc2line_info(s, bc_out.size, line_num);
                    push_short_int(&bc_out, cc.label);
                    dbuf_putc(&bc_out, OP_add_loc);
                    dbuf_putc(&bc_out, idx);
                    pos_next = cc.pos;
                    break;
                }
                /* transformation: XXX: also do these:
                   get_loc(n) get_loc(x) add dup put_loc(n) drop -> get_loc(x) add_loc(n)
                   get_loc(n) get_arg(x) add dup put_loc(n) drop -> get_arg(x) add_loc(n)
                   get_loc(n) get_var_ref(x) add dup put_loc(n) drop -> get_var_ref(x) add_loc(n)
                 */
                if (code_match(&cc, pos_next, M3(OP_get_loc, OP_get_arg, OP_get_var_ref), -1, OP_add, OP_dup, OP_put_loc, idx, OP_drop, -1)) {
                    if (cc.line_num >= 0) line_num = cc.line_num;
                    add_pc2line_info(s, bc_out.size, line_num);
                    put_short_code(&bc_out, cc.op, cc.idx);
                    dbuf_putc(&bc_out, OP_add_loc);
                    dbuf_putc(&bc_out, idx);
                    pos_next = cc.pos;
                    break;
                }
                add_pc2line_info(s, bc_out.size, line_num);
                put_short_code(&bc_out, op, idx);
                break;
            }
            goto no_change;
#if SHORT_OPCODES
        case OP_get_arg:
        case OP_get_var_ref:
            if (OPTIMIZE) {
                int idx;
                idx = get_u16(bc_buf + pos + 1);
                add_pc2line_info(s, bc_out.size, line_num);
                put_short_code(&bc_out, op, idx);
                break;
            }
            goto no_change;
#endif
        case OP_put_loc:
        case OP_put_arg:
        case OP_put_var_ref:
            if (OPTIMIZE) {
                /* transformation: put_x(n) get_x(n) -> set_x(n) */
                int idx;
                idx = get_u16(bc_buf + pos + 1);
                if (code_match(&cc, pos_next, op - 1, idx, -1)) {
                    if (cc.line_num >= 0) line_num = cc.line_num;
                    add_pc2line_info(s, bc_out.size, line_num);
                    put_short_code(&bc_out, op + 1, idx);
                    pos_next = cc.pos;
                    break;
                }
                add_pc2line_info(s, bc_out.size, line_num);
                put_short_code(&bc_out, op, idx);
                break;
            }
            goto no_change;
        case OP_post_inc:
        case OP_post_dec:
            if (OPTIMIZE) {
                /* transformation:
                   post_inc put_x drop -> inc put_x
                   post_inc perm3 put_field drop -> inc put_field
                   post_inc perm3 put_var_strict drop -> inc put_var_strict
                   post_inc perm4 put_array_el drop -> inc put_array_el
                 */
                int op1, idx;
                if (code_match(&cc, pos_next, M3(OP_put_loc, OP_put_arg, OP_put_var_ref), -1, OP_drop, -1)) {
                    if (cc.line_num >= 0) line_num = cc.line_num;
                    op1 = cc.op;
                    idx = cc.idx;
                    pos_next = cc.pos;
                    if (code_match(&cc, cc.pos, op1 - 1, idx, -1)) {
                        if (cc.line_num >= 0) line_num = cc.line_num;
                        op1 += 1;   /* put_x(n) get_x(n) -> set_x(n) */
                        pos_next = cc.pos;
                    }
                    add_pc2line_info(s, bc_out.size, line_num);
                    dbuf_putc(&bc_out, OP_dec + (op - OP_post_dec));
                    put_short_code(&bc_out, op1, idx);
                    break;
                }
                if (code_match(&cc, pos_next, OP_perm3, M2(OP_put_field, OP_put_var_strict), OP_drop, -1)) {
                    if (cc.line_num >= 0) line_num = cc.line_num;
                    add_pc2line_info(s, bc_out.size, line_num);
                    dbuf_putc(&bc_out, OP_dec + (op - OP_post_dec));
                    dbuf_putc(&bc_out, cc.op);
                    dbuf_put_u32(&bc_out, cc.atom);
                    pos_next = cc.pos;
                    break;
                }
                if (code_match(&cc, pos_next, OP_perm4, OP_put_array_el, OP_drop, -1)) {
                    if (cc.line_num >= 0) line_num = cc.line_num;
                    add_pc2line_info(s, bc_out.size, line_num);
                    dbuf_putc(&bc_out, OP_dec + (op - OP_post_dec));
                    dbuf_putc(&bc_out, OP_put_array_el);
                    pos_next = cc.pos;
                    break;
                }
            }
            goto no_change;
#if SHORT_OPCODES
        case OP_typeof:
            if (OPTIMIZE) {
                /* simplify typeof tests */
                if (code_match(&cc, pos_next, OP_push_atom_value, M4(OP_strict_eq, OP_strict_neq, OP_eq, OP_neq), -1)) {
                    if (cc.line_num >= 0) line_num = cc.line_num;
                    int op1 = (cc.op == OP_strict_eq || cc.op == OP_eq) ? OP_strict_eq : OP_strict_neq;
                    int op2 = -1;
                    switch (cc.atom) {
                    case JS_ATOM_undefined:
                        op2 = OP_typeof_is_undefined;
                        break;
                    case JS_ATOM_function:
                        op2 = OP_typeof_is_function;
                        break;
                    }
                    if (op2 >= 0) {
                        /* transform typeof(s) == "<type>" into is_<type> */
                        if (op1 == OP_strict_eq) {
                            add_pc2line_info(s, bc_out.size, line_num);
                            dbuf_putc(&bc_out, op2);
                            JS_FreeAtom(ctx, cc.atom);
                            pos_next = cc.pos;
                            break;
                        }
                        if (op1 == OP_strict_neq && code_match(&cc, cc.pos, OP_if_false, -1)) {
                            /* transform typeof(s) != "<type>" if_false into is_<type> if_true */
                            if (cc.line_num >= 0) line_num = cc.line_num;
                            add_pc2line_info(s, bc_out.size, line_num);
                            dbuf_putc(&bc_out, op2);
                            JS_FreeAtom(ctx, cc.atom);
                            pos_next = cc.pos;
                            label = cc.label;
                            op = OP_if_true;
                            goto has_label;
                        }
                    }
                }
            }
            goto no_change;
#endif
        default:
        no_change:
            add_pc2line_info(s, bc_out.size, line_num);
            dbuf_put(&bc_out, bc_buf + pos, len);
            break;
        }
    }
    /* check that there were no missing labels */
    for(i = 0; i < s->label_count; i++) {
        assert(label_slots[i].first_reloc == NULL);
    }
#if SHORT_OPCODES
    if (OPTIMIZE) {
        /* more jump optimizations */
        int patch_offsets = 0;
        for (i = 0, jp = s->jump_slots; i < s->jump_count; i++, jp++) {
            LabelSlot *ls;
            JumpSlot *jp1;
            int j, pos, diff, delta;
            delta = 3;
            switch (op = jp->op) {
            case OP_goto16:
                delta = 1;
                /* fall thru */
            case OP_if_false:
            case OP_if_true:
            case OP_goto:
                pos = jp->pos;
                diff = s->label_slots[jp->label].addr - pos;
                if (diff >= -128 && diff <= 127 + delta) {
                    //put_u8(bc_out.buf + pos, diff);
                    jp->size = 1;
                    if (op == OP_goto16) {
                        bc_out.buf[pos - 1] = jp->op = OP_goto8;
                    } else {
                        bc_out.buf[pos - 1] = jp->op = OP_if_false8 + (op - OP_if_false);
                    }
                    goto shrink;
                } else
                if (diff == (int16_t)diff && op == OP_goto) {
                    //put_u16(bc_out.buf + pos, diff);
                    jp->size = 2;
                    delta = 2;
                    bc_out.buf[pos - 1] = jp->op = OP_goto16;
                shrink:
                    /* XXX: should reduce complexity, using 2 finger copy scheme */
                    memmove(bc_out.buf + pos + jp->size, bc_out.buf + pos + jp->size + delta,
                            bc_out.size - pos - jp->size - delta);
                    bc_out.size -= delta;
                    patch_offsets++;
                    for (j = 0, ls = s->label_slots; j < s->label_count; j++, ls++) {
                        if (ls->addr > pos)
                            ls->addr -= delta;
                    }
                    for (j = i + 1, jp1 = jp + 1; j < s->jump_count; j++, jp1++) {
                        if (jp1->pos > pos)
                            jp1->pos -= delta;
                    }
                    for (j = 0; j < s->line_number_count; j++) {
                        if (s->line_number_slots[j].pc > pos)
                            s->line_number_slots[j].pc -= delta;
                    }
                    continue;
                }
                break;
            }
        }
        if (patch_offsets) {
            JumpSlot *jp1;
            int j;
            for (j = 0, jp1 = s->jump_slots; j < s->jump_count; j++, jp1++) {
                int diff1 = s->label_slots[jp1->label].addr - jp1->pos;
                switch (jp1->size) {
                case 1:
                    put_u8(bc_out.buf + jp1->pos, diff1);
                    break;
                case 2:
                    put_u16(bc_out.buf + jp1->pos, diff1);
                    break;
                case 4:
                    put_u32(bc_out.buf + jp1->pos, diff1);
                    break;
                }
            }
        }
    }
    js_free(ctx, s->jump_slots);
    s->jump_slots = NULL;
#endif
    js_free(ctx, s->label_slots);
    s->label_slots = NULL;
    /* XXX: should delay until copying to runtime bytecode function */
    compute_pc2line_info(s);
    js_free(ctx, s->line_number_slots);
    s->line_number_slots = NULL;
    /* set the new byte code */
    dbuf_free(&s->byte_code);
    s->byte_code = bc_out;
    s->use_short_opcodes = TRUE;
    if (dbuf_error(&s->byte_code)) {
        JS_ThrowOutOfMemory(ctx);
        return -1;
    }
    return 0;
 fail:
    /* XXX: not safe */
    dbuf_free(&bc_out);
    return -1;
}

/* compute the maximum stack size needed by the function */
typedef struct StackSizeState {
    int bc_len;
    int stack_len_max;
    uint16_t *stack_level_tab;
    int *pc_stack;
    int pc_stack_len;
    int pc_stack_size;
} StackSizeState;

/* 'op' is only used for error indication */
static __exception int ss_check(JSContext *ctx, StackSizeState *s,
                                int pos, int op, int stack_len)
{
    if ((unsigned)pos >= s->bc_len) {
        JS_ThrowInternalError(ctx, "bytecode buffer overflow (op=%d, pc=%d)", op, pos);
        return -1;
    }
    if (stack_len > s->stack_len_max) {
        s->stack_len_max = stack_len;
        if (s->stack_len_max > JS_STACK_SIZE_MAX) {
            JS_ThrowInternalError(ctx, "stack overflow (op=%d, pc=%d)", op, pos);
            return -1;
        }
    }
    if (s->stack_level_tab[pos] != 0xffff) {
        /* already explored: check that the stack size is consistent */
        if (s->stack_level_tab[pos] != stack_len) {
            JS_ThrowInternalError(ctx, "unconsistent stack size: %d %d (pc=%d)",
                                  s->stack_level_tab[pos], stack_len, pos);
            return -1;
        } else {
            return 0;
        }
    }
    /* mark as explored and store the stack size */
    s->stack_level_tab[pos] = stack_len;
    /* queue the new PC to explore */
    if (js_resize_array(ctx, (void **)&s->pc_stack, sizeof(s->pc_stack[0]),
                        &s->pc_stack_size, s->pc_stack_len + 1))
        return -1;
    s->pc_stack[s->pc_stack_len++] = pos;
    return 0;
}

static __exception int compute_stack_size(JSContext *ctx,
                                          JSFunctionDef *fd,
                                          int *pstack_size)
{
    StackSizeState s_s, *s = &s_s;
    int i, diff, n_pop, pos_next, stack_len, pos, op;
    const JSOpCode *oi;
    const uint8_t *bc_buf;
    bc_buf = fd->byte_code.buf;
    s->bc_len = fd->byte_code.size;
    /* bc_len > 0 */
    s->stack_level_tab = js_malloc(ctx, sizeof(s->stack_level_tab[0]) *
                                   s->bc_len);
    if (!s->stack_level_tab)
        return -1;
    for(i = 0; i < s->bc_len; i++)
        s->stack_level_tab[i] = 0xffff;
    s->stack_len_max = 0;
    s->pc_stack = NULL;
    s->pc_stack_len = 0;
    s->pc_stack_size = 0;
    /* breadth-first graph exploration */
    if (ss_check(ctx, s, 0, OP_invalid, 0))
        goto fail;
    while (s->pc_stack_len > 0) {
        pos = s->pc_stack[--s->pc_stack_len];
        stack_len = s->stack_level_tab[pos];
        op = bc_buf[pos];
        if (op == 0 || op >= OP_COUNT) {
            JS_ThrowInternalError(ctx, "invalid opcode (op=%d, pc=%d)", op, pos);
            goto fail;
        }
        oi = &short_opcode_info(op);
        pos_next = pos + oi->size;
        if (pos_next > s->bc_len) {
            JS_ThrowInternalError(ctx, "bytecode buffer overflow (op=%d, pc=%d)", op, pos);
            goto fail;
        }
        n_pop = oi->n_pop;
        /* call pops a variable number of arguments */
        if (oi->fmt == OP_FMT_npop || oi->fmt == OP_FMT_npop_u16) {
            n_pop += get_u16(bc_buf + pos + 1);
        } else {
#if SHORT_OPCODES
            if (oi->fmt == OP_FMT_npopx) {
                n_pop += op - OP_call0;
            }
#endif
        }
        if (stack_len < n_pop) {
            JS_ThrowInternalError(ctx, "stack underflow (op=%d, pc=%d)", op, pos);
            goto fail;
        }
        stack_len += oi->n_push - n_pop;
        if (stack_len > s->stack_len_max) {
            s->stack_len_max = stack_len;
            if (s->stack_len_max > JS_STACK_SIZE_MAX) {
                JS_ThrowInternalError(ctx, "stack overflow (op=%d, pc=%d)", op, pos);
                goto fail;
            }
        }
        switch(op) {
        case OP_tail_call:
        case OP_tail_call_method:
        case OP_return:
        case OP_return_undef:
        case OP_return_async:
        case OP_throw:
        case OP_throw_error:
        case OP_ret:
            goto done_insn;
        case OP_goto:
            diff = get_u32(bc_buf + pos + 1);
            pos_next = pos + 1 + diff;
            break;
#if SHORT_OPCODES
        case OP_goto16:
            diff = (int16_t)get_u16(bc_buf + pos + 1);
            pos_next = pos + 1 + diff;
            break;
        case OP_goto8:
            diff = (int8_t)bc_buf[pos + 1];
            pos_next = pos + 1 + diff;
            break;
        case OP_if_true8:
        case OP_if_false8:
            diff = (int8_t)bc_buf[pos + 1];
            if (ss_check(ctx, s, pos + 1 + diff, op, stack_len))
                goto fail;
            break;
#endif
        case OP_if_true:
        case OP_if_false:
        case OP_catch:
            diff = get_u32(bc_buf + pos + 1);
            if (ss_check(ctx, s, pos + 1 + diff, op, stack_len))
                goto fail;
            break;
        case OP_gosub:
            diff = get_u32(bc_buf + pos + 1);
            if (ss_check(ctx, s, pos + 1 + diff, op, stack_len + 1))
                goto fail;
            break;
        case OP_with_get_var:
        case OP_with_delete_var:
            diff = get_u32(bc_buf + pos + 5);
            if (ss_check(ctx, s, pos + 5 + diff, op, stack_len + 1))
                goto fail;
            break;
        case OP_with_make_ref:
        case OP_with_get_ref:
        case OP_with_get_ref_undef:
            diff = get_u32(bc_buf + pos + 5);
            if (ss_check(ctx, s, pos + 5 + diff, op, stack_len + 2))
                goto fail;
            break;
        case OP_with_put_var:
            diff = get_u32(bc_buf + pos + 5);
            if (ss_check(ctx, s, pos + 5 + diff, op, stack_len - 1))
                goto fail;
            break;
        default:
            break;
        }
        if (ss_check(ctx, s, pos_next, op, stack_len))
            goto fail;
    done_insn: ;
    }
    js_free(ctx, s->stack_level_tab);
    js_free(ctx, s->pc_stack);
    *pstack_size = s->stack_len_max;
    return 0;
 fail:
    js_free(ctx, s->stack_level_tab);
    js_free(ctx, s->pc_stack);
    *pstack_size = 0;
    return -1;
}

static int add_module_variables(JSContext *ctx, JSFunctionDef *fd)
{
    int i, idx;
    JSModuleDef *m = fd->module;
    JSExportEntry *me;
    JSGlobalVar *hf;
    /* The imported global variables were added as closure variables
       in js_parse_import(). We add here the module global
       variables. */
    for(i = 0; i < fd->global_var_count; i++) {
        hf = &fd->global_vars[i];
        if (add_closure_var(ctx, fd, TRUE, FALSE, i, hf->var_name, hf->is_const,
                            hf->is_lexical, FALSE) < 0)
            return -1;
    }
    /* resolve the variable names of the local exports */
    for(i = 0; i < m->export_entries_count; i++) {
        me = &m->export_entries[i];
        if (me->export_type == JS_EXPORT_TYPE_LOCAL) {
            idx = find_closure_var(ctx, fd, me->local_name);
            if (idx < 0) {
                JS_ThrowSyntaxErrorAtom(ctx, "exported variable '%s' does not exist",
                                        me->local_name);
                return -1;
            }
            me->u.local.var_idx = idx;
        }
    }
    return 0;
}

/* create a function object from a function definition. The function
   definition is freed. All the child functions are also created. It
   must be done this way to resolve all the variables. */
static JSValue js_create_function(JSContext *ctx, JSFunctionDef *fd)
{
    JSValue func_obj;
    JSFunctionBytecode *b;
    struct list_head *el, *el1;
    int stack_size, scope, idx;
    int function_size, byte_code_offset, cpool_offset;
    int closure_var_offset, vardefs_offset;
    /* recompute scope linkage */
    for (scope = 0; scope < fd->scope_count; scope++) {
        fd->scopes[scope].first = -1;
    }
    if (fd->has_parameter_expressions) {
        /* special end of variable list marker for the argument scope */
        fd->scopes[ARG_SCOPE_INDEX].first = ARG_SCOPE_END;
    }
    for (idx = 0; idx < fd->var_count; idx++) {
        JSVarDef *vd = &fd->vars[idx];
        vd->scope_next = fd->scopes[vd->scope_level].first;
        fd->scopes[vd->scope_level].first = idx;
    }
    for (scope = 2; scope < fd->scope_count; scope++) {
        JSVarScope *sd = &fd->scopes[scope];
        if (sd->first < 0)
            sd->first = fd->scopes[sd->parent].first;
    }
    for (idx = 0; idx < fd->var_count; idx++) {
        JSVarDef *vd = &fd->vars[idx];
        if (vd->scope_next < 0 && vd->scope_level > 1) {
            scope = fd->scopes[vd->scope_level].parent;
            vd->scope_next = fd->scopes[scope].first;
        }
    }
    /* if the function contains an eval call, the closure variables
       are used to compile the eval and they must be ordered by scope,
       so it is necessary to create the closure variables before any
       other variable lookup is done. */
    if (fd->has_eval_call)
        add_eval_variables(ctx, fd);
    /* add the module global variables in the closure */
    if (fd->module) {
        if (add_module_variables(ctx, fd))
            goto fail;
    }
    /* first create all the child functions */
    list_for_each_safe(el, el1, &fd->child_list) {
        JSFunctionDef *fd1;
        int cpool_idx;
        fd1 = list_entry(el, JSFunctionDef, link);
        cpool_idx = fd1->parent_cpool_idx;
        func_obj = js_create_function(ctx, fd1);
        if (JS_IsException(func_obj))
            goto fail;
        /* save it in the constant pool */
        assert(cpool_idx >= 0);
        fd->cpool[cpool_idx] = func_obj;
    }
#if defined(DUMP_BYTECODE) && (DUMP_BYTECODE & 4)
    if (!(fd->js_mode & JS_MODE_STRIP)) {
        printf("pass 1\n");
        dump_byte_code(ctx, 1, fd->byte_code.buf, fd->byte_code.size,
                       fd->args, fd->arg_count, fd->vars, fd->var_count,
                       fd->closure_var, fd->closure_var_count,
                       fd->cpool, fd->cpool_count, fd->source, fd->line_num,
                       fd->label_slots, NULL);
        printf("\n");
    }
#endif
    if (resolve_variables(ctx, fd))
        goto fail;
#if defined(DUMP_BYTECODE) && (DUMP_BYTECODE & 2)
    if (!(fd->js_mode & JS_MODE_STRIP)) {
        printf("pass 2\n");
        dump_byte_code(ctx, 2, fd->byte_code.buf, fd->byte_code.size,
                       fd->args, fd->arg_count, fd->vars, fd->var_count,
                       fd->closure_var, fd->closure_var_count,
                       fd->cpool, fd->cpool_count, fd->source, fd->line_num,
                       fd->label_slots, NULL);
        printf("\n");
    }
#endif
    if (resolve_labels(ctx, fd))
        goto fail;
    if (compute_stack_size(ctx, fd, &stack_size) < 0)
        goto fail;
    if (fd->js_mode & JS_MODE_STRIP) {
        function_size = offsetof(JSFunctionBytecode, debug);
    } else {
        function_size = sizeof(*b);
    }
    cpool_offset = function_size;
    function_size += fd->cpool_count * sizeof(*fd->cpool);
    vardefs_offset = function_size;
    if (!(fd->js_mode & JS_MODE_STRIP) || fd->has_eval_call) {
        function_size += (fd->arg_count + fd->var_count) * sizeof(*b->vardefs);
    }
    closure_var_offset = function_size;
    function_size += fd->closure_var_count * sizeof(*fd->closure_var);
    byte_code_offset = function_size;
    function_size += fd->byte_code.size;
    b = js_mallocz(ctx, function_size);
    if (!b)
        goto fail;
    b->header.ref_count = 1;
    b->byte_code_buf = (void *)((uint8_t*)b + byte_code_offset);
    b->byte_code_len = fd->byte_code.size;
    memcpy(b->byte_code_buf, fd->byte_code.buf, fd->byte_code.size);
    js_free(ctx, fd->byte_code.buf);
    fd->byte_code.buf = NULL;
    b->func_name = fd->func_name;
    if (fd->arg_count + fd->var_count > 0) {
        if ((fd->js_mode & JS_MODE_STRIP) && !fd->has_eval_call) {
            /* Strip variable definitions not needed at runtime */
            int i;
            for(i = 0; i < fd->var_count; i++) {
                JS_FreeAtom(ctx, fd->vars[i].var_name);
            }
            for(i = 0; i < fd->arg_count; i++) {
                JS_FreeAtom(ctx, fd->args[i].var_name);
            }
            for(i = 0; i < fd->closure_var_count; i++) {
                JS_FreeAtom(ctx, fd->closure_var[i].var_name);
                fd->closure_var[i].var_name = JS_ATOM_NULL;
            }
        } else {
            b->vardefs = (void *)((uint8_t*)b + vardefs_offset);
            if (fd->arg_count) {
              memcpy(b->vardefs, fd->args, fd->arg_count * sizeof(fd->args[0]));
            }
            if (fd->var_count) {
              memcpy(b->vardefs + fd->arg_count, fd->vars, fd->var_count * sizeof(fd->vars[0]));
            }
        }
        b->var_count = fd->var_count;
        b->arg_count = fd->arg_count;
        b->defined_arg_count = fd->defined_arg_count;
        js_free(ctx, fd->args);
        js_free(ctx, fd->vars);
    }
    b->cpool_count = fd->cpool_count;
    if (b->cpool_count) {
        b->cpool = (void *)((uint8_t*)b + cpool_offset);
        memcpy(b->cpool, fd->cpool, b->cpool_count * sizeof(*b->cpool));
    }
    js_free(ctx, fd->cpool);
    fd->cpool = NULL;
    b->stack_size = stack_size;
    if (fd->js_mode & JS_MODE_STRIP) {
        JS_FreeAtom(ctx, fd->filename);
        dbuf_free(&fd->pc2line);    // probably useless
    } else {
        /* XXX: source and pc2line info should be packed at the end of the
           JSFunctionBytecode structure, avoiding allocation overhead
         */
        b->has_debug = 1;
        b->debug.filename = fd->filename;
        b->debug.line_num = fd->line_num;
        //DynBuf pc2line;
        //compute_pc2line_info(fd, &pc2line);
        //js_free(ctx, fd->line_number_slots)
        b->debug.pc2line_buf = js_realloc(ctx, fd->pc2line.buf, fd->pc2line.size);
        if (!b->debug.pc2line_buf)
            b->debug.pc2line_buf = fd->pc2line.buf;
        b->debug.pc2line_len = fd->pc2line.size;
        b->debug.source = fd->source;
        b->debug.source_len = fd->source_len;
    }
    if (fd->scopes != fd->def_scope_array)
        js_free(ctx, fd->scopes);
    b->closure_var_count = fd->closure_var_count;
    if (b->closure_var_count) {
        b->closure_var = (void *)((uint8_t*)b + closure_var_offset);
        memcpy(b->closure_var, fd->closure_var, b->closure_var_count * sizeof(*b->closure_var));
    }
    js_free(ctx, fd->closure_var);
    fd->closure_var = NULL;
    b->has_prototype = fd->has_prototype;
    b->has_simple_parameter_list = fd->has_simple_parameter_list;
    b->js_mode = fd->js_mode;
    b->is_derived_class_constructor = fd->is_derived_class_constructor;
    b->func_kind = fd->func_kind;
    b->need_home_object = (fd->home_object_var_idx >= 0 ||
                           fd->need_home_object);
    b->new_target_allowed = fd->new_target_allowed;
    b->super_call_allowed = fd->super_call_allowed;
    b->super_allowed = fd->super_allowed;
    b->arguments_allowed = fd->arguments_allowed;
    b->backtrace_barrier = fd->backtrace_barrier;
    b->realm = JS_DupContext(ctx);
    add_gc_object(ctx->rt, &b->header, JS_GC_OBJ_TYPE_FUNCTION_BYTECODE);
#if defined(DUMP_BYTECODE) && (DUMP_BYTECODE & 1)
    if (!(fd->js_mode & JS_MODE_STRIP)) {
        js_dump_function_bytecode(ctx, b);
    }
#endif
    if (fd->parent) {
        /* remove from parent list */
        list_del(&fd->link);
    }
    js_free(ctx, fd);
    return JS_MKPTR(JS_TAG_FUNCTION_BYTECODE, b);
 fail:
    js_free_function_def(ctx, fd);
    return JS_EXCEPTION;
}

static JSValue JS_EvalFunctionInternal(JSContext *ctx, JSValue fun_obj,
                                       JSValueConst this_obj,
                                       JSVarRef **var_refs, JSStackFrame *sf)
{
    JSValue ret_val;
    uint32_t tag;
    tag = JS_VALUE_GET_TAG(fun_obj);
    if (tag == JS_TAG_FUNCTION_BYTECODE) {
        fun_obj = js_closure(ctx, fun_obj, var_refs, sf);
        ret_val = JS_CallFree(ctx, fun_obj, this_obj, 0, NULL);
    } else if (tag == JS_TAG_MODULE) {
        JSModuleDef *m;
        m = JS_VALUE_GET_PTR(fun_obj);
        /* the module refcount should be >= 2 */
        JS_FreeValue(ctx, fun_obj);
        if (js_create_module_function(ctx, m) < 0)
            goto fail;
        if (js_link_module(ctx, m) < 0)
            goto fail;
        ret_val = js_evaluate_module(ctx, m);
        if (JS_IsException(ret_val)) {
        fail:
            js_free_modules(ctx, JS_FREE_MODULE_NOT_EVALUATED);
            return JS_EXCEPTION;
        }
    } else {
        JS_FreeValue(ctx, fun_obj);
        ret_val = JS_ThrowTypeError(ctx, "bytecode function expected");
    }
    return ret_val;
}

JSValue JS_EvalFunction(JSContext *ctx, JSValue fun_obj)
{
    return JS_EvalFunctionInternal(ctx, fun_obj, ctx->global_obj, NULL, NULL);
}

static void skip_shebang(JSParseState *s)
{
    const uint8_t *p = s->buf_ptr;
    int c;
    if (p[0] == '#' && p[1] == '!') {
        p += 2;
        while (p < s->buf_end) {
            if (*p == '\n' || *p == '\r') {
                break;
            } else if (*p >= 0x80) {
                c = unicode_from_utf8(p, UTF8_CHAR_LEN_MAX, &p);
                if (c == CP_LS || c == CP_PS) {
                    break;
                } else if (c == -1) {
                    p++; /* skip invalid UTF-8 */
                }
            } else {
                p++;
            }
        }
        s->buf_ptr = p;
    }
}

/* 'input' must be zero terminated i.e. input[input_len] = '\0'. */
static JSValue __JS_EvalInternal(JSContext *ctx, JSValueConst this_obj,
                                 const char *input, size_t input_len,
                                 const char *filename, int flags, int scope_idx)
{
    JSParseState s1, *s = &s1;
    int err, js_mode, eval_type;
    JSValue fun_obj, ret_val;
    JSStackFrame *sf;
    JSVarRef **var_refs;
    JSFunctionBytecode *b;
    JSFunctionDef *fd;
    JSModuleDef *m;
    js_parse_init(ctx, s, input, input_len, filename);
    skip_shebang(s);
    eval_type = flags & JS_EVAL_TYPE_MASK;
    m = NULL;
    if (eval_type == JS_EVAL_TYPE_DIRECT) {
        JSObject *p;
        sf = ctx->rt->current_stack_frame;
        assert(sf != NULL);
        assert(JS_VALUE_GET_TAG(sf->cur_func) == JS_TAG_OBJECT);
        p = JS_VALUE_GET_OBJ(sf->cur_func);
        assert(js_class_has_bytecode(p->class_id));
        b = p->u.func.function_bytecode;
        var_refs = p->u.func.var_refs;
        js_mode = b->js_mode;
    } else {
        sf = NULL;
        b = NULL;
        var_refs = NULL;
        js_mode = 0;
        if (flags & JS_EVAL_FLAG_STRICT)
            js_mode |= JS_MODE_STRICT;
        if (flags & JS_EVAL_FLAG_STRIP)
            js_mode |= JS_MODE_STRIP;
        if (eval_type == JS_EVAL_TYPE_MODULE) {
            JSAtom module_name = JS_NewAtom(ctx, filename);
            if (module_name == JS_ATOM_NULL)
                return JS_EXCEPTION;
            m = js_new_module_def(ctx, module_name);
            if (!m)
                return JS_EXCEPTION;
            js_mode |= JS_MODE_STRICT;
        }
    }
    fd = js_new_function_def(ctx, NULL, TRUE, FALSE, filename, 1);
    if (!fd)
        goto fail1;
    s->cur_func = fd;
    fd->eval_type = eval_type;
    fd->has_this_binding = (eval_type != JS_EVAL_TYPE_DIRECT);
    fd->backtrace_barrier = ((flags & JS_EVAL_FLAG_BACKTRACE_BARRIER) != 0);
    if (eval_type == JS_EVAL_TYPE_DIRECT) {
        fd->new_target_allowed = b->new_target_allowed;
        fd->super_call_allowed = b->super_call_allowed;
        fd->super_allowed = b->super_allowed;
        fd->arguments_allowed = b->arguments_allowed;
    } else {
        fd->new_target_allowed = FALSE;
        fd->super_call_allowed = FALSE;
        fd->super_allowed = FALSE;
        fd->arguments_allowed = TRUE;
    }
    fd->js_mode = js_mode;
    fd->func_name = JS_DupAtom(ctx, JS_ATOM__eval_);
    if (b) {
        if (add_closure_variables(ctx, fd, b, scope_idx))
            goto fail;
    }
    fd->module = m;
    s->is_module = (m != NULL);
    s->allow_html_comments = !s->is_module;
    push_scope(s); /* body scope */
    fd->body_scope = fd->scope_level;
    err = js_parse_program(s);
    if (err) {
    fail:
        free_token(s, &s->token);
        js_free_function_def(ctx, fd);
        goto fail1;
    }
    /* create the function object and all the enclosed functions */
    fun_obj = js_create_function(ctx, fd);
    if (JS_IsException(fun_obj))
        goto fail1;
    /* Could add a flag to avoid resolution if necessary */
    if (m) {
        m->func_obj = fun_obj;
        if (js_resolve_module(ctx, m) < 0)
            goto fail1;
        fun_obj = JS_DupValue(ctx, JS_MKPTR(JS_TAG_MODULE, m));
    }
    if (flags & JS_EVAL_FLAG_COMPILE_ONLY) {
        ret_val = fun_obj;
    } else {
        ret_val = JS_EvalFunctionInternal(ctx, fun_obj, this_obj, var_refs, sf);
    }
    return ret_val;
 fail1:
    /* XXX: should free all the unresolved dependencies */
    if (m)
        js_free_module_def(ctx, m);
    return JS_EXCEPTION;
}

/* the indirection is needed to make 'eval' optional */
static JSValue JS_EvalInternal(JSContext *ctx, JSValueConst this_obj,
                               const char *input, size_t input_len,
                               const char *filename, int flags, int scope_idx)
{
    if (UNLIKELY(!ctx->eval_internal)) {
        return JS_ThrowTypeError(ctx, "eval is not supported");
    }
    return ctx->eval_internal(ctx, this_obj, input, input_len, filename,
                              flags, scope_idx);
}

JSValue JS_EvalObject(JSContext *ctx, JSValueConst this_obj,
                      JSValueConst val, int flags, int scope_idx)
{
    JSValue ret;
    const char *str;
    size_t len;
    if (!JS_IsString(val))
        return JS_DupValue(ctx, val);
    str = JS_ToCStringLen(ctx, &len, val);
    if (!str)
        return JS_EXCEPTION;
    ret = JS_EvalInternal(ctx, this_obj, str, len, "<input>", flags, scope_idx);
    JS_FreeCString(ctx, str);
    return ret;

}

JSValue JS_EvalThis(JSContext *ctx, JSValueConst this_obj,
                    const char *input, size_t input_len,
                    const char *filename, int eval_flags)
{
    int eval_type = eval_flags & JS_EVAL_TYPE_MASK;
    JSValue ret;
    assert(eval_type == JS_EVAL_TYPE_GLOBAL ||
           eval_type == JS_EVAL_TYPE_MODULE);
    ret = JS_EvalInternal(ctx, this_obj, input, input_len, filename,
                          eval_flags, -1);
    return ret;
}

JSValue JS_Eval(JSContext *ctx, const char *input, size_t input_len,
                const char *filename, int eval_flags)
{
    return JS_EvalThis(ctx, ctx->global_obj, input, input_len, filename,
                       eval_flags);
}

int JS_ResolveModule(JSContext *ctx, JSValueConst obj)
{
    if (JS_VALUE_GET_TAG(obj) == JS_TAG_MODULE) {
        JSModuleDef *m = JS_VALUE_GET_PTR(obj);
        if (js_resolve_module(ctx, m) < 0) {
            js_free_modules(ctx, JS_FREE_MODULE_NOT_RESOLVED);
            return -1;
        }
    }
    return 0;
}

/*******************************************************************/
/* runtime functions & objects */

static JSValue js_boolean_constructor(JSContext *ctx, JSValueConst this_val,
                                      int argc, JSValueConst *argv);
static JSValue js_number_constructor(JSContext *ctx, JSValueConst this_val,
                                     int argc, JSValueConst *argv);

int check_function(JSContext *ctx, JSValueConst obj)
{
    if (LIKELY(JS_IsFunction(ctx, obj)))
        return 0;
    JS_ThrowTypeError(ctx, "not a function");
    return -1;
}

int check_exception_free(JSContext *ctx, JSValue obj)
{
    JS_FreeValue(ctx, obj);
    return JS_IsException(obj);
}

static JSAtom find_atom(JSContext *ctx, const char *name)
{
    JSAtom atom;
    int len;
    if (*name == '[') {
        name++;
        len = strlen(name) - 1;
        /* We assume 8 bit non null strings, which is the case for these
           symbols */
        for(atom = JS_ATOM_Symbol_toPrimitive; atom < JS_ATOM_END; atom++) {
            JSAtomStruct *p = ctx->rt->atom_array[atom];
            JSString *str = p;
            if (str->len == len && !memcmp(str->u.str8, name, len))
                return JS_DupAtom(ctx, atom);
        }
        abort();
    } else {
        atom = JS_NewAtom(ctx, name);
    }
    return atom;
}

static JSValue JS_InstantiateFunctionListItem2(JSContext *ctx, JSObject *p,
                                               JSAtom atom, void *opaque)
{
    const JSCFunctionListEntry *e = opaque;
    JSValue val;
    switch(e->def_type) {
    case JS_DEF_CFUNC:
        val = JS_NewCFunction2(ctx, e->u.func.cfunc.generic,
                               e->name, e->u.func.length, e->u.func.cproto, e->magic);
        break;
    case JS_DEF_PROP_STRING:
        val = JS_NewAtomString(ctx, e->u.str);
        break;
    case JS_DEF_OBJECT:
        val = JS_NewObject(ctx);
        JS_SetPropertyFunctionList(ctx, val, e->u.prop_list.tab, e->u.prop_list.len);
        break;
    default:
        abort();
    }
    return val;
}

static int JS_InstantiateFunctionListItem(JSContext *ctx, JSValueConst obj,
                                          JSAtom atom,
                                          const JSCFunctionListEntry *e)
{
    JSValue val;
    int prop_flags = e->prop_flags;
    switch(e->def_type) {
    case JS_DEF_ALIAS: /* using autoinit for aliases is not safe */
        {
            JSAtom atom1 = find_atom(ctx, e->u.alias.name);
            switch (e->u.alias.base) {
            case -1:
                val = JS_GetProperty(ctx, obj, atom1);
                break;
            case 0:
                val = JS_GetProperty(ctx, ctx->global_obj, atom1);
                break;
            case 1:
                val = JS_GetProperty(ctx, ctx->class_proto[JS_CLASS_ARRAY], atom1);
                break;
            default:
                abort();
            }
            JS_FreeAtom(ctx, atom1);
            if (atom == JS_ATOM_Symbol_toPrimitive) {
                /* Symbol.toPrimitive functions are not writable */
                prop_flags = JS_PROP_CONFIGURABLE;
            } else if (atom == JS_ATOM_Symbol_hasInstance) {
                /* Function.prototype[Symbol.hasInstance] is not writable nor configurable */
                prop_flags = 0;
            }
        }
        break;
    case JS_DEF_CFUNC:
        if (atom == JS_ATOM_Symbol_toPrimitive) {
            /* Symbol.toPrimitive functions are not writable */
            prop_flags = JS_PROP_CONFIGURABLE;
        } else if (atom == JS_ATOM_Symbol_hasInstance) {
            /* Function.prototype[Symbol.hasInstance] is not writable nor configurable */
            prop_flags = 0;
        }
        JS_DefineAutoInitProperty(ctx, obj, atom, JS_AUTOINIT_ID_PROP,
                                  (void *)e, prop_flags);
        return 0;
    case JS_DEF_CGETSET: /* XXX: use autoinit again ? */
    case JS_DEF_CGETSET_MAGIC:
        {
            JSValue getter, setter;
            char buf[64];
            getter = JS_UNDEFINED;
            if (e->u.getset.get.generic) {
                snprintf(buf, sizeof(buf), "get %s", e->name);
                getter = JS_NewCFunction2(ctx, e->u.getset.get.generic,
                                          buf, 0, e->def_type == JS_DEF_CGETSET_MAGIC ? JS_CFUNC_getter_magic : JS_CFUNC_getter,
                                          e->magic);
            }
            setter = JS_UNDEFINED;
            if (e->u.getset.set.generic) {
                snprintf(buf, sizeof(buf), "set %s", e->name);
                setter = JS_NewCFunction2(ctx, e->u.getset.set.generic,
                                          buf, 1, e->def_type == JS_DEF_CGETSET_MAGIC ? JS_CFUNC_setter_magic : JS_CFUNC_setter,
                                          e->magic);
            }
            JS_DefinePropertyGetSet(ctx, obj, atom, getter, setter, prop_flags);
            return 0;
        }
        break;
    case JS_DEF_PROP_INT32:
        val = JS_NewInt32(ctx, e->u.i32);
        break;
    case JS_DEF_PROP_INT64:
        val = JS_NewInt64(ctx, e->u.i64);
        break;
    case JS_DEF_PROP_DOUBLE:
        val = __JS_NewFloat64(ctx, e->u.f64);
        break;
    case JS_DEF_PROP_UNDEFINED:
        val = JS_UNDEFINED;
        break;
    case JS_DEF_PROP_STRING:
    case JS_DEF_OBJECT:
        JS_DefineAutoInitProperty(ctx, obj, atom, JS_AUTOINIT_ID_PROP,
                                  (void *)e, prop_flags);
        return 0;
    default:
        abort();
    }
    JS_DefinePropertyValue(ctx, obj, atom, val, prop_flags);
    return 0;
}

void JS_SetPropertyFunctionList(JSContext *ctx, JSValueConst obj,
                                const JSCFunctionListEntry *tab, int len)
{
    int i;
    for (i = 0; i < len; i++) {
        const JSCFunctionListEntry *e = &tab[i];
        JSAtom atom = find_atom(ctx, e->name);
        JS_InstantiateFunctionListItem(ctx, obj, atom, e);
        JS_FreeAtom(ctx, atom);
    }
}

int JS_AddModuleExportList(JSContext *ctx, JSModuleDef *m,
                           const JSCFunctionListEntry *tab, int len)
{
    int i;
    for(i = 0; i < len; i++) {
        if (JS_AddModuleExport(ctx, m, tab[i].name))
            return -1;
    }
    return 0;
}

int JS_SetModuleExportList(JSContext *ctx, JSModuleDef *m,
                           const JSCFunctionListEntry *tab, int len)
{
    int i;
    JSValue val;
    for(i = 0; i < len; i++) {
        const JSCFunctionListEntry *e = &tab[i];
        switch(e->def_type) {
        case JS_DEF_CFUNC:
            val = JS_NewCFunction2(ctx, e->u.func.cfunc.generic,
                                   e->name, e->u.func.length, e->u.func.cproto, e->magic);
            break;
        case JS_DEF_PROP_STRING:
            val = JS_NewString(ctx, e->u.str);
            break;
        case JS_DEF_PROP_INT32:
            val = JS_NewInt32(ctx, e->u.i32);
            break;
        case JS_DEF_PROP_INT64:
            val = JS_NewInt64(ctx, e->u.i64);
            break;
        case JS_DEF_PROP_DOUBLE:
            val = __JS_NewFloat64(ctx, e->u.f64);
            break;
        case JS_DEF_OBJECT:
            val = JS_NewObject(ctx);
            JS_SetPropertyFunctionList(ctx, val, e->u.prop_list.tab, e->u.prop_list.len);
            break;
        default:
            abort();
        }
        if (JS_SetModuleExport(ctx, m, e->name, val))
            return -1;
    }
    return 0;
}

/* Note: 'func_obj' is not necessarily a constructor */
void JS_SetConstructor2(JSContext *ctx,
                        JSValueConst func_obj,
                        JSValueConst proto,
                        int proto_flags, int ctor_flags)
{
    JS_DefinePropertyValue(ctx, func_obj, JS_ATOM_prototype,
                           JS_DupValue(ctx, proto), proto_flags);
    JS_DefinePropertyValue(ctx, proto, JS_ATOM_constructor,
                           JS_DupValue(ctx, func_obj),
                           ctor_flags);
    set_cycle_flag(ctx, func_obj);
    set_cycle_flag(ctx, proto);
}

void JS_SetConstructor(JSContext *ctx, JSValueConst func_obj,
                       JSValueConst proto)
{
    JS_SetConstructor2(ctx, func_obj, proto,
                       0, JS_PROP_WRITABLE | JS_PROP_CONFIGURABLE);
}

void JS_NewGlobalCConstructor2(JSContext *ctx,
                               JSValue func_obj,
                               const char *name,
                               JSValueConst proto)
{
    JS_DefinePropertyValueStr(ctx, ctx->global_obj, name,
                           JS_DupValue(ctx, func_obj),
                           JS_PROP_WRITABLE | JS_PROP_CONFIGURABLE);
    JS_SetConstructor(ctx, func_obj, proto);
    JS_FreeValue(ctx, func_obj);
}

JSValueConst JS_NewGlobalCConstructor(JSContext *ctx, const char *name,
                                      JSCFunction *func, int length,
                                      JSValueConst proto)
{
    JSValue func_obj;
    func_obj = JS_NewCFunction2(ctx, func, name, length, JS_CFUNC_constructor_or_func, 0);
    JS_NewGlobalCConstructor2(ctx, func_obj, name, proto);
    return func_obj;
}

JSValueConst JS_NewGlobalCConstructorOnly(JSContext *ctx, const char *name,
                                          JSCFunction *func, int length,
                                          JSValueConst proto)
{
    JSValue func_obj;
    func_obj = JS_NewCFunction2(ctx, func, name, length, JS_CFUNC_constructor, 0);
    JS_NewGlobalCConstructor2(ctx, func_obj, name, proto);
    return func_obj;
}

static JSValue js_global_eval(JSContext *ctx, JSValueConst this_val,
                              int argc, JSValueConst *argv)
{
    return JS_EvalObject(ctx, ctx->global_obj, argv[0], JS_EVAL_TYPE_INDIRECT, -1);
}

static JSValue js_global_isNaN(JSContext *ctx, JSValueConst this_val,
                               int argc, JSValueConst *argv)
{
    double d;

    /* XXX: does this work for bigfloat? */
    if (UNLIKELY(JS_ToFloat64(ctx, &d, argv[0])))
        return JS_EXCEPTION;
    return JS_NewBool(ctx, isnan(d));
}

static JSValue js_global_isFinite(JSContext *ctx, JSValueConst this_val,
                                  int argc, JSValueConst *argv)
{
    BOOL res;
    double d;
    if (UNLIKELY(JS_ToFloat64(ctx, &d, argv[0])))
        return JS_EXCEPTION;
    res = isfinite(d);
    return JS_NewBool(ctx, res);
}

/* Object class */

/* Function class */

static JSValue js_function_proto(JSContext *ctx, JSValueConst this_val,
                                 int argc, JSValueConst *argv)
{
    return JS_UNDEFINED;
}

int js_get_length32(JSContext *ctx, uint32_t *pres, JSValueConst obj)
{
    JSValue len_val;
    len_val = JS_GetProperty(ctx, obj, JS_ATOM_length);
    if (JS_IsException(len_val)) {
        *pres = 0;
        return -1;
    }
    return JS_ToUint32Free(ctx, pres, len_val);
}

int js_get_length64(JSContext *ctx, int64_t *pres, JSValueConst obj)
{
    JSValue len_val;
    len_val = JS_GetProperty(ctx, obj, JS_ATOM_length);
    if (JS_IsException(len_val)) {
        *pres = 0;
        return -1;
    }
    return JS_ToLengthFree(ctx, pres, len_val);
}

void free_arg_list(JSContext *ctx, JSValue *tab, uint32_t len)
{
    uint32_t i;
    for(i = 0; i < len; i++) {
        JS_FreeValue(ctx, tab[i]);
    }
    js_free(ctx, tab);
}

/* XXX: should use ValueArray */
JSValue *build_arg_list(JSContext *ctx, uint32_t *plen,
                        JSValueConst array_arg)
{
    uint32_t len, i;
    JSValue *tab, ret;
    JSObject *p;
    if (JS_VALUE_GET_TAG(array_arg) != JS_TAG_OBJECT) {
        JS_ThrowTypeError(ctx, "not a object");
        return NULL;
    }
    if (js_get_length32(ctx, &len, array_arg))
        return NULL;
    if (len > JS_MAX_LOCAL_VARS) {
        JS_ThrowInternalError(ctx, "too many arguments");
        return NULL;
    }
    /* avoid allocating 0 bytes */
    tab = js_mallocz(ctx, sizeof(tab[0]) * max_uint32(1, len));
    if (!tab)
        return NULL;
    p = JS_VALUE_GET_OBJ(array_arg);
    if ((p->class_id == JS_CLASS_ARRAY || p->class_id == JS_CLASS_ARGUMENTS) &&
        p->fast_array &&
        len == p->u.array.count) {
        for(i = 0; i < len; i++) {
            tab[i] = JS_DupValue(ctx, p->u.array.u.values[i]);
        }
    } else {
        for(i = 0; i < len; i++) {
            ret = JS_GetPropertyUint32(ctx, array_arg, i);
            if (JS_IsException(ret)) {
                free_arg_list(ctx, tab, i);
                return NULL;
            }
            tab[i] = ret;
        }
    }
    *plen = len;
    return tab;
}

/* magic value: 0 = normal apply, 1 = apply for constructor, 2 =
   Reflect.apply */
JSValue js_function_apply(JSContext *ctx, JSValueConst this_val,
                          int argc, JSValueConst *argv, int magic)
{
    JSValueConst this_arg, array_arg;
    uint32_t len;
    JSValue *tab, ret;
    if (check_function(ctx, this_val))
        return JS_EXCEPTION;
    this_arg = argv[0];
    array_arg = argv[1];
    if ((JS_VALUE_GET_TAG(array_arg) == JS_TAG_UNDEFINED ||
         JS_VALUE_GET_TAG(array_arg) == JS_TAG_NULL) && magic != 2) {
        return JS_Call(ctx, this_val, this_arg, 0, NULL);
    }
    tab = build_arg_list(ctx, &len, array_arg);
    if (!tab)
        return JS_EXCEPTION;
    if (magic & 1) {
        ret = JS_CallConstructor2(ctx, this_val, this_arg, len, (JSValueConst *)tab);
    } else {
        ret = JS_Call(ctx, this_val, this_arg, len, (JSValueConst *)tab);
    }
    free_arg_list(ctx, tab, len);
    return ret;
}

static JSValue js_function_call(JSContext *ctx, JSValueConst this_val,
                                int argc, JSValueConst *argv)
{
    if (argc <= 0) {
        return JS_Call(ctx, this_val, JS_UNDEFINED, 0, NULL);
    } else {
        return JS_Call(ctx, this_val, argv[0], argc - 1, argv + 1);
    }
}

static JSValue js_function_bind(JSContext *ctx, JSValueConst this_val,
                                int argc, JSValueConst *argv)
{
    JSBoundFunction *bf;
    JSValue func_obj, name1, len_val;
    JSObject *p;
    int arg_count, i, ret;
    if (check_function(ctx, this_val))
        return JS_EXCEPTION;
    func_obj = JS_NewObjectProtoClass(ctx, ctx->function_proto,
                                 JS_CLASS_BOUND_FUNCTION);
    if (JS_IsException(func_obj))
        return JS_EXCEPTION;
    p = JS_VALUE_GET_OBJ(func_obj);
    p->is_constructor = JS_IsConstructor(ctx, this_val);
    arg_count = max_int(0, argc - 1);
    bf = js_malloc(ctx, sizeof(*bf) + arg_count * sizeof(JSValue));
    if (!bf)
        goto exception;
    bf->func_obj = JS_DupValue(ctx, this_val);
    bf->this_val = JS_DupValue(ctx, argv[0]);
    bf->argc = arg_count;
    for(i = 0; i < arg_count; i++) {
        bf->argv[i] = JS_DupValue(ctx, argv[i + 1]);
    }
    p->u.bound_function = bf;
    /* XXX: the spec could be simpler by only using GetOwnProperty */
    ret = JS_GetOwnProperty(ctx, NULL, this_val, JS_ATOM_length);
    if (ret < 0)
        goto exception;
    if (!ret) {
        len_val = JS_NewInt32(ctx, 0);
    } else {
        len_val = JS_GetProperty(ctx, this_val, JS_ATOM_length);
        if (JS_IsException(len_val))
            goto exception;
        if (JS_VALUE_GET_TAG(len_val) == JS_TAG_INT) {
            /* most common case */
            int len1 = JS_VALUE_GET_INT(len_val);
            if (len1 <= arg_count)
                len1 = 0;
            else
                len1 -= arg_count;
            len_val = JS_NewInt32(ctx, len1);
        } else if (JS_VALUE_GET_NORM_TAG(len_val) == JS_TAG_FLOAT64) {
            double d = JS_VALUE_GET_FLOAT64(len_val);
            if (isnan(d)) {
                d = 0.0;
            } else {
                d = trunc(d);
                if (d <= (double)arg_count)
                    d = 0.0;
                else
                    d -= (double)arg_count; /* also converts -0 to +0 */
            }
            len_val = JS_NewFloat64(ctx, d);
        } else {
            JS_FreeValue(ctx, len_val);
            len_val = JS_NewInt32(ctx, 0);
        }
    }
    JS_DefinePropertyValue(ctx, func_obj, JS_ATOM_length,
                           len_val, JS_PROP_CONFIGURABLE);
    name1 = JS_GetProperty(ctx, this_val, JS_ATOM_name);
    if (JS_IsException(name1))
        goto exception;
    if (!JS_IsString(name1)) {
        JS_FreeValue(ctx, name1);
        name1 = JS_AtomToString(ctx, JS_ATOM_empty_string);
    }
    name1 = JS_ConcatString3(ctx, "bound ", name1, "");
    if (JS_IsException(name1))
        goto exception;
    JS_DefinePropertyValue(ctx, func_obj, JS_ATOM_name, name1,
                           JS_PROP_CONFIGURABLE);
    return func_obj;
 exception:
    JS_FreeValue(ctx, func_obj);
    return JS_EXCEPTION;
}

static JSValue js_function_toString(JSContext *ctx, JSValueConst this_val,
                                    int argc, JSValueConst *argv)
{
    JSObject *p;
    JSFunctionKindEnum func_kind = JS_FUNC_NORMAL;
    if (check_function(ctx, this_val))
        return JS_EXCEPTION;
    p = JS_VALUE_GET_OBJ(this_val);
    if (js_class_has_bytecode(p->class_id)) {
        JSFunctionBytecode *b = p->u.func.function_bytecode;
        if (b->has_debug && b->debug.source) {
            return JS_NewStringLen(ctx, b->debug.source, b->debug.source_len);
        }
        func_kind = b->func_kind;
    }
    {
        JSValue name;
        const char *pref, *suff;
        switch(func_kind) {
        default:
        case JS_FUNC_NORMAL:
            pref = "function ";
            break;
        case JS_FUNC_GENERATOR:
            pref = "function *";
            break;
        case JS_FUNC_ASYNC:
            pref = "async function ";
            break;
        case JS_FUNC_ASYNC_GENERATOR:
            pref = "async function *";
            break;
        }
        suff = "() {\n    [native code]\n}";
        name = JS_GetProperty(ctx, this_val, JS_ATOM_name);
        if (JS_IsUndefined(name))
            name = JS_AtomToString(ctx, JS_ATOM_empty_string);
        return JS_ConcatString3(ctx, pref, name, suff);
    }
}

static JSValue js_function_hasInstance(JSContext *ctx, JSValueConst this_val,
                                       int argc, JSValueConst *argv)
{
    int ret;
    ret = JS_OrdinaryIsInstanceOf(ctx, argv[0], this_val);
    if (ret < 0)
        return JS_EXCEPTION;
    else
        return JS_NewBool(ctx, ret);
}

static const JSCFunctionListEntry js_function_proto_funcs[] = {
    JS_CFUNC_DEF("call", 1, js_function_call ),
    JS_CFUNC_MAGIC_DEF("apply", 2, js_function_apply, 0 ),
    JS_CFUNC_DEF("bind", 1, js_function_bind ),
    JS_CFUNC_DEF("toString", 0, js_function_toString ),
    JS_CFUNC_DEF("[Symbol.hasInstance]", 1, js_function_hasInstance ),
    JS_CGETSET_DEF("fileName", js_function_proto_fileName, NULL ),
    JS_CGETSET_DEF("lineNumber", js_function_proto_lineNumber, NULL ),
};

/* Error class */

static JSValue iterator_to_array(JSContext *ctx, JSValueConst items)
{
    JSValue iter, next_method = JS_UNDEFINED;
    JSValue v, r = JS_UNDEFINED;
    int64_t k;
    BOOL done;

    iter = JS_GetIterator(ctx, items, FALSE);
    if (JS_IsException(iter))
        goto exception;
    next_method = JS_GetProperty(ctx, iter, JS_ATOM_next);
    if (JS_IsException(next_method))
        goto exception;
    r = JS_NewArray(ctx);
    if (JS_IsException(r))
        goto exception;
    for (k = 0;; k++) {
        v = JS_IteratorNext(ctx, iter, next_method, 0, NULL, &done);
        if (JS_IsException(v))
            goto exception_close;
        if (done)
            break;
        if (JS_DefinePropertyValueInt64(ctx, r, k, v,
                                        JS_PROP_C_W_E | JS_PROP_THROW) < 0)
            goto exception_close;
    }
 done:
    JS_FreeValue(ctx, next_method);
    JS_FreeValue(ctx, iter);
    return r;
 exception_close:
    JS_IteratorClose(ctx, iter, TRUE);
 exception:
    JS_FreeValue(ctx, r);
    r = JS_EXCEPTION;
    goto done;
}

static JSValue js_error_constructor(JSContext *ctx, JSValueConst new_target,
                                    int argc, JSValueConst *argv, int magic)
{
    JSValue obj, msg, proto;
    JSValueConst message;

    if (JS_IsUndefined(new_target))
        new_target = JS_GetActiveFunction(ctx);
    proto = JS_GetProperty(ctx, new_target, JS_ATOM_prototype);
    if (JS_IsException(proto))
        return proto;
    if (!JS_IsObject(proto)) {
        JSContext *realm;
        JSValueConst proto1;

        JS_FreeValue(ctx, proto);
        realm = JS_GetFunctionRealm(ctx, new_target);
        if (!realm)
            return JS_EXCEPTION;
        if (magic < 0) {
            proto1 = realm->class_proto[JS_CLASS_ERROR];
        } else {
            proto1 = realm->native_error_proto[magic];
        }
        proto = JS_DupValue(ctx, proto1);
    }
    obj = JS_NewObjectProtoClass(ctx, proto, JS_CLASS_ERROR);
    JS_FreeValue(ctx, proto);
    if (JS_IsException(obj))
        return obj;
    if (magic == JS_AGGREGATE_ERROR) {
        message = argv[1];
    } else {
        message = argv[0];
    }

    if (!JS_IsUndefined(message)) {
        msg = JS_ToString(ctx, message);
        if (UNLIKELY(JS_IsException(msg)))
            goto exception;
        JS_DefinePropertyValue(ctx, obj, JS_ATOM_message, msg,
                               JS_PROP_WRITABLE | JS_PROP_CONFIGURABLE);
    }

    if (magic == JS_AGGREGATE_ERROR) {
        JSValue error_list = iterator_to_array(ctx, argv[0]);
        if (JS_IsException(error_list))
            goto exception;
        JS_DefinePropertyValue(ctx, obj, JS_ATOM_errors, error_list,
                               JS_PROP_WRITABLE | JS_PROP_CONFIGURABLE);
    }

    /* skip the Error() function in the backtrace */
    build_backtrace(ctx, obj, NULL, 0, JS_BACKTRACE_FLAG_SKIP_FIRST_LEVEL);
    return obj;
 exception:
    JS_FreeValue(ctx, obj);
    return JS_EXCEPTION;
}

static JSValue js_error_toString(JSContext *ctx, JSValueConst this_val,
                                 int argc, JSValueConst *argv)
{
    JSValue name, msg;

    if (!JS_IsObject(this_val))
        return JS_ThrowTypeErrorNotAnObject(ctx);
    name = JS_GetProperty(ctx, this_val, JS_ATOM_name);
    if (JS_IsUndefined(name))
        name = JS_AtomToString(ctx, JS_ATOM_Error);
    else
        name = JS_ToStringFree(ctx, name);
    if (JS_IsException(name))
        return JS_EXCEPTION;

    msg = JS_GetProperty(ctx, this_val, JS_ATOM_message);
    if (JS_IsUndefined(msg))
        msg = JS_AtomToString(ctx, JS_ATOM_empty_string);
    else
        msg = JS_ToStringFree(ctx, msg);
    if (JS_IsException(msg)) {
        JS_FreeValue(ctx, name);
        return JS_EXCEPTION;
    }
    if (!JS_IsEmptyString(name) && !JS_IsEmptyString(msg))
        name = JS_ConcatString3(ctx, "", name, ": ");
    return JS_ConcatString(ctx, name, msg);
}

static const JSCFunctionListEntry js_error_proto_funcs[] = {
    JS_CFUNC_DEF("toString", 0, js_error_toString ),
    JS_PROP_STRING_DEF("name", "Error", JS_PROP_WRITABLE | JS_PROP_CONFIGURABLE ),
    JS_PROP_STRING_DEF("message", "", JS_PROP_WRITABLE | JS_PROP_CONFIGURABLE ),
};

/* Number */

static JSValue js_number_constructor(JSContext *ctx, JSValueConst new_target,
                                     int argc, JSValueConst *argv)
{
    JSValue val, obj;
    if (argc == 0) {
        val = JS_NewInt32(ctx, 0);
    } else {
        val = JS_ToNumeric(ctx, argv[0]);
        if (JS_IsException(val))
            return val;
        switch(JS_VALUE_GET_TAG(val)) {
#ifdef CONFIG_BIGNUM
        case JS_TAG_BIG_INT:
        case JS_TAG_BIG_FLOAT:
            {
                JSBigFloat *p = JS_VALUE_GET_PTR(val);
                double d;
                bf_get_float64(&p->num, &d, BF_RNDN);
                JS_FreeValue(ctx, val);
                val = __JS_NewFloat64(ctx, d);
            }
            break;
        case JS_TAG_BIG_DECIMAL:
            val = JS_ToStringFree(ctx, val);
            if (JS_IsException(val))
                return val;
            val = JS_ToNumberFree(ctx, val);
            if (JS_IsException(val))
                return val;
            break;
#endif
        default:
            break;
        }
    }
    if (!JS_IsUndefined(new_target)) {
        obj = js_create_from_ctor(ctx, new_target, JS_CLASS_NUMBER);
        if (!JS_IsException(obj))
            JS_SetObjectData(ctx, obj, val);
        return obj;
    } else {
        return val;
    }
}

#if 0
static JSValue js_number___toInteger(JSContext *ctx, JSValueConst this_val,
                                     int argc, JSValueConst *argv)
{
    return JS_ToIntegerFree(ctx, JS_DupValue(ctx, argv[0]));
}

static JSValue js_number___toLength(JSContext *ctx, JSValueConst this_val,
                                    int argc, JSValueConst *argv)
{
    int64_t v;
    if (JS_ToLengthFree(ctx, &v, JS_DupValue(ctx, argv[0])))
        return JS_EXCEPTION;
    return JS_NewInt64(ctx, v);
}
#endif

static JSValue js_number_isNaN(JSContext *ctx, JSValueConst this_val,
                               int argc, JSValueConst *argv)
{
    if (!JS_IsNumber(argv[0]))
        return JS_FALSE;
    return js_global_isNaN(ctx, this_val, argc, argv);
}

static JSValue js_number_isFinite(JSContext *ctx, JSValueConst this_val,
                                  int argc, JSValueConst *argv)
{
    if (!JS_IsNumber(argv[0]))
        return JS_FALSE;
    return js_global_isFinite(ctx, this_val, argc, argv);
}

static JSValue js_number_isInteger(JSContext *ctx, JSValueConst this_val,
                                   int argc, JSValueConst *argv)
{
    int ret;
    ret = JS_NumberIsInteger(ctx, argv[0]);
    if (ret < 0)
        return JS_EXCEPTION;
    else
        return JS_NewBool(ctx, ret);
}

static JSValue js_number_isSafeInteger(JSContext *ctx, JSValueConst this_val,
                                       int argc, JSValueConst *argv)
{
    double d;
    if (!JS_IsNumber(argv[0]))
        return JS_FALSE;
    if (UNLIKELY(JS_ToFloat64(ctx, &d, argv[0])))
        return JS_EXCEPTION;
    return JS_NewBool(ctx, is_safe_integer(d));
}

static const JSCFunctionListEntry js_number_funcs[] = {
    /* global ParseInt and parseFloat should be defined already or delayed */
    JS_ALIAS_BASE_DEF("parseInt", "parseInt", 0 ),
    JS_ALIAS_BASE_DEF("parseFloat", "parseFloat", 0 ),
    JS_CFUNC_DEF("isNaN", 1, js_number_isNaN ),
    JS_CFUNC_DEF("isFinite", 1, js_number_isFinite ),
    JS_CFUNC_DEF("isInteger", 1, js_number_isInteger ),
    JS_CFUNC_DEF("isSafeInteger", 1, js_number_isSafeInteger ),
    JS_PROP_DOUBLE_DEF("MAX_VALUE", 1.7976931348623157e+308, 0 ),
    JS_PROP_DOUBLE_DEF("MIN_VALUE", 5e-324, 0 ),
    JS_PROP_DOUBLE_DEF("NaN", NAN, 0 ),
    JS_PROP_DOUBLE_DEF("NEGATIVE_INFINITY", -INFINITY, 0 ),
    JS_PROP_DOUBLE_DEF("POSITIVE_INFINITY", INFINITY, 0 ),
    JS_PROP_DOUBLE_DEF("EPSILON", 2.220446049250313e-16, 0 ), /* ES6 */
    JS_PROP_DOUBLE_DEF("MAX_SAFE_INTEGER", 9007199254740991.0, 0 ), /* ES6 */
    JS_PROP_DOUBLE_DEF("MIN_SAFE_INTEGER", -9007199254740991.0, 0 ), /* ES6 */
    //JS_CFUNC_DEF("__toInteger", 1, js_number___toInteger ),
    //JS_CFUNC_DEF("__toLength", 1, js_number___toLength ),
};

static JSValue js_thisNumberValue(JSContext *ctx, JSValueConst this_val)
{
    if (JS_IsNumber(this_val))
        return JS_DupValue(ctx, this_val);

    if (JS_VALUE_GET_TAG(this_val) == JS_TAG_OBJECT) {
        JSObject *p = JS_VALUE_GET_OBJ(this_val);
        if (p->class_id == JS_CLASS_NUMBER) {
            if (JS_IsNumber(p->u.object_data))
                return JS_DupValue(ctx, p->u.object_data);
        }
    }
    return JS_ThrowTypeError(ctx, "not a number");
}

static JSValue js_number_valueOf(JSContext *ctx, JSValueConst this_val,
                                 int argc, JSValueConst *argv)
{
    return js_thisNumberValue(ctx, this_val);
}

int js_get_radix(JSContext *ctx, JSValueConst val)
{
    int radix;
    if (JS_ToInt32Sat(ctx, &radix, val))
        return -1;
    if (radix < 2 || radix > 36) {
        JS_ThrowRangeError(ctx, "radix must be between 2 and 36");
        return -1;
    }
    return radix;
}

static JSValue js_number_toString(JSContext *ctx, JSValueConst this_val,
                                  int argc, JSValueConst *argv, int magic)
{
    JSValue val;
    int base;
    double d;
    val = js_thisNumberValue(ctx, this_val);
    if (JS_IsException(val))
        return val;
    if (magic || JS_IsUndefined(argv[0])) {
        base = 10;
    } else {
        base = js_get_radix(ctx, argv[0]);
        if (base < 0)
            goto fail;
    }
    if (JS_ToFloat64Free(ctx, &d, val))
        return JS_EXCEPTION;
    return js_dtoa(ctx, d, base, 0, JS_DTOA_VAR_FORMAT);
 fail:
    JS_FreeValue(ctx, val);
    return JS_EXCEPTION;
}

static JSValue js_number_toFixed(JSContext *ctx, JSValueConst this_val,
                                 int argc, JSValueConst *argv)
{
    JSValue val;
    int f;
    double d;
    val = js_thisNumberValue(ctx, this_val);
    if (JS_IsException(val))
        return val;
    if (JS_ToFloat64Free(ctx, &d, val))
        return JS_EXCEPTION;
    if (JS_ToInt32Sat(ctx, &f, argv[0]))
        return JS_EXCEPTION;
    if (f < 0 || f > 100)
        return JS_ThrowRangeError(ctx, "invalid number of digits");
    if (fabs(d) >= 1e21) {
        return JS_ToStringFree(ctx, __JS_NewFloat64(ctx, d));
    } else {
        return js_dtoa(ctx, d, 10, f, JS_DTOA_FRAC_FORMAT);
    }
}

static JSValue js_number_toExponential(JSContext *ctx, JSValueConst this_val,
                                       int argc, JSValueConst *argv)
{
    JSValue val;
    int f, flags;
    double d;
    val = js_thisNumberValue(ctx, this_val);
    if (JS_IsException(val))
        return val;
    if (JS_ToFloat64Free(ctx, &d, val))
        return JS_EXCEPTION;
    if (JS_ToInt32Sat(ctx, &f, argv[0]))
        return JS_EXCEPTION;
    if (!isfinite(d)) {
        return JS_ToStringFree(ctx,  __JS_NewFloat64(ctx, d));
    }
    if (JS_IsUndefined(argv[0])) {
        flags = 0;
        f = 0;
    } else {
        if (f < 0 || f > 100)
            return JS_ThrowRangeError(ctx, "invalid number of digits");
        f++;
        flags = JS_DTOA_FIXED_FORMAT;
    }
    return js_dtoa(ctx, d, 10, f, flags | JS_DTOA_FORCE_EXP);
}

static JSValue js_number_toPrecision(JSContext *ctx, JSValueConst this_val,
                                     int argc, JSValueConst *argv)
{
    JSValue val;
    int p;
    double d;
    val = js_thisNumberValue(ctx, this_val);
    if (JS_IsException(val))
        return val;
    if (JS_ToFloat64Free(ctx, &d, val))
        return JS_EXCEPTION;
    if (JS_IsUndefined(argv[0]))
        goto to_string;
    if (JS_ToInt32Sat(ctx, &p, argv[0]))
        return JS_EXCEPTION;
    if (!isfinite(d)) {
    to_string:
        return JS_ToStringFree(ctx,  __JS_NewFloat64(ctx, d));
    }
    if (p < 1 || p > 100)
        return JS_ThrowRangeError(ctx, "invalid number of digits");
    return js_dtoa(ctx, d, 10, p, JS_DTOA_FIXED_FORMAT);
}

static const JSCFunctionListEntry js_number_proto_funcs[] = {
    JS_CFUNC_DEF("toExponential", 1, js_number_toExponential ),
    JS_CFUNC_DEF("toFixed", 1, js_number_toFixed ),
    JS_CFUNC_DEF("toPrecision", 1, js_number_toPrecision ),
    JS_CFUNC_MAGIC_DEF("toString", 1, js_number_toString, 0 ),
    JS_CFUNC_MAGIC_DEF("toLocaleString", 0, js_number_toString, 1 ),
    JS_CFUNC_DEF("valueOf", 0, js_number_valueOf ),
};

static JSValue js_parseInt(JSContext *ctx, JSValueConst this_val,
                           int argc, JSValueConst *argv)
{
    const char *str, *p;
    int radix, flags;
    JSValue ret;
    str = JS_ToCString(ctx, argv[0]);
    if (!str)
        return JS_EXCEPTION;
    if (JS_ToInt32(ctx, &radix, argv[1])) {
        JS_FreeCString(ctx, str);
        return JS_EXCEPTION;
    }
    if (radix != 0 && (radix < 2 || radix > 36)) {
        ret = JS_NAN;
    } else {
        p = str;
        p += skip_spaces(p);
        flags = ATOD_INT_ONLY | ATOD_ACCEPT_PREFIX_AFTER_SIGN;
        ret = js_atof(ctx, p, NULL, radix, flags);
    }
    JS_FreeCString(ctx, str);
    return ret;
}

static JSValue js_parseFloat(JSContext *ctx, JSValueConst this_val,
                             int argc, JSValueConst *argv)
{
    const char *str, *p;
    JSValue ret;
    str = JS_ToCString(ctx, argv[0]);
    if (!str)
        return JS_EXCEPTION;
    p = str;
    p += skip_spaces(p);
    ret = js_atof(ctx, p, NULL, 10, 0);
    JS_FreeCString(ctx, str);
    return ret;
}

/* Boolean */
static JSValue js_boolean_constructor(JSContext *ctx, JSValueConst new_target,
                                     int argc, JSValueConst *argv)
{
    JSValue val, obj;
    val = JS_NewBool(ctx, JS_ToBool(ctx, argv[0]));
    if (!JS_IsUndefined(new_target)) {
        obj = js_create_from_ctor(ctx, new_target, JS_CLASS_BOOLEAN);
        if (!JS_IsException(obj))
            JS_SetObjectData(ctx, obj, val);
        return obj;
    } else {
        return val;
    }
}

static JSValue js_thisBooleanValue(JSContext *ctx, JSValueConst this_val)
{
    if (JS_VALUE_GET_TAG(this_val) == JS_TAG_BOOL)
        return JS_DupValue(ctx, this_val);
    if (JS_VALUE_GET_TAG(this_val) == JS_TAG_OBJECT) {
        JSObject *p = JS_VALUE_GET_OBJ(this_val);
        if (p->class_id == JS_CLASS_BOOLEAN) {
            if (JS_VALUE_GET_TAG(p->u.object_data) == JS_TAG_BOOL)
                return p->u.object_data;
        }
    }
    return JS_ThrowTypeError(ctx, "not a boolean");
}

static JSValue js_boolean_toString(JSContext *ctx, JSValueConst this_val,
                                   int argc, JSValueConst *argv)
{
    JSValue val = js_thisBooleanValue(ctx, this_val);
    if (JS_IsException(val))
        return val;
    return JS_AtomToString(ctx, JS_VALUE_GET_BOOL(val) ?
                       JS_ATOM_true : JS_ATOM_false);
}

static JSValue js_boolean_valueOf(JSContext *ctx, JSValueConst this_val,
                                  int argc, JSValueConst *argv)
{
    return js_thisBooleanValue(ctx, this_val);
}

static const JSCFunctionListEntry js_boolean_proto_funcs[] = {
    JS_CFUNC_DEF("toString", 0, js_boolean_toString ),
    JS_CFUNC_DEF("valueOf", 0, js_boolean_valueOf ),
};

/* Symbol */

static JSValue js_symbol_constructor(JSContext *ctx, JSValueConst new_target,
                                     int argc, JSValueConst *argv)
{
    JSValue str;
    JSString *p;
    if (!JS_IsUndefined(new_target))
        return JS_ThrowTypeError(ctx, "not a constructor");
    if (argc == 0 || JS_IsUndefined(argv[0])) {
        p = NULL;
    } else {
        str = JS_ToString(ctx, argv[0]);
        if (JS_IsException(str))
            return JS_EXCEPTION;
        p = JS_VALUE_GET_STRING(str);
    }
    return JS_NewSymbol(ctx, p, JS_ATOM_TYPE_SYMBOL);
}

static JSValue js_thisSymbolValue(JSContext *ctx, JSValueConst this_val)
{
    if (JS_VALUE_GET_TAG(this_val) == JS_TAG_SYMBOL)
        return JS_DupValue(ctx, this_val);
    if (JS_VALUE_GET_TAG(this_val) == JS_TAG_OBJECT) {
        JSObject *p = JS_VALUE_GET_OBJ(this_val);
        if (p->class_id == JS_CLASS_SYMBOL) {
            if (JS_VALUE_GET_TAG(p->u.object_data) == JS_TAG_SYMBOL)
                return JS_DupValue(ctx, p->u.object_data);
        }
    }
    return JS_ThrowTypeError(ctx, "not a symbol");
}

static JSValue js_symbol_toString(JSContext *ctx, JSValueConst this_val,
                                  int argc, JSValueConst *argv)
{
    JSValue val, ret;
    val = js_thisSymbolValue(ctx, this_val);
    if (JS_IsException(val))
        return val;
    /* XXX: use JS_ToStringInternal() with a flags */
    ret = js_string_constructor(ctx, JS_UNDEFINED, 1, (JSValueConst *)&val);
    JS_FreeValue(ctx, val);
    return ret;
}

static JSValue js_symbol_valueOf(JSContext *ctx, JSValueConst this_val,
                                 int argc, JSValueConst *argv)
{
    return js_thisSymbolValue(ctx, this_val);
}

static JSValue js_symbol_get_description(JSContext *ctx, JSValueConst this_val)
{
    JSValue val, ret;
    JSAtomStruct *p;

    val = js_thisSymbolValue(ctx, this_val);
    if (JS_IsException(val))
        return val;
    p = JS_VALUE_GET_PTR(val);
    if (p->len == 0 && p->is_wide_char != 0) {
        ret = JS_UNDEFINED;
    } else {
        ret = JS_AtomToString(ctx, js_get_atom_index(ctx->rt, p));
    }
    JS_FreeValue(ctx, val);
    return ret;
}

static const JSCFunctionListEntry js_symbol_proto_funcs[] = {
    JS_CFUNC_DEF("toString", 0, js_symbol_toString ),
    JS_CFUNC_DEF("valueOf", 0, js_symbol_valueOf ),
    // XXX: should have writable: false
    JS_CFUNC_DEF("[Symbol.toPrimitive]", 1, js_symbol_valueOf ),
    JS_PROP_STRING_DEF("[Symbol.toStringTag]", "Symbol", JS_PROP_CONFIGURABLE ),
    JS_CGETSET_DEF("description", js_symbol_get_description, NULL ),
};

static JSValue js_symbol_for(JSContext *ctx, JSValueConst this_val,
                             int argc, JSValueConst *argv)
{
    JSValue str;
    str = JS_ToString(ctx, argv[0]);
    if (JS_IsException(str))
        return JS_EXCEPTION;
    return JS_NewSymbol(ctx, JS_VALUE_GET_STRING(str), JS_ATOM_TYPE_GLOBAL_SYMBOL);
}

static JSValue js_symbol_keyFor(JSContext *ctx, JSValueConst this_val,
                                int argc, JSValueConst *argv)
{
    JSAtomStruct *p;
    if (!JS_IsSymbol(argv[0]))
        return JS_ThrowTypeError(ctx, "not a symbol");
    p = JS_VALUE_GET_PTR(argv[0]);
    if (p->atom_type != JS_ATOM_TYPE_GLOBAL_SYMBOL)
        return JS_UNDEFINED;
    return JS_DupValue(ctx, JS_MKPTR(JS_TAG_STRING, p));
}

static const JSCFunctionListEntry js_symbol_funcs[] = {
    JS_CFUNC_DEF("for", 1, js_symbol_for ),
    JS_CFUNC_DEF("keyFor", 1, js_symbol_keyFor ),
};

/* AsyncFromSyncIteratorPrototype */

static JSValue JS_CreateAsyncFromSyncIterator(JSContext *ctx,
                                              JSValueConst sync_iter)
{
    JSValue async_iter, next_method;
    JSAsyncFromSyncIteratorData *s;
    next_method = JS_GetProperty(ctx, sync_iter, JS_ATOM_next);
    if (JS_IsException(next_method))
        return JS_EXCEPTION;
    async_iter = JS_NewObjectClass(ctx, JS_CLASS_ASYNC_FROM_SYNC_ITERATOR);
    if (JS_IsException(async_iter)) {
        JS_FreeValue(ctx, next_method);
        return async_iter;
    }
    s = js_mallocz(ctx, sizeof(*s));
    if (!s) {
        JS_FreeValue(ctx, async_iter);
        JS_FreeValue(ctx, next_method);
        return JS_EXCEPTION;
    }
    s->sync_iter = JS_DupValue(ctx, sync_iter);
    s->next_method = next_method;
    JS_SetOpaque(async_iter, s);
    return async_iter;
}

/* global object */

static const JSCFunctionListEntry js_global_funcs[] = {
    JS_CFUNC_DEF("parseInt", 2, js_parseInt ),
    JS_CFUNC_DEF("parseFloat", 1, js_parseFloat ),
    JS_CFUNC_DEF("isNaN", 1, js_global_isNaN ),
    JS_CFUNC_DEF("isFinite", 1, js_global_isFinite ),
    JS_CFUNC_MAGIC_DEF("decodeURI", 1, js_global_decodeURI, 0 ),
    JS_CFUNC_MAGIC_DEF("decodeURIComponent", 1, js_global_decodeURI, 1 ),
    JS_CFUNC_MAGIC_DEF("encodeURI", 1, js_global_encodeURI, 0 ),
    JS_CFUNC_MAGIC_DEF("encodeURIComponent", 1, js_global_encodeURI, 1 ),
    JS_CFUNC_DEF("escape", 1, js_global_escape ),
    JS_CFUNC_DEF("unescape", 1, js_global_unescape ),
    JS_PROP_DOUBLE_DEF("Infinity", 1.0 / 0.0, 0 ),
    JS_PROP_DOUBLE_DEF("NaN", NAN, 0 ),
    JS_PROP_UNDEFINED_DEF("undefined", 0 ),
    /* for the 'Date' implementation */
    JS_CFUNC_DEF("__date_clock", 0, js___date_clock ),
    //JS_CFUNC_DEF("__date_now", 0, js___date_now ),
    //JS_CFUNC_DEF("__date_getTimezoneOffset", 1, js___date_getTimezoneOffset ),
    //JS_CFUNC_DEF("__date_create", 3, js___date_create ),
};

/* eval */

void JS_AddIntrinsicEval(JSContext *ctx)
{
    ctx->eval_internal = __JS_EvalInternal;
}

#ifdef CONFIG_BIGNUM

/* Operators */

static void js_operator_set_finalizer(JSRuntime *rt, JSValue val)
{
    JSOperatorSetData *opset = JS_GetOpaque(val, JS_CLASS_OPERATOR_SET);
    int i, j;
    JSBinaryOperatorDefEntry *ent;
    if (opset) {
        for(i = 0; i < JS_OVOP_COUNT; i++) {
            if (opset->self_ops[i])
                JS_FreeValueRT(rt, JS_MKPTR(JS_TAG_OBJECT, opset->self_ops[i]));
        }
        for(j = 0; j < opset->left.count; j++) {
            ent = &opset->left.tab[j];
            for(i = 0; i < JS_OVOP_BINARY_COUNT; i++) {
                if (ent->ops[i])
                    JS_FreeValueRT(rt, JS_MKPTR(JS_TAG_OBJECT, ent->ops[i]));
            }
        }
        js_free_rt(rt, opset->left.tab);
        for(j = 0; j < opset->right.count; j++) {
            ent = &opset->right.tab[j];
            for(i = 0; i < JS_OVOP_BINARY_COUNT; i++) {
                if (ent->ops[i])
                    JS_FreeValueRT(rt, JS_MKPTR(JS_TAG_OBJECT, ent->ops[i]));
            }
        }
        js_free_rt(rt, opset->right.tab);
        js_free_rt(rt, opset);
    }
}

static void js_operator_set_mark(JSRuntime *rt, JSValueConst val,
                                 JS_MarkFunc *mark_func)
{
    JSOperatorSetData *opset = JS_GetOpaque(val, JS_CLASS_OPERATOR_SET);
    int i, j;
    JSBinaryOperatorDefEntry *ent;
    if (opset) {
        for(i = 0; i < JS_OVOP_COUNT; i++) {
            if (opset->self_ops[i])
                JS_MarkValue(rt, JS_MKPTR(JS_TAG_OBJECT, opset->self_ops[i]),
                             mark_func);
        }
        for(j = 0; j < opset->left.count; j++) {
            ent = &opset->left.tab[j];
            for(i = 0; i < JS_OVOP_BINARY_COUNT; i++) {
                if (ent->ops[i])
                    JS_MarkValue(rt, JS_MKPTR(JS_TAG_OBJECT, ent->ops[i]),
                                 mark_func);
            }
        }
        for(j = 0; j < opset->right.count; j++) {
            ent = &opset->right.tab[j];
            for(i = 0; i < JS_OVOP_BINARY_COUNT; i++) {
                if (ent->ops[i])
                    JS_MarkValue(rt, JS_MKPTR(JS_TAG_OBJECT, ent->ops[i]),
                                 mark_func);
            }
        }
    }
}

/* create an OperatorSet object */
static JSValue js_operators_create_internal(JSContext *ctx,
                                            int argc, JSValueConst *argv,
                                            BOOL is_primitive)
{
    JSValue opset_obj, prop, obj;
    JSOperatorSetData *opset, *opset1;
    JSBinaryOperatorDef *def;
    JSValueConst arg;
    int i, j;
    JSBinaryOperatorDefEntry *new_tab;
    JSBinaryOperatorDefEntry *ent;
    uint32_t op_count;
    if (ctx->rt->operator_count == UINT32_MAX) {
        return JS_ThrowTypeError(ctx, "too many operators");
    }
    opset_obj = JS_NewObjectProtoClass(ctx, JS_NULL, JS_CLASS_OPERATOR_SET);
    if (JS_IsException(opset_obj))
        goto fail;
    opset = js_mallocz(ctx, sizeof(*opset));
    if (!opset)
        goto fail;
    JS_SetOpaque(opset_obj, opset);
    if (argc >= 1) {
        arg = argv[0];
        /* self operators */
        for(i = 0; i < JS_OVOP_COUNT; i++) {
            prop = JS_GetPropertyStr(ctx, arg, js_overloadable_operator_names[i]);
            if (JS_IsException(prop))
                goto fail;
            if (!JS_IsUndefined(prop)) {
                if (check_function(ctx, prop)) {
                    JS_FreeValue(ctx, prop);
                    goto fail;
                }
                opset->self_ops[i] = JS_VALUE_GET_OBJ(prop);
            }
        }
    }
    /* left & right operators */
    for(j = 1; j < argc; j++) {
        arg = argv[j];
        prop = JS_GetPropertyStr(ctx, arg, "left");
        if (JS_IsException(prop))
            goto fail;
        def = &opset->right;
        if (JS_IsUndefined(prop)) {
            prop = JS_GetPropertyStr(ctx, arg, "right");
            if (JS_IsException(prop))
                goto fail;
            if (JS_IsUndefined(prop)) {
                JS_ThrowTypeError(ctx, "left or right property must be present");
                goto fail;
            }
            def = &opset->left;
        }
        /* get the operator set */
        obj = JS_GetProperty(ctx, prop, JS_ATOM_prototype);
        JS_FreeValue(ctx, prop);
        if (JS_IsException(obj))
            goto fail;
        prop = JS_GetProperty(ctx, obj, JS_ATOM_Symbol_operatorSet);
        JS_FreeValue(ctx, obj);
        if (JS_IsException(prop))
            goto fail;
        opset1 = JS_GetOpaque2(ctx, prop, JS_CLASS_OPERATOR_SET);
        if (!opset1) {
            JS_FreeValue(ctx, prop);
            goto fail;
        }
        op_count = opset1->operator_counter;
        JS_FreeValue(ctx, prop);
        /* we assume there are few entries */
        new_tab = js_realloc(ctx, def->tab,
                             (def->count + 1) * sizeof(def->tab[0]));
        if (!new_tab)
            goto fail;
        def->tab = new_tab;
        def->count++;
        ent = def->tab + def->count - 1;
        bzero(ent, sizeof(def->tab[0]));
        ent->operator_index = op_count;
        for(i = 0; i < JS_OVOP_BINARY_COUNT; i++) {
            prop = JS_GetPropertyStr(ctx, arg,
                                     js_overloadable_operator_names[i]);
            if (JS_IsException(prop))
                goto fail;
            if (!JS_IsUndefined(prop)) {
                if (check_function(ctx, prop)) {
                    JS_FreeValue(ctx, prop);
                    goto fail;
                }
                ent->ops[i] = JS_VALUE_GET_OBJ(prop);
            }
        }
    }
    opset->is_primitive = is_primitive;
    opset->operator_counter = ctx->rt->operator_count++;
    return opset_obj;
 fail:
    JS_FreeValue(ctx, opset_obj);
    return JS_EXCEPTION;
}

static JSValue js_operators_create(JSContext *ctx, JSValueConst this_val,
                                int argc, JSValueConst *argv)
{
    return js_operators_create_internal(ctx, argc, argv, FALSE);
}

static JSValue js_operators_updateBigIntOperators(JSContext *ctx, JSValueConst this_val,
                                                  int argc, JSValueConst *argv)
{
    JSValue opset_obj, prop;
    JSOperatorSetData *opset;
    const JSOverloadableOperatorEnum ops[2] = { JS_OVOP_DIV, JS_OVOP_POW };
    JSOverloadableOperatorEnum op;
    int i;
    opset_obj = JS_GetProperty(ctx, ctx->class_proto[JS_CLASS_BIG_INT],
                               JS_ATOM_Symbol_operatorSet);
    if (JS_IsException(opset_obj))
        goto fail;
    opset = JS_GetOpaque2(ctx, opset_obj, JS_CLASS_OPERATOR_SET);
    if (!opset)
        goto fail;
    for(i = 0; i < countof(ops); i++) {
        op = ops[i];
        prop = JS_GetPropertyStr(ctx, argv[0],
                                 js_overloadable_operator_names[op]);
        if (JS_IsException(prop))
            goto fail;
        if (!JS_IsUndefined(prop)) {
            if (!JS_IsNull(prop) && check_function(ctx, prop)) {
                JS_FreeValue(ctx, prop);
                goto fail;
            }
            if (opset->self_ops[op])
                JS_FreeValue(ctx, JS_MKPTR(JS_TAG_OBJECT, opset->self_ops[op]));
            if (JS_IsNull(prop)) {
                opset->self_ops[op] = NULL;
            } else {
                opset->self_ops[op] = JS_VALUE_GET_PTR(prop);
            }
        }
    }
    JS_FreeValue(ctx, opset_obj);
    return JS_UNDEFINED;
 fail:
    JS_FreeValue(ctx, opset_obj);
    return JS_EXCEPTION;
}

static int js_operators_set_default(JSContext *ctx, JSValueConst obj)
{
    JSValue opset_obj;
    if (!JS_IsObject(obj)) /* in case the prototype is not defined */
        return 0;
    opset_obj = js_operators_create_internal(ctx, 0, NULL, TRUE);
    if (JS_IsException(opset_obj))
        return -1;
    /* cannot be modified by the user */
    JS_DefinePropertyValue(ctx, obj, JS_ATOM_Symbol_operatorSet,
                           opset_obj, 0);
    return 0;
}

static JSValue js_dummy_operators_ctor(JSContext *ctx, JSValueConst new_target,
                                       int argc, JSValueConst *argv)
{
    return js_create_from_ctor(ctx, new_target, JS_CLASS_OBJECT);
}

static JSValue js_global_operators(JSContext *ctx, JSValueConst this_val,
                                   int argc, JSValueConst *argv)
{
    JSValue func_obj, proto, opset_obj;
    func_obj = JS_UNDEFINED;
    proto = JS_NewObject(ctx);
    if (JS_IsException(proto))
        return JS_EXCEPTION;
    opset_obj = js_operators_create_internal(ctx, argc, argv, FALSE);
    if (JS_IsException(opset_obj))
        goto fail;
    JS_DefinePropertyValue(ctx, proto, JS_ATOM_Symbol_operatorSet,
                           opset_obj, JS_PROP_WRITABLE | JS_PROP_CONFIGURABLE);
    func_obj = JS_NewCFunction2(ctx, js_dummy_operators_ctor, "Operators",
                                0, JS_CFUNC_constructor, 0);
    if (JS_IsException(func_obj))
        goto fail;
    JS_SetConstructor2(ctx, func_obj, proto,
                       0, JS_PROP_WRITABLE | JS_PROP_CONFIGURABLE);
    JS_FreeValue(ctx, proto);
    return func_obj;
 fail:
    JS_FreeValue(ctx, proto);
    JS_FreeValue(ctx, func_obj);
    return JS_EXCEPTION;
}

static const JSCFunctionListEntry js_operators_funcs[] = {
    JS_CFUNC_DEF("create", 1, js_operators_create ),
    JS_CFUNC_DEF("updateBigIntOperators", 2, js_operators_updateBigIntOperators ),
};

/* must be called after all overloadable base types are initialized */
void JS_AddIntrinsicOperators(JSContext *ctx)
{
    JSValue obj;
    ctx->allow_operator_overloading = TRUE;
    obj = JS_NewCFunction(ctx, js_global_operators, "Operators", 1);
    JS_SetPropertyFunctionList(ctx, obj,
                               js_operators_funcs,
                               countof(js_operators_funcs));
    JS_DefinePropertyValue(ctx, ctx->global_obj, JS_ATOM_Operators,
                           obj,
                           JS_PROP_WRITABLE | JS_PROP_CONFIGURABLE);
    /* add default operatorSets */
    js_operators_set_default(ctx, ctx->class_proto[JS_CLASS_BOOLEAN]);
    js_operators_set_default(ctx, ctx->class_proto[JS_CLASS_NUMBER]);
    js_operators_set_default(ctx, ctx->class_proto[JS_CLASS_STRING]);
    js_operators_set_default(ctx, ctx->class_proto[JS_CLASS_BIG_INT]);
    js_operators_set_default(ctx, ctx->class_proto[JS_CLASS_BIG_FLOAT]);
    js_operators_set_default(ctx, ctx->class_proto[JS_CLASS_BIG_DECIMAL]);
}

#endif /* CONFIG_BIGNUM */

static const char * const native_error_name[JS_NATIVE_ERROR_COUNT] = {
    "EvalError", "RangeError", "ReferenceError",
    "SyntaxError", "TypeError", "URIError",
    "InternalError", "AggregateError",
};

/* Minimum amount of objects to be able to compile code and display
   error messages. No JSAtom should be allocated by this function. */
static void JS_AddIntrinsicBasicObjects(JSContext *ctx)
{
    JSValue proto;
    int i;
    ctx->class_proto[JS_CLASS_OBJECT] = JS_NewObjectProto(ctx, JS_NULL);
    ctx->function_proto = JS_NewCFunction3(ctx, js_function_proto, "", 0,
                                           JS_CFUNC_generic, 0,
                                           ctx->class_proto[JS_CLASS_OBJECT]);
    ctx->class_proto[JS_CLASS_BYTECODE_FUNCTION] = JS_DupValue(ctx, ctx->function_proto);
    ctx->class_proto[JS_CLASS_ERROR] = JS_NewObject(ctx);
#if 0
    /* these are auto-initialized from js_error_proto_funcs,
       but delaying might be a problem */
    JS_DefinePropertyValue(ctx, ctx->class_proto[JS_CLASS_ERROR], JS_ATOM_name,
                           JS_AtomToString(ctx, JS_ATOM_Error),
                           JS_PROP_WRITABLE | JS_PROP_CONFIGURABLE);
    JS_DefinePropertyValue(ctx, ctx->class_proto[JS_CLASS_ERROR], JS_ATOM_message,
                           JS_AtomToString(ctx, JS_ATOM_empty_string),
                           JS_PROP_WRITABLE | JS_PROP_CONFIGURABLE);
#endif
    JS_SetPropertyFunctionList(ctx, ctx->class_proto[JS_CLASS_ERROR],
                               js_error_proto_funcs,
                               countof(js_error_proto_funcs));
    for(i = 0; i < JS_NATIVE_ERROR_COUNT; i++) {
        proto = JS_NewObjectProto(ctx, ctx->class_proto[JS_CLASS_ERROR]);
        JS_DefinePropertyValue(ctx, proto, JS_ATOM_name,
                               JS_NewAtomString(ctx, native_error_name[i]),
                               JS_PROP_WRITABLE | JS_PROP_CONFIGURABLE);
        JS_DefinePropertyValue(ctx, proto, JS_ATOM_message,
                               JS_AtomToString(ctx, JS_ATOM_empty_string),
                               JS_PROP_WRITABLE | JS_PROP_CONFIGURABLE);
        ctx->native_error_proto[i] = proto;
    }
    /* the array prototype is an array */
    ctx->class_proto[JS_CLASS_ARRAY] =
        JS_NewObjectProtoClass(ctx, ctx->class_proto[JS_CLASS_OBJECT],
                               JS_CLASS_ARRAY);
    ctx->array_shape = js_new_shape2(ctx, get_proto_obj(ctx->class_proto[JS_CLASS_ARRAY]),
                                     JS_PROP_INITIAL_HASH_SIZE, 1);
    add_shape_property(ctx, &ctx->array_shape, NULL,
                       JS_ATOM_length, JS_PROP_WRITABLE | JS_PROP_LENGTH);
    /* XXX: could test it on first context creation to ensure that no
       new atoms are created in JS_AddIntrinsicBasicObjects(). It is
       necessary to avoid useless renumbering of atoms after
       JS_EvalBinary() if it is done just after
       JS_AddIntrinsicBasicObjects(). */
    //    assert(ctx->rt->atom_count == JS_ATOM_END);
}

void JS_AddIntrinsicBaseObjects(JSContext *ctx)
{
    int i;
    JSValue obj1;
    JSValueConst obj, number_obj;
    ctx->throw_type_error = JS_NewCFunction(ctx, js_throw_type_error, NULL, 0);
    /* add caller and arguments properties to throw a TypeError */
    obj1 = JS_NewCFunction(ctx, js_function_proto_caller, NULL, 0);
    JS_DefineProperty(ctx, ctx->function_proto, JS_ATOM_caller, JS_UNDEFINED,
                      obj1, ctx->throw_type_error,
                      JS_PROP_HAS_GET | JS_PROP_HAS_SET |
                      JS_PROP_HAS_CONFIGURABLE | JS_PROP_CONFIGURABLE);
    JS_DefineProperty(ctx, ctx->function_proto, JS_ATOM_arguments, JS_UNDEFINED,
                      obj1, ctx->throw_type_error,
                      JS_PROP_HAS_GET | JS_PROP_HAS_SET |
                      JS_PROP_HAS_CONFIGURABLE | JS_PROP_CONFIGURABLE);
    JS_FreeValue(ctx, obj1);
    JS_FreeValue(ctx, js_object_seal(ctx, JS_UNDEFINED, 1, (JSValueConst *)&ctx->throw_type_error, 1));
    ctx->global_obj = JS_NewObject(ctx);
    ctx->global_var_obj = JS_NewObjectProto(ctx, JS_NULL);
    JS_AddIntrinsicObject(ctx);
    /* Function */
    JS_SetPropertyFunctionList(ctx, ctx->function_proto, js_function_proto_funcs, countof(js_function_proto_funcs));
    ctx->function_ctor = JS_NewCFunctionMagic(ctx, js_function_constructor,
                                              "Function", 1, JS_CFUNC_constructor_or_func_magic,
                                              JS_FUNC_NORMAL);
    JS_NewGlobalCConstructor2(ctx, JS_DupValue(ctx, ctx->function_ctor), "Function",
                              ctx->function_proto);
    /* Error */
    obj1 = JS_NewCFunctionMagic(ctx, js_error_constructor,
                                "Error", 1, JS_CFUNC_constructor_or_func_magic, -1);
    JS_NewGlobalCConstructor2(ctx, obj1,
                              "Error", ctx->class_proto[JS_CLASS_ERROR]);
    for(i = 0; i < JS_NATIVE_ERROR_COUNT; i++) {
        JSValue func_obj;
        int n_args;
        n_args = 1 + (i == JS_AGGREGATE_ERROR);
        func_obj = JS_NewCFunction3(ctx, (JSCFunction *)js_error_constructor,
                                    native_error_name[i], n_args,
                                    JS_CFUNC_constructor_or_func_magic, i, obj1);
        JS_NewGlobalCConstructor2(ctx, func_obj, native_error_name[i],
                                  ctx->native_error_proto[i]);
    }
    JS_AddIteratorProto(ctx);
    JS_AddIntrinsicArray(ctx);
    /* XXX: create auto_initializer */
    {
        /* initialize Array.prototype[Symbol.unscopables] */
        char const unscopables[] = "copyWithin" "\0" "entries" "\0" "fill" "\0" "find" "\0"
            "findIndex" "\0" "flat" "\0" "flatMap" "\0" "includes" "\0" "keys" "\0" "values" "\0";
        const char *p = unscopables;
        obj1 = JS_NewObjectProto(ctx, JS_NULL);
        for(p = unscopables; *p; p += strlen(p) + 1) {
            JS_DefinePropertyValueStr(ctx, obj1, p, JS_TRUE, JS_PROP_C_W_E);
        }
        JS_DefinePropertyValue(ctx, ctx->class_proto[JS_CLASS_ARRAY],
                               JS_ATOM_Symbol_unscopables, obj1,
                               JS_PROP_CONFIGURABLE);
    }
    /* needed to initialize arguments[Symbol.iterator] */
    JS_AddArrayIteratorProto(ctx);
    /* parseFloat and parseInteger must be defined before Number
       because of the Number.parseFloat and Number.parseInteger
       aliases */
    JS_SetPropertyFunctionList(ctx, ctx->global_obj, js_global_funcs,
                               countof(js_global_funcs));
    /* Number */
    ctx->class_proto[JS_CLASS_NUMBER] = JS_NewObjectProtoClass(ctx, ctx->class_proto[JS_CLASS_OBJECT],
                                                               JS_CLASS_NUMBER);
    JS_SetObjectData(ctx, ctx->class_proto[JS_CLASS_NUMBER], JS_NewInt32(ctx, 0));
    JS_SetPropertyFunctionList(ctx, ctx->class_proto[JS_CLASS_NUMBER],
                               js_number_proto_funcs,
                               countof(js_number_proto_funcs));
    number_obj = JS_NewGlobalCConstructor(ctx, "Number", js_number_constructor, 1,
                                          ctx->class_proto[JS_CLASS_NUMBER]);
    JS_SetPropertyFunctionList(ctx, number_obj, js_number_funcs, countof(js_number_funcs));
    /* Boolean */
    ctx->class_proto[JS_CLASS_BOOLEAN] = JS_NewObjectProtoClass(ctx, ctx->class_proto[JS_CLASS_OBJECT],
                                                                JS_CLASS_BOOLEAN);
    JS_SetObjectData(ctx, ctx->class_proto[JS_CLASS_BOOLEAN], JS_NewBool(ctx, FALSE));
    JS_SetPropertyFunctionList(ctx, ctx->class_proto[JS_CLASS_BOOLEAN], js_boolean_proto_funcs,
                               countof(js_boolean_proto_funcs));
    JS_NewGlobalCConstructor(ctx, "Boolean", js_boolean_constructor, 1,
                             ctx->class_proto[JS_CLASS_BOOLEAN]);
    JS_AddIntrinsicString(ctx);
    JS_AddIntrinsicMath(ctx);
    /* ES6 Reflect: create as autoinit object */
    JS_AddIntrinsicReflect(ctx);
    /* ES6 Symbol */
    ctx->class_proto[JS_CLASS_SYMBOL] = JS_NewObject(ctx);
    JS_SetPropertyFunctionList(ctx, ctx->class_proto[JS_CLASS_SYMBOL], js_symbol_proto_funcs,
                               countof(js_symbol_proto_funcs));
    obj = JS_NewGlobalCConstructor(ctx, "Symbol", js_symbol_constructor, 0,
                                   ctx->class_proto[JS_CLASS_SYMBOL]);
    JS_SetPropertyFunctionList(ctx, obj, js_symbol_funcs,
                               countof(js_symbol_funcs));
    for(i = JS_ATOM_Symbol_toPrimitive; i < JS_ATOM_END; i++) {
        char buf[ATOM_GET_STR_BUF_SIZE];
        const char *str, *p;
        str = JS_AtomGetStr(ctx, buf, sizeof(buf), i);
        /* skip "Symbol." */
        p = strchr(str, '.');
        if (p)
            str = p + 1;
        JS_DefinePropertyValueStr(ctx, obj, str, JS_AtomToValue(ctx, i), 0);
    }
    /* ES6 Generator */
    JS_AddIntrinsicGenerator(ctx);
    /* global properties */
    ctx->eval_obj = JS_NewCFunction(ctx, js_global_eval, "eval", 1);
    JS_DefinePropertyValue(ctx, ctx->global_obj, JS_ATOM_eval,
                           JS_DupValue(ctx, ctx->eval_obj),
                           JS_PROP_WRITABLE | JS_PROP_CONFIGURABLE);
    JS_DefinePropertyValue(ctx, ctx->global_obj, JS_ATOM_globalThis,
                           JS_DupValue(ctx, ctx->global_obj),
                           JS_PROP_CONFIGURABLE | JS_PROP_WRITABLE);
}
