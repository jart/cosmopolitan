#ifndef __cplusplus
#define COSMOPOLITAN_C_START_
#define COSMOPOLITAN_C_END_
#define COSMOPOLITAN_CXX_START_
#define COSMOPOLITAN_CXX_END_
#define COSMOPOLITAN_CXX_USING_
#endif

#if !defined(__GNUC__) && __cplusplus + 0 >= 201103L
#define typeof(x) decltype(x)
#elif !defined(__GNUC__) && __STDC_VERSION__ + 0 < 201112
#define typeof(x) __typeof(x)
#endif

#ifdef __chibicc__
#define __extension__
#endif

#ifdef __llvm__
#define __gnu_printf__ __printf__
#define __gnu_scanf__  __scanf__
#endif

#if __cplusplus + 0 >= 201103L
#define NULL nullptr
#elif !defined(__cplusplus)
#define NULL ((void *)0)
#else
#define NULL 0
#endif

#ifndef __cplusplus
#if defined(__GNUC__) && !defined(__llvm__)
#pragma GCC push_options
#pragma GCC diagnostic ignored "-Wc++-compat"
#endif
typedef __WCHAR_TYPE__ wchar_t;
typedef __CHAR16_TYPE__ char16_t;
typedef __CHAR32_TYPE__ char32_t;
#if defined(__GNUC__) && !defined(__llvm__)
#pragma GCC pop_options
#endif
#endif /* __cplusplus */

#ifndef __COSMOCC__
#include "libc/stdbool.h"
#endif

typedef int errno_t;
typedef __SIZE_TYPE__ size_t;
typedef __PTRDIFF_TYPE__ ssize_t;
typedef __INTPTR_TYPE__ intptr_t;
typedef __UINTPTR_TYPE__ uintptr_t;
typedef __PTRDIFF_TYPE__ ptrdiff_t;
typedef __WINT_TYPE__ wint_t; /* uint32_t on linux but int32_t on xnu */
typedef __INT8_TYPE__ int8_t;
typedef __UINT8_TYPE__ uint8_t;
typedef __INT16_TYPE__ int16_t;
typedef __UINT16_TYPE__ uint16_t;
typedef __INT32_TYPE__ bool32;
typedef __INT32_TYPE__ int32_t;
typedef __UINT32_TYPE__ uint32_t;
typedef __INT64_TYPE__ int64_t;
typedef __UINT64_TYPE__ uint64_t;
typedef __INTMAX_TYPE__ intmax_t;
typedef __UINTMAX_TYPE__ uintmax_t;

/* TODO(jart): re-import compiler-rt once they have it */
#if defined(__x86_64__) && defined(__FLT128_MAX_10_EXP__)
#undef __FLT128_MAX_10_EXP__
#undef __FLT128_DENORM_MIN__
#undef __FLT128_MIN_EXP__
#undef __FLT128_MIN_10_EXP__
#undef __FLT128_MANT_DIG__
#undef __FLT128_HAS_INFINITY__
#undef __FLT128_EPSILON__
#undef __FLT128_MAX_EXP__
#undef __FLT128_HAS_DENORM__
#undef __FLT128_DIG__
#undef __FLT128_MIN__
#undef __FLT128_MAX__
#undef __FLT128_NORM_MAX__
#undef __FLT128_HAS_QUIET_NAN__
#undef __FLT128_IS_IEC_60559__
#undef __FLT128_DECIMAL_DIG__
#endif

#define __DEFINED_max_align_t
typedef long double max_align_t;

#ifdef _COSMO_SOURCE
#if (__GNUC__ + 0) * 100 + (__GNUC_MINOR__ + 0) >= 406 || defined(__llvm__)
typedef signed __int128 int128_t;
typedef unsigned __int128 uint128_t;
#endif
#endif /* _COSMO_SOURCE */

#ifndef __AXDX_T
#define __AXDX_T
typedef struct {
  intptr_t ax, dx;
} axdx_t;
#endif

#ifndef __chibicc__
#define va_list            __builtin_va_list
#define va_arg(ap, type)   __builtin_va_arg(ap, type)
#define va_copy(dest, src) __builtin_va_copy(dest, src)
#define va_end(ap)         __builtin_va_end(ap)
#define va_start(ap, last) __builtin_va_start(ap, last)
#else
#include "libc/integral/lp64arg.inc"
#endif

#define libcesque   dontthrow dontcallback
#define memcpyesque libcesque
#define strlenesque libcesque nosideeffect paramsnonnull()
#define vallocesque \
  libcesque __wur returnsaligned((65536)) returnspointerwithnoaliases
#define reallocesque libcesque returnsaligned((16))
#define mallocesque  reallocesque returnspointerwithnoaliases
#define interruptfn  nocallersavedregisters forcealignargpointer

#ifndef pureconst
#define pureconst __attribute__((__const__))
#endif

#ifndef forcealign
#define forcealign(bytes) __attribute__((__aligned__(bytes)))
#endif
#define thatispacked __attribute__((__packed__))

#define printfesque(n)   __attribute__((__format__(__gnu_printf__, n, n + 1)))
#define scanfesque(n)    __attribute__((__format__(__gnu_scanf__, n, n + 1)))
#define strftimeesque(n) __attribute__((__format__(__strftime__, n, 0)))

#ifndef privileged
#define privileged _Section(".privileged") dontinstrument dontubsan
#endif

#ifndef wontreturn
#if (__has_attribute(__noreturn__) || \
     (__GNUC__ + 0) * 100 + (__GNUC_MINOR__ + 0) >= 208)
#define wontreturn __attribute__((__noreturn__))
#else
#define wontreturn
#endif
#endif

#ifndef nosideeffect
#if (__has_attribute(__pure__) || \
     (__GNUC__ + 0) * 100 + (__GNUC_MINOR__ + 0) >= 296)
#define nosideeffect __attribute__((__pure__))
#else
#define nosideeffect
#endif
#endif

#ifndef dontinline
#ifdef _MSC_VER
#define dontinline __declspec(noinline)
#elif (__has_attribute(__noinline__) || \
       (__GNUC__ + 0) * 100 + (__GNUC_MINOR__ + 0) >= 301)
#define dontinline __attribute__((__noinline__))
#else
#define dontinline
#endif
#endif

#ifndef dontclone
#if (__has_attribute(__noclone__) || \
     (__GNUC__ + 0) * 100 + (__GNUC_MINOR__ + 0) >= 405)
#define dontclone __attribute__((__noclone__))
#else
#define dontclone
#endif
#endif

#ifndef forceinline
#ifdef __cplusplus
#define forceinline inline
#else
#if (__GNUC__ + 0) * 100 + (__GNUC_MINOR__ + 0) >= 302
#if (__GNUC__ + 0) * 100 + (__GNUC_MINOR__ + 0) >= 403 || \
    !defined(__cplusplus) ||                              \
    (defined(__clang__) &&                                \
     (defined(__GNUC_STDC_INLINE__) || defined(__GNUC_GNU_INLINE__)))
#if defined(__GNUC_STDC_INLINE__) || defined(__cplusplus)
#define forceinline                                                 \
  static __inline __attribute__((__always_inline__, __gnu_inline__, \
                                 __no_instrument_function__, __unused__))
#else
#define forceinline                                 \
  static __inline __attribute__((__always_inline__, \
                                 __no_instrument_function__, __unused__))
#endif /* __GNUC_STDC_INLINE__ */
#endif /* GCC >= 4.3 */
#elif defined(_MSC_VER)
#define forceinline __forceinline
#else
#define forceinline static inline
#endif /* !ANSI && GCC >= 3.2 */
#endif /* __cplusplus */
#endif /* forceinline */

#ifndef __wur
#if ((__GNUC__ + 0) * 100 + (__GNUC_MINOR__ + 0) >= 304 || \
     __has_attribute(__warn_unused_result__))
#define __wur __attribute__((__warn_unused_result__))
#else
#define __wur
#endif
#endif

#ifndef nullterminated
#if __has_attribute(__sentinel__) || __GNUC__ + 0 >= 4
#define nullterminated(x) __attribute__((__sentinel__ x))
#else
#define nullterminated(x)
#endif
#endif

#ifndef flattenout
#if __has_attribute(__flatten__) || \
    ((__GNUC__ + 0) * 100 + (__GNUC_MINOR__ + 0) >= 401 && !defined(__llvm__))
#define flattenout __attribute__((__flatten__))
#else
#define flattenout
#endif
#endif

#ifndef externinline
#if (!defined(__cplusplus) ||                              \
     (__GNUC__ + 0) * 100 + (__GNUC_MINOR__ + 0) >= 403 || \
     (defined(__clang__) &&                                \
      (defined(__GNUC_STDC_INLINE__) || defined(__GNUC_GNU_INLINE__))))
#if defined(__GNUC_STDC_INLINE__) || defined(__cplusplus)
#define externinline extern __inline __attribute__((__gnu_inline__))
#else
#define externinline extern __inline __attribute__((__always_inline__))
#endif
#else
#define externinline inline
#endif
#endif

#ifndef relegated
#if (__has_attribute(__cold__) || \
     (__GNUC__ + 0) * 100 + (__GNUC_MINOR__ + 0) >= 403)
#define relegated __attribute__((__cold__))
#else
#define relegated
#endif
#endif

#if (__has_attribute(__warning__) || \
     (__GNUC__ + 0) * 100 + (__GNUC_MINOR__ + 0) >= 403)
#define warnifused(s) __attribute__((__warning__(s)))
#else
#define warnifused(s)
#endif

#ifndef firstclass
#if (__has_attribute(__hot__) || \
     (__GNUC__ + 0) * 100 + (__GNUC_MINOR__ + 0) >= 403)
#define firstclass __attribute__((__hot__))
#else
#define firstclass
#endif
#endif

#ifndef paramsnonnull
#if (__has_attribute(__nonnull__) || \
     (__GNUC__ + 0) * 100 + (__GNUC_MINOR__ + 0) >= 403)
#define paramsnonnull(opt_1idxs) __attribute__((__nonnull__ opt_1idxs))
#else
#define paramsnonnull(opt_1idxs)
#endif
#endif

#if __STDC_VERSION__ + 0 >= 199901L
#define hasatleast static
#else
#define hasatleast
#endif

#ifndef dontcallback
#if (__has_attribute(__leaf__) || \
     (!defined(__llvm__) &&       \
      (__GNUC__ + 0) * 100 + (__GNUC_MINOR__ + 0) >= 406))
#define dontcallback __attribute__((__leaf__))
#else
#define dontcallback
#endif
#endif

#ifndef dontthrow
#if defined(__cplusplus) &&        \
    (__has_attribute(dontthrow) || \
     (__GNUC__ + 0) * 100 + (__GNUC_MINOR__ + 0) >= 303)
#define dontthrow __attribute__((__nothrow__))
#elif defined(_MSC_VER)
#define dontthrow __declspec(nothrow)
#else
#define dontthrow
#endif
#endif

#ifndef returnstwice
#if (__has_attribute(__returns_twice__) || \
     (__GNUC__ + 0) * 100 + (__GNUC_MINOR__ + 0) >= 402)
#define returnstwice __attribute__((__returns_twice__))
#else
#define returnstwice
#endif
#endif

#ifndef nodebuginfo
#if __has_attribute(__nodebug__) || defined(__llvm__)
#define nodebuginfo __attribute__((__nodebug__))
#else
#define nodebuginfo
#endif
#endif

#if (__GNUC__ + 0) * 100 + (__GNUC_MINOR__ + 0) >= 408 || \
    __has_attribute(__force_align_arg_pointer__)
#define forcealignargpointer __attribute__((__force_align_arg_pointer__))
#else
#define forcealignargpointer "need modern compiler"
#endif

#ifndef returnsnonnull
#if (__has_attribute(__returns_nonnull__) || \
     (__GNUC__ + 0) * 100 + (__GNUC_MINOR__ + 0) >= 409)
#define returnsnonnull __attribute__((__returns_nonnull__))
#else
#define returnsnonnull
#endif
#endif

#if (__has_attribute(__assume_aligned__) || \
     (__GNUC__ + 0) * 100 + (__GNUC_MINOR__ + 0) >= 409)
#define returnsaligned(x) __attribute__((__assume_aligned__ x))
#else
#define returnsaligned(x)
#endif

#ifndef returnspointerwithnoaliases
#if (__has_attribute(__malloc__) || \
     (__GNUC__ + 0) * 100 + (__GNUC_MINOR__ + 0) >= 409)
#define returnspointerwithnoaliases __attribute__((__malloc__))
#elif defined(_MSC_VER)
#define returnspointerwithnoaliases __declspec(allocator)
#else
#define returnspointerwithnoaliases
#endif
#endif

#ifndef attributeallocsize
#if (__has_attribute(__alloc_size__) || \
     (__GNUC__ + 0) * 100 + (__GNUC_MINOR__ + 0) >= 409)
#define attributeallocsize(x) __attribute__((__alloc_size__ x))
#else
#define attributeallocsize(x)
#endif
#endif

#ifndef attributeallocalign
#if (__has_attribute(__alloc_align__) || \
     (__GNUC__ + 0) * 100 + (__GNUC_MINOR__ + 0) >= 409)
#define attributeallocalign(x) __attribute__((__alloc_align__ x))
#else
#define attributeallocalign(x)
#endif
#endif

#if __cplusplus + 0 >= 201103L
#define autotype(x) auto
#elif ((__has_builtin(__auto_type) || defined(__llvm__) ||     \
        (__GNUC__ + 0) * 100 + (__GNUC_MINOR__ + 0) >= 409) && \
       !defined(__chibicc__))
#define autotype(x) __auto_type
#else
#define autotype(x) typeof(x)
#endif

#define offsetof(type, member) __builtin_offsetof(type, member)

#if defined(__GNUC__) && __GNUC__ >= 10
#define __read_only(...) __attribute__((__access__(__read_only__, __VA_ARGS__)))
#define __write_only(...) \
  __attribute__((__access__(__write_only__, __VA_ARGS__)))
#define __read_write(...) \
  __attribute__((__access__(__read_write__, __VA_ARGS__)))
#else
#define __read_only(...)
#define __write_only(...)
#define __read_write(...)
#endif

#if defined(__GNUC__) && __GNUC__ >= 13
#define __fd_arg(N) __attribute__((__fd_arg__(N)))
#else
#define __fd_arg(N)
#endif

#ifdef _COSMO_SOURCE

#ifndef dontinstrument
#if (__has_attribute(__no_instrument_function__) || \
     (__GNUC__ + 0) * 100 + (__GNUC_MINOR__ + 0) >= 204)
#if ((__GNUC__ + 0) >= 7 && !defined(__chibicc__)) || \
    __has_attribute(__patchable_function_entry__)
#define dontinstrument                       \
  __attribute__((__no_instrument_function__, \
                 __patchable_function_entry__(0, 0)))
#else
#define dontinstrument __attribute__((__no_instrument_function__))
#endif
#else
#define dontinstrument
#endif
#endif

#ifndef mayalias
#if (__has_attribute(__may_alias__) || \
     (__GNUC__ + 0) * 100 + (__GNUC_MINOR__ + 0) >= 303)
#define mayalias __attribute__((__may_alias__))
#else
#define mayalias
#endif
#endif

#ifndef dontoptimize
#if defined(__llvm__) || __has_attribute(__optnone__)
#define dontoptimize __attribute__((__optnone__))
#elif (__GNUC__ + 0) * 100 + (__GNUC_MINOR__ + 0) >= 407 || \
    __has_attribute(__optimize__)
#define dontoptimize __attribute__((__optimize__(0)))
#endif
#endif

#ifndef optimizesize
#if (__GNUC__ + 0) * 100 + (__GNUC_MINOR__ + 0) >= 407 || \
    __has_attribute(__optimize__)
#define optimizesize __attribute__((__optimize__("s")))
#elif defined(__llvm__) || __has_attribute(__optnone__)
#define optimizesize __attribute__((__optnone__))
#endif
#endif

#ifndef optimizespeed
/* warning: corrupts frame pointer; only use on leaf functions */
#if ((__GNUC__ + 0) * 100 + (__GNUC_MINOR__ + 0) >= 407 || \
     __has_attribute(__optimize__))
#define optimizespeed __attribute__((__optimize__(3)))
#else
#define optimizespeed
#endif
#endif

#ifndef unrollloops
#if ((__GNUC__ + 0) * 100 + (__GNUC_MINOR__ + 0) >= 407 || \
     __has_attribute(__optimize__))
#define unrollloops __attribute__((__optimize__("unroll-loops")))
#else
#define unrollloops
#endif
#endif

#ifndef _Microarchitecture
#if (__has_attribute(__target__) || \
     (__GNUC__ + 0) * 100 + (__GNUC_MINOR__ + 0) >= 404)
#define _Microarchitecture(march) __attribute__((__target__(march)))
#else
#define _Microarchitecture(march)
#endif
#endif

#ifdef __x86_64__
#if __GNUC__ >= 7 || __has_attribute(__no_caller_saved_registers__)
#define nocallersavedregisters __attribute__((__no_caller_saved_registers__))
#else
#define nocallersavedregisters "need modern compiler"
#endif
#else
#define nocallersavedregisters
#endif

#if ((__GNUC__ + 0) * 100 + (__GNUC_MINOR__ + 0) >= 408 || \
     __has_attribute(__no_sanitize_undefined__))
#define dontubsan __attribute__((__no_sanitize_undefined__))
#else
#define dontubsan
#endif

#ifdef __x86_64__
#define notpossible          \
  do {                       \
    __asm__("ud2");          \
    __builtin_unreachable(); \
  } while (0)
#elif defined(__aarch64__)
#define notpossible          \
  do {                       \
    __asm__("udf\t#0");      \
    __builtin_unreachable(); \
  } while (0)
#else
#define notpossible __builtin_trap()
#endif

#define donothing \
  do {            \
  } while (0)

#define textstartup _Section(".text.startup")
#define textexit    _Section(".text.exit")
#define textreal    _Section(".text.real")
#define texthead    _Section(".text.head")
#define textwindows _Section(".text.windows")
#define antiquity   _Section(".text.antiquity")

#ifdef __llvm__
#define __builtin_ia32_movntdq(x, y) (*(x) = (y))
#endif

#ifndef _Section
#define _Section(s) __attribute__((__section__(s)))
#endif

#ifndef __llvm__
#pragma GCC diagnostic ignored "-Wformat=0" /* todo: patch gcc */
#pragma GCC diagnostic ignored "-Wbuiltin-declaration-mismatch"
#pragma GCC diagnostic warning "-Wunknown-pragmas"
#else
#pragma GCC diagnostic ignored "-Wformat"
#pragma GCC diagnostic ignored "-Wconstant-logical-operand" /* what */
#pragma GCC diagnostic ignored "-Wunknown-warning-option"
#pragma GCC diagnostic ignored "-Wunknown-pragmas"
#pragma GCC diagnostic ignored "-Wstring-plus-int"       /* features 4 losers */
#pragma GCC diagnostic ignored "-Wkeyword-compat"        /* c++ upgrade */
#pragma GCC diagnostic ignored "-Wuser-defined-literals" /* reserved for me */
#endif

#pragma GCC diagnostic ignored "-Wformat-extra-args"     /* todo: patch gcc */
#pragma GCC diagnostic ignored "-Wunused-function"       /* contradicts dce */
#pragma GCC diagnostic ignored "-Wunused-const-variable" /* sooo ridiculous */
#ifndef __cplusplus
#pragma GCC diagnostic ignored "-Wold-style-definition" /* orwellian bullsh */
#endif

#ifdef __x86_64__
#define DebugBreak() __asm__("int3")
#elif defined(__aarch64__)
#define DebugBreak() __asm__("brk\t#0x666")
#else
#define DebugBreak() __builtin_trap()
#endif

#ifdef __aarch64__
/* raise sigill (not sigtrap) like x86 does */
#define __builtin_trap() \
  (({ __asm__("udf\t#0x666"); }), __builtin_unreachable())
#endif

#endif /* _COSMO_SOURCE */

#define __veil(CONSTRAINT, EXPRESSION)                               \
  __extension__({                                                    \
    autotype(EXPRESSION) VeiledValue = (EXPRESSION);                 \
    __asm__("" : "=" CONSTRAINT ""(VeiledValue) : "0"(VeiledValue)); \
    VeiledValue;                                                     \
  })

#define __conceal(CONSTRAINT, EXPRESSION)                                     \
  __extension__({                                                             \
    autotype(EXPRESSION) VeiledValue = (EXPRESSION);                          \
    __asm__ volatile("" : "=" CONSTRAINT ""(VeiledValue) : "0"(VeiledValue)); \
    VeiledValue;                                                              \
  })

#define __expropriate(EXPRESSION)                      \
  __extension__({                                      \
    __asm__ volatile("" ::"g"(EXPRESSION) : "memory"); \
    0;                                                 \
  })

#if !defined(__APPLE__) && defined(__x86_64__)
#define __yoink(SYMBOL) \
  __asm__(".section .yoink\n\tnopl\t%0\n\t.previous" : : "m"(SYMBOL))
#elif defined(__aarch64__)
#define __yoink(SYMBOL) \
  __asm__(".section .yoink\n\tb\t%0\n\t.previous" : : "m"(SYMBOL))
#else
#define __yoink(SYMBOL) (void)0
#endif

#if !defined(__APPLE__) && defined(__x86_64__)
#define __static_yoink(SYMBOLSTR) \
  __asm__(".section .yoink\n\tnopl\t\"" SYMBOLSTR "\"\n\t.previous")
#elif defined(__aarch64__)
#define __static_yoink(SYMBOLSTR) \
  __asm__(".section .yoink\n\tb\t\"" SYMBOLSTR "\"\n\t.previous")
#else
#define __static_yoink(SYMBOLSTR)
#endif

#if !defined(IM_FEELING_NAUGHTY)
#define __static_yoink_source(PATH) __static_yoink(PATH)
#else
#define __static_yoink_source(PATH)
#endif

#define __weak_reference(sym, alias) __weak_reference_impl(sym, alias)
#define __weak_reference_impl(sym, alias)  \
  __asm__(".weak\t" #alias "\n\t"          \
          ".equ\t" #alias ", " #sym "\n\t" \
          ".type\t" #alias ",@notype")

#ifndef __chibicc__
#define __strong_reference(sym, alias) \
  extern __typeof(sym) alias __attribute__((__alias__(#sym)))
#else
#define __strong_reference(sym, alias) __weak_reference(sym, alias)
#endif

#if defined(__GNUC__) || defined(__llvm__)
#define __funline \
  extern __inline \
      __attribute__((__gnu_inline__, __always_inline__, __artificial__))
#else
#define __funline static inline
#endif

#if defined(__x86_64__) && (defined(__GNUC__) || defined(__llvm__)) && \
    !defined(__chibicc__) && defined(__OPTIMIZE__)
#define __target_clones(x) __attribute__((__target_clones__(x ",default")))
#else
#define __target_clones(x)
#endif

#if !defined(TINY) && !defined(__AVX__)
#define __vex __target_clones("avx")
#else
#define __vex
#endif

#define __notice(sym, str)                                                   \
  __attribute__((__section__(".notice"), __aligned__(1))) const char sym[] = \
      "\n\n" str

#define MACHINE_CODE_ANALYSIS_BEGIN_
#define MACHINE_CODE_ANALYSIS_END_
