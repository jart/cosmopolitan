#ifndef COSMOPOLITAN_LIBC_RUNTIME_WINARGS_INTERNAL_H_
#define COSMOPOLITAN_LIBC_RUNTIME_WINARGS_INTERNAL_H_
#include "libc/limits.h"
COSMOPOLITAN_C_START_

struct WinArgs {
  union {
    struct {
      char *argv[8192];
      char *envp[512];
      intptr_t auxv[2][2];
      char argv0buf[256];
      char argblock[32767];
      char envblock[32767];
    };
    char16_t tmp16[257];
  };
} forcealign(16);

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_RUNTIME_WINARGS_INTERNAL_H_ */
