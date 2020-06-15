#if 0
/*─────────────────────────────────────────────────────────────────╗
│ To the extent possible under law, Justine Tunney has waived      │
│ all copyright and related or neighboring rights to this file,    │
│ as it is written in the following disclaimers:                   │
│   • http://unlicense.org/                                        │
│   • http://creativecommons.org/publicdomain/zero/1.0/            │
╚─────────────────────────────────────────────────────────────────*/
#endif
#include "libc/dce.h"
#include "libc/nt/dll.h"
#include "libc/nt/nt/process.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/stdio.h"
#include "libc/sysv/consts/exit.h"

int main() {
  if (IsWindows()) {
    /*
     * Cosmopolitan imports this automatically if it's referenced the
     * normal way. But Microsoft wants us to use loose coupling when
     * referencing their internal APIs. Don't think for a moment they
     * won't break open source projects that fail to heed the warning.
     */
    typeof(NtTerminateProcess) *NtTerminateProcess_ =
        GetProcAddress(GetModuleHandle("ntdll.dll"), "NtTerminateProcess");
    printf("%ld\n", GetModuleHandle("ntdll.dll"));
    printf("%p\n", NtTerminateProcess_);
    if (NtTerminateProcess_) {
      fflush(stdout);
      for (;;) NtTerminateProcess_(-1, 42);
    }
  } else {
    fprintf(stderr, "error: intended for windows\n");
  }
  return EXIT_FAILURE;
}
