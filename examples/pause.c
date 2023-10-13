#if 0
/*─────────────────────────────────────────────────────────────────╗
│ To the extent possible under law, Justine Tunney has waived      │
│ all copyright and related or neighboring rights to this file,    │
│ as it is written in the following disclaimers:                   │
│   • http://unlicense.org/                                        │
│   • http://creativecommons.org/publicdomain/zero/1.0/            │
╚─────────────────────────────────────────────────────────────────*/
#endif
#include "libc/calls/calls.h"
#include "libc/calls/struct/sigaction.h"
#include "libc/fmt/itoa.h"
#include "libc/str/str.h"

volatile int g_sig;

void OnSig(int sig) {
  g_sig = sig;
}

int main(int argc, char *argv[]) {

  // listen for all signals
  for (int sig = 1; sig <= NSIG; ++sig) {
    signal(sig, OnSig);
  }

  // wait for a signal
  char ibuf[12];
  FormatInt32(ibuf, getpid());
  tinyprint(2, "waiting for signal to be sent to ", ibuf, "\n", NULL);
  pause();

  // report the signal
  tinyprint(1, "got ", strsignal(g_sig), "\n", NULL);
}
