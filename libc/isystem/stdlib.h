#ifndef _STDLIB_H
#define _STDLIB_H
#include "libc/calls/calls.h"
#include "libc/calls/termios.h"
#include "libc/fmt/conv.h"
#include "libc/limits.h"
#include "libc/mem/alg.h"
#include "libc/mem/alloca.h"
#include "libc/mem/mem.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/dprintf.h"
#include "libc/stdio/rand.h"
#include "libc/stdlib.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/exit.h"
#include "libc/temp.h"
#include "third_party/musl/rand48.h"

#if defined(_XOPEN_SOURCE) || defined(_GNU_SOURCE) || defined(_BSD_SOURCE) || \
    defined(_COSMO_SOURCE)
#include "libc/bsdstdlib.h"
#endif

#endif /* _STDLIB_H */
