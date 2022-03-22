#ifndef COSMOPOLITAN_THIRD_PARTY_QUICKJS_INTERNAL_H_
#define COSMOPOLITAN_THIRD_PARTY_QUICKJS_INTERNAL_H_
#include "third_party/quickjs/cutils.h"
#include "third_party/quickjs/libbf.h"
#include "third_party/quickjs/list.h"
#include "third_party/quickjs/quickjs.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_
/* clang-format off */

#define OPTIMIZE         1
#define SHORT_OPCODES    1
#if defined(EMSCRIPTEN)
#define DIRECT_DISPATCH  0
#else
#define DIRECT_DISPATCH  1
#endif

#if defined(__APPLE__)
#define MALLOC_OVERHEAD  0
#else
#define MALLOC_OVERHEAD  8
#endif

#if !defined(_WIN32)
/* define it if printf uses the RNDN rounding mode instead of RNDNA */
#define CONFIG_PRINTF_RNDN
#endif

/* define to include Atomics.* operations which depend on the OS
   threads */
#if !defined(EMSCRIPTEN) && defined(USE_WORKER)
#define CONFIG_ATOMICS
#endif

#if !defined(EMSCRIPTEN)
/* enable stack limitation */
#define CONFIG_STACK_CHECK
#endif

#define MAX_SAFE_INTEGER (((int64_t)1 << 53) - 1)

#define HINT_STRING  0
#define HINT_NUMBER  1
#define HINT_NONE    2
#define HINT_FORCE_ORDINARY (1 << 4) /* don't try Symbol.toPrimitive */

/* radix != 10 is only supported with flags = JS_DTOA_VAR_FORMAT */
/* use as many digits as necessary */
#define JS_DTOA_VAR_FORMAT   (0 << 0)
/* use n_digits significant digits (1 <= n_digits <= 101) */
#define JS_DTOA_FIXED_FORMAT (1 << 0)
/* force fractional format: [-]dd.dd with n_digits fractional digits */
#define JS_DTOA_FRAC_FORMAT  (2 << 0)
/* force exponential notation either in fixed or variable format */
#define JS_DTOA_FORCE_EXP    (1 << 2)

#define ATOM_GET_STR_BUF_SIZE 64

#define JS_ATOM_TAG_INT (1U << 31)
#define JS_ATOM_MAX_INT (JS_ATOM_TAG_INT - 1)
#define JS_ATOM_MAX     ((1U << 30) - 1)

/* return the max count from the hash size */
#define JS_ATOM_COUNT_RESIZE(n) ((n) * 2)

#if SHORT_OPCODES
/* After the final compilation pass, short opcodes are used. Their
   opcodes overlap with the temporary opcodes which cannot appear in
   the final bytecode. Their description is after the temporary
   opcodes in opcode_info[]. */
#define short_opcode_info(op)           \
    opcode_info[(op) >= OP_TEMP_START ? \
                (op) + (OP_TEMP_END - OP_TEMP_START) : (op)]
#else
#define short_opcode_info(op) opcode_info[op]
#endif

typedef enum {
    PUT_LVALUE_NOKEEP, /* [depth] v -> */
    PUT_LVALUE_NOKEEP_DEPTH, /* [depth] v -> , keep depth (currently
                                just disable optimizations) */
    PUT_LVALUE_KEEP_TOP,  /* [depth] v -> v */
    PUT_LVALUE_KEEP_SECOND, /* [depth] v0 v -> v0 */
    PUT_LVALUE_NOKEEP_BOTTOM, /* v [depth] -> */
} PutLValueEnum;

/* argument of OP_special_object */
typedef enum {
    OP_SPECIAL_OBJECT_ARGUMENTS,
    OP_SPECIAL_OBJECT_MAPPED_ARGUMENTS,
    OP_SPECIAL_OBJECT_THIS_FUNC,
    OP_SPECIAL_OBJECT_NEW_TARGET,
    OP_SPECIAL_OBJECT_HOME_OBJECT,
    OP_SPECIAL_OBJECT_VAR_OBJECT,
    OP_SPECIAL_OBJECT_IMPORT_META,
} OPSpecialObjectEnum;

typedef enum FuncCallType {
    FUNC_CALL_NORMAL,
    FUNC_CALL_NEW,
    FUNC_CALL_SUPER_CTOR,
    FUNC_CALL_TEMPLATE,
} FuncCallType;

typedef enum {
    JS_VAR_DEF_WITH,
    JS_VAR_DEF_LET,
    JS_VAR_DEF_CONST,
    JS_VAR_DEF_FUNCTION_DECL, /* function declaration */
    JS_VAR_DEF_NEW_FUNCTION_DECL, /* async/generator function declaration */
    JS_VAR_DEF_CATCH,
    JS_VAR_DEF_VAR,
} JSVarDefEnum;

#define FUNC_RET_AWAIT      0
#define FUNC_RET_YIELD      1
#define FUNC_RET_YIELD_STAR 2

#define FE_PREC      (-1)
#define FE_EXP       (-2)
#define FE_RNDMODE   (-3)
#define FE_SUBNORMAL (-4)

#define JS_CALL_FLAG_COPY_ARGV   (1 << 1)
#define JS_CALL_FLAG_GENERATOR   (1 << 2)

#define TOK_FIRST_KEYWORD   TOK_NULL
#define TOK_LAST_KEYWORD    TOK_AWAIT

#define JS_BACKTRACE_FLAG_SKIP_FIRST_LEVEL (1 << 0)
/* only taken into account if filename is provided */
#define JS_BACKTRACE_FLAG_SINGLE_LEVEL     (1 << 1)

/* unicode code points */
#define CP_NBSP 0x00a0
#define CP_BOM  0xfeff

#define CP_LS   0x2028
#define CP_PS   0x2029

#define MAGIC_SET (1 << 0)
#define MAGIC_WEAK (1 << 1)

/* XXX: use enum */
#define GEN_MAGIC_NEXT   0
#define GEN_MAGIC_RETURN 1
#define GEN_MAGIC_THROW  2

#define OP_DEFINE_METHOD_METHOD 0
#define OP_DEFINE_METHOD_GETTER 1
#define OP_DEFINE_METHOD_SETTER 2
#define OP_DEFINE_METHOD_ENUMERABLE 4

#define JS_THROW_VAR_RO             0
#define JS_THROW_VAR_REDECL         1
#define JS_THROW_VAR_UNINITIALIZED  2
#define JS_THROW_ERROR_DELETE_SUPER   3
#define JS_THROW_ERROR_ITERATOR_THROW 4

#define GLOBAL_VAR_OFFSET 0x40000000
#define ARGUMENT_VAR_OFFSET 0x20000000

#define DEFINE_GLOBAL_LEX_VAR (1 << 7)
#define DEFINE_GLOBAL_FUNC_VAR (1 << 6)

#define JS_DEFINE_CLASS_HAS_HERITAGE     (1 << 0)

#define ATOD_INT_ONLY        (1 << 0)
/* accept Oo and Ob prefixes in addition to 0x prefix if radix = 0 */
#define ATOD_ACCEPT_BIN_OCT  (1 << 2)
/* accept O prefix as octal if radix == 0 and properly formed (Annex B) */
#define ATOD_ACCEPT_LEGACY_OCTAL  (1 << 4)
/* accept _ between digits as a digit separator */
#define ATOD_ACCEPT_UNDERSCORES  (1 << 5)
/* allow a suffix to override the type */
#define ATOD_ACCEPT_SUFFIX    (1 << 6)
/* default type */
#define ATOD_TYPE_MASK        (3 << 7)
#define ATOD_TYPE_FLOAT64     (0 << 7)
#define ATOD_TYPE_BIG_INT     (1 << 7)
#define ATOD_TYPE_BIG_FLOAT   (2 << 7)
#define ATOD_TYPE_BIG_DECIMAL (3 << 7)
/* assume bigint mode: floats are parsed as integers if no decimal
   point nor exponent */
#define ATOD_MODE_BIGINT      (1 << 9)
/* accept -0x1 */
#define ATOD_ACCEPT_PREFIX_AFTER_SIGN (1 << 10)

#define special_reduce       0
#define special_reduceRight  1

#define special_every    0
#define special_some     1
#define special_forEach  2
#define special_map      3
#define special_filter   4
#define special_TA       8

#define PF_IN_ACCEPTED  (1 << 0) /* allow the 'in' binary operator */
#define PF_POSTFIX_CALL (1 << 1) /* allow function calls parsing in js_parse_postfix_expr() */
#define PF_ARROW_FUNC   (1 << 2) /* allow arrow functions parsing in js_parse_postfix_expr() */
#define PF_POW_ALLOWED  (1 << 3) /* allow the exponentiation operator in js_parse_unary() */
#define PF_POW_FORBIDDEN (1 << 4) /* forbid the exponentiation operator in js_parse_unary() */

#define PROP_TYPE_IDENT 0
#define PROP_TYPE_VAR   1
#define PROP_TYPE_GET   2
#define PROP_TYPE_SET   3
#define PROP_TYPE_STAR  4
#define PROP_TYPE_ASYNC 5
#define PROP_TYPE_ASYNC_STAR 6

#define PROP_TYPE_PRIVATE (1 << 4)

#define SKIP_HAS_SEMI       (1 << 0)
#define SKIP_HAS_ELLIPSIS   (1 << 1)
#define SKIP_HAS_ASSIGNMENT (1 << 2)

#define DECL_MASK_FUNC  (1 << 0) /* allow normal function declaration */
/* ored with DECL_MASK_FUNC if function declarations are allowed with a label */
#define DECL_MASK_FUNC_WITH_LABEL (1 << 1)
#define DECL_MASK_OTHER (1 << 2) /* all other declarations */
#define DECL_MASK_ALL   (DECL_MASK_FUNC | DECL_MASK_FUNC_WITH_LABEL | DECL_MASK_OTHER)

/* dump object free */
//#define DUMP_FREE
//#define DUMP_CLOSURE
/* dump the bytecode of the compiled functions: combination of bits
   1: dump pass 3 final byte code
   2: dump pass 2 code
   4: dump pass 1 code
   8: dump stdlib functions
  16: dump bytecode in hex
  32: dump line number table
 */
//#define DUMP_BYTECODE  (1)
/* dump the occurence of the automatic GC */
//#define DUMP_GC
/* dump objects freed by the garbage collector */
//#define DUMP_GC_FREE
/* dump objects leaking when freeing the runtime */
//#define DUMP_LEAKS  1
/* dump memory usage before running the garbage collector */
//#define DUMP_MEM
//#define DUMP_OBJECTS    /* dump objects in JS_FreeContext */
//#define DUMP_ATOMS      /* dump atoms in JS_FreeContext */
//#define DUMP_SHAPES     /* dump shapes in JS_FreeContext */
//#define DUMP_MODULE_RESOLVE
//#define DUMP_PROMISE
//#define DUMP_READ_OBJECT

/* test the GC by forcing it before each object allocation */
//#define FORCE_GC_AT_MALLOC

enum {
    MATH_OP_ABS,
    MATH_OP_FLOOR,
    MATH_OP_CEIL,
    MATH_OP_ROUND,
    MATH_OP_TRUNC,
    MATH_OP_SQRT,
    MATH_OP_FPROUND,
    MATH_OP_ACOS,
    MATH_OP_ASIN,
    MATH_OP_ATAN,
    MATH_OP_ATAN2,
    MATH_OP_COS,
    MATH_OP_EXP,
    MATH_OP_LOG,
    MATH_OP_POW,
    MATH_OP_SIN,
    MATH_OP_TAN,
    MATH_OP_FMOD,
    MATH_OP_REM,
    MATH_OP_SIGN,
    MATH_OP_ADD,
    MATH_OP_SUB,
    MATH_OP_MUL,
    MATH_OP_DIV,
};

enum {
    /* classid tag        */    /* union usage   | properties */
    JS_CLASS_OBJECT = 1,        /* must be first */
    JS_CLASS_ARRAY,             /* u.array       | length */
    JS_CLASS_ERROR,
    JS_CLASS_NUMBER,            /* u.object_data */
    JS_CLASS_STRING,            /* u.object_data */
    JS_CLASS_BOOLEAN,           /* u.object_data */
    JS_CLASS_SYMBOL,            /* u.object_data */
    JS_CLASS_ARGUMENTS,         /* u.array       | length */
    JS_CLASS_MAPPED_ARGUMENTS,  /*               | length */
    JS_CLASS_DATE,              /* u.object_data */
    JS_CLASS_MODULE_NS,
    JS_CLASS_C_FUNCTION,        /* u.cfunc */
    JS_CLASS_BYTECODE_FUNCTION, /* u.func */
    JS_CLASS_BOUND_FUNCTION,    /* u.bound_function */
    JS_CLASS_C_FUNCTION_DATA,   /* u.c_function_data_record */
    JS_CLASS_GENERATOR_FUNCTION, /* u.func */
    JS_CLASS_FOR_IN_ITERATOR,   /* u.for_in_iterator */
    JS_CLASS_REGEXP,            /* u.regexp */
    JS_CLASS_ARRAY_BUFFER,      /* u.array_buffer */
    JS_CLASS_SHARED_ARRAY_BUFFER, /* u.array_buffer */
    JS_CLASS_UINT8C_ARRAY,      /* u.array (typed_array) */
    JS_CLASS_INT8_ARRAY,        /* u.array (typed_array) */
    JS_CLASS_UINT8_ARRAY,       /* u.array (typed_array) */
    JS_CLASS_INT16_ARRAY,       /* u.array (typed_array) */
    JS_CLASS_UINT16_ARRAY,      /* u.array (typed_array) */
    JS_CLASS_INT32_ARRAY,       /* u.array (typed_array) */
    JS_CLASS_UINT32_ARRAY,      /* u.array (typed_array) */
#ifdef CONFIG_BIGNUM
    JS_CLASS_BIG_INT64_ARRAY,   /* u.array (typed_array) */
    JS_CLASS_BIG_UINT64_ARRAY,  /* u.array (typed_array) */
#endif
    JS_CLASS_FLOAT32_ARRAY,     /* u.array (typed_array) */
    JS_CLASS_FLOAT64_ARRAY,     /* u.array (typed_array) */
    JS_CLASS_DATAVIEW,          /* u.typed_array */
#ifdef CONFIG_BIGNUM
    JS_CLASS_BIG_INT,           /* u.object_data */
    JS_CLASS_BIG_FLOAT,         /* u.object_data */
    JS_CLASS_FLOAT_ENV,         /* u.float_env */
    JS_CLASS_BIG_DECIMAL,       /* u.object_data */
    JS_CLASS_OPERATOR_SET,      /* u.operator_set */
#endif
    JS_CLASS_MAP,               /* u.map_state */
    JS_CLASS_SET,               /* u.map_state */
    JS_CLASS_WEAKMAP,           /* u.map_state */
    JS_CLASS_WEAKSET,           /* u.map_state */
    JS_CLASS_MAP_ITERATOR,      /* u.map_iterator_data */
    JS_CLASS_SET_ITERATOR,      /* u.map_iterator_data */
    JS_CLASS_ARRAY_ITERATOR,    /* u.array_iterator_data */
    JS_CLASS_STRING_ITERATOR,   /* u.array_iterator_data */
    JS_CLASS_REGEXP_STRING_ITERATOR,   /* u.regexp_string_iterator_data */
    JS_CLASS_GENERATOR,         /* u.generator_data */
    JS_CLASS_PROXY,             /* u.proxy_data */
    JS_CLASS_PROMISE,           /* u.promise_data */
    JS_CLASS_PROMISE_RESOLVE_FUNCTION,  /* u.promise_function_data */
    JS_CLASS_PROMISE_REJECT_FUNCTION,   /* u.promise_function_data */
    JS_CLASS_ASYNC_FUNCTION,            /* u.func */
    JS_CLASS_ASYNC_FUNCTION_RESOLVE,    /* u.async_function_data */
    JS_CLASS_ASYNC_FUNCTION_REJECT,     /* u.async_function_data */
    JS_CLASS_ASYNC_FROM_SYNC_ITERATOR,  /* u.async_from_sync_iterator_data */
    JS_CLASS_ASYNC_GENERATOR_FUNCTION,  /* u.func */
    JS_CLASS_ASYNC_GENERATOR,   /* u.async_generator_data */
    JS_CLASS_INIT_COUNT, /* last entry for predefined classes */
};

typedef enum OPCodeFormat {
#define FMT(f) OP_FMT_ ## f,
#define DEF(id, size, n_pop, n_push, f)
#include "third_party/quickjs/quickjs-opcode.inc"
#undef DEF
#undef FMT
} OPCodeFormat;

enum OPCodeEnum {
#define FMT(f)
#define DEF(id, size, n_pop, n_push, f) OP_ ## id,
#define def(id, size, n_pop, n_push, f)
#include "third_party/quickjs/quickjs-opcode.inc"
#undef def
#undef DEF
#undef FMT
    OP_COUNT, /* excluding temporary opcodes */
    /* temporary opcodes : overlap with the short opcodes */
    OP_TEMP_START = OP_nop + 1,
    OP___dummy = OP_TEMP_START - 1,
#define FMT(f)
#define DEF(id, size, n_pop, n_push, f)
#define def(id, size, n_pop, n_push, f) OP_ ## id,
#include "third_party/quickjs/quickjs-opcode.inc"
#undef def
#undef DEF
#undef FMT
    OP_TEMP_END,
};

enum {
    __JS_ATOM_NULL = JS_ATOM_NULL,
#define DEF(name, str) JS_ATOM_ ## name,
#include "third_party/quickjs/quickjs-atom.inc"
#undef DEF
    JS_ATOM_END,
};
#define JS_ATOM_LAST_KEYWORD JS_ATOM_super
#define JS_ATOM_LAST_STRICT_KEYWORD JS_ATOM_yield

typedef enum JSStrictEqModeEnum {
    JS_EQ_STRICT,
    JS_EQ_SAME_VALUE,
    JS_EQ_SAME_VALUE_ZERO,
} JSStrictEqModeEnum;

/* number of typed array types */
#define JS_TYPED_ARRAY_COUNT  (JS_CLASS_FLOAT64_ARRAY - JS_CLASS_UINT8C_ARRAY + 1)
static uint8_t const typed_array_size_log2[JS_TYPED_ARRAY_COUNT];
#define typed_array_size_log2(classid)  (typed_array_size_log2[(classid)- JS_CLASS_UINT8C_ARRAY])

typedef enum JSErrorEnum {
    JS_EVAL_ERROR,
    JS_RANGE_ERROR,
    JS_REFERENCE_ERROR,
    JS_SYNTAX_ERROR,
    JS_TYPE_ERROR,
    JS_URI_ERROR,
    JS_INTERNAL_ERROR,
    JS_AGGREGATE_ERROR,
    JS_NATIVE_ERROR_COUNT, /* number of different NativeError objects */
} JSErrorEnum;

#define JS_MAX_LOCAL_VARS 65536
#define JS_STACK_SIZE_MAX 65534
#define JS_STRING_LEN_MAX ((1 << 30) - 1)

#define __exception dontdiscard

typedef struct JSShape JSShape;
typedef struct JSString JSString;
typedef struct JSString JSAtomStruct;

typedef enum {
    JS_GC_PHASE_NONE,
    JS_GC_PHASE_DECREF,
    JS_GC_PHASE_REMOVE_CYCLES,
} JSGCPhaseEnum;

typedef enum OPCodeEnum OPCodeEnum;

#ifdef CONFIG_BIGNUM
/* function pointers are used for numeric operations so that it is
   possible to remove some numeric types */
typedef struct {
    JSValue (*to_string)(JSContext *ctx, JSValueConst val);
    JSValue (*from_string)(JSContext *ctx, const char *buf,
                           int radix, int flags, slimb_t *pexponent);
    int (*unary_arith)(JSContext *ctx,
                       JSValue *pres, OPCodeEnum op, JSValue op1);
    int (*binary_arith)(JSContext *ctx, OPCodeEnum op,
                        JSValue *pres, JSValue op1, JSValue op2);
    int (*compare)(JSContext *ctx, OPCodeEnum op,
                   JSValue op1, JSValue op2);
    /* only for bigfloat: */
    JSValue (*mul_pow10_to_float64)(JSContext *ctx, const bf_t *a,
                                    int64_t exponent);
    int (*mul_pow10)(JSContext *ctx, JSValue *sp);
} JSNumericOperations;
#endif

typedef enum JSIteratorKindEnum {
    JS_ITERATOR_KIND_KEY,
    JS_ITERATOR_KIND_VALUE,
    JS_ITERATOR_KIND_KEY_AND_VALUE,
} JSIteratorKindEnum;

typedef struct JSMapRecord {
    int ref_count; /* used during enumeration to avoid freeing the record */
    BOOL empty; /* TRUE if the record is deleted */
    struct JSMapState *map;
    struct JSMapRecord *next_weak_ref;
    struct list_head link;
    struct list_head hash_link;
    JSValue key;
    JSValue value;
} JSMapRecord;

typedef struct JSMapIteratorData {
    JSValue obj;
    JSIteratorKindEnum kind;
    JSMapRecord *cur_record;
} JSMapIteratorData;

typedef struct JSMapState {
    BOOL is_weak; /* TRUE if WeakSet/WeakMap */
    struct list_head records; /* list of JSMapRecord.link */
    uint32_t record_count;
    struct list_head *hash_table;
    uint32_t hash_size; /* must be a power of two */
    uint32_t record_count_threshold; /* count at which a hash table
                                        resize is needed */
} JSMapState;

struct JSRuntime {
    JSMallocFunctions mf;
    JSMallocState malloc_state;
    const char *rt_info;
    int atom_hash_size; /* power of two */
    int atom_count;
    int atom_size;
    int atom_count_resize; /* resize hash table at this count */
    uint32_t *atom_hash;
    JSAtomStruct **atom_array;
    int atom_free_index; /* 0 = none */
    int class_count;    /* size of class_array */
    JSClass *class_array;
    struct list_head context_list; /* list of JSContext.link */
    /* list of JSGCObjectHeader.link. List of allocated GC objects (used
       by the garbage collector) */
    struct list_head gc_obj_list;
    /* list of JSGCObjectHeader.link. Used during JS_FreeValueRT() */
    struct list_head gc_zero_ref_count_list;
    struct list_head tmp_obj_list; /* used during GC */
    JSGCPhaseEnum gc_phase : 8;
    size_t malloc_gc_threshold;
#ifdef DUMP_LEAKS
    struct list_head string_list; /* list of JSString.link */
#endif
    /* stack limitation */
    uintptr_t stack_size; /* in bytes, 0 if no limit */
    uintptr_t stack_top;
    uintptr_t stack_limit; /* lower stack limit */
    JSValue current_exception;
    /* true if inside an out of memory error, to avoid recursing */
    BOOL in_out_of_memory : 8;
    struct JSStackFrame *current_stack_frame;
    JSInterruptHandler *interrupt_handler;
    void *interrupt_opaque;
    JSHostPromiseRejectionTracker *host_promise_rejection_tracker;
    void *host_promise_rejection_tracker_opaque;
    struct list_head job_list; /* list of JSJobEntry.link */
    JSModuleNormalizeFunc *module_normalize_func;
    JSModuleLoaderFunc *module_loader_func;
    void *module_loader_opaque;
    BOOL can_block : 8; /* TRUE if Atomics.wait can block */
    /* used to allocate, free and clone SharedArrayBuffers */
    JSSharedArrayBufferFunctions sab_funcs;
    /* Shape hash table */
    int shape_hash_bits;
    int shape_hash_size;
    int shape_hash_count; /* number of hashed shapes */
    JSShape **shape_hash;
#ifdef CONFIG_BIGNUM
    bf_context_t bf_ctx;
    JSNumericOperations bigint_ops;
    JSNumericOperations bigfloat_ops;
    JSNumericOperations bigdecimal_ops;
    uint32_t operator_count;
#endif
    void *user_opaque;
};

struct JSClass {
    uint32_t class_id; /* 0 means free entry */
    JSAtom class_name;
    JSClassFinalizer *finalizer;
    JSClassGCMark *gc_mark;
    JSClassCall *call;
    /* pointers for exotic behavior, can be NULL if none are present */
    const JSClassExoticMethods *exotic;
};

#define JS_MODE_STRICT (1 << 0)
#define JS_MODE_STRIP  (1 << 1)
#define JS_MODE_MATH   (1 << 2)

typedef struct JSStackFrame {
    struct JSStackFrame *prev_frame; /* NULL if first stack frame */
    JSValue cur_func; /* current function, JS_UNDEFINED if the frame is detached */
    JSValue *arg_buf; /* arguments */
    JSValue *var_buf; /* variables */
    struct list_head var_ref_list; /* list of JSVarRef.link */
    const uint8_t *cur_pc; /* only used in bytecode functions : PC of the
                        instruction after the call */
    int arg_count;
    int js_mode; /* 0 or JS_MODE_MATH for C functions */
    /* only used in generators. Current stack pointer value. NULL if
       the function is running. */
    JSValue *cur_sp;
} JSStackFrame;

typedef enum {
    JS_GC_OBJ_TYPE_JS_OBJECT,
    JS_GC_OBJ_TYPE_FUNCTION_BYTECODE,
    JS_GC_OBJ_TYPE_SHAPE,
    JS_GC_OBJ_TYPE_VAR_REF,
    JS_GC_OBJ_TYPE_ASYNC_FUNCTION,
    JS_GC_OBJ_TYPE_JS_CONTEXT,
} JSGCObjectTypeEnum;

/* header for GC objects. GC objects are C data structures with a
   reference count that can reference other GC objects. JS Objects are
   a particular type of GC object. */
struct JSGCObjectHeader {
    int ref_count; /* must come first, 32-bit */
    JSGCObjectTypeEnum gc_obj_type : 4;
    uint8_t mark : 4; /* used by the GC */
    uint8_t dummy1; /* not used by the GC */
    uint16_t dummy2; /* not used by the GC */
    struct list_head link;
};

typedef struct JSVarRef {
    union {
        JSGCObjectHeader header; /* must come first */
        struct {
            int __gc_ref_count; /* corresponds to header.ref_count */
            uint8_t __gc_mark; /* corresponds to header.mark/gc_obj_type */
            /* 0 : the JSVarRef is on the stack. header.link is an element
               of JSStackFrame.var_ref_list.
               1 : the JSVarRef is detached. header.link has the normal meanning
            */
            uint8_t is_detached : 1;
            uint8_t is_arg : 1;
            uint16_t var_idx; /* index of the corresponding function variable on
                                 the stack */
        };
    };
    JSValue *pvalue; /* pointer to the value, either on the stack or
                        to 'value' */
    JSValue value; /* used when the variable is no longer on the stack */
} JSVarRef;

typedef struct JSFloatEnv {
    limb_t prec;
    bf_flags_t flags;
    unsigned int status;
} JSFloatEnv;

/* the same structure is used for big integers and big floats. Big
   integers are never infinite or NaNs */
typedef struct JSBigFloat {
    JSRefCountHeader header; /* must come first, 32-bit */
    bf_t num;
} JSBigFloat;

typedef struct JSBigDecimal {
    JSRefCountHeader header; /* must come first, 32-bit */
    bfdec_t num;
} JSBigDecimal;

typedef enum {
    JS_AUTOINIT_ID_PROTOTYPE,
    JS_AUTOINIT_ID_MODULE_NS,
    JS_AUTOINIT_ID_PROP,
} JSAutoInitIDEnum;

/* must be large enough to have a negligible runtime cost and small
   enough to call the interrupt callback often. */
#define JS_INTERRUPT_COUNTER_INIT 10000

struct JSContext {
    JSGCObjectHeader header; /* must come first */
    JSRuntime *rt;
    struct list_head link;
    uint16_t binary_object_count;
    int binary_object_size;
    JSShape *array_shape;   /* initial shape for Array objects */
    JSValue *class_proto;
    JSValue function_proto;
    JSValue function_ctor;
    JSValue array_ctor;
    JSValue regexp_ctor;
    JSValue promise_ctor;
    JSValue native_error_proto[JS_NATIVE_ERROR_COUNT];
    JSValue iterator_proto;
    JSValue async_iterator_proto;
    JSValue array_proto_values;
    JSValue throw_type_error;
    JSValue eval_obj;
    JSValue global_obj; /* global object */
    JSValue global_var_obj; /* contains the global let/const definitions */
    uint64_t random_state;
#ifdef CONFIG_BIGNUM
    bf_context_t *bf_ctx;   /* points to rt->bf_ctx, shared by all contexts */
    JSFloatEnv fp_env; /* global FP environment */
    BOOL bignum_ext : 8; /* enable math mode */
    BOOL allow_operator_overloading : 8;
#endif
    /* when the counter reaches zero, JSRutime.interrupt_handler is called */
    int interrupt_counter;
    BOOL is_error_property_enabled;
    struct list_head loaded_modules; /* list of JSModuleDef.link */
    /* if NULL, RegExp compilation is not supported */
    JSValue (*compile_regexp)(JSContext *ctx, JSValueConst pattern,
                              JSValueConst flags);
    /* if NULL, eval is not supported */
    JSValue (*eval_internal)(JSContext *ctx, JSValueConst this_obj,
                             const char *input, size_t input_len,
                             const char *filename, int flags, int scope_idx);
    void *user_opaque;
};

typedef union JSFloat64Union {
    double d;
    uint64_t u64;
    uint32_t u32[2];
} JSFloat64Union;

enum {
    JS_ATOM_TYPE_STRING = 1,
    JS_ATOM_TYPE_GLOBAL_SYMBOL,
    JS_ATOM_TYPE_SYMBOL,
    JS_ATOM_TYPE_PRIVATE,
};

enum {
    JS_ATOM_HASH_SYMBOL,
    JS_ATOM_HASH_PRIVATE,
};

typedef enum {
    JS_ATOM_KIND_STRING,
    JS_ATOM_KIND_SYMBOL,
    JS_ATOM_KIND_PRIVATE,
} JSAtomKindEnum;

#define JS_ATOM_HASH_MASK  ((1 << 30) - 1)

struct JSString {
    JSRefCountHeader header; /* must come first, 32-bit */
    uint32_t len : 31;
    uint8_t is_wide_char : 1; /* 0 = 8 bits, 1 = 16 bits characters */
    /* for JS_ATOM_TYPE_SYMBOL: hash = 0, atom_type = 3,
       for JS_ATOM_TYPE_PRIVATE: hash = 1, atom_type = 3
       XXX: could change encoding to have one more bit in hash */
    uint32_t hash : 30;
    uint8_t atom_type : 2; /* != 0 if atom, JS_ATOM_TYPE_x */
    uint32_t hash_next; /* atom_index for JS_ATOM_TYPE_SYMBOL */
#ifdef DUMP_LEAKS
    struct list_head link; /* string list */
#endif
    union {
        uint8_t str8[0]; /* 8 bit strings will get an extra null terminator */
        uint16_t str16[0];
    } u;
};

typedef struct JSClosureVar {
    uint8_t is_local : 1;
    uint8_t is_arg : 1;
    uint8_t is_const : 1;
    uint8_t is_lexical : 1;
    uint8_t var_kind : 4; /* see JSVarKindEnum */
    /* 8 bits available */
    uint16_t var_idx; /* is_local = TRUE: index to a normal variable of the
                    parent function. otherwise: index to a closure
                    variable of the parent function */
    JSAtom var_name;
} JSClosureVar;

#define ARG_SCOPE_INDEX 1
#define ARG_SCOPE_END (-2)

typedef struct JSVarScope {
    int parent;  /* index into fd->scopes of the enclosing scope */
    int first;   /* index into fd->vars of the last variable in this scope */
} JSVarScope;

typedef enum {
    /* XXX: add more variable kinds here instead of using bit fields */
    JS_VAR_NORMAL,
    JS_VAR_FUNCTION_DECL, /* lexical var with function declaration */
    JS_VAR_NEW_FUNCTION_DECL, /* lexical var with async/generator
                                 function declaration */
    JS_VAR_CATCH,
    JS_VAR_FUNCTION_NAME, /* function expression name */
    JS_VAR_PRIVATE_FIELD,
    JS_VAR_PRIVATE_METHOD,
    JS_VAR_PRIVATE_GETTER,
    JS_VAR_PRIVATE_SETTER, /* must come after JS_VAR_PRIVATE_GETTER */
    JS_VAR_PRIVATE_GETTER_SETTER, /* must come after JS_VAR_PRIVATE_SETTER */
} JSVarKindEnum;

/* XXX: could use a different structure in bytecode functions to save
   memory */
typedef struct JSVarDef {
    JSAtom var_name;
    /* index into fd->scopes of this variable lexical scope */
    int scope_level;
    /* during compilation:
        - if scope_level = 0: scope in which the variable is defined
        - if scope_level != 0: index into fd->vars of the next
          variable in the same or enclosing lexical scope
       in a bytecode function:
       index into fd->vars of the next
       variable in the same or enclosing lexical scope
    */
    int scope_next;
    uint8_t is_const : 1;
    uint8_t is_lexical : 1;
    uint8_t is_captured : 1;
    uint8_t var_kind : 4; /* see JSVarKindEnum */
    /* only used during compilation: function pool index for lexical
       variables with var_kind =
       JS_VAR_FUNCTION_DECL/JS_VAR_NEW_FUNCTION_DECL or scope level of
       the definition of the 'var' variables (they have scope_level =
       0) */
    int func_pool_idx : 24; /* only used during compilation : index in
                               the constant pool for hoisted function
                               definition */
} JSVarDef;

/* for the encoding of the pc2line table */
#define PC2LINE_BASE     (-1)
#define PC2LINE_RANGE    5
#define PC2LINE_OP_FIRST 1
#define PC2LINE_DIFF_PC_MAX ((255 - PC2LINE_OP_FIRST) / PC2LINE_RANGE)

typedef enum JSFunctionKindEnum {
    JS_FUNC_NORMAL = 0,
    JS_FUNC_GENERATOR = (1 << 0),
    JS_FUNC_ASYNC = (1 << 1),
    JS_FUNC_ASYNC_GENERATOR = (JS_FUNC_GENERATOR | JS_FUNC_ASYNC),
} JSFunctionKindEnum;

typedef struct JSRegExpStringIteratorData {
    JSValue iterating_regexp;
    JSValue iterated_string;
    BOOL global;
    BOOL unicode;
    BOOL done;
} JSRegExpStringIteratorData;

typedef struct JSFunctionBytecode {
    JSGCObjectHeader header; /* must come first */
    uint8_t js_mode;
    uint8_t has_prototype : 1; /* true if a prototype field is necessary */
    uint8_t has_simple_parameter_list : 1;
    uint8_t is_derived_class_constructor : 1;
    /* true if home_object needs to be initialized */
    uint8_t need_home_object : 1;
    uint8_t func_kind : 2;
    uint8_t new_target_allowed : 1;
    uint8_t super_call_allowed : 1;
    uint8_t super_allowed : 1;
    uint8_t arguments_allowed : 1;
    uint8_t has_debug : 1;
    uint8_t backtrace_barrier : 1; /* stop backtrace on this function */
    uint8_t read_only_bytecode : 1;
    /* XXX: 4 bits available */
    uint8_t *byte_code_buf; /* (self pointer) */
    int byte_code_len;
    JSAtom func_name;
    JSVarDef *vardefs; /* arguments + local variables (arg_count + var_count) (self pointer) */
    JSClosureVar *closure_var; /* list of variables in the closure (self pointer) */
    uint16_t arg_count;
    uint16_t var_count;
    uint16_t defined_arg_count; /* for length function property */
    uint16_t stack_size; /* maximum stack size */
    JSContext *realm; /* function realm */
    JSValue *cpool; /* constant pool (self pointer) */
    int cpool_count;
    int closure_var_count;
    struct {
        /* debug info, move to separate structure to save memory? */
        JSAtom filename;
        int line_num;
        int source_len;
        int pc2line_len;
        uint8_t *pc2line_buf;
        char *source;
    } debug;
} JSFunctionBytecode;

typedef struct JSBoundFunction {
    JSValue func_obj;
    JSValue this_val;
    int argc;
    JSValue argv[0];
} JSBoundFunction;

typedef struct JSForInIterator {
    JSValue obj;
    BOOL is_array;
    uint32_t array_length;
    uint32_t idx;
} JSForInIterator;

typedef struct JSRegExp {
    JSString *pattern;
    JSString *bytecode; /* also contains the flags */
} JSRegExp;

typedef struct JSProxyData {
    JSValue target;
    JSValue handler;
    uint8_t is_func;
    uint8_t is_revoked;
} JSProxyData;

typedef struct JSArrayBuffer {
    int byte_length; /* 0 if detached */
    uint8_t detached;
    uint8_t shared; /* if shared, the array buffer cannot be detached */
    uint8_t *data; /* NULL if detached */
    struct list_head array_list;
    void *opaque;
    JSFreeArrayBufferDataFunc *free_func;
} JSArrayBuffer;

typedef struct JSTypedArray {
    struct list_head link; /* link to arraybuffer */
    JSObject *obj; /* back pointer to the TypedArray/DataView object */
    JSObject *buffer; /* based array buffer */
    uint32_t offset; /* offset in the array buffer */
    uint32_t length; /* length in the array buffer */
} JSTypedArray;

typedef struct JSAsyncFunctionState {
    JSValue this_val; /* 'this' generator argument */
    int argc; /* number of function arguments */
    BOOL throw_flag; /* used to throw an exception in JS_CallInternal() */
    JSStackFrame frame;
} JSAsyncFunctionState;

/* XXX: could use an object instead to avoid the
   JS_TAG_ASYNC_FUNCTION tag for the GC */
typedef struct JSAsyncFunctionData {
    JSGCObjectHeader header; /* must come first */
    JSValue resolving_funcs[2];
    BOOL is_active; /* true if the async function state is valid */
    JSAsyncFunctionState func_state;
} JSAsyncFunctionData;

typedef enum {
   /* binary operators */
   JS_OVOP_ADD,
   JS_OVOP_SUB,
   JS_OVOP_MUL,
   JS_OVOP_DIV,
   JS_OVOP_MOD,
   JS_OVOP_POW,
   JS_OVOP_OR,
   JS_OVOP_AND,
   JS_OVOP_XOR,
   JS_OVOP_SHL,
   JS_OVOP_SAR,
   JS_OVOP_SHR,
   JS_OVOP_EQ,
   JS_OVOP_LESS,
   JS_OVOP_BINARY_COUNT,
   /* unary operators */
   JS_OVOP_POS = JS_OVOP_BINARY_COUNT,
   JS_OVOP_NEG,
   JS_OVOP_INC,
   JS_OVOP_DEC,
   JS_OVOP_NOT,
   JS_OVOP_COUNT,
} JSOverloadableOperatorEnum;

typedef struct StringBuffer {
    JSContext *ctx;
    JSString *str;
    int len;
    int size;
    int is_wide_char;
    int error_status;
} StringBuffer;

typedef struct {
    uint32_t operator_index;
    JSObject *ops[JS_OVOP_BINARY_COUNT]; /* self operators */
} JSBinaryOperatorDefEntry;

typedef struct {
    int count;
    JSBinaryOperatorDefEntry *tab;
} JSBinaryOperatorDef;

typedef struct {
    uint32_t operator_counter;
    BOOL is_primitive; /* OperatorSet for a primitive type */
    /* NULL if no operator is defined */
    JSObject *self_ops[JS_OVOP_COUNT]; /* self operators */
    JSBinaryOperatorDef left;
    JSBinaryOperatorDef right;
} JSOperatorSetData;

typedef struct JSReqModuleEntry {
    JSAtom module_name;
    JSModuleDef *module; /* used using resolution */
} JSReqModuleEntry;

typedef enum JSExportTypeEnum {
    JS_EXPORT_TYPE_LOCAL,
    JS_EXPORT_TYPE_INDIRECT,
} JSExportTypeEnum;

typedef struct JSExportEntry {
    union {
        struct {
            int var_idx; /* closure variable index */
            JSVarRef *var_ref; /* if != NULL, reference to the variable */
        } local; /* for local export */
        int req_module_idx; /* module for indirect export */
    } u;
    JSExportTypeEnum export_type;
    JSAtom local_name; /* '*' if export ns from. not used for local
                          export after compilation */
    JSAtom export_name; /* exported variable name */
} JSExportEntry;

typedef struct JSStarExportEntry {
    int req_module_idx; /* in req_module_entries */
} JSStarExportEntry;

typedef struct JSImportEntry {
    int var_idx; /* closure variable index */
    JSAtom import_name;
    int req_module_idx; /* in req_module_entries */
} JSImportEntry;

struct JSModuleDef {
    JSRefCountHeader header; /* must come first, 32-bit */
    JSAtom module_name;
    struct list_head link;
    JSReqModuleEntry *req_module_entries;
    int req_module_entries_count;
    int req_module_entries_size;
    JSExportEntry *export_entries;
    int export_entries_count;
    int export_entries_size;
    JSStarExportEntry *star_export_entries;
    int star_export_entries_count;
    int star_export_entries_size;
    JSImportEntry *import_entries;
    int import_entries_count;
    int import_entries_size;
    JSValue module_ns;
    JSValue func_obj; /* only used for JS modules */
    JSModuleInitFunc *init_func; /* only used for C modules */
    BOOL resolved : 8;
    BOOL func_created : 8;
    BOOL instantiated : 8;
    BOOL evaluated : 8;
    BOOL eval_mark : 8; /* temporary use during js_evaluate_module() */
    /* true if evaluation yielded an exception. It is saved in
       eval_exception */
    BOOL eval_has_exception : 8;
    JSValue eval_exception;
    JSValue meta_obj; /* for import.meta */
};

typedef struct JSJobEntry {
    struct list_head link;
    JSContext *ctx;
    JSJobFunc *job_func;
    int argc;
    JSValue argv[0];
} JSJobEntry;

typedef struct JSProperty {
    union {
        JSValue value;      /* JS_PROP_NORMAL */
        struct {            /* JS_PROP_GETSET */
            JSObject *getter; /* NULL if undefined */
            JSObject *setter; /* NULL if undefined */
        } getset;
        JSVarRef *var_ref;  /* JS_PROP_VARREF */
        struct {            /* JS_PROP_AUTOINIT */
            /* in order to use only 2 pointers, we compress the realm
               and the init function pointer */
            uintptr_t realm_and_id; /* realm and init_id (JS_AUTOINIT_ID_x)
                                       in the 2 low bits */
            void *opaque;
        } init;
    } u;
} JSProperty;

#define JS_PROP_INITIAL_SIZE 2
#define JS_PROP_INITIAL_HASH_SIZE 4 /* must be a power of two */
#define JS_ARRAY_INITIAL_SIZE 2

typedef struct JSShapeProperty {
    uint32_t hash_next : 26; /* 0 if last in list */
    uint32_t flags : 6;   /* JS_PROP_XXX */
    JSAtom atom; /* JS_ATOM_NULL = free property entry */
} JSShapeProperty;

struct JSShape {
    /* hash table of size hash_mask + 1 before the start of the
       structure (see prop_hash_end()). */
    JSGCObjectHeader header;
    /* true if the shape is inserted in the shape hash table. If not,
       JSShape.hash is not valid */
    uint8_t is_hashed;
    /* If true, the shape may have small array index properties 'n' with 0
       <= n <= 2^31-1. If false, the shape is guaranteed not to have
       small array index properties */
    uint8_t has_small_array_index;
    uint32_t hash; /* current hash value */
    uint32_t prop_hash_mask;
    int prop_size; /* allocated properties */
    int prop_count; /* include deleted properties */
    int deleted_prop_count;
    JSShape *shape_hash_next; /* in JSRuntime.shape_hash[h] list */
    JSObject *proto;
    JSShapeProperty prop[0]; /* prop_size elements */
};

struct JSObject {
    union {
        JSGCObjectHeader header;
        struct {
            int __gc_ref_count; /* corresponds to header.ref_count */
            uint8_t __gc_mark; /* corresponds to header.mark/gc_obj_type */

            uint8_t extensible : 1;
            uint8_t free_mark : 1; /* only used when freeing objects with cycles */
            uint8_t is_exotic : 1; /* TRUE if object has exotic property handlers */
            uint8_t fast_array : 1; /* TRUE if u.array is used for get/put (for JS_CLASS_ARRAY, JS_CLASS_ARGUMENTS and typed arrays) */
            uint8_t is_constructor : 1; /* TRUE if object is a constructor function */
            uint8_t is_uncatchable_error : 1; /* if TRUE, error is not catchable */
            uint8_t tmp_mark : 1; /* used in JS_WriteObjectRec() */
            uint8_t is_HTMLDDA : 1; /* specific annex B IsHtmlDDA behavior */
            uint16_t class_id; /* see JS_CLASS_x */
        };
    };
    /* byte offsets: 16/24 */
    JSShape *shape; /* prototype and property names + flag */
    JSProperty *prop; /* array of properties */
    /* byte offsets: 24/40 */
    struct JSMapRecord *first_weak_ref; /* XXX: use a bit and an external hash table? */
    /* byte offsets: 28/48 */
    union {
        void *opaque;
        struct JSBoundFunction *bound_function; /* JS_CLASS_BOUND_FUNCTION */
        struct JSCFunctionDataRecord *c_function_data_record; /* JS_CLASS_C_FUNCTION_DATA */
        struct JSForInIterator *for_in_iterator; /* JS_CLASS_FOR_IN_ITERATOR */
        struct JSArrayBuffer *array_buffer; /* JS_CLASS_ARRAY_BUFFER, JS_CLASS_SHARED_ARRAY_BUFFER */
        struct JSTypedArray *typed_array; /* JS_CLASS_UINT8C_ARRAY..JS_CLASS_DATAVIEW */
#ifdef CONFIG_BIGNUM
        struct JSFloatEnv *float_env; /* JS_CLASS_FLOAT_ENV */
        struct JSOperatorSetData *operator_set; /* JS_CLASS_OPERATOR_SET */
#endif
        struct JSMapState *map_state;   /* JS_CLASS_MAP..JS_CLASS_WEAKSET */
        struct JSMapIteratorData *map_iterator_data; /* JS_CLASS_MAP_ITERATOR, JS_CLASS_SET_ITERATOR */
        struct JSArrayIteratorData *array_iterator_data; /* JS_CLASS_ARRAY_ITERATOR, JS_CLASS_STRING_ITERATOR */
        struct JSRegExpStringIteratorData *regexp_string_iterator_data; /* JS_CLASS_REGEXP_STRING_ITERATOR */
        struct JSGeneratorData *generator_data; /* JS_CLASS_GENERATOR */
        struct JSProxyData *proxy_data; /* JS_CLASS_PROXY */
        struct JSPromiseData *promise_data; /* JS_CLASS_PROMISE */
        struct JSPromiseFunctionData *promise_function_data; /* JS_CLASS_PROMISE_RESOLVE_FUNCTION, JS_CLASS_PROMISE_REJECT_FUNCTION */
        struct JSAsyncFunctionData *async_function_data; /* JS_CLASS_ASYNC_FUNCTION_RESOLVE, JS_CLASS_ASYNC_FUNCTION_REJECT */
        struct JSAsyncFromSyncIteratorData *async_from_sync_iterator_data; /* JS_CLASS_ASYNC_FROM_SYNC_ITERATOR */
        struct JSAsyncGeneratorData *async_generator_data; /* JS_CLASS_ASYNC_GENERATOR */
        struct { /* JS_CLASS_BYTECODE_FUNCTION: 12/24 bytes */
            /* also used by JS_CLASS_GENERATOR_FUNCTION, JS_CLASS_ASYNC_FUNCTION and JS_CLASS_ASYNC_GENERATOR_FUNCTION */
            struct JSFunctionBytecode *function_bytecode;
            JSVarRef **var_refs;
            JSObject *home_object; /* for 'super' access */
        } func;
        struct { /* JS_CLASS_C_FUNCTION: 12/20 bytes */
            JSContext *realm;
            JSCFunctionType c_function;
            uint8_t length;
            uint8_t cproto;
            int16_t magic;
        } cfunc;
        /* array part for fast arrays and typed arrays */
        struct { /* JS_CLASS_ARRAY, JS_CLASS_ARGUMENTS, JS_CLASS_UINT8C_ARRAY..JS_CLASS_FLOAT64_ARRAY */
            union {
                uint32_t size;          /* JS_CLASS_ARRAY, JS_CLASS_ARGUMENTS */
                struct JSTypedArray *typed_array; /* JS_CLASS_UINT8C_ARRAY..JS_CLASS_FLOAT64_ARRAY */
            } u1;
            union {
                JSValue *values;        /* JS_CLASS_ARRAY, JS_CLASS_ARGUMENTS */
                void *ptr;              /* JS_CLASS_UINT8C_ARRAY..JS_CLASS_FLOAT64_ARRAY */
                int8_t *int8_ptr;       /* JS_CLASS_INT8_ARRAY */
                uint8_t *uint8_ptr;     /* JS_CLASS_UINT8_ARRAY, JS_CLASS_UINT8C_ARRAY */
                int16_t *int16_ptr;     /* JS_CLASS_INT16_ARRAY */
                uint16_t *uint16_ptr;   /* JS_CLASS_UINT16_ARRAY */
                int32_t *int32_ptr;     /* JS_CLASS_INT32_ARRAY */
                uint32_t *uint32_ptr;   /* JS_CLASS_UINT32_ARRAY */
                int64_t *int64_ptr;     /* JS_CLASS_INT64_ARRAY */
                uint64_t *uint64_ptr;   /* JS_CLASS_UINT64_ARRAY */
                float *float_ptr;       /* JS_CLASS_FLOAT32_ARRAY */
                double *double_ptr;     /* JS_CLASS_FLOAT64_ARRAY */
            } u;
            uint32_t count; /* <= 2^31-1. 0 for a detached typed array */
        } array;    /* 12/20 bytes */
        JSRegExp regexp;    /* JS_CLASS_REGEXP: 8/16 bytes */
        JSValue object_data;    /* for JS_SetObjectData(): 8/16/16 bytes */
    } u;
    /* byte sizes: 40/48/72 */
};

enum {
    TOK_NUMBER = -128,
    TOK_STRING,
    TOK_TEMPLATE,
    TOK_IDENT,
    TOK_REGEXP,
    /* warning: order matters (see js_parse_assign_expr) */
    TOK_MUL_ASSIGN,
    TOK_DIV_ASSIGN,
    TOK_MOD_ASSIGN,
    TOK_PLUS_ASSIGN,
    TOK_MINUS_ASSIGN,
    TOK_SHL_ASSIGN,
    TOK_SAR_ASSIGN,
    TOK_SHR_ASSIGN,
    TOK_AND_ASSIGN,
    TOK_XOR_ASSIGN,
    TOK_OR_ASSIGN,
#ifdef CONFIG_BIGNUM
    TOK_MATH_POW_ASSIGN,
#endif
    TOK_POW_ASSIGN,
    TOK_LAND_ASSIGN,
    TOK_LOR_ASSIGN,
    TOK_DOUBLE_QUESTION_MARK_ASSIGN,
    TOK_DEC,
    TOK_INC,
    TOK_SHL,
    TOK_SAR,
    TOK_SHR,
    TOK_LT,
    TOK_LTE,
    TOK_GT,
    TOK_GTE,
    TOK_EQ,
    TOK_STRICT_EQ,
    TOK_NEQ,
    TOK_STRICT_NEQ,
    TOK_LAND,
    TOK_LOR,
#ifdef CONFIG_BIGNUM
    TOK_MATH_POW,
#endif
    TOK_POW,
    TOK_ARROW,
    TOK_ELLIPSIS,
    TOK_DOUBLE_QUESTION_MARK,
    TOK_QUESTION_MARK_DOT,
    TOK_ERROR,
    TOK_PRIVATE_NAME,
    TOK_EOF,
    /* keywords: WARNING: same order as atoms */
    TOK_NULL, /* must be first */
    TOK_FALSE,
    TOK_TRUE,
    TOK_IF,
    TOK_ELSE,
    TOK_RETURN,
    TOK_VAR,
    TOK_THIS,
    TOK_DELETE,
    TOK_VOID,
    TOK_TYPEOF,
    TOK_NEW,
    TOK_IN,
    TOK_INSTANCEOF,
    TOK_DO,
    TOK_WHILE,
    TOK_FOR,
    TOK_BREAK,
    TOK_CONTINUE,
    TOK_SWITCH,
    TOK_CASE,
    TOK_DEFAULT,
    TOK_THROW,
    TOK_TRY,
    TOK_CATCH,
    TOK_FINALLY,
    TOK_FUNCTION,
    TOK_DEBUGGER,
    TOK_WITH,
    /* FutureReservedWord */
    TOK_CLASS,
    TOK_CONST,
    TOK_ENUM,
    TOK_EXPORT,
    TOK_EXTENDS,
    TOK_IMPORT,
    TOK_SUPER,
    /* FutureReservedWords when parsing strict mode code */
    TOK_IMPLEMENTS,
    TOK_INTERFACE,
    TOK_LET,
    TOK_PACKAGE,
    TOK_PRIVATE,
    TOK_PROTECTED,
    TOK_PUBLIC,
    TOK_STATIC,
    TOK_YIELD,
    TOK_AWAIT, /* must be last */
    TOK_OF,     /* only used for js_parse_skip_parens_token() */
};

typedef struct BlockEnv {
    struct BlockEnv *prev;
    JSAtom label_name; /* JS_ATOM_NULL if none */
    int label_break; /* -1 if none */
    int label_cont; /* -1 if none */
    int drop_count; /* number of stack elements to drop */
    int label_finally; /* -1 if none */
    int scope_level;
    int has_iterator;
} BlockEnv;

typedef struct JSGlobalVar {
    int cpool_idx; /* if >= 0, index in the constant pool for hoisted
                      function defintion*/
    uint8_t force_init : 1; /* force initialization to undefined */
    uint8_t is_lexical : 1; /* global let/const definition */
    uint8_t is_const   : 1; /* const definition */
    int scope_level;    /* scope of definition */
    JSAtom var_name;  /* variable name */
} JSGlobalVar;

typedef struct RelocEntry {
    struct RelocEntry *next;
    uint32_t addr; /* address to patch */
    int size;   /* address size: 1, 2 or 4 bytes */
} RelocEntry;

typedef struct JumpSlot {
    int op;
    int size;
    int pos;
    int label;
} JumpSlot;

typedef struct LabelSlot {
    int ref_count;
    int pos;    /* phase 1 address, -1 means not resolved yet */
    int pos2;   /* phase 2 address, -1 means not resolved yet */
    int addr;   /* phase 3 address, -1 means not resolved yet */
    RelocEntry *first_reloc;
} LabelSlot;

typedef struct LineNumberSlot {
    uint32_t pc;
    int line_num;
} LineNumberSlot;

typedef enum JSParseFunctionEnum {
    JS_PARSE_FUNC_STATEMENT,
    JS_PARSE_FUNC_VAR,
    JS_PARSE_FUNC_EXPR,
    JS_PARSE_FUNC_ARROW,
    JS_PARSE_FUNC_GETTER,
    JS_PARSE_FUNC_SETTER,
    JS_PARSE_FUNC_METHOD,
    JS_PARSE_FUNC_CLASS_CONSTRUCTOR,
    JS_PARSE_FUNC_DERIVED_CLASS_CONSTRUCTOR,
} JSParseFunctionEnum;

typedef enum JSParseExportEnum {
    JS_PARSE_EXPORT_NONE,
    JS_PARSE_EXPORT_NAMED,
    JS_PARSE_EXPORT_DEFAULT,
} JSParseExportEnum;

typedef struct JSFunctionDef {
    JSContext *ctx;
    struct JSFunctionDef *parent;
    int parent_cpool_idx; /* index in the constant pool of the parent
                             or -1 if none */
    int parent_scope_level; /* scope level in parent at point of definition */
    struct list_head child_list; /* list of JSFunctionDef.link */
    struct list_head link;
    BOOL is_eval; /* TRUE if eval code */
    int eval_type; /* only valid if is_eval = TRUE */
    BOOL is_global_var; /* TRUE if variables are not defined locally:
                           eval global, eval module or non strict eval */
    BOOL is_func_expr; /* TRUE if function expression */
    BOOL has_home_object; /* TRUE if the home object is available */
    BOOL has_prototype; /* true if a prototype field is necessary */
    BOOL has_simple_parameter_list;
    BOOL has_parameter_expressions; /* if true, an argument scope is created */
    BOOL has_use_strict; /* to reject directive in special cases */
    BOOL has_eval_call; /* true if the function contains a call to eval() */
    BOOL has_arguments_binding; /* true if the 'arguments' binding is
                                   available in the function */
    BOOL has_this_binding; /* true if the 'this' and new.target binding are
                              available in the function */
    BOOL new_target_allowed; /* true if the 'new.target' does not
                                throw a syntax error */
    BOOL super_call_allowed; /* true if super() is allowed */
    BOOL super_allowed; /* true if super. or super[] is allowed */
    BOOL arguments_allowed; /* true if the 'arguments' identifier is allowed */
    BOOL is_derived_class_constructor;
    BOOL in_function_body;
    BOOL backtrace_barrier;
    JSFunctionKindEnum func_kind : 8;
    JSParseFunctionEnum func_type : 8;
    uint8_t js_mode; /* bitmap of JS_MODE_x */
    JSAtom func_name; /* JS_ATOM_NULL if no name */
    JSVarDef *vars;
    int var_size; /* allocated size for vars[] */
    int var_count;
    JSVarDef *args;
    int arg_size; /* allocated size for args[] */
    int arg_count; /* number of arguments */
    int defined_arg_count;
    int var_object_idx; /* -1 if none */
    int arg_var_object_idx; /* -1 if none (var object for the argument scope) */
    int arguments_var_idx; /* -1 if none */
    int arguments_arg_idx; /* argument variable definition in argument scope,
                              -1 if none */
    int func_var_idx; /* variable containing the current function (-1
                         if none, only used if is_func_expr is true) */
    int eval_ret_idx; /* variable containing the return value of the eval, -1 if none */
    int this_var_idx; /* variable containg the 'this' value, -1 if none */
    int new_target_var_idx; /* variable containg the 'new.target' value, -1 if none */
    int this_active_func_var_idx; /* variable containg the 'this.active_func' value, -1 if none */
    int home_object_var_idx;
    BOOL need_home_object;
    int scope_level;    /* index into fd->scopes if the current lexical scope */
    int scope_first;    /* index into vd->vars of first lexically scoped variable */
    int scope_size;     /* allocated size of fd->scopes array */
    int scope_count;    /* number of entries used in the fd->scopes array */
    JSVarScope *scopes;
    JSVarScope def_scope_array[4];
    int body_scope; /* scope of the body of the function or eval */
    int global_var_count;
    int global_var_size;
    JSGlobalVar *global_vars;
    DynBuf byte_code;
    int last_opcode_pos; /* -1 if no last opcode */
    int last_opcode_line_num;
    BOOL use_short_opcodes; /* true if short opcodes are used in byte_code */
    LabelSlot *label_slots;
    int label_size; /* allocated size for label_slots[] */
    int label_count;
    BlockEnv *top_break; /* break/continue label stack */
    /* constant pool (strings, functions, numbers) */
    JSValue *cpool;
    int cpool_count;
    int cpool_size;
    /* list of variables in the closure */
    int closure_var_count;
    int closure_var_size;
    JSClosureVar *closure_var;
    JumpSlot *jump_slots;
    int jump_size;
    int jump_count;
    LineNumberSlot *line_number_slots;
    int line_number_size;
    int line_number_count;
    int line_number_last;
    int line_number_last_pc;
    /* pc2line table */
    JSAtom filename;
    int line_num;
    DynBuf pc2line;
    char *source;  /* raw source, utf-8 encoded */
    int source_len;
    JSModuleDef *module; /* != NULL when parsing a module */
} JSFunctionDef;

typedef struct JSCFunctionDataRecord {
    JSCFunctionData *func;
    uint8_t length;
    uint8_t data_len;
    uint16_t magic;
    JSValue data[0];
} JSCFunctionDataRecord;

typedef struct {
    JSFunctionDef *fields_init_fd;
    int computed_fields_count;
    BOOL has_brand;
    int brand_push_pos;
} ClassFieldsDef;

typedef struct JSToken {
    int val;
    int line_num;   /* line number of token start */
    const uint8_t *ptr;
    union {
        struct {
            JSValue str;
            int sep;
        } str;
        struct {
            JSValue val;
#ifdef CONFIG_BIGNUM
            slimb_t exponent; /* may be != 0 only if val is a float */
#endif
        } num;
        struct {
            JSAtom atom;
            BOOL has_escape;
            BOOL is_reserved;
        } ident;
        struct {
            JSValue body;
            JSValue flags;
        } regexp;
    } u;
} JSToken;

typedef struct JSParseState {
    JSContext *ctx;
    int last_line_num;  /* line number of last token */
    int line_num;       /* line number of current offset */
    const char *filename;
    JSToken token;
    BOOL got_lf; /* true if got line feed before the current token */
    const uint8_t *last_ptr;
    const uint8_t *buf_ptr;
    const uint8_t *buf_end;
    /* current function code */
    JSFunctionDef *cur_func;
    BOOL is_module; /* parsing a module */
    BOOL allow_html_comments;
    BOOL ext_json; /* true if accepting JSON superset */
} JSParseState;

typedef struct JSOpCode {
#ifdef DUMP_BYTECODE
    const char *name;
#endif
    uint8_t size; /* in bytes */
    /* the opcodes remove n_pop items from the top of the stack, then
       pushes n_push items */
    uint8_t n_pop;
    uint8_t n_push;
    uint8_t fmt;
} JSOpCode;

typedef struct JSArrayIteratorData {
    JSValue obj;
    JSIteratorKindEnum kind;
    uint32_t idx;
} JSArrayIteratorData;

typedef struct JSParsePos {
    int last_line_num;
    int line_num;
    BOOL got_lf;
    const uint8_t *ptr;
} JSParsePos;

typedef struct JSClassShortDef {
    JSAtom class_name;
    JSClassFinalizer *finalizer;
    JSClassGCMark *gc_mark;
} JSClassShortDef;

typedef enum JSAsyncGeneratorStateEnum {
    JS_ASYNC_GENERATOR_STATE_SUSPENDED_START,
    JS_ASYNC_GENERATOR_STATE_SUSPENDED_YIELD,
    JS_ASYNC_GENERATOR_STATE_SUSPENDED_YIELD_STAR,
    JS_ASYNC_GENERATOR_STATE_EXECUTING,
    JS_ASYNC_GENERATOR_STATE_AWAITING_RETURN,
    JS_ASYNC_GENERATOR_STATE_COMPLETED,
} JSAsyncGeneratorStateEnum;

typedef struct JSAsyncGeneratorRequest {
    struct list_head link;
    /* completion */
    int completion_type; /* GEN_MAGIC_x */
    JSValue result;
    /* promise capability */
    JSValue promise;
    JSValue resolving_funcs[2];
} JSAsyncGeneratorRequest;

typedef struct JSAsyncGeneratorData {
    JSObject *generator; /* back pointer to the object (const) */
    JSAsyncGeneratorStateEnum state;
    JSAsyncFunctionState func_state;
    struct list_head queue; /* list of JSAsyncGeneratorRequest.link */
} JSAsyncGeneratorData;

typedef struct JSAsyncFromSyncIteratorData {
    JSValue sync_iter;
    JSValue next_method;
} JSAsyncFromSyncIteratorData;

typedef struct JSONStringifyContext {
    JSValueConst replacer_func;
    JSValue stack;
    JSValue property_list;
    JSValue gap;
    JSValue empty;
    StringBuffer *b;
} JSONStringifyContext;

typedef struct BCReaderState {
    JSContext *ctx;
    const uint8_t *buf_start, *ptr, *buf_end;
    uint32_t first_atom;
    uint32_t idx_to_atom_count;
    JSAtom *idx_to_atom;
    int error_state;
    BOOL allow_sab : 8;
    BOOL allow_bytecode : 8;
    BOOL is_rom_data : 8;
    BOOL allow_reference : 8;
    /* object references */
    JSObject **objects;
    int objects_count;
    int objects_size;
#ifdef DUMP_READ_OBJECT
    const uint8_t *ptr_last;
    int level;
#endif
} BCReaderState;

extern const JSClassShortDef js_std_class_def[];
extern const JSOpCode opcode_info[];

void JS_FreeAtomStruct(JSRuntime *, JSAtomStruct *);

JSString *js_alloc_string(JSContext *, int, int);
JSString *js_alloc_string_rt(JSRuntime *, int, int);
JSValue js_new_string16(JSContext *, const uint16_t *, int);
JSValue js_new_string8(JSContext *, const uint8_t *, int);
JSValue js_new_string_char(JSContext *, uint16_t);
int js_string_compare(JSContext *, const JSString *, const JSString *);
int js_string_memcmp(const JSString *, const JSString *, int);
void js_free_string(JSRuntime *, JSString *);

JSValue string_buffer_end(StringBuffer *);
int string_buffer_concat(StringBuffer *, const JSString *, uint32_t, uint32_t);
int string_buffer_concat_value(StringBuffer *, JSValueConst);
int string_buffer_concat_value_free(StringBuffer *, JSValue);
int string_buffer_fill(StringBuffer *, int, int);
int string_buffer_init(JSContext *, StringBuffer *, int);
int string_buffer_init2(JSContext *, StringBuffer *, int, int);
int string_buffer_putc(StringBuffer *, uint32_t);
int string_buffer_putc16(StringBuffer *, uint32_t);
int string_buffer_putc8(StringBuffer *, uint32_t);
int string_buffer_putc_slow(StringBuffer *, uint32_t);
int string_buffer_puts8(StringBuffer *, const char *);
int string_buffer_realloc(StringBuffer *, int, int);
int string_buffer_set_error(StringBuffer *);
int string_buffer_widen(StringBuffer *, int);
int string_buffer_write16(StringBuffer *, const uint16_t *, int);
int string_buffer_write8(StringBuffer *, const uint8_t *, int);
void string_buffer_free(StringBuffer *);

JSContext *JS_GetFunctionRealm(JSContext *, JSValueConst);
JSValue JS_ThrowTypeErrorInvalidClass(JSContext *, int);
JSValue JS_ThrowTypeErrorRevokedProxy(JSContext *);
JSValue js_create_from_ctor(JSContext *, JSValueConst, int);
int JS_ToBoolFree(JSContext *, JSValue);

BOOL js_same_value(JSContext *, JSValueConst, JSValueConst);
BOOL js_same_value_zero(JSContext *, JSValueConst, JSValueConst);
BOOL js_strict_eq(JSContext *, JSValue, JSValue);
BOOL js_strict_eq2(JSContext *, JSValue, JSValue, JSStrictEqModeEnum);
int js_strict_eq_slow(JSContext *, JSValue *, BOOL);

JSRegExp *js_get_regexp(JSContext *, JSValueConst, BOOL);
JSValue JS_RegExpExec(JSContext *, JSValueConst, JSValueConst);
JSValue js_compile_regexp(JSContext *, JSValueConst, JSValueConst);
JSValue js_regexp_Symbol_matchAll(JSContext *, JSValueConst, int, JSValueConst *);
JSValue js_regexp_compile(JSContext *, JSValueConst, int, JSValueConst *);
JSValue js_regexp_constructor(JSContext *, JSValueConst, int, JSValueConst *);
JSValue js_regexp_constructor_internal(JSContext *, JSValueConst, JSValue, JSValue);
JSValue js_regexp_exec(JSContext *, JSValueConst, int, JSValueConst *);
JSValue js_regexp_get_flag(JSContext *, JSValueConst, int);
JSValue js_regexp_get_flags(JSContext *, JSValueConst);
JSValue js_regexp_get_source(JSContext *, JSValueConst);
JSValue js_regexp_string_iterator_next(JSContext *, JSValueConst, int, JSValueConst *, BOOL *, int);
JSValue js_regexp_toString(JSContext *, JSValueConst, int, JSValueConst *);
int js_is_regexp(JSContext *, JSValueConst);
void js_regexp_finalizer(JSRuntime *, JSValue);
void js_regexp_string_iterator_finalizer(JSRuntime *, JSValue);
void js_regexp_string_iterator_mark(JSRuntime *, JSValueConst, JS_MarkFunc *);

JSValue JS_GetPropertyInt64(JSContext *, JSValueConst, int64_t);
JSValue JS_ThrowError(JSContext *, JSErrorEnum, const char *, va_list);
JSValue JS_ThrowError2(JSContext *, JSErrorEnum, const char *, va_list, BOOL);
JSValue JS_ThrowReferenceErrorNotDefined(JSContext *, JSAtom);
JSValue JS_ThrowReferenceErrorUninitialized(JSContext *, JSAtom);
JSValue JS_ThrowReferenceErrorUninitialized2(JSContext *, JSFunctionBytecode *, int, BOOL);
JSValue JS_ThrowStackOverflow(JSContext *);
JSValue JS_ThrowSyntaxErrorVarRedeclaration(JSContext *, JSAtom);
JSValue JS_ThrowTypeErrorDetachedArrayBuffer(JSContext *);
JSValue JS_ThrowTypeErrorNotASymbol(JSContext *);
JSValue JS_ThrowTypeErrorNotAnObject(JSContext *);
JSValue JS_ThrowTypeErrorPrivateNotFound(JSContext *, JSAtom);
JSValue __JS_ThrowSyntaxErrorAtom(JSContext *, JSAtom, const char *, ...) printfesque(3);
JSValue __JS_ThrowTypeErrorAtom(JSContext *, JSAtom, const char *, ...) printfesque(3);
JSValue throw_bf_exception(JSContext *, int);
int JS_ThrowTypeErrorOrFalse(JSContext *, int, const char *, ...) printfesque(3);
int JS_ThrowTypeErrorReadOnly(JSContext *, int, JSAtom);
int js_parse_error(JSParseState *, const char *, ...) printfesque(2);
int js_throw_URIError(JSContext *, const char *, ...) printfesque(2);
void build_backtrace(JSContext *, JSValueConst, const char *, int, int);
int js_compare_bigfloat(JSContext *, OPCodeEnum, JSValue, JSValue);

/* %s is replaced by 'atom'. The macro is used so that gcc can check the format string. */
#define JS_ThrowTypeErrorAtom(ctx, fmt, atom) __JS_ThrowTypeErrorAtom(ctx, atom, fmt, "")
#define JS_ThrowSyntaxErrorAtom(ctx, fmt, atom) __JS_ThrowSyntaxErrorAtom(ctx, atom, fmt, "")

JSValue JS_ToLocaleStringFree(JSContext *, JSValue);
JSValue JS_ToStringFree(JSContext *, JSValue);
JSValue JS_ToStringInternal(JSContext *, JSValueConst, BOOL);
JSValue js_atof(JSContext *, const char *, const char **, int, int);
JSValue js_atof2(JSContext *, const char *, const char **, int, int, slimb_t *);
JSValue js_bigdecimal_to_string(JSContext *, JSValueConst);
JSValue js_bigdecimal_to_string1(JSContext *, JSValueConst, limb_t, int);
JSValue js_bigfloat_to_string(JSContext *, JSValueConst);
JSValue js_bigint_to_string(JSContext *, JSValueConst);
JSValue js_bigint_to_string1(JSContext *, JSValueConst, int);
JSValue js_dtoa(JSContext *, double, int, int, int);
JSValue js_ftoa(JSContext *, JSValueConst, int, limb_t, bf_flags_t);
bfdec_t *JS_ToBigDecimal(JSContext *, JSValueConst);

bf_t *JS_ToBigInt(JSContext *, bf_t *, JSValueConst);
void JS_FreeBigInt(JSContext *, bf_t *, bf_t *);
JSValue JS_ToNumeric(JSContext *, JSValueConst);
bf_t *JS_ToBigFloat(JSContext *, bf_t *, JSValueConst);
JSValue JS_NewBigInt64_1(JSContext *, int64_t);

JSValue js_sub_string(JSContext *, JSString *, int, int);
int string_cmp(JSString *, JSString *, int, int, int);
int string_getc(const JSString *, int *);
int string_indexof(JSString *, JSString *, int);
int string_indexof_char(JSString *, int, int);
int64_t string_advance_index(JSString *, int64_t, BOOL);

BOOL js_class_has_bytecode(JSClassID);
JSFunctionBytecode *JS_GetFunctionBytecode(JSValueConst);
int check_function(JSContext *, JSValueConst);
int find_line_num(JSContext *, JSFunctionBytecode *, uint32_t);
void js_bytecode_function_mark(JSRuntime *, JSValueConst, JS_MarkFunc *);

JSAtom js_get_atom_index(JSRuntime *, JSAtomStruct *);
const char *JS_AtomGetStrRT(JSRuntime *, char *, int, JSAtom);
const char *JS_AtomGetStr(JSContext *, char *, int, JSAtom);
BOOL JS_AtomIsArrayIndex(JSContext *, uint32_t *, JSAtom);
BOOL is_num_string(uint32_t *, const JSString *);
JSAtom js_symbol_to_atom(JSContext *, JSValue);
JSValue JS_NewSymbol(JSContext *, JSString *, int);
int JS_DefineObjectNameComputed(JSContext *, JSValueConst, JSValueConst, int);
int JS_SetPrototypeInternal(JSContext *, JSValueConst, JSValueConst, BOOL);
void js_method_set_home_object(JSContext *, JSValueConst, JSValueConst);
JSValue js_get_function_name(JSContext *, JSAtom);
JSValue JS_ConcatString3(JSContext *, const char *, JSValue, const char *);

BOOL JS_IsCFunction(JSContext *, JSValueConst, JSCFunction *, int);
JSAtom JS_NewAtomInt64(JSContext *, int64_t);
JSAtom JS_NewAtomStr(JSContext *, JSString *);
JSAtomKindEnum JS_AtomGetKind(JSContext *, JSAtom);
JSValue JS_CallFree(JSContext *, JSValue, JSValueConst, int, JSValueConst *);
JSValue JS_ConcatString(JSContext *, JSValue, JSValue);
JSValue JS_EvalObject(JSContext *, JSValueConst, JSValueConst, int, int);
JSValue JS_GetIterator(JSContext *, JSValueConst, BOOL);
JSValue JS_GetPropertyValue(JSContext *, JSValueConst, JSValue);
JSValue JS_GetPrototypeFree(JSContext *, JSValue);
JSValue JS_InvokeFree(JSContext *, JSValue, JSAtom, int, JSValueConst *);
JSValue JS_IteratorNext(JSContext *, JSValueConst, JSValueConst, int, JSValueConst *, BOOL *);
JSValue JS_IteratorNext2(JSContext *, JSValueConst, JSValueConst, int, JSValueConst *, int *);
JSValue JS_NewBigFloat(JSContext *);
JSValue JS_SpeciesConstructor(JSContext *, JSValueConst, JSValueConst);
JSValue JS_ToNumberFree(JSContext *, JSValue);
JSValue JS_ToNumericFree(JSContext *, JSValue);
JSValue JS_ToObjectFree(JSContext *, JSValue);
JSValue JS_ToPrimitive(JSContext *, JSValueConst, int);
JSValue JS_ToPrimitiveFree(JSContext *, JSValue, int);
JSValue JS_ToQuotedString(JSContext *, JSValueConst);
JSValue JS_ToStringCheckObject(JSContext *, JSValueConst);
int JS_CreateDataPropertyUint32(JSContext *, JSValueConst, int64_t, JSValue, int);
int JS_DefinePropertyValueInt64(JSContext *, JSValueConst, int64_t, JSValue, int);
int JS_DefinePropertyValueValue(JSContext *, JSValueConst, JSValue, JSValue, int);
int JS_ReadFunctionBytecode(BCReaderState *, JSFunctionBytecode *, int, uint32_t);
int JS_SetPropertyGeneric(JSContext *, JSValueConst, JSAtom, JSValue, JSValueConst, int);
int JS_SetPropertyValue(JSContext *, JSValueConst, JSValue, JSValue, int);
int JS_ToInt32Free(JSContext *, int32_t *, JSValue);
int JS_ToInt32Sat(JSContext *, int *, JSValueConst);
int JS_ToInt64Clamp(JSContext *, int64_t *, JSValueConst, int64_t, int64_t, int64_t);
int JS_ToInt64SatFree(JSContext *, int64_t *, JSValue);
void JS_SetConstructor2(JSContext *, JSValueConst, JSValueConst, int, int);

JSValue JS_NewBigInt(JSContext *);
JSValue JS_CompactBigInt(JSContext *, JSValue);
JSValue JS_StringToBigInt(JSContext *, JSValue);
int JS_ToBigInt64Free(JSContext *, int64_t *, JSValue);

BOOL check_define_prop_flags(int, int);
JSAtom get_private_setter_name(JSContext *, JSAtom);
JSAtom js_atom_concat_str(JSContext *, JSAtom, const char *);
JSExportEntry *add_export_entry(JSParseState *, JSModuleDef *, JSAtom, JSAtom, JSExportTypeEnum);
JSFunctionDef *js_new_function_def(JSContext *, JSFunctionDef *, BOOL, BOOL, const char *, int);
JSGlobalVar *add_global_var(JSContext *, JSFunctionDef *, JSAtom);
JSGlobalVar *find_global_var(JSFunctionDef *, JSAtom);
JSModuleDef *js_new_module_def(JSContext *, JSAtom);
JSValue JS_GetIterator2(JSContext *, JSValueConst, JSValueConst);
JSValue JS_GetOwnPropertyNames2(JSContext *, JSValueConst, int, int);
JSValue JS_NewBigDecimal(JSContext *);
JSValue JS_ToObject(JSContext *, JSValueConst);
JSValue js___date_clock(JSContext *, JSValueConst, int, JSValueConst *);
JSValue js_generator_function_call(JSContext *, JSValueConst, JSValueConst, int, JSValueConst *, int);
JSValue js_global_decodeURI(JSContext *, JSValueConst, int, JSValueConst *, int);
JSValue js_global_encodeURI(JSContext *, JSValueConst, int, JSValueConst *, int);
JSValue js_global_escape(JSContext *, JSValueConst, int, JSValueConst *);
JSValue js_global_unescape(JSContext *, JSValueConst, int, JSValueConst *);
JSValue js_object_defineProperty(JSContext *, JSValueConst, int, JSValueConst *, int);
JSValue js_object_getOwnPropertyDescriptor(JSContext *, JSValueConst, int, JSValueConst *, int);
JSValue js_object_getPrototypeOf(JSContext *, JSValueConst, int, JSValueConst *, int);
JSValue js_object_isExtensible(JSContext *, JSValueConst, int, JSValueConst *, int);
JSValue js_object_keys(JSContext *, JSValueConst, int, JSValueConst *, int);
JSValue js_object_preventExtensions(JSContext *, JSValueConst, int, JSValueConst *, int);
JSValue js_object_toString(JSContext *, JSValueConst, int, JSValueConst *);
JSValue js_promise_resolve(JSContext *, JSValueConst, int, JSValueConst *, int);
JSValue js_string___GetSubstitution(JSContext *, JSValueConst, int, JSValueConst *);
JSValue js_string_constructor(JSContext *, JSValueConst, int, JSValueConst *);
JSValueConst JS_NewGlobalCConstructor(JSContext *, const char *, JSCFunction *, int, JSValueConst);
int JS_CopyDataProperties(JSContext *, JSValueConst, JSValueConst, JSValueConst, BOOL) dontdiscard;
int JS_GetOwnPropertyInternal(JSContext *, JSPropertyDescriptor *, JSObject *, JSAtom);
int JS_GetOwnPropertyNamesInternal(JSContext *, JSPropertyEnum **, uint32_t *, JSObject *, int) dontdiscard;
int JS_SetObjectData(JSContext *, JSValueConst, JSValue);
int JS_ToArrayLengthFree(JSContext *, uint32_t *, JSValue, BOOL) dontdiscard;
int JS_ToInt32Clamp(JSContext *, int *, JSValueConst, int, int, int);
int JS_ToInt64Sat(JSContext *, int64_t *, JSValueConst);
int JS_ToLengthFree(JSContext *, int64_t *, JSValue) dontdiscard;
int JS_ToUint8ClampFree(JSContext *, int32_t *, JSValue);
int __JS_ToFloat64Free(JSContext *, double *, JSValue) dontdiscard;
int __js_poll_interrupts(JSContext *) dontdiscard;
int add_closure_var(JSContext *, JSFunctionDef *, BOOL, BOOL, int, JSAtom, BOOL, BOOL, JSVarKindEnum);
int add_scope_var(JSContext *, JSFunctionDef *, JSAtom, JSVarKindEnum);
int add_var(JSContext *, JSFunctionDef *, JSAtom);
int bc_get_buf(BCReaderState *, uint8_t *, uint32_t);
int bc_idx_to_atom(BCReaderState *, JSAtom *, uint32_t);
int bc_read_error_end(BCReaderState *);
int check_exception_free(JSContext *, JSValue);
int cpool_add(JSParseState *, JSValue);
int define_var(JSParseState *, JSFunctionDef *, JSAtom, JSVarDefEnum);
int delete_property(JSContext *, JSObject *, JSAtom);
int emit_label(JSParseState *, int);
int find_lexical_decl(JSContext *, JSFunctionDef *, JSAtom, int, BOOL);
int find_private_class_field(JSContext *, JSFunctionDef *, JSAtom, int);
int find_var(JSContext *, JSFunctionDef *, JSAtom);
int get_ovop_from_opcode(OPCodeEnum);
int ident_realloc(JSContext *, char **, size_t *, char *) dontdiscard;
int init_class_range(JSRuntime *, JSClassShortDef const *, int, int);
int js_add_slow(JSContext *, JSValue *) dontdiscard;
int js_binary_arith_slow(JSContext *, JSValue *, OPCodeEnum) dontdiscard;
int js_binary_logic_slow(JSContext *, JSValue *, OPCodeEnum);
int js_call_binary_op_fallback(JSContext *, JSValue *, JSValueConst, JSValueConst, OPCodeEnum, BOOL, int);
int js_eq_slow(JSContext *, JSValue *, BOOL) dontdiscard;
int js_for_of_start(JSContext *, JSValue *, BOOL) dontdiscard;
int js_get_length32(JSContext *, uint32_t *, JSValueConst) dontdiscard;
int js_get_length64(JSContext *, int64_t *, JSValueConst) dontdiscard;
int js_get_radix(JSContext *, JSValueConst);
int js_not_slow(JSContext *, JSValue *);
int js_obj_to_desc(JSContext *, JSPropertyDescriptor *, JSValueConst);
int js_operator_in(JSContext *, JSValue *) dontdiscard;
int js_parse_export(JSParseState *) dontdiscard;
int js_parse_string(JSParseState *, int, BOOL, const uint8_t *, JSToken *, const uint8_t **);
int js_parse_template_part(JSParseState *, const uint8_t *) dontdiscard;
int js_post_inc_slow(JSContext *, JSValue *, OPCodeEnum) dontdiscard;
int js_relational_slow(JSContext *, JSValue *, OPCodeEnum);
int js_shr_slow(JSContext *, JSValue *);
int js_unary_arith_slow(JSContext *, JSValue *, OPCodeEnum) dontdiscard;
int js_update_property_flags(JSContext *, JSObject *, JSShapeProperty **, int);
int new_label(JSParseState *);
int perform_promise_then(JSContext *, JSValueConst, JSValueConst *, JSValueConst *);
int push_scope(JSParseState *);
int resize_properties(JSContext *, JSShape **, JSObject *, uint32_t);
int skip_spaces(const char *);
int to_digit(int);
int update_label(JSFunctionDef *, int, int);
uint32_t hash_string(const JSString *, uint32_t);
uint32_t js_string_obj_get_length(JSContext *, JSValueConst);
void close_scopes(JSParseState *, int, int);
void emit_atom(JSParseState *, JSAtom);
void emit_op(JSParseState *, uint8_t);
void js_async_generator_resume_next(JSContext *, JSAsyncGeneratorData *);
void js_bytecode_function_finalizer(JSRuntime *, JSValue);
void js_free_desc(JSContext *, JSPropertyDescriptor *);
void js_free_function_def(JSContext *, JSFunctionDef *);
void js_free_module_def(JSContext *, JSModuleDef *);
void js_free_prop_enum(JSContext *, JSPropertyEnum *, uint32_t);
void js_generator_finalizer(JSRuntime *, JSValue);
void js_generator_mark(JSRuntime *, JSValueConst, JS_MarkFunc *);
void js_parse_init(JSContext *, JSParseState *, const char *, size_t, const char *);

int js_proxy_setPrototypeOf(JSContext *, JSValueConst, JSValueConst, BOOL);
JSValue js_proxy_getPrototypeOf(JSContext *, JSValueConst);
int js_proxy_isExtensible(JSContext *, JSValueConst);
int js_proxy_isArray(JSContext *, JSValueConst);
int js_proxy_preventExtensions(JSContext *, JSValueConst);

void js_map_iterator_finalizer(JSRuntime *, JSValue);
void js_map_finalizer(JSRuntime *, JSValue);
void js_map_mark(JSRuntime *, JSValueConst, JS_MarkFunc *);
void js_map_iterator_mark(JSRuntime *, JSValueConst, JS_MarkFunc *);
void reset_weak_ref(JSRuntime *, JSObject *);

void free_property(JSRuntime *, JSProperty *, int);
void js_autoinit_mark(JSRuntime *, JSProperty *, JS_MarkFunc *);
void free_zero_refcount(JSRuntime *);

void free_token(JSParseState *, JSToken *);
int next_token(JSParseState *) dontdiscard;
int simple_next_token(const uint8_t **, BOOL);
int js_parse_program(JSParseState *) dontdiscard;

JSValue JS_IteratorGetCompleteValue(JSContext *, JSValueConst, BOOL *);
JSValue js_create_iterator_result(JSContext *, JSValue, BOOL);
JSValue js_iterator_proto_iterator(JSContext *, JSValueConst, int, JSValueConst *);
int JS_IteratorClose(JSContext *, JSValueConst, BOOL);
void JS_AddIteratorProto(JSContext *);

JSValue *build_arg_list(JSContext *, uint32_t *, JSValueConst);
void free_arg_list(JSContext *, JSValue *, uint32_t);

JSValue js_get_module_ns(JSContext *, JSModuleDef *);

static inline BOOL JS_IsEmptyString(JSValueConst v) { return JS_VALUE_GET_TAG(v) == JS_TAG_STRING && JS_VALUE_GET_STRING(v)->len == 0; }
static inline BOOL __JS_AtomIsTaggedInt(JSAtom v) { return (v & JS_ATOM_TAG_INT) != 0; }
static inline BOOL atom_is_free(const JSAtomStruct *p) { return (uintptr_t)p & 1; }
static inline JSAtom __JS_AtomFromUInt32(uint32_t v) { return v | JS_ATOM_TAG_INT; }
static inline JSAtomStruct *atom_set_free(uint32_t v) { return (JSAtomStruct *)(((uintptr_t)v << 1) | 1); }
static inline JSValueConst JS_GetActiveFunction(JSContext *ctx) { return ctx->rt->current_stack_frame->cur_func; } /* only valid inside C functions */
static inline int string_get(const JSString *p, int idx) { return p->is_wide_char ? p->u.str16[idx] : p->u.str8[idx]; }
static inline uint32_t __JS_AtomToUInt32(JSAtom atom) { return atom & ~JS_ATOM_TAG_INT; }
static inline uint32_t atom_get_free(const JSAtomStruct *p) { return (uintptr_t)p >> 1; }

JSValue async_func_resume(JSContext *, JSAsyncFunctionState *);
JSValue js_call_bound_function(JSContext *, JSValueConst, JSValueConst, int, JSValueConst *, int);
JSValue js_call_c_function(JSContext *, JSValueConst, JSValueConst, int, JSValueConst *, int);
JSValue js_closure(JSContext *, JSValue, JSVarRef **, JSStackFrame *);
JSValue js_closure2(JSContext *, JSValue, JSFunctionBytecode *, JSVarRef **, JSStackFrame *);
JSValue js_function_apply(JSContext *, JSValueConst, int, JSValueConst *, int);
JSValue js_function_constructor(JSContext *, JSValueConst, int, JSValueConst *, int);
JSVarRef *get_var_ref(JSContext *, JSStackFrame *, int, BOOL);
int async_func_init(JSContext *, JSAsyncFunctionState *, JSValueConst, JSValueConst, int, JSValueConst *) dontdiscard;
void async_func_free(JSRuntime *, JSAsyncFunctionState *);
void async_func_mark(JSRuntime *, JSAsyncFunctionState *, JS_MarkFunc *);
void js_async_generator_free(JSRuntime *, JSAsyncGeneratorData *);

JSProperty *add_property(JSContext *, JSObject *, JSAtom, int);
int JS_DefineAutoInitProperty(JSContext *, JSValueConst, JSAtom, JSAutoInitIDEnum, void *, int);
void free_var_ref(JSRuntime *, JSVarRef *);
void js_function_set_properties(JSContext *, JSValueConst, JSAtom, int);
int JS_DefineObjectName(JSContext *, JSValueConst, JSAtom, int);
JSValue js_get_this(JSContext *, JSValueConst);

BOOL js_get_fast_array(JSContext *, JSValueConst, JSValue **, uint32_t *);
JSValue js_array_constructor(JSContext *, JSValueConst, int, JSValueConst *);
JSValue js_array_every(JSContext *, JSValueConst, int, JSValueConst *, int);
JSValue js_array_from(JSContext *, JSValueConst, int, JSValueConst *);
JSValue js_array_includes(JSContext *, JSValueConst, int, JSValueConst *);
JSValue js_array_iterator_next(JSContext *, JSValueConst, int, JSValueConst *, BOOL *, int);
JSValue js_array_pop(JSContext *, JSValueConst, int, JSValueConst *, int);
JSValue js_array_push(JSContext *, JSValueConst, int, JSValueConst *, int);
JSValue js_array_reduce(JSContext *, JSValueConst, int, JSValueConst *, int);
JSValue js_create_array(JSContext *, int, JSValueConst *);
JSValue js_create_array_iterator(JSContext *, JSValueConst, int, JSValueConst *, int);
int add_fast_array_element(JSContext *, JSObject *, JSValue, int);
int js_realloc_array(JSContext *, void **, int, int *, int);
int set_array_length(JSContext *, JSObject *, JSValue, int);
void JS_AddArrayIteratorProto(JSContext *);
void JS_AddIntrinsicArray(JSContext *);
void JS_AddIntrinsicGenerator(JSContext *);
void JS_AddIntrinsicMath(JSContext *);
void JS_AddIntrinsicObject(JSContext *);
void JS_AddIntrinsicReflect(JSContext *);
void JS_AddIntrinsicString(JSContext *);
void js_array_finalizer(JSRuntime *, JSValue);
void js_array_iterator_finalizer(JSRuntime *, JSValue);
void js_array_iterator_mark(JSRuntime *, JSValueConst, JS_MarkFunc *);
void js_array_mark(JSRuntime *, JSValueConst, JS_MarkFunc *);

void js_object_data_finalizer(JSRuntime *, JSValue);
void js_object_data_mark(JSRuntime *, JSValueConst, JS_MarkFunc *);
JSValue js_object_seal(JSContext *, JSValueConst, int, JSValueConst *, int);

void free_function_bytecode(JSRuntime *, JSFunctionBytecode *);
void free_bytecode_atoms(JSRuntime *, const uint8_t *, int, BOOL);

JSShape *find_hashed_shape_proto(JSRuntime *, JSObject *);
JSShape *js_dup_shape(JSShape *);
JSShape *js_new_shape(JSContext *, JSObject *);
JSValue JS_NewObjectFromShape(JSContext *, JSShape *, JSClassID);
int add_shape_property(JSContext *, JSShape **, JSObject *, JSAtom, int);
void js_shape_hash_unlink(JSRuntime *, JSShape *);
void js_shape_hash_link(JSRuntime *, JSShape *);
JSShape *find_hashed_shape_prop(JSRuntime *, JSShape *, JSAtom, int);
JSShape *js_clone_shape(JSContext *, JSShape *);
JSShape *js_new_shape2(JSContext *, JSObject *, int, int);
int init_shape_hash(JSRuntime *);
void js_free_shape(JSRuntime *, JSShape *);
double js_pow(double, double);

void js_trigger_gc(JSRuntime *, size_t);
void js_async_function_free0(JSRuntime *, JSAsyncFunctionData *);

BOOL typed_array_is_detached(JSContext *, JSObject *);
JSValue js_typed_array___speciesCreate(JSContext *, JSValueConst, int, JSValueConst *);
JSValue js_typed_array_constructor(JSContext *, JSValueConst, int, JSValueConst *, int);
JSValue js_typed_array_get_buffer(JSContext *, JSValueConst, int);
JSValue js_typed_array_get_byteLength(JSContext *, JSValueConst, int);
JSValue js_typed_array_get_byteOffset(JSContext *, JSValueConst, int);
int js_typed_array_get_length_internal(JSContext *, JSValueConst);
uint32_t typed_array_get_length(JSContext *, JSObject *);
void js_typed_array_finalizer(JSRuntime *, JSValue);
void js_typed_array_mark(JSRuntime *, JSValueConst, JS_MarkFunc *);

JSArrayBuffer *js_get_array_buffer(JSContext *, JSValueConst);
JSValue js_array_buffer_constructor(JSContext *, JSValueConst, int, JSValueConst *);
JSValue js_array_buffer_constructor1(JSContext *, JSValueConst, uint64_t);
JSValue js_array_buffer_constructor2(JSContext *, JSValueConst, uint64_t, JSClassID);
JSValue js_array_buffer_constructor3(JSContext *, JSValueConst, uint64_t, JSClassID, uint8_t *, JSFreeArrayBufferDataFunc *, void *, BOOL);
void js_array_buffer_finalizer(JSRuntime *, JSValue);

JSValue JS_NewCFunction3(JSContext *, JSCFunction *, const char *, int, JSCFunctionEnum, int, JSValueConst);
JSValueConst JS_NewGlobalCConstructorOnly(JSContext *, const char *, JSCFunction *, int, JSValueConst);
void JS_NewGlobalCConstructor2(JSContext *, JSValue, const char *, JSValueConst);

JSModuleDef *js_find_loaded_module(JSContext *, JSAtom);

void dbuf_put_leb128(DynBuf *, uint32_t);
void dbuf_put_sleb128(DynBuf *, int32_t);

static inline BOOL is_strict_mode(JSContext *ctx)
{
    JSStackFrame *sf = ctx->rt->current_stack_frame;
    return (sf && (sf->js_mode & JS_MODE_STRICT));
}

#ifdef CONFIG_BIGNUM
static inline BOOL is_math_mode(JSContext *ctx)
{
    JSStackFrame *sf = ctx->rt->current_stack_frame;
    return (sf && (sf->js_mode & JS_MODE_MATH));
}
#endif

static inline BOOL __JS_AtomIsConst(JSAtom v)
{
#if defined(DUMP_LEAKS) && DUMP_LEAKS > 1
        return (int32_t)v <= 0;
#else
        return (int32_t)v < JS_ATOM_END;
#endif
}

static inline int JS_ToFloat64Free(JSContext *ctx, double *pres, JSValue val) {
    uint32_t tag;
    tag = JS_VALUE_GET_TAG(val);
    if (tag <= JS_TAG_NULL) {
        *pres = JS_VALUE_GET_INT(val);
        return 0;
    } else if (JS_TAG_IS_FLOAT64(tag)) {
        *pres = JS_VALUE_GET_FLOAT64(val);
        return 0;
    } else {
        return __JS_ToFloat64Free(ctx, pres, val);
    }
}

#if !defined(CONFIG_STACK_CHECK)
static inline uintptr_t js_get_stack_pointer(void) { return 0; }
static inline BOOL js_check_stack_overflow(JSRuntime *rt, size_t alloca_size) { return FALSE; }
#else
static inline uintptr_t js_get_stack_pointer(void) { return (uintptr_t)__builtin_frame_address(0); }
static inline BOOL js_check_stack_overflow(JSRuntime *rt, size_t alloca_size) {
    uintptr_t sp;
    sp = js_get_stack_pointer() - alloca_size;
    return UNLIKELY(sp < rt->stack_limit);
}
#endif

static inline __exception int js_poll_interrupts(JSContext *ctx) {
    if (UNLIKELY(--ctx->interrupt_counter <= 0)) {
        return __js_poll_interrupts(ctx);
    } else {
        return 0;
    }
}

static inline uint32_t *prop_hash_end(JSShape *sh) {
    return (uint32_t *)sh;
}

static inline JSShapeProperty *get_shape_prop(JSShape *sh) {
    return sh->prop;
}

forceinline JSShapeProperty *find_own_property1(JSObject *p, JSAtom atom) {
    JSShape *sh;
    JSShapeProperty *pr, *prop;
    intptr_t h;
    sh = p->shape;
    h = (uintptr_t)atom & sh->prop_hash_mask;
    h = prop_hash_end(sh)[-h - 1];
    prop = get_shape_prop(sh);
    while (h) {
        pr = &prop[h - 1];
        if (LIKELY(pr->atom == atom)) {
            return pr;
        }
        h = pr->hash_next;
    }
    return NULL;
}

forceinline JSShapeProperty *find_own_property(JSProperty **ppr,
                                               JSObject *p,
                                               JSAtom atom) {
    JSShape *sh;
    JSShapeProperty *pr, *prop;
    intptr_t h;
    sh = p->shape;
    h = (uintptr_t)atom & sh->prop_hash_mask;
    h = prop_hash_end(sh)[-h - 1];
    prop = get_shape_prop(sh);
    while (h) {
        pr = &prop[h - 1];
        if (LIKELY(pr->atom == atom)) {
            *ppr = &p->prop[h - 1];
            /* the compiler should be able to assume that pr != NULL here */
            return pr;
        }
        h = pr->hash_next;
    }
    *ppr = NULL;
    return NULL;
}

/* set the new value and free the old value after (freeing the value
   can reallocate the object data) */
static inline void set_value(JSContext *ctx, JSValue *pval, JSValue new_val) {
    JSValue old_val;
    old_val = *pval;
    *pval = new_val;
    JS_FreeValue(ctx, old_val);
}

/* indicate that the object may be part of a function prototype cycle */
static inline void set_cycle_flag(JSContext *ctx, JSValueConst obj) {}

static inline void js_dbuf_init(JSContext *ctx, DynBuf *s) {
    dbuf_init2(s, ctx->rt, (DynBufReallocFunc *)js_realloc_rt);
}

static inline void emit_u8(JSParseState *s, uint8_t val) {
    dbuf_putc(&s->cur_func->byte_code, val);
}

static inline void emit_u16(JSParseState *s, uint16_t val) {
    dbuf_put_u16(&s->cur_func->byte_code, val);
}

static inline void emit_u32(JSParseState *s, uint32_t val) {
    dbuf_put_u32(&s->cur_func->byte_code, val);
}

static inline int get_prev_opcode(JSFunctionDef *fd) {
    if (fd->last_opcode_pos < 0)
        return OP_invalid;
    else
        return fd->byte_code.buf[fd->last_opcode_pos];
}

/* resize the array and update its size if req_size > *psize */
static inline int js_resize_array(JSContext *ctx, void **parray, int elem_size,
                                  int *psize, int req_size) {
    if (UNLIKELY(req_size > *psize))
        return js_realloc_array(ctx, parray, elem_size, psize, req_size);
    else
        return 0;
}

static inline bf_t *JS_GetBigFloat(JSValueConst val) {
    JSBigFloat *p = JS_VALUE_GET_PTR(val);
    return &p->num;
}

static inline bfdec_t *JS_GetBigDecimal(JSValueConst val) {
    JSBigDecimal *p = JS_VALUE_GET_PTR(val);
    return &p->num;
}

static inline void add_gc_object(JSRuntime *rt, JSGCObjectHeader *h, JSGCObjectTypeEnum type) {
    h->mark = 0;
    h->gc_obj_type = type;
    list_add_tail(&h->link, &rt->gc_obj_list);
}

static inline void remove_gc_object(JSGCObjectHeader *h) {
    list_del(&h->link);
}

static inline size_t get_shape_size(size_t hash_size, size_t prop_size) {
    return hash_size * sizeof(uint32_t) + sizeof(JSShape) +
           prop_size * sizeof(JSShapeProperty);
}

static inline JSShape *get_shape_from_alloc(void *sh_alloc, size_t hash_size) {
    return (JSShape *)(void *)((uint32_t *)sh_alloc + hash_size);
}

static inline void *get_alloc_from_shape(JSShape *sh) {
    return prop_hash_end(sh) - ((intptr_t)sh->prop_hash_mask + 1);
}

static inline JSObject *get_proto_obj(JSValueConst proto_val) {
    if (JS_VALUE_GET_TAG(proto_val) != JS_TAG_OBJECT)
        return NULL;
    else
        return JS_VALUE_GET_OBJ(proto_val);
}

static inline void js_free_shape_null(JSRuntime *rt, JSShape *sh) {
    if (sh) js_free_shape(rt, sh);
}

static inline bf_t *JS_GetBigInt(JSValueConst val) {
    JSBigFloat *p = JS_VALUE_GET_PTR(val);
    return &p->num;
}

static inline void js_async_function_free(JSRuntime *rt, JSAsyncFunctionData *s) {
    if (--s->header.ref_count == 0) {
        js_async_function_free0(rt, s);
    }
}

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_THIRD_PARTY_QUICKJS_INTERNAL_H_ */
