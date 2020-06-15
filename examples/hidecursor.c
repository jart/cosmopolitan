#if 0
/*─────────────────────────────────────────────────────────────────╗
│ To the extent possible under law, Justine Tunney has waived      │
│ all copyright and related or neighboring rights to this file,    │
│ as it is written in the following disclaimers:                   │
│   • http://unlicense.org/                                        │
│   • http://creativecommons.org/publicdomain/zero/1.0/            │
╚─────────────────────────────────────────────────────────────────*/
#endif
#include "libc/nt/console.h"
#include "libc/nt/runtime.h"
#include "libc/nt/struct/consolecursorinfo.h"
#include "libc/nt/synchronization.h"

int main(int argc, char *argv[]) {
  struct NtConsoleCursorInfo ntcursor;
  SleepEx(1000, false);
  GetConsoleCursorInfo(GetStdHandle(kNtStdOutputHandle), &ntcursor);
  ntcursor.bVisible = false;
  SetConsoleCursorInfo(GetStdHandle(kNtStdOutputHandle), &ntcursor);
  SleepEx(1000, false);
  ntcursor.bVisible = true;
  SetConsoleCursorInfo(GetStdHandle(kNtStdOutputHandle), &ntcursor);
  SleepEx(1000, false);
  return 0;
}
