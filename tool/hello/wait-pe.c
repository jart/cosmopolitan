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

__dll_import("kernel32.dll", uint32_t, SleepEx, (uint32_t, bool32));

__attribute__((__ms_abi__)) long WinMain(void) {
  SleepEx(-1, true);
  return 0;
}
