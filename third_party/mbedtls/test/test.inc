#include "libc/calls/calls.h"
#include "libc/errno.h"
#include "libc/fmt/conv.h"
#include "libc/limits.h"
#include "libc/macros.internal.h"
#include "libc/sysv/consts/exit.h"
#include "third_party/mbedtls/config.h"
#include "third_party/mbedtls/test/lib.h"

asm(".ident\t\"\\n\\n\
Mbed TLS (Apache 2.0)\\n\
Copyright ARM Limited\\n\
Copyright Mbed TLS Contributors\"");
asm(".include \"libc/disclaimer.inc\"");

__static_yoink("zipos");
