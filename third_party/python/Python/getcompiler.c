#include "third_party/python/Include/pylifecycle.h"
/* clang-format off */

/* Return the compiler identification, if possible. */

#ifndef COMPILER

#ifdef __GNUC__
#define COMPILER "\n[GCC " __VERSION__ "]"
#endif

#endif /* !COMPILER */

#ifndef COMPILER

#ifdef __cplusplus
#define COMPILER "[C++]"
#else
#define COMPILER "[C]"
#endif

#endif /* !COMPILER */

const char *
Py_GetCompiler(void)
{
    return COMPILER;
}
