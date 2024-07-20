#if 0
/*─────────────────────────────────────────────────────────────────╗
│ To the extent possible under law, Justine Tunney has waived      │
│ all copyright and related or neighboring rights to this file,    │
│ as it is written in the following disclaimers:                   │
│   • http://unlicense.org/                                        │
│   • http://creativecommons.org/publicdomain/zero/1.0/            │
╚─────────────────────────────────────────────────────────────────*/
#endif
#include <cosmo.h>
#include <math.h>

void crash(long x0, long x1, long x2,  //
           double v0, double v1, double v2) {
  __builtin_trap();
}

void (*pCrash)(long, long, long, double, double, double) = crash;

int main(int argc, char *argv[]) {

  // // by default we launch an addr2line subprocess to print backtraces
  // // with line numbers. you can force it to use the embedded solution
  // setenv("ADDR2LINE", "", true);

  // // using a seccomp sandbox is another way to force embedded backtraces
  // pledge("stdio", NULL);

  // enable the crash reporting feature
  ShowCrashReports();

  // time to die
  pCrash(1, 2, 3, NAN, NAN, NAN);
}
