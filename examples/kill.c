#/*─────────────────────────────────────────────────────────────────╗
 │ To the extent possible under law, Justine Tunney has waived      │
 │ all copyright and related or neighboring rights to this file,    │
 │ as it is written in the following disclaimers:                   │
 │   • http://unlicense.org/                                        │
 │   • http://creativecommons.org/publicdomain/zero/1.0/            │
 ╚─────────────────────────────────────────────────────────────────*/
#include <ctype.h>
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// POSIX kill command
//
//   - kill -l
//   - kill -9 123 456
//   - kill -KILL 123 456
//   - kill -SIGKILL 123 456
//   - kill -s SIGKILL 123 456
//

const char kSignals[] = {
    SIGHUP,  SIGINT,  SIGQUIT, SIGILL,    SIGTRAP, SIGABRT,  SIGBUS,
    SIGFPE,  SIGKILL, SIGUSR1, SIGSEGV,   SIGUSR2, SIGPIPE,  SIGALRM,
    SIGTERM, SIGCHLD, SIGCONT, SIGSTOP,   SIGTSTP, SIGTTIN,  SIGTTOU,
    SIGURG,  SIGXCPU, SIGXFSZ, SIGVTALRM, SIGPROF, SIGWINCH, SIGSYS,
};

const char kSignalNames[][8] = {
    "HUP",  "INT",  "QUIT", "ILL",    "TRAP", "ABRT",  "BUS",
    "FPE",  "KILL", "USR1", "SEGV",   "USR2", "PIPE",  "ALRM",
    "TERM", "CHLD", "CONT", "STOP",   "TSTP", "TTIN",  "TTOU",
    "URG",  "XCPU", "XFSZ", "VTALRM", "PROF", "WINCH", "SYS",
};

void list_signals(void) {
  for (int i = 0; i < sizeof(kSignals); i += 4) {
    for (int j = 0; j < 4; ++j) {
      if (i + j < sizeof(kSignals)) {
        int sig = kSignals[i + j];
        printf("%2d) SIG%-7s", sig, kSignalNames[i + j]);
      }
    }
    printf("\n");
  }
}

int main(int argc, char *argv[]) {

  const char *prog = argv[0];
  if (!prog)
    prog = "kill";

  int arg = 1;
  const char *sigstr = 0;
  if (arg < argc) {
    if (argv[arg][0] == '-') {
      if (argv[arg][1] == 's' && !argv[arg][2]) {
        if (++arg < argc)
          sigstr = argv[arg++];
      } else if (argv[arg][1] == 'l' && !argv[arg][2]) {
        list_signals();
        exit(0);
      } else {
        sigstr = argv[arg++] + 1;
      }
    }
  }
  if (arg == argc) {
    fprintf(stderr, "%s: missing operand\n", prog);
    exit(1);
  }

  int sig;
  if (!sigstr) {
    sig = SIGTERM;
  } else {
    sig = atoi(sigstr);
    if (sig) {
      if (!(1 <= sig && sig <= NSIG)) {
        fprintf(stderr, "%s: bad signal number: %d\n", prog, sig);
        exit(1);
      }
    } else {
      if (toupper(sigstr[0]) == 'S' &&  //
          toupper(sigstr[1]) == 'I' &&  //
          toupper(sigstr[2]) == 'G')
        sigstr += 3;
      for (int i = 0; i < sizeof(kSignals); ++i)
        if (!strcasecmp(sigstr, kSignalNames[i]))
          sig = kSignals[i];
      if (!sig) {
        fprintf(stderr, "%s: bad signal name: %s\n", prog, sigstr);
        exit(1);
      }
    }
  }

  int rc = 0;
  for (int i = arg; i < argc; ++i) {
    int pid = atoi(argv[i]);
    if (kill(pid, sig)) {
      fprintf(stderr, "%s: %d: %s\n", prog, pid, strerror(errno));
      rc = 1;
    }
  }
  exit(rc);
}
