#/*─────────────────────────────────────────────────────────────────╗
 │ To the extent possible under law, Justine Tunney has waived      │
 │ all copyright and related or neighboring rights to this file,    │
 │ as it is written in the following disclaimers:                   │
 │   • http://unlicense.org/                                        │
 │   • http://creativecommons.org/publicdomain/zero/1.0/            │
 ╚─────────────────────────────────────────────────────────────────*/
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

void onsig(int sig) {
  fprintf(stderr, "got %s\n", strsignal(sig));
}

int main(int argc, char *argv[]) {
  fprintf(stderr, "my pid is %d\n", getpid());
  for (int sig = 1; sig <= NSIG; ++sig)
    signal(sig, onsig);
  pause();
}
