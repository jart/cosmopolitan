#if 0
/*─────────────────────────────────────────────────────────────────╗
│ To the extent possible under law, Justine Tunney has waived      │
│ all copyright and related or neighboring rights to this file,    │
│ as it is written in the following disclaimers:                   │
│   • http://unlicense.org/                                        │
│   • http://creativecommons.org/publicdomain/zero/1.0/            │
╚─────────────────────────────────────────────────────────────────*/
#endif
#include "tool/build/elf2pe.h"

#define STD_OUTPUT_HANDLE -11u

__dll_import("kernel32.dll", long, GetStdHandle, (unsigned));
__dll_import("kernel32.dll", int, WriteFile,
             (long, const void *, unsigned, unsigned *, void *));

__attribute__((__ms_abi__)) long WinMain(void) {
  WriteFile(GetStdHandle(STD_OUTPUT_HANDLE), "hello world\n", 12, 0, 0);
  return 0;
}
