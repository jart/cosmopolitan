#if __GNUC__ + 0 < 2
#define __attribute__(x)
#endif

#ifndef __cplusplus
#define COSMOPOLITAN_C_START_
#define COSMOPOLITAN_C_END_
#define COSMOPOLITAN_CXX_START_
#define COSMOPOLITAN_CXX_END_
#define COSMOPOLITAN_CXX_USING_
#endif

#ifndef __ia16__
#define __far
#endif

#if !defined(__GNUC__) && __cplusplus + 0 >= 201103L
#define typeof(x) decltype(x)
#elif (defined(__STRICT_ANSI__) || !defined(__GNUC__)) && \
    __STDC_VERSION__ + 0 < 201112
#define typeof(x) __typeof(x)
#endif

#ifdef __cplusplus
#if __cplusplus >= 201103L
#define _Alignof(x) alignof(x)
#endif /* C++11 */
#else  /* __cplusplus */
#if __STDC_VERSION__ + 0 < 201112
#if __GNUC__ + _MSC_VER + 0 && !defined(__STRICT_ANSI__)
#define _Alignof(x) __alignof(x)
#else
#define _Alignof(x) /* basically all it ever did lool */ sizeof(x)
#endif /* GNU/MSVC/!ANSI */
#endif /* C11 */
#endif /* __cplusplus */

#if !defined(__cplusplus) && !defined(inline) && __STDC_VERSION__ + 0 < 199901
#if !defined(__STRICT_ANSI__) && (defined(__GNUC__) || defined(_MSC_VER))
#define inline __inline
#else
#define inline
#define __inline
#endif
#endif

#ifdef __chibicc__
#define __extension__
#endif

#if __STDC_VERSION__ + 0 < 201112
#if defined(__GNUC__) && !defined(__STRICT_ANSI__)
#define _Alignas(x) __attribute__((__aligned__(x)))
#elif defined(_MSC_VER)
#define _Alignas(x) __declspec(align(x))
#endif
#endif

#ifdef _MSC_VER
#define __builtin_unreachable() __assume(false)
#elif defined(__STRICT_ANSI__) ||                           \
    !((__GNUC__ + 0) * 100 + (__GNUC_MINOR__ + 0) >= 405 || \
      defined(__clang__) || defined(__INTEL_COMPILER) ||    \
      __has_builtin(__builtin_unreachable))
#define __builtin_unreachable() \
  for (;;) {                    \
  }
#endif

#if defined(__STRICT_ANSI__) || \
    (!defined(__llvm__) && !__has_builtin(__builtin_assume))
#define __builtin_assume(x)            \
  do {                                 \
    if (!(x)) __builtin_unreachable(); \
  } while (0)
#endif

#if __STDC_VERSION__ + 0 < 201112
#define _Atomic(TYPE) TYPE volatile
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

#define _LIBCPP_STDINT_H

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

#define __DEFINED_max_align_t
typedef long double max_align_t;

#ifdef _COSMO_SOURCE
#if ((__GNUC__ + 0) * 100 + (__GNUC_MINOR__ + 0) >= 406 || \
     defined(__llvm__)) &&                                 \
    !defined(__STRICT_ANSI__)
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
#ifndef __STRICT_ANSI__
#define pureconst __attribute__((__const__))
#else
#define pureconst
#endif
#endif

#ifndef forcealign
#define forcealign(bytes) __attribute__((__aligned__(bytes)))
#endif
#define thatispacked __attribute__((__packed__))

#ifndef __STRICT_ANSI__
#define printfesque(n)   __attribute__((__format__(__gnu_printf__, n, n + 1)))
#define scanfesque(n)    __attribute__((__format__(__gnu_scanf__, n, n + 1)))
#define strftimeesque(n) __attribute__((__format__(__strftime__, n, 0)))
#else
#define printfesque(n)
#define scanfesque(n)
#define strftimeesque(n)
#endif

#ifndef privileged
#define privileged \
  _Section(".privileged") dontinline dontinstrument dontubsan dontasan
#endif

#ifndef wontreturn
#if !defined(__STRICT_ANSI__) &&      \
    (__has_attribute(__noreturn__) || \
     (__GNUC__ + 0) * 100 + (__GNUC_MINOR__ + 0) >= 208)
#define wontreturn __attribute__((__noreturn__))
#else
#define wontreturn
#endif
#endif

#ifndef nosideeffect
#if !defined(__STRICT_ANSI__) &&  \
    (__has_attribute(__pure__) || \
     (__GNUC__ + 0) * 100 + (__GNUC_MINOR__ + 0) >= 296)
#define nosideeffect __attribute__((__pure__))
#else
#define nosideeffect
#endif
#endif

#ifndef dontinline
#ifdef _MSC_VER
#define dontinline __declspec(noinline)
#elif !defined(__STRICT_ANSI__) &&    \
    (__has_attribute(__noinline__) || \
     (__GNUC__ + 0) * 100 + (__GNUC_MINOR__ + 0) >= 301)
#define dontinline __attribute__((__noinline__))
#else
#define dontinline
#endif
#endif

#ifndef dontclone
#if !defined(__STRICT_ANSI__) &&     \
    (__has_attribute(__noclone__) || \
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
#if !defined(__STRICT_ANSI__) && \
    (__GNUC__ + 0) * 100 + (__GNUC_MINOR__ + 0) >= 302
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
#if !defined(__STRICT_ANSI__) &&                           \
    ((__GNUC__ + 0) * 100 + (__GNUC_MINOR__ + 0) >= 304 || \
     __has_attribute(__warn_unused_result__))
#define __wur __attribute__((__warn_unused_result__))
#else
#define __wur
#endif
#endif

#ifndef nullterminated
#if !defined(__STRICT_ANSI__) && \
    (__has_attribute(__sentinel__) || __GNUC__ + 0 >= 4)
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
#if !defined(__STRICT_ANSI__) &&                           \
    (!defined(__cplusplus) ||                              \
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
#if !defined(__STRICT_ANSI__) &&  \
    (__has_attribute(__cold__) || \
     (__GNUC__ + 0) * 100 + (__GNUC_MINOR__ + 0) >= 403)
#define relegated __attribute__((__cold__))
#else
#define relegated
#endif
#endif

#if !defined(__STRICT_ANSI__) &&     \
    (__has_attribute(__warning__) || \
     (__GNUC__ + 0) * 100 + (__GNUC_MINOR__ + 0) >= 403)
#define warnifused(s) __attribute__((__warning__(s)))
#else
#define warnifused(s)
#endif

#ifndef firstclass
#if !defined(__STRICT_ANSI__) && \
    (__has_attribute(__hot__) || \
     (__GNUC__ + 0) * 100 + (__GNUC_MINOR__ + 0) >= 403)
#define firstclass __attribute__((__hot__))
#else
#define firstclass
#endif
#endif

#ifndef paramsnonnull
#if !defined(__STRICT_ANSI__) &&     \
    (__has_attribute(__nonnull__) || \
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

#if __STDC_VERSION__ + 0 < 199901L && !defined(restrict)
#if !defined(__STRICT_ANSI__) && !defined(__cplusplus) && \
    ((__GNUC__ + 0) * 100 + (__GNUC_MINOR__ + 0) >= 301 || defined(_MSC_VER))
#define restrict __restrict__
#else
#define restrict
#define __restrict
#endif
#endif

#ifndef dontcallback
#if !defined(__STRICT_ANSI__) &&  \
    (__has_attribute(__leaf__) || \
     (!defined(__llvm__) &&       \
      (__GNUC__ + 0) * 100 + (__GNUC_MINOR__ + 0) >= 406))
#define dontcallback __attribute__((__leaf__))
#else
#define dontcallback
#endif
#endif

#ifndef dontthrow
#if defined(__cplusplus) && !defined(__STRICT_ANSI__) && \
    (__has_attribute(dontthrow) ||                       \
     (__GNUC__ + 0) * 100 + (__GNUC_MINOR__ + 0) >= 303)
#define dontthrow __attribute__((__nothrow__))
#elif defined(_MSC_VER)
#define dontthrow __declspec(nothrow)
#else
#define dontthrow
#endif
#endif

#ifndef returnstwice
#if !defined(__STRICT_ANSI__) &&           \
    (__has_attribute(__returns_twice__) || \
     (__GNUC__ + 0) * 100 + (__GNUC_MINOR__ + 0) >= 402)
#define returnstwice __attribute__((__returns_twice__))
#else
#define returnstwice
#endif
#endif

#ifndef nodebuginfo
#if !defined(__STRICT_ANSI__) && \
    (__has_attribute(__nodebug__) || defined(__llvm__))
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
#if !defined(__STRICT_ANSI__) &&             \
    (__has_attribute(__returns_nonnull__) || \
     (__GNUC__ + 0) * 100 + (__GNUC_MINOR__ + 0) >= 409)
#define returnsnonnull __attribute__((__returns_nonnull__))
#else
#define returnsnonnull
#endif
#endif

#if !defined(__STRICT_ANSI__) &&            \
    (__has_attribute(__assume_aligned__) || \
     (__GNUC__ + 0) * 100 + (__GNUC_MINOR__ + 0) >= 409)
#define returnsaligned(x) __attribute__((__assume_aligned__ x))
#else
#define returnsaligned(x)
#endif

#ifndef returnspointerwithnoaliases
#if !defined(__STRICT_ANSI__) &&    \
    (__has_attribute(__malloc__) || \
     (__GNUC__ + 0) * 100 + (__GNUC_MINOR__ + 0) >= 409)
#define returnspointerwithnoaliases __attribute__((__malloc__))
#elif defined(_MSC_VER)
#define returnspointerwithnoaliases __declspec(allocator)
#else
#define returnspointerwithnoaliases
#endif
#endif

#ifndef attributeallocsize
#if !defined(__STRICT_ANSI__) &&        \
    (__has_attribute(__alloc_size__) || \
     (__GNUC__ + 0) * 100 + (__GNUC_MINOR__ + 0) >= 409)
#define attributeallocsize(x) __attribute__((__alloc_size__ x))
#else
#define attributeallocsize(x)
#endif
#endif

#ifndef attributeallocalign
#if !defined(__STRICT_ANSI__) &&         \
    (__has_attribute(__alloc_align__) || \
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

#ifdef _COSMO_SOURCE

#ifndef dontinstrument
#if !defined(__STRICT_ANSI__) &&                    \
    (__has_attribute(__no_instrument_function__) || \
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
#if !defined(__STRICT_ANSI__) &&       \
    (__has_attribute(__may_alias__) || \
     (__GNUC__ + 0) * 100 + (__GNUC_MINOR__ + 0) >= 303)
#define mayalias __attribute__((__may_alias__))
#else
#define mayalias
#endif
#endif

#ifndef dontoptimize
#ifndef __STRICT_ANSI__
#if defined(__llvm__) || __has_attribute(__optnone__)
#define dontoptimize __attribute__((__optnone__))
#elif (__GNUC__ + 0) * 100 + (__GNUC_MINOR__ + 0) >= 407 || \
    __has_attribute(__optimize__)
#define dontoptimize __attribute__((__optimize__(0)))
#endif
#else
#define dontoptimize
#endif
#endif

#ifndef optimizesize
#ifndef __STRICT_ANSI__
#if (__GNUC__ + 0) * 100 + (__GNUC_MINOR__ + 0) >= 407 || \
    __has_attribute(__optimize__)
#define optimizesize __attribute__((__optimize__("s")))
#elif defined(__llvm__) || __has_attribute(__optnone__)
#define optimizesize __attribute__((__optnone__))
#endif
#else
#define optimizesize
#endif
#endif

#ifndef optimizespeed
/* warning: corrupts frame pointer; only use on leaf functions */
#if !defined(__STRICT_ANSI__) &&                           \
    ((__GNUC__ + 0) * 100 + (__GNUC_MINOR__ + 0) >= 407 || \
     __has_attribute(__optimize__))
#define optimizespeed __attribute__((__optimize__(3)))
#else
#define optimizespeed
#endif
#endif

#ifndef unrollloops
#if !defined(__STRICT_ANSI__) &&                           \
    ((__GNUC__ + 0) * 100 + (__GNUC_MINOR__ + 0) >= 407 || \
     __has_attribute(__optimize__))
#define unrollloops __attribute__((__optimize__("unroll-loops")))
#else
#define unrollloops
#endif
#endif

#ifndef _Microarchitecture
#if !defined(__STRICT_ANSI__) &&    \
    (__has_attribute(__target__) || \
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
     __has_attribute(__no_sanitize_address__)) &&          \
    !defined(__STRICT_ANSI__)
#define dontasan __attribute__((__no_sanitize_address__))
#else
#define dontasan
#endif

#if ((__GNUC__ + 0) * 100 + (__GNUC_MINOR__ + 0) >= 408 || \
     __has_attribute(__no_sanitize_undefined__)) &&        \
    !defined(__STRICT_ANSI__)
#define dontubsan __attribute__((__no_sanitize_undefined__))
#else
#define dontubsan
#endif

#ifdef __STRICT_ANSI__
void abort(void) wontreturn;
#define notpossible abort()
#else
#ifdef __x86_64__
#define notpossible          \
  do {                       \
    asm("nop\n\t"            \
        "ud2\n\t"            \
        "nop");              \
    __builtin_unreachable(); \
  } while (0)
#elif defined(__aarch64__)
#define notpossible          \
  do {                       \
    asm("udf\t#0\n\t"        \
        "nop");              \
    __builtin_unreachable(); \
  } while (0)
#else
#define notpossible __builtin_trap()
#endif
#endif

#define donothing \
  do {            \
  } while (0)

#ifndef __STRICT_ANSI__
#define textstartup _Section(".text.startup")
#define textexit    _Section(".text.exit")
#define textreal    _Section(".text.real")
#define texthead    _Section(".text.head")
#define textwindows _Section(".text.windows")
#define antiquity   _Section(".text.antiquity")
#else
#define textstartup
#define textexit
#define textreal
#define texthead
#define textwindows
#define antiquity
#endif

#ifdef __llvm__
#define __builtin_ia32_movntdq(x, y) (*(x) = (y))
#endif

#ifndef _Section
#define _Section(s) __attribute__((__section__(s)))
#endif

#if defined(__x86_64__) && !defined(__llvm__)
#define initarray _Section(".init_array,\"a\",@init_array #")
#else
#define initarray _Section(".init_array")
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

#ifndef __STRICT_ANSI__
#ifdef __x86_64__
#define DebugBreak() asm("int3")
#else
#define DebugBreak() __builtin_trap()
#endif
#else
#define DebugBreak() (void)0
#endif

#endif /* _COSMO_SOURCE */

#ifndef __STRICT_ANSI__
#define __veil(CONSTRAINT, EXPRESSION)                           \
  ({                                                             \
    autotype(EXPRESSION) VeiledValue = (EXPRESSION);             \
    asm("" : "=" CONSTRAINT ""(VeiledValue) : "0"(VeiledValue)); \
    VeiledValue;                                                 \
  })
#else
#define __veil(CONSTRAINT, EXPRESSION) (EXPRESSION)
#endif

#ifndef __STRICT_ANSI__
#define __conceal(CONSTRAINT, EXPRESSION)                                 \
  ({                                                                      \
    autotype(EXPRESSION) VeiledValue = (EXPRESSION);                      \
    asm volatile("" : "=" CONSTRAINT ""(VeiledValue) : "0"(VeiledValue)); \
    VeiledValue;                                                          \
  })
#else
#define __conceal(CONSTRAINT, EXPRESSION) (EXPRESSION)
#endif

#ifndef __STRICT_ANSI__
#define __expropriate(EXPRESSION)                  \
  ({                                               \
    asm volatile("" ::"g"(EXPRESSION) : "memory"); \
    0;                                             \
  })
#else
#define __expropriate(EXPRESSION) (EXPRESSION)
#endif

#if !defined(__STRICT_ANSI__) && !defined(__APPLE__) && defined(__x86_64__)
#define __yoink(SYMBOL) \
  asm(".section .yoink\n\tnopl\t%0\n\t.previous" : : "m"(SYMBOL))
#elif defined(__aarch64__)
#define __yoink(SYMBOL) \
  asm(".section .yoink\n\tb\t%0\n\t.previous" : : "m"(SYMBOL))
#else
#define __yoink(SYMBOL) (void)0
#endif

#if !defined(__STRICT_ANSI__) && !defined(__APPLE__) && defined(__x86_64__)
#define __static_yoink(SYMBOLSTR) \
  asm(".section .yoink\n\tnopl\t\"" SYMBOLSTR "\"\n\t.previous")
#elif defined(__aarch64__)
#define __static_yoink(SYMBOLSTR) \
  asm(".section .yoink\n\tb\t\"" SYMBOLSTR "\"\n\t.previous")
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
    !defined(__chibicc__) && 0 /* TODO: enable with toolchain upgrade */
#define __target_clones(x) __attribute__((__target_clones__(x ",default")))
#else
#define __target_clones(x)
#endif

#if !defined(TINY) && !defined(__AVX__) && 0
#define __vex __target_clones("avx")
#else
#define __vex
#endif

#define MACHINE_CODE_ANALYSIS_BEGIN_
#define MACHINE_CODE_ANALYSIS_END_
