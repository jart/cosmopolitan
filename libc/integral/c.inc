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

#ifdef __STRICT_ANSI__
#define asm __asm__
#endif

#ifndef __ia16__
#define __far
#endif

/**
 * Gets type of expression.
 * @see autotype() which is a better alternative for certain use cases
 */
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
#define alignof(x) _Alignof(x)
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

#if defined(__STRICT_ANSI__) || \
    (!defined(__GNUC__) && !__has_builtin(unreachable))
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
#define ____Static_assert(x, y) A##B
#define ___Static_assert(x, y)  ____Static_assert(x, y)
#ifndef __cplusplus /* todo jart what */
#define __Static_assert(x) __builtin_choose_expr(__builtin_constant_p(x), x, 1)
#else
#define __Static_assert(x) (x)
#endif
#define _Static_assert(x, s) /* clang-format off */ do { \
  __builtin_assume(x);                                   \
  enum { ___Static_assert(_Assert, __COUNTER__) =        \
         1 / !!__Static_assert(x)                        \
  } /*_Unused*/; /* clang-format on */ \
  } while (0)
#endif

#if __STDC_VERSION__ + 0 < 201112 && defined(__x86__)
#define _Atomic(TYPE) TYPE
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

/**
 * AX:DX register pair.
 *
 * Every ABI we support permits functions to return two machine words.
 * Normally it's best to define a one-off struct. Sometimes we don't
 * want the boilerplate.
 *
 * @see System V Application Binary Interface NexGen32e Architecture
 *     Processor Supplement, Version 1.0, December 5th, 2018
 * @see agner.org/optimize/calling_conventions.pdf (chapter 6)
 * @see LISP primitives CONS[CAR,CDR] w/ IBM 704 naming
 * @see int128_t
 */
typedef struct {
  intptr_t ax, dx;
} axdx_t;

/*
 * GCC, Clang, and System V ABI all incorrectly define intmax_t.
 *
 *   “[intmax_t] designates a signed integer type capable of
 *    representing any value of any signed integer type.”
 *    ──Quoth ISO/IEC 9899:201x 7.20.1.5
 *
 * This surprising contradiction is most likely due to Linux distro
 * practices of using dynamic shared objects which needs to change.
 *
 * http://www.open-std.org/jtc1/sc22/wg14/www/docs/n2303.pdf
 */
#ifdef __SIZEOF_INTMAX__
#undef __SIZEOF_INTMAX__
#endif
#if !defined(__STRICT_ANSI__) && __SIZEOF_POINTER__ == 8 && \
    ((__GNUC__ + 0) * 100 + (__GNUC_MINOR__ + 0) >= 406 || defined(__llvm__))
#define __SIZEOF_INTMAX__ 16
#else
#define __SIZEOF_INTMAX__ __SIZEOF_POINTER__
#endif
#if __SIZEOF_INTMAX__ == 16
typedef signed __int128 int128_t;
typedef unsigned __int128 uint128_t;
typedef int128_t intmax_t;
typedef uint128_t uintmax_t;
#elif __SIZEOF_INTMAX__ == 8
typedef int64_t intmax_t;
typedef uint64_t uintmax_t;
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

#define libcesque   nothrow nocallback
#define memcpyesque libcesque
#define strlenesque libcesque nosideeffect paramsnonnull()
#define vallocesque \
  libcesque nodiscard returnsaligned((PAGESIZE)) returnspointerwithnoaliases
#define reallocesque libcesque returnsaligned((__BIGGEST_ALIGNMENT__))
#define mallocesque  reallocesque returnspointerwithnoaliases
#define interruptfn  nocallersavedregisters forcealignargpointer

/**
 * Declares combinator, i.e. never reads/writes global memory.
 * Thus enabling LICM, CSE, DCE, etc. optimizations.
 * @see nosideeffect
 */
#ifndef pureconst
#ifndef __STRICT_ANSI__
#define pureconst __attribute__((__const__))
#else
#define pureconst
#endif
#endif

/**
 * Aligns automatic or static variable.
 */
#ifndef forcealign
#ifndef __STRICT_ANSI__
#define forcealign(bytes) __attribute__((__aligned__(bytes)))
#else
#define forcealign(bytes)
#endif
#endif

/**
 * Disables alignment.
 * @param opt_bytes defaults to __BIGGEST_ALIGNMENT__
 * @see nosideeffect
 */
#ifndef __STRICT_ANSI__
#define thatispacked __attribute__((__packed__))
#else
#define thatispacked
#endif

/**
 * Declares prototype as using well-known format string DSL.
 * Thereby allowing compiler to identify certain bugs.
 */
#ifndef __STRICT_ANSI__
#define printfesque(n)   __attribute__((__format__(__gnu_printf__, n, n + 1)))
#define scanfesque(n)    __attribute__((__format__(__gnu_scanf__, n, n + 1)))
#define strftimeesque(n) __attribute__((__format__(__strftime__, n, 0)))
#else
#define printfesque(n)
#define scanfesque(n)
#define strftimeesque(n)
#endif

#ifndef hidden
#if !defined(__STRICT_ANSI__) && \
    (__has_attribute(__visibility__) || defined(__GNUC__)) && !defined(_WIN32)
#define hidden __attribute__((__visibility__("hidden")))
#else
#define hidden
#endif
#endif

#ifndef privileged
#if !defined(__STRICT_ANSI__) && \
    (__has_attribute(__visibility__) || defined(__GNUC__))
#define privileged _Section(".privileged") noinstrument
#else
#define privileged _Section(".privileged") noinstrument
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

/**
 * Declares prototype as never mutating global memory.
 * Thus enabling CSE, DCE, LICM [clang-only?], etc. optimizations.
 * @see pureconst
 */
#ifndef nosideeffect
#if !defined(__STRICT_ANSI__) &&  \
    (__has_attribute(__pure__) || \
     (__GNUC__ + 0) * 100 + (__GNUC_MINOR__ + 0) >= 296)
#define nosideeffect __attribute__((__pure__))
#else
#define nosideeffect
#endif
#endif

#ifndef noinline
#if !defined(__STRICT_ANSI__) &&      \
    (__has_attribute(__noinline__) || \
     (__GNUC__ + 0) * 100 + (__GNUC_MINOR__ + 0) >= 301)
#define noinline __attribute__((__noinline__))
#else
#define noinline
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

/**
 * Makes function behave as much like macro as possible, meaning:
 *
 *   1. always inlined, i.e. even with -fno-inline
 *   2. unlinkable, i.e. elf data is not generated
 *   3. steppable, i.e. dwarf data is generated
 *   4. unprofilable
 *   5. unhookable
 *
 * @note consider static or writing a macro
 * @see externinline
 */
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
#define forceinline              \
  static __inline __attribute__( \
      (__always_inline__, __no_instrument_function__, __unused__))
#endif /* __GNUC_STDC_INLINE__ */
#endif /* GCC >= 4.3 */
#elif defined(_MSC_VER)
#define forceinline __forceinline
#else
#define forceinline static inline
#endif /* !ANSI && GCC >= 3.2 */
#endif /* __cplusplus */
#endif /* forceinline */

/**
 * Permits untyped or punned memory manipulation w/o asm.
 *
 *   “The fundamental problem is that it is not possible to write real
 *    programs using the X3J11 definition of C. The committee has created
 *    an unreal language that no one can or will actually use. While the
 *    problems of `const' may owe to careless drafting of the
 *    specification, `noalias' is an altogether mistaken notion, and must
 *    not survive.” ──Dennis Ritchie in 1988-03-20.
 *
 * @see asm(), memcpy(), memset(), read32be(), etc.
 * @see unsigned char
 */
#ifndef mayalias
#if !defined(__STRICT_ANSI__) &&       \
    (__has_attribute(__may_alias__) || \
     (__GNUC__ + 0) * 100 + (__GNUC_MINOR__ + 0) >= 303)
#define mayalias __attribute__((__may_alias__))
#else
#define mayalias
#endif
#endif

/**
 * Declares prototype as returning freeable resource.
 * Compilation will fail if caller ignores return value.
 * @see gc(), free(), close(), etc.
 */
#ifndef nodiscard
#if !defined(__STRICT_ANSI__) &&                           \
    ((__GNUC__ + 0) * 100 + (__GNUC_MINOR__ + 0) >= 304 || \
     __has_attribute(__warn_unused_result__))
#define nodiscard __attribute__((__warn_unused_result__))
#else
#define nodiscard
#endif
#endif

/**
 * Declares variadic function as needing NULL sentinel argument.
 * @see execl() for example
 */
#ifndef nullterminated
#if !defined(__STRICT_ANSI__) && \
    (__has_attribute(__sentinel__) || __GNUC__ >= 4)
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

/**
 * Relocates function to .text.unlikely section of binary.
 * @note can be used to minimize page-faults and improve locality
 */
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

/**
 * Relocates function to .text.hot section of binary.
 * @note can be used to minimize page-faults w/ improved locality
 * @note most appropriately automated by profile-guided opts
 */
#ifndef firstclass
#if !defined(__STRICT_ANSI__) && \
    (__has_attribute(__hot__) || \
     (__GNUC__ + 0) * 100 + (__GNUC_MINOR__ + 0) >= 403)
#define firstclass __attribute__((__hot__))
#else
#define firstclass
#endif
#endif

/**
 * Declares all or specific parameters as never receiving NULL.
 *
 * This can be checked at both compile-time (only for constexprs) and
 * runtime too (only in MODE=dbg mode) by synthetic Ubsan code.
 */
#ifndef paramsnonnull
#if !defined(__STRICT_ANSI__) &&     \
    (__has_attribute(__nonnull__) || \
     (__GNUC__ + 0) * 100 + (__GNUC_MINOR__ + 0) >= 403)
#define paramsnonnull(opt_1idxs) __attribute__((__nonnull__ opt_1idxs))
#else
#define paramsnonnull(opt_1idxs)
#endif
#endif

/**
 * Declares array argument w/ minimum size contract, e.g.
 *
 *   int foo(int bar[hasatleast 2]) { ... }
 */
#if __STDC_VERSION__ + 0 >= 199901L
#define hasatleast static
#else
#define hasatleast
#endif

/**
 * Qualifies char pointer so it's treated like every other type.
 *
 *   int foo(int bar[hasatleast 2]) { ... }
 */
#if __STDC_VERSION__ + 0 < 199901L && !defined(restrict)
#if !defined(__STRICT_ANSI__) && !defined(__cplusplus) && \
    ((__GNUC__ + 0) * 100 + (__GNUC_MINOR__ + 0) >= 301 || defined(_MSC_VER))
#define restrict __restrict__
#else
#define restrict
#define __restrict
#endif
#endif

/**
 * Declares prototype that can't mutate caller's static variables.
 * @note consider more .c files or declare in function
 */
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

#ifndef nothrow
#if defined(__cplusplus) && !defined(__STRICT_ANSI__) && \
    (__has_attribute(nothrow) ||                         \
     (__GNUC__ + 0) * 100 + (__GNUC_MINOR__ + 0) >= 303)
#define nothrow __attribute__((__nothrow__))
#elif defined(_MSC_VER)
#define nothrow __declspec(nothrow)
#else
#define nothrow
#endif
#endif

/**
 * Asks compiler to not optimize function definition.
 *
 * @todo this is dangerous delete?
 */
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

/**
 * Asks compiler to generate as little code as possible for function.
 *
 * This does the same thing as relegated, but without relocation.
 *
 * @todo this is dangerous delete?
 */
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

/**
 * Asks compiler to always heavily optimize function.
 *
 * This keyword provides an alternative to build flag tuning, in cases
 * where the compiler is reluctant to vectorize mathematical code that's
 * written in standards-compliant C rather than GCC extensions.
 *
 * @todo this is dangerous delete?
 */
#ifndef optimizespeed
#if !defined(__STRICT_ANSI__) &&                           \
    ((__GNUC__ + 0) * 100 + (__GNUC_MINOR__ + 0) >= 407 || \
     __has_attribute(__optimize__))
#define optimizespeed __attribute__((__optimize__(3)))
#else
#define optimizespeed
#endif
#endif

/**
 * Declares prototype that behaves similar to setjmp() or vfork().
 */
#ifndef returnstwice
#if !defined(__STRICT_ANSI__) &&           \
    (__has_attribute(__returns_twice__) || \
     (__GNUC__ + 0) * 100 + (__GNUC_MINOR__ + 0) >= 402)
#define returnstwice __attribute__((__returns_twice__))
#else
#define returnstwice
#endif
#endif

/**
 * Asks compiler to not emit DWARF assembly for function.
 * @see artificial
 */
#ifndef nodebuginfo
#if !defined(__STRICT_ANSI__) && \
    (__has_attribute(__nodebug__) || defined(__llvm__))
#define nodebuginfo __attribute__((__nodebug__))
#else
#define nodebuginfo
#endif
#endif

/**
 * Associates debug information with call site.
 * @see nodebuginfo
 */
#ifndef artificial
#if !defined(__STRICT_ANSI__) &&        \
    (__has_attribute(__artificial__) || \
     (__GNUC__ + 0) * 100 + (__GNUC_MINOR__ + 0) >= 403)
#define artificial __attribute__((__artificial__))
#else
#define artificial
#endif
#endif

/**
 * Defines function as specially compiled for newer cpu model.
 * @see -ftree-vectorize and consider assembly
 * @see libc/dce.h
 */
#ifndef microarchitecture
#if !defined(__STRICT_ANSI__) &&    \
    (__has_attribute(__target__) || \
     (__GNUC__ + 0) * 100 + (__GNUC_MINOR__ + 0) >= 404)
#define microarchitecture(march) __attribute__((__target__(march)))
#else
#define microarchitecture(march)
#endif
#endif

/**
 * Compiles function multiple times for different cpu models.
 * @see libc/dce.h
 */
#ifndef targetclones
#if !defined(__STRICT_ANSI__) && \
    (__has_attribute(__target_clones__) || __GNUC__ >= 6)
#define targetclones(archs) __attribute__((__target_clones__(archs)))
#else
#define targetclones(archs)
#endif
#endif

/**
 * Defines function with prologue that fixes misaligned stack.
 * @see nocallersavedregisters and consider assembly
 */
#if (__GNUC__ + 0) * 100 + (__GNUC_MINOR__ + 0) >= 408 || \
    __has_attribute(__force_align_arg_pointer__)
#define forcealignargpointer __attribute__((__force_align_arg_pointer__))
#else
#define forcealignargpointer "need modern compiler"
#endif

/**
 * Declares prototype as never returning NULL.
 *
 * This is checked at compile-time for constexprs. It'll be checked at
 * runtime too by synthetic code, only in MODE=dbg mode.
 */
#ifndef returnsnonnull
#if !defined(__STRICT_ANSI__) &&             \
    (__has_attribute(__returns_nonnull__) || \
     (__GNUC__ + 0) * 100 + (__GNUC_MINOR__ + 0) >= 409)
#define returnsnonnull __attribute__((__returns_nonnull__))
#else
#define returnsnonnull
#endif
#endif

/**
 * Attests return value is aligned.
 *
 * @param (alignment)
 * @param (alignment, misalignment)
 * @see attributeallocalign(), returnspointerwithnoaliases, mallocesque
 */
#if !defined(__STRICT_ANSI__) &&            \
    (__has_attribute(__assume_aligned__) || \
     (__GNUC__ + 0) * 100 + (__GNUC_MINOR__ + 0) >= 409)
#define returnsaligned(x) __attribute__((__assume_aligned__ x))
#else
#define returnsaligned(x)
#endif

/**
 * Declares prototype as behaving similar to malloc().
 * @see attributeallocsize(), attributeallocalign()
 */
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

/**
 * Defines variable as having same type as right-hand side.
 *
 * This enables safe, backwards-compatible, non-explosive macros, e.g.:
 *
 *   #define bar(FOO)                \
 *     ({                            \
 *       autotype(FOO) Foo = (FOO);  \
 *       Foo + Foo * 2;              \
 *     })
 *
 * @param x must be identical to rhs
 * @note typeof goes back to gcc 2.x
 */
#if __cplusplus + 0 >= 201103L
#define autotype(x) auto
#elif ((__has_builtin(auto_type) || defined(__llvm__) ||       \
        (__GNUC__ + 0) * 100 + (__GNUC_MINOR__ + 0) >= 409) && \
       !defined(__chibicc__))
#define autotype(x) __auto_type
#else
#define autotype(x) typeof(x)
#endif

/**
 * Defines interrupt handler that can call non-interrupt functions.
 * @see forcealignargpointer, -mgeneral-regs-only and consider assembly
 */
#if __GNUC__ >= 7 || __has_attribute(__no_caller_saved_registers__)
#define nocallersavedregisters __attribute__((__no_caller_saved_registers__))
#else
#define nocallersavedregisters "need modern compiler"
#endif

/**
 * Attests that execution of statement is impossible.
 */
#ifndef unreachable
#define unreachable __builtin_unreachable()
#endif

/**
 * Statement that does nothing.
 * @note can help avoid drama w/ linters, warnings, formatters, etc.
 */
#define donothing \
  do {            \
  } while (0)

#ifndef likely
#define likely(expr) __builtin_expect(!!(expr), 1)
#endif

#ifndef unlikely
#define unlikely(expr) __builtin_expect(!!(expr), 0)
#endif

/**
 * Evaluates ternary expression without type promotion.
 */
#ifndef chooseexpr
#define chooseexpr(pred, a, b) __builtin_choose_expr(pred, a, b)
#endif

/**
 * Returns true if expression can be evaluated at compile-time.
 */
#ifndef isconstant
#define isconstant(expr) __builtin_constant_p(expr)
#endif

#ifndef static_assert
#define static_assert(expr) _Static_assert(expr, #expr)
#endif

#ifndef typescompatible
#define typescompatible(a, b) __builtin_types_compatible_p(a, b)
#endif

#ifndef __STRICT_ANSI__
#define testonly    noinline _Section(".test")
#define textstartup _Section(".text.startup") noinstrument
#define textexit    _Section(".text.exit") noinstrument
#define textreal    _Section(".text.real")
#define textwindows _Section(".text.windows")
#define antiquity   _Section(".text.antiquity")
#else
#define testonly
#define textstartup
#define textexit
#define textreal
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

#ifndef alignas
#define alignas(x) _Alignas(x)
#endif

#ifndef _Section
#ifndef __STRICT_ANSI__
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

/**
 * Systemic suppressions.
 */
#ifndef __STRICT_ANSI__
#if defined(__GNUC__) || defined(__llvm__)
#pragma GCC diagnostic ignored "-Wsign-compare"      /* lint needs to change  */
#pragma GCC diagnostic ignored "-Wtype-limits"       /* makes macros unsafe  */
#pragma GCC diagnostic ignored "-Woverflow"          /* also breaks macros  */
#pragma GCC diagnostic ignored "-Wformat"            /* forces only gnu pf */
#pragma GCC diagnostic ignored "-Wunused-parameter"  /* extreme prejudice */
#pragma GCC diagnostic ignored "-Wunused-function"   /* contradicts dce! */
#pragma GCC diagnostic ignored "-Wunused-variable"   /* belongs in tidy */
#pragma GCC diagnostic ignored "-Wformat-extra-args" /* is also broken */
#pragma GCC diagnostic ignored "-Wparentheses"       /* annoying tidy */
#pragma GCC diagnostic ignored "-Wdangling-else"     /* come on tidy */
#ifndef __cplusplus
#pragma GCC diagnostic ignored "-Wimplicit-int"
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

/**
 * Elevate warnings of material consequence.
 *
 * These aren't stylistic in nature; but are perfectly fine to disable,
 * assuming we're ok with the compiler simply generating a runtime crash
 * instead. Otherwise what usually happens with these is that a weakness
 * is introduced, important optimizations can't be performed; or worst
 * of all: the code will need patching if ported to a toy or any machine
 * designed by an engineer who hadn't understood John von Neumann at the
 * time, e.g. 1's complement, big endian, under 32bit word size, etc.
 */
#ifndef __W__
#ifndef __STRICT_ANSI__
#if defined(__GNUC__) || defined(__llvm__)
#pragma GCC diagnostic error "-Wpointer-arith"
#pragma GCC diagnostic error "-Wnonnull"
#pragma GCC diagnostic error "-Wunused-result"
#pragma GCC diagnostic error "-Wuninitialized"
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
#pragma GCC diagnostic error "-Wmaybe-uninitialized"
#pragma GCC diagnostic error "-Wredundant-decls"
#if __GNUC__ >= 6
#pragma GCC diagnostic error "-Wnonnull-compare"
#if !defined(MODE_DBG) && !defined(STACK_FRAME_UNLIMITED)
#pragma GCC diagnostic error "-Wframe-larger-than=4096"
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

/**
 * Sets manual breakpoint.
 * @see showcrashreports() for auto gdb attach
 */
#define DebugBreak() asm("int3")

#define VEIL(CONSTRAINT, EXPRESSION)                             \
  ({                                                             \
    autotype(EXPRESSION) VeiledValue = (EXPRESSION);             \
    asm("" : "=" CONSTRAINT ""(VeiledValue) : "0"(VeiledValue)); \
    VeiledValue;                                                 \
  })

#define CONCEAL(CONSTRAINT, EXPRESSION)                                   \
  ({                                                                      \
    autotype(EXPRESSION) VeiledValue = (EXPRESSION);                      \
    asm volatile("" : "=" CONSTRAINT ""(VeiledValue) : "0"(VeiledValue)); \
    VeiledValue;                                                          \
  })

#define EXPROPRIATE(EXPRESSION)                    \
  ({                                               \
    asm volatile("" ::"g"(EXPRESSION) : "memory"); \
    0;                                             \
  })

/**
 * Pulls another module, by symbol, into linkage.
 * @note nop is discarded by ape/ape.lds
 */
#define YOINK(SYMBOL)                                              \
  do {                                                             \
    _Static_assert(!typescompatible(typeof(SYMBOL), char[]),       \
                   "Please YOINK(symbol), not YOINK(\"symbol\")"); \
    asm(".pushsection .yoink\n\t"                                  \
        "nop\t%a0\n\t"                                             \
        ".popsection"                                              \
        : /* no outputs */                                         \
        : "X"(SYMBOL));                                            \
  } while (0)

/**
 * Pulls another module into linkage from top-level scope.
 * @note nop is discarded by ape/ape.lds
 */
#define STATIC_YOINK(SYMBOLSTR) \
  asm(".pushsection .yoink\n\tnop\t\"" SYMBOLSTR "\"\n\t.popsection")

/**
 * Pulls source file into ZIP portion of binary.
 * @see build/rules.mk which defines the wildcard build rule %.zip.o
 */
#if !defined(IM_FEELING_NAUGHTY) && !defined(__STRICT_ANSI__)
#define STATIC_YOINK_SOURCE(PATH) STATIC_YOINK(PATH)
#else
#define STATIC_YOINK_SOURCE(PATH)
#endif

/**
 * Pulls source of object being compiled into zip.
 * @note automates most compliance with gpl terms
 * @see libc/zipos/zipcentraldir.S
 * @see ape/ape.lds
 */
#ifdef __BASE_FILE__
STATIC_YOINK_SOURCE(__BASE_FILE__);
#endif

#define MACHINE_CODE_ANALYSIS_BEGIN_
#define MACHINE_CODE_ANALYSIS_END_
