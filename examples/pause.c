#if 0
/*─────────────────────────────────────────────────────────────────╗
│ To the extent possible under law, Justine Tunney has waived      │
│ all copyright and related or neighboring rights to this file,    │
│ as it is written in the following disclaimers:                   │
│   • http://unlicense.org/                                        │
│   • http://creativecommons.org/publicdomain/zero/1.0/            │
╚─────────────────────────────────────────────────────────────────*/
#endif
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

volatile int g_sig;

void OnSig(int sig) {
  g_sig = sig;
}

int main(int argc, char *argv[]) {

  // listen for all signals
  for (int sig = 1; sig <= NSIG; ++sig)
    signal(sig, OnSig);

  // wait for a signal
  printf("waiting for signal to be sent to my pid %d\n", getpid());
  pause();

  // report the signal
  printf("got %s\n", strsignal(g_sig));
}
