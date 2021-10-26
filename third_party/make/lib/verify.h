/* Compile-time assert-like macros.

   Copyright (C) 2005-2006, 2009-2020 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

/* Written by Paul Eggert, Bruno Haible, and Jim Meyering.  */

#ifndef _GL_VERIFY_H
#define _GL_VERIFY_H


/* Define _GL_HAVE__STATIC_ASSERT to 1 if _Static_assert (R, DIAGNOSTIC)
   works as per C11.  This is supported by GCC 4.6.0 and later, in C
   mode.

   Define _GL_HAVE__STATIC_ASSERT1 to 1 if _Static_assert (R) works as
   per C2X, and define _GL_HAVE_STATIC_ASSERT1 if static_assert (R)
   works as per C++17.  This is supported by GCC 9.1 and later.

   Support compilers claiming conformance to the relevant standard,
   and also support GCC when not pedantic.  If we were willing to slow
   'configure' down we could also use it with other compilers, but
   since this affects only the quality of diagnostics, why bother?  */
#ifndef __cplusplus
# if (201112L <= __STDC_VERSION__ \
      || (!defined __STRICT_ANSI__ && 4 < __GNUC__ + (6 <= __GNUC_MINOR__)))
#  define _GL_HAVE__STATIC_ASSERT 1
# endif
# if (202000L <= __STDC_VERSION__ \
      || (!defined __STRICT_ANSI__ && 9 <= __GNUC__))
#  define _GL_HAVE__STATIC_ASSERT1 1
# endif
#else
# if 201703L <= __cplusplus || 9 <= __GNUC__
#  define _GL_HAVE_STATIC_ASSERT1 1
# endif
#endif

/* FreeBSD 9.1 <sys/cdefs.h>, included by <stddef.h> and lots of other
   system headers, defines a conflicting _Static_assert that is no
   better than ours; override it.  */
#ifndef _GL_HAVE__STATIC_ASSERT
# include <stddef.h>
# undef _Static_assert
#endif

/* Each of these macros verifies that its argument R is nonzero.  To
   be portable, R should be an integer constant expression.  Unlike
   assert (R), there is no run-time overhead.

   If _Static_assert works, verify (R) uses it directly.  Similarly,
   _GL_VERIFY_TRUE works by packaging a _Static_assert inside a struct
   that is an operand of sizeof.

   The code below uses several ideas for C++ compilers, and for C
   compilers that do not support _Static_assert:

   * The first step is ((R) ? 1 : -1).  Given an expression R, of
     integral or boolean or floating-point type, this yields an
     expression of integral type, whose value is later verified to be
     constant and nonnegative.

   * Next this expression W is wrapped in a type
     struct _gl_verify_type {
       unsigned int _gl_verify_error_if_negative: W;
     }.
     If W is negative, this yields a compile-time error.  No compiler can
     deal with a bit-field of negative size.

     One might think that an array size check would have the same
     effect, that is, that the type struct { unsigned int dummy[W]; }
     would work as well.  However, inside a function, some compilers
     (such as C++ compilers and GNU C) allow local parameters and
     variables inside array size expressions.  With these compilers,
     an array size check would not properly diagnose this misuse of
     the verify macro:

       void function (int n) { verify (n < 0); }

   * For the verify macro, the struct _gl_verify_type will need to
     somehow be embedded into a declaration.  To be portable, this
     declaration must declare an object, a constant, a function, or a
     typedef name.  If the declared entity uses the type directly,
     such as in

       struct dummy {...};
       typedef struct {...} dummy;
       extern struct {...} *dummy;
       extern void dummy (struct {...} *);
       extern struct {...} *dummy (void);

     two uses of the verify macro would yield colliding declarations
     if the entity names are not disambiguated.  A workaround is to
     attach the current line number to the entity name:

       #define _GL_CONCAT0(x, y) x##y
       #define _GL_CONCAT(x, y) _GL_CONCAT0 (x, y)
       extern struct {...} * _GL_CONCAT (dummy, __LINE__);

     But this has the problem that two invocations of verify from
     within the same macro would collide, since the __LINE__ value
     would be the same for both invocations.  (The GCC __COUNTER__
     macro solves this problem, but is not portable.)

     A solution is to use the sizeof operator.  It yields a number,
     getting rid of the identity of the type.  Declarations like

       extern int dummy [sizeof (struct {...})];
       extern void dummy (int [sizeof (struct {...})]);
       extern int (*dummy (void)) [sizeof (struct {...})];

     can be repeated.

   * Should the implementation use a named struct or an unnamed struct?
     Which of the following alternatives can be used?

       extern int dummy [sizeof (struct {...})];
       extern int dummy [sizeof (struct _gl_verify_type {...})];
       extern void dummy (int [sizeof (struct {...})]);
       extern void dummy (int [sizeof (struct _gl_verify_type {...})]);
       extern int (*dummy (void)) [sizeof (struct {...})];
       extern int (*dummy (void)) [sizeof (struct _gl_verify_type {...})];

     In the second and sixth case, the struct type is exported to the
     outer scope; two such declarations therefore collide.  GCC warns
     about the first, third, and fourth cases.  So the only remaining
     possibility is the fifth case:

       extern int (*dummy (void)) [sizeof (struct {...})];

   * GCC warns about duplicate declarations of the dummy function if
     -Wredundant-decls is used.  GCC 4.3 and later have a builtin
     __COUNTER__ macro that can let us generate unique identifiers for
     each dummy function, to suppress this warning.

   * This implementation exploits the fact that older versions of GCC,
     which do not support _Static_assert, also do not warn about the
     last declaration mentioned above.

   * GCC warns if -Wnested-externs is enabled and 'verify' is used
     within a function body; but inside a function, you can always
     arrange to use verify_expr instead.

   * In C++, any struct definition inside sizeof is invalid.
     Use a template type to work around the problem.  */

/* Concatenate two preprocessor tokens.  */
#define _GL_CONCAT(x, y) _GL_CONCAT0 (x, y)
#define _GL_CONCAT0(x, y) x##y

/* _GL_COUNTER is an integer, preferably one that changes each time we
   use it.  Use __COUNTER__ if it works, falling back on __LINE__
   otherwise.  __LINE__ isn't perfect, but it's better than a
   constant.  */
#if defined __COUNTER__ && __COUNTER__ != __COUNTER__
# define _GL_COUNTER __COUNTER__
#else
# define _GL_COUNTER __LINE__
#endif

/* Generate a symbol with the given prefix, making it unique if
   possible.  */
#define _GL_GENSYM(prefix) _GL_CONCAT (prefix, _GL_COUNTER)

/* Verify requirement R at compile-time, as an integer constant expression
   that returns 1.  If R is false, fail at compile-time, preferably
   with a diagnostic that includes the string-literal DIAGNOSTIC.  */

#define _GL_VERIFY_TRUE(R, DIAGNOSTIC) \
   (!!sizeof (_GL_VERIFY_TYPE (R, DIAGNOSTIC)))

#ifdef __cplusplus
# if !GNULIB_defined_struct__gl_verify_type
template <int w>
  struct _gl_verify_type {
    unsigned int _gl_verify_error_if_negative: w;
  };
#  define GNULIB_defined_struct__gl_verify_type 1
# endif
# define _GL_VERIFY_TYPE(R, DIAGNOSTIC) \
    _gl_verify_type<(R) ? 1 : -1>
#elif defined _GL_HAVE__STATIC_ASSERT
# define _GL_VERIFY_TYPE(R, DIAGNOSTIC) \
    struct {                                   \
      _Static_assert (R, DIAGNOSTIC);          \
      int _gl_dummy;                          \
    }
#else
# define _GL_VERIFY_TYPE(R, DIAGNOSTIC) \
    struct { unsigned int _gl_verify_error_if_negative: (R) ? 1 : -1; }
#endif

/* Verify requirement R at compile-time, as a declaration without a
   trailing ';'.  If R is false, fail at compile-time.

   This macro requires three or more arguments but uses at most the first
   two, so that the _Static_assert macro optionally defined below supports
   both the C11 two-argument syntax and the C2X one-argument syntax.

   Unfortunately, unlike C11, this implementation must appear as an
   ordinary declaration, and cannot appear inside struct { ... }.  */

#if defined _GL_HAVE__STATIC_ASSERT
# define _GL_VERIFY(R, DIAGNOSTIC, ...) _Static_assert (R, DIAGNOSTIC)
#else
# define _GL_VERIFY(R, DIAGNOSTIC, ...)                                \
    extern int (*_GL_GENSYM (_gl_verify_function) (void))	       \
      [_GL_VERIFY_TRUE (R, DIAGNOSTIC)]
#endif

/* _GL_STATIC_ASSERT_H is defined if this code is copied into assert.h.  */
#ifdef _GL_STATIC_ASSERT_H
# if !defined _GL_HAVE__STATIC_ASSERT1 && !defined _Static_assert
#  define _Static_assert(...) \
     _GL_VERIFY (__VA_ARGS__, "static assertion failed", -)
# endif
# if !defined _GL_HAVE_STATIC_ASSERT1 && !defined static_assert
#  define static_assert _Static_assert /* C11 requires this #define.  */
# endif
#endif

/* @assert.h omit start@  */

#if 3 < __GNUC__ + (3 < __GNUC_MINOR__ + (4 <= __GNUC_PATCHLEVEL__))
# define _GL_HAS_BUILTIN_TRAP 1
#elif defined __has_builtin
# define _GL_HAS_BUILTIN_TRAP __has_builtin (__builtin_trap)
#else
# define _GL_HAS_BUILTIN_TRAP 0
#endif

#if 4 < __GNUC__ + (5 <= __GNUC_MINOR__)
# define _GL_HAS_BUILTIN_UNREACHABLE 1
#elif defined __has_builtin
# define _GL_HAS_BUILTIN_UNREACHABLE __has_builtin (__builtin_unreachable)
#else
# define _GL_HAS_BUILTIN_UNREACHABLE 0
#endif

/* Each of these macros verifies that its argument R is nonzero.  To
   be portable, R should be an integer constant expression.  Unlike
   assert (R), there is no run-time overhead.

   There are two macros, since no single macro can be used in all
   contexts in C.  verify_expr (R, E) is for scalar contexts, including
   integer constant expression contexts.  verify (R) is for declaration
   contexts, e.g., the top level.  */

/* Verify requirement R at compile-time.  Return the value of the
   expression E.  */

#define verify_expr(R, E) \
   (_GL_VERIFY_TRUE (R, "verify_expr (" #R ", " #E ")") ? (E) : (E))

/* Verify requirement R at compile-time, as a declaration without a
   trailing ';'.  verify (R) acts like static_assert (R) except that
   it is portable to C11/C++14 and earlier, it can issue better
   diagnostics, and its name is shorter and may be more convenient.  */

#ifdef __PGI
/* PGI barfs if R is long.  */
# define verify(R) _GL_VERIFY (R, "verify (...)", -)
#else
# define verify(R) _GL_VERIFY (R, "verify (" #R ")", -)
#endif

/* Assume that R always holds.  Behavior is undefined if R is false,
   fails to evaluate, or has side effects.  Although assuming R can
   help a compiler generate better code or diagnostics, performance
   can suffer if R uses hard-to-optimize features such as function
   calls not inlined by the compiler.  */

#if _GL_HAS_BUILTIN_UNREACHABLE
# define assume(R) ((R) ? (void) 0 : __builtin_unreachable ())
#elif 1200 <= _MSC_VER
# define assume(R) __assume (R)
#elif (defined GCC_LINT || defined lint) && _GL_HAS_BUILTIN_TRAP
  /* Doing it this way helps various packages when configured with
     --enable-gcc-warnings, which compiles with -Dlint.  It's nicer
     when 'assume' silences warnings even with older GCCs.  */
# define assume(R) ((R) ? (void) 0 : __builtin_trap ())
#else
  /* Some tools grok NOTREACHED, e.g., Oracle Studio 12.6.  */
# define assume(R) ((R) ? (void) 0 : /*NOTREACHED*/ (void) 0)
#endif

/* @assert.h omit end@  */

#endif
