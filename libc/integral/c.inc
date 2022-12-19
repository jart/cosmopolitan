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

#if __STDC_VERSION__ + 0 < 201112
#if defined(__GNUC__) && !defined(__STRICT_ANSI__)
#define _Alignas(x) __attribute__((__aligned__(x)))
#elif defined(_MSC_VER)
#define _Alignas(x) __declspec(align(x))
#endif
#endif

#ifdef _MSC_VER
#define __builtin_unreachable() __assume(0)
#elif defined(__STRICT_ANSI__) ||                           \
    !((__GNUC__ + 0) * 100 + (__GNUC_MINOR__ + 0) >= 405 || \
      defined(__clang__) || defined(__INTEL_COMPILER))
#define __builtin_unreachable() \
  for (;;) {                    \
  }
#endif

#if defined(__STRICT_ANSI__) || (!defined(__llvm__) && !__has_builtin(assume))
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
#define HAVE_STDBOOL_H 1
#if __STDC_VERSION__ + 0 >= 201112
typedef _Bool bool;
#define true ((bool)+1)
#define false ((bool)+0)
#else
#define bool int
#define true 1
#define false 0
#endif
#endif

#ifndef __cplusplus
typedef __WCHAR_TYPE__ wchar_t;
typedef __CHAR16_TYPE__ char16_t;
typedef __CHAR32_TYPE__ char32_t;
#endif

#define _LIBCPP_STDINT_H

typedef int errno_t;
typedef __SIZE_TYPE__ size_t;
typedef __PTRDIFF_TYPE__ ssize_t;
typedef __INTPTR_TYPE__ intptr_t;
typedef __UINTPTR_TYPE__ uintptr_t;
typedef __PTRDIFF_TYPE__ ptrdiff_t;
typedef __WINT_TYPE__ wint_t; /* uint32_t on linux but int32_t on xnu */
typedef __INT32_TYPE__ bool32;
typedef __INT8_TYPE__ int8_t;
typedef __UINT8_TYPE__ uint8_t;
typedef __INT16_TYPE__ int16_t;
typedef __UINT16_TYPE__ uint16_t;
typedef __INT32_TYPE__ int32_t;
typedef __UINT32_TYPE__ uint32_t;
typedef __INT64_TYPE__ int64_t;
typedef __UINT64_TYPE__ uint64_t;
typedef __INTMAX_TYPE__ intmax_t;
typedef __UINTMAX_TYPE__ uintmax_t;

#if ((__GNUC__ + 0) * 100 + (__GNUC_MINOR__ + 0) >= 406 || \
     defined(__llvm__)) &&                                 \
    !defined(__STRICT_ANSI__)
typedef signed __int128 int128_t;
typedef unsigned __int128 uint128_t;
#endif

typedef struct {
  intptr_t ax, dx;
} axdx_t;

#ifndef __chibicc__
#define va_list            __builtin_va_list
#define va_arg(ap, type)   __builtin_va_arg(ap, type)
#define va_copy(dest, src) __builtin_va_copy(dest, src)
#define va_end(ap)         __builtin_va_end(ap)
#define va_start(ap, last) __builtin_va_start(ap, last)
#else
#include "libc/integral/lp64arg.inc"
#endif

#define libcesque   dontthrow nocallback
#define memcpyesque libcesque
#define strlenesque libcesque nosideeffect paramsnonnull()
#define vallocesque \
  libcesque dontdiscard returnsaligned((GUARDSIZE)) returnspointerwithnoaliases
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
#ifndef __STRICT_ANSI__
#define forcealign(bytes) __attribute__((__aligned__(bytes)))
#else
#define forcealign(bytes)
#endif
#endif

#ifndef __STRICT_ANSI__
#define thatispacked __attribute__((__packed__))
#else
#define thatispacked
#endif

#ifndef __STRICT_ANSI__
#define printfesque(n)   __attribute__((__format__(__gnu_printf__, n, n + 1)))
#define scanfesque(n)    __attribute__((__format__(__gnu_scanf__, n, n + 1)))
#define strftimeesque(n) __attribute__((__format__(__strftime__, n, 0)))
#else
#define printfesque(n)
#define scanfesque(n)
#define strftimeesque(n)
#endif

#ifndef _Hide
#if !defined(__STRICT_ANSI__) && \
    (__has_attribute(__visibility__) || defined(__GNUC__)) && !defined(_WIN32)
#define _Hide __attribute__((__visibility__("hidden")))
#else
#define _Hide
#endif
#endif

#ifndef privileged
#if !defined(__STRICT_ANSI__) && \
    (__has_attribute(__visibility__) || defined(__GNUC__))
#define privileged _Section(".privileged")
#else
#define privileged _Section(".privileged")
#endif
#endif

#ifndef noinstrument
#if !defined(__STRICT_ANSI__) &&                    \
    (__has_attribute(__no_instrument_function__) || \
     (__GNUC__ + 0) * 100 + (__GNUC_MINOR__ + 0) >= 204)
#define noinstrument __attribute__((__no_instrument_function__))
#else
#define noinstrument
#endif
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

#ifndef noclone
#if !defined(__STRICT_ANSI__) &&     \
    (__has_attribute(__noclone__) || \
     (__GNUC__ + 0) * 100 + (__GNUC_MINOR__ + 0) >= 405)
#define noclone __attribute__((__noclone__))
#else
#define noclone
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

#ifndef mayalias
#if !defined(__STRICT_ANSI__) &&       \
    (__has_attribute(__may_alias__) || \
     (__GNUC__ + 0) * 100 + (__GNUC_MINOR__ + 0) >= 303)
#define mayalias __attribute__((__may_alias__))
#else
#define mayalias
#endif
#endif

#ifndef dontdiscard
#if !defined(__STRICT_ANSI__) &&                           \
    ((__GNUC__ + 0) * 100 + (__GNUC_MINOR__ + 0) >= 304 || \
     __has_attribute(__warn_unused_result__))
#define dontdiscard __attribute__((__warn_unused_result__))
#else
#define dontdiscard
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

#ifndef nocallback
#if !defined(__STRICT_ANSI__) &&  \
    (__has_attribute(__leaf__) || \
     (!defined(__llvm__) &&       \
      (__GNUC__ + 0) * 100 + (__GNUC_MINOR__ + 0) >= 406))
#define nocallback __attribute__((__leaf__))
#else
#define nocallback
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

#ifndef nooptimize
#ifndef __STRICT_ANSI__
#if (__GNUC__ + 0) * 100 + (__GNUC_MINOR__ + 0) >= 407 || \
    __has_attribute(__optimize__)
#define nooptimize __attribute__((__optimize__(1)))
#elif defined(__llvm__) || __has_attribute(__optnone__)
#define nooptimize __attribute__((__optnone__))
#endif
#else
#define nooptimize
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

#ifndef artificial
#if !defined(__STRICT_ANSI__) &&        \
    (__has_attribute(__artificial__) || \
     (__GNUC__ + 0) * 100 + (__GNUC_MINOR__ + 0) >= 403)
#define artificial __attribute__((__artificial__))
#else
#define artificial
#endif
#endif

#ifndef microarchitecture
#if !defined(__STRICT_ANSI__) &&    \
    (__has_attribute(__target__) || \
     (__GNUC__ + 0) * 100 + (__GNUC_MINOR__ + 0) >= 404)
#define microarchitecture(march) __attribute__((__target__(march)))
#else
#define microarchitecture(march)
#endif
#endif

#ifndef targetclones
#if !defined(__STRICT_ANSI__) && \
    (__has_attribute(__target_clones__) || __GNUC__ >= 6)
#define targetclones(archs) __attribute__((__target_clones__(archs)))
#else
#define targetclones(archs)
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
#elif ((__has_builtin(auto_type) || defined(__llvm__) ||       \
        (__GNUC__ + 0) * 100 + (__GNUC_MINOR__ + 0) >= 409) && \
       !defined(__chibicc__))
#define autotype(x) __auto_type
#else
#define autotype(x) typeof(x)
#endif

#if __GNUC__ >= 7 || __has_attribute(__no_caller_saved_registers__)
#define nocallersavedregisters __attribute__((__no_caller_saved_registers__))
#else
#define nocallersavedregisters "need modern compiler"
#endif

#if (__GNUC__ + 0) * 100 + (__GNUC_MINOR__ + 0) >= 408 || \
    __has_attribute(__no_sanitize_address__)
#define noasan __attribute__((__no_sanitize_address__))
#else
#define noasan
#endif

#if (__GNUC__ + 0) * 100 + (__GNUC_MINOR__ + 0) >= 408 || \
    __has_attribute(__no_sanitize_undefined__)
#define noubsan __attribute__((__no_sanitize_undefined__))
#else
#define noubsan
#endif

#ifndef unreachable
#if defined(__GNUC__) && !defined(__STRICT_ANSI__)
#define unreachable __builtin_unreachable()
#else
#define unreachable \
  do {              \
  } while (1)
#endif
#endif

#define notpossible           \
  do {                        \
    asm("nop\n\tud2\n\tnop"); \
    unreachable;              \
  } while (0)

#define donothing \
  do {            \
  } while (0)

#ifndef __STRICT_ANSI__
#define textstartup _Section(".text.startup") noinstrument
#define textexit    _Section(".text.exit") noinstrument
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

#ifndef compatfn
#define compatfn
#endif

#ifndef frownedupon
#define frownedupon(alternative)
#endif

#if defined(__GNUC__) && !defined(__STRICT_ANSI__)
#define _Vector_size(k) __attribute__((__vector_size__(k)))
#else
#define _Vector_size(k) [k]
#endif

#if defined(__STRICT_ANSI__) || \
    (!defined(__GNUC__) && !defined(__builtin_offsetof))
#define offsetof(type, member) ((unsigned long)&((type *)0)->member)
#else
#define offsetof(type, member) __builtin_offsetof(type, member)
#endif

#ifdef __llvm__
#define __builtin_ia32_movntdq(x, y) (*(x) = (y))
#endif

#ifndef _Section
#if !defined(__STRICT_ANSI__) && !defined(__APPLE__)
#define _Section(s) __attribute__((__section__(s)))
#else
#define _Section(s)
#endif
#endif

#ifndef __llvm__
#define initarray _Section(".init_array,\"a\",@init_array #")
#else
#define initarray _Section(".init_array")
#endif

#ifndef __STRICT_ANSI__
#if defined(__GNUC__) || defined(__llvm__)
#pragma GCC diagnostic ignored \
    "-Wundef" /* complaints about __ASSEMBLER__/__LINKER__  */
#pragma GCC diagnostic ignored "-Wsign-compare"     /* lint needs to change  */
#pragma GCC diagnostic ignored "-Wtype-limits"      /* makes macros unsafe  */
#pragma GCC diagnostic ignored "-Woverflow"         /* also breaks macros  */
#pragma GCC diagnostic ignored "-Wformat"           /* forces only gnu pf */
#pragma GCC diagnostic ignored "-Wunused-parameter" /* extreme prejudice */
#pragma GCC diagnostic ignored "-Wunused-function"  /* contradicts dce! */
#pragma GCC diagnostic ignored "-Wunused-const-variable"   /* let me dce */
#pragma GCC diagnostic ignored "-Wunused-variable"         /* belongs in tidy */
#pragma GCC diagnostic ignored "-Wformat-extra-args"       /* is also broken */
#pragma GCC diagnostic ignored "-Wparentheses"             /* annoying tidy */
#pragma GCC diagnostic ignored "-Wdangling-else"           /* come on tidy */
#pragma GCC diagnostic ignored "-Wformat-security"         /* come on tidy */
#pragma GCC diagnostic ignored "-Wunused-value"            /* breaks macros */
#pragma GCC diagnostic ignored "-Wdeprecated-declarations" /* libcxx */
#ifndef __cplusplus
#pragma GCC diagnostic ignored "-Wimplicit-int"
#pragma GCC diagnostic ignored "-Wc++-compat"
#endif /* C++ */
#endif /* GCC || LLVM */
#if defined(__GNUC__) && !defined(__llvm__)
/* why we need authorization to name a variable `yn' */
#pragma GCC diagnostic ignored "-Wbuiltin-declaration-mismatch"
#pragma GCC diagnostic ignored "-Wfree-nonheap-object" /* broken #54202 */
#ifndef __cplusplus
#if __GNUC__ >= 6
#pragma GCC diagnostic ignored /* wut */ "-Wdiscarded-qualifiers"
#pragma GCC diagnostic ignored /* tidy */ "-Wunused-but-set-variable"
#pragma GCC diagnostic ignored /* tidy */ "-Wunused-but-set-parameter"
#endif /* GCC6+ */
#if __GNUC__ >= 8
#pragma GCC diagnostic ignored "-Wstringop-truncation"
#pragma GCC diagnostic ignored "-Wstringop-overflow" /* breaks strndup */
#endif                                               /* GCC8+ */
#if __GNUC__ + 0 >= 9
#pragma GCC diagnostic ignored /* "always true" breaks dce */ "-Waddress"
#endif /* GCC9+ */
#endif /* !C++ */
#endif /* GCC && !LLVM */
#ifdef __llvm__
#pragma clang diagnostic ignored \
    "-Wincompatible-pointer-types-discards-qualifiers"
#pragma clang diagnostic ignored "-Wbuiltin-requires-header"
#pragma clang diagnostic ignored "-Wparentheses-equality" /*-save-temps*/
#pragma clang diagnostic ignored "-Wunused-value"         /*({-save-temps})*/
#pragma clang diagnostic ignored "-Wstring-plus-int"      /* special ed */
#pragma clang diagnostic ignored "-Wunused-value" /* extreme prejudice */
#pragma clang diagnostic ignored "-Wbuiltin-requires-header"
#pragma clang diagnostic ignored \
    "-Wincompatible-pointer-types-discards-qualifiers"
#endif /* !GCC && LLVM */
#endif /* ANSI */

#ifndef __W__
#ifndef __STRICT_ANSI__
#if defined(__GNUC__) || defined(__llvm__)
#pragma GCC diagnostic error "-Wpointer-arith"
#pragma GCC diagnostic error "-Wnonnull"
#pragma GCC diagnostic error "-Wunused-result"
#pragma GCC diagnostic error "-Wstrict-aliasing"
#pragma GCC diagnostic error "-Wshift-negative-value"
#ifndef __cplusplus
#pragma GCC diagnostic error "-Wimplicit-function-declaration"
#if __GNUC__ >= 6
#pragma GCC diagnostic error "-Wincompatible-pointer-types"
#if __GNUC__ >= 8
#pragma GCC diagnostic error "-Wmultistatement-macros"
#pragma GCC diagnostic error "-Wpacked-not-aligned"
#pragma GCC diagnostic error "-Wcast-align=strict"
#pragma GCC diagnostic error "-Wif-not-aligned"
#endif /* GCC 8+ */
#endif /* GCC 6+ */
#endif /* __cplusplus */
#endif /* GCC || LLVM */
#if defined(__GNUC__) && !defined(__llvm__)
#pragma GCC diagnostic error "-Wwrite-strings"
#pragma GCC diagnostic error "-Wtrampolines"
#if __GNUC__ >= 6
#pragma GCC diagnostic error "-Wnonnull-compare"
#if defined(COSMO) && !defined(MODE_DBG) && !defined(STACK_FRAME_UNLIMITED)
#pragma GCC diagnostic error "-Wframe-larger-than=16384"
#if __GNUC__ >= 9
#pragma GCC diagnostic error "-Walloca-larger-than=1024"
#pragma GCC diagnostic error "-Wvla-larger-than=1024"
#endif /* GCC 9+ */
#endif /* STACK_FRAME_UNLIMITED */
#elif __GNUC__ >= 9
#pragma GCC diagnostic error /* e.g. fabs not abs */ "-Wabsolute-value"
#endif /* GCC 6+ */
#endif /* GCC && !LLVM */
#ifdef __llvm__
#pragma clang diagnostic error "-Wassume"
#endif /* !GCC && LLVM */
#endif /* ANSI */
#endif /* -w */

#ifndef __STRICT_ANSI__
#define DebugBreak() asm("int3")
#else
#define DebugBreak() (void)0
#endif

#ifndef __STRICT_ANSI__
#define VEIL(CONSTRAINT, EXPRESSION)                             \
  ({                                                             \
    autotype(EXPRESSION) VeiledValue = (EXPRESSION);             \
    asm("" : "=" CONSTRAINT ""(VeiledValue) : "0"(VeiledValue)); \
    VeiledValue;                                                 \
  })
#else
#define VEIL(CONSTRAINT, EXPRESSION) (EXPRESSION)
#endif

#ifndef __STRICT_ANSI__
#define CONCEAL(CONSTRAINT, EXPRESSION)                                   \
  ({                                                                      \
    autotype(EXPRESSION) VeiledValue = (EXPRESSION);                      \
    asm volatile("" : "=" CONSTRAINT ""(VeiledValue) : "0"(VeiledValue)); \
    VeiledValue;                                                          \
  })
#else
#define CONCEAL(CONSTRAINT, EXPRESSION) (EXPRESSION)
#endif

#ifndef __STRICT_ANSI__
#define EXPROPRIATE(EXPRESSION)                    \
  ({                                               \
    asm volatile("" ::"g"(EXPRESSION) : "memory"); \
    0;                                             \
  })
#else
#define EXPROPRIATE(EXPRESSION) (EXPRESSION)
#endif

#if !defined(__STRICT_ANSI__) && !defined(__APPLE__)
#define YOINK(SYMBOL) \
  asm(".section .yoink\n\tnopl\t%a0\n\t.previous" : : "X"(SYMBOL))
#else
#define YOINK(SYMBOL) (void)0
#endif

#if !defined(__STRICT_ANSI__) && !defined(__APPLE__)
#define STATIC_YOINK(SYMBOLSTR) \
  asm(".section .yoink\n\tnopl\t\"" SYMBOLSTR "\"\n\t.previous")
#else
#define STATIC_YOINK(SYMBOLSTR)
#endif

#if !defined(IM_FEELING_NAUGHTY)
#define STATIC_YOINK_SOURCE(PATH) STATIC_YOINK(PATH)
#else
#define STATIC_YOINK_SOURCE(PATH)
#endif

#define MACHINE_CODE_ANALYSIS_BEGIN_
#define MACHINE_CODE_ANALYSIS_END_
