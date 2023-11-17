#include "libc/runtime/runtime.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"

__static_yoink("sys_mmap");       /* asan needs it */
__static_yoink("__track_memory"); /* asan needs it */

#define ASSERT(x, y)    Assert2(x, y, #y, __FILE__, __LINE__)
#define ASSERT128(x, y) Assert128(x, y, #y, __FILE__, __LINE__)

void Assert(long, long, char *);
void Assert2(long, long, char *, char *, int);
void Assert128(__int128, __int128, char *, char *, int);
