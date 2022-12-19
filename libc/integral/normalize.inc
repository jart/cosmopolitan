#define __COSMOPOLITAN__ 1

#ifndef __COUNTER__
#define __COUNTER__ __LINE__
#endif

#if __GNUC__ + 0 < 2
#undef __GNUC__
#elif defined(__GNUC__) && defined(SWIG) /* lool */
#undef __GNUC__
#elif defined(__GNUC__) && defined(__NVCC__) /* lool */
#undef __GNUC__
#elif !defined(__GNUC__) && defined(__APPLE__) /* modesty */
#define __GNUC__            4
#define __GNUC_MINOR__      2
#define __GNUC_PATCHLEVEL__ 1
#elif !defined(__GNUC__) && defined(__TINYC__)
#define __GNUC__            2
#define __GNUC_MINOR__      0
#define __GNUC_PATCHLEVEL__ 0
#endif

#if !defined(__x86_64__) && \
    (defined(__amd64__) || (defined(_M_AMD64) && defined(_M_X64)))
#define __x86_64__ 1
#elif !defined(__i386__) && ((defined(__i486__) || defined(__i586__) ||  \
                              defined(__i686__) || defined(__i786__)) || \
                             _M_IX86 + 0 >= 400)
#define __i386__ 1
#elif !defined(__ia16__) && (defined(__MSDOS__) || defined(__BCC__))
#define __ia16__ 1
#endif
#if __ia16__ + __i386__ + __x86_64__ + 0
#define __x86__ 1
#endif

#ifdef _MSC_VER
#define __STRICT_ANSI__
#ifndef __STDC__
#define __STDC__
#endif
#endif

#ifndef __has_attribute
#define __has_attribute(x) 0
#endif
#ifndef __has_builtin
#define __has_builtin(x) 0
#endif
#ifndef __has_cpp_attribute
#define __has_cpp_attribute(x) 0
#endif
#ifndef __has_extension
#define __has_extension(x) 0
#endif

#ifdef unix
#undef unix
#endif

#ifdef linux
#undef linux
#endif

#ifndef __BIGGEST_ALIGNMENT__
#define __BIGGEST_ALIGNMENT__ 16
#endif

/* TODO(jart): Remove this in favor of GetStackSize() */
#if defined(COSMO) && (defined(MODE_DBG) || defined(__SANITIZE_ADDRESS__))
#define STACKSIZE 262144 /* 256kb stack */
#elif defined(COSMO)
#define STACKSIZE 131072 /* 128kb stack */
#else
#define STACKSIZE 8388608 /* 8mb stack */
#endif

#define BIGPAGESIZE 0x200000
#define FRAMESIZE   0x10000 /* 8086 */
#define GUARDSIZE   0x4000  /* b/c apple m1 */
#define PAGESIZE    0x1000  /* i386+ */
#define BUFSIZ      0x1000  /* best stdio default */
#define CACHELINE   0x40    /* nexgen32e */
#define CHAR_BIT    8       /* b/c von neumann */
#define ARG_MAX     0xfffe  /* for argv and envp; see CreateProcess (32767*2) */
#define PATH_MAX    1024    /* b/c _XOPEN_PATH_MAX */
#define NAME_MAX    255     /* 511 on netbsd */
#define CHILD_MAX   16      /* only if malloc isn't linked */
#define OPEN_MAX    16      /* only if malloc isn't linked */
#define ATEXIT_MAX  32      /* only if malloc isn't linked */
#define NSIG        128     /* b/c freebsd */

#if defined(__LP64__) && !defined(__INT64_TYPE__)
#include "libc/integral/lp64.inc"
#elif defined(_MSC_VER) && !defined(__INT64_TYPE__)
#include "libc/integral/llp64.inc"
#endif

#if !(__ASSEMBLER__ + __LINKER__ + 0)
#ifdef __STDC__
#include "libc/integral/c.inc"
#else
#define const
#define volatile
#endif
#ifdef __cplusplus
#include "libc/integral/cxx.inc"
#endif
#endif
