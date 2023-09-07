#if 0
/*─────────────────────────────────────────────────────────────────╗
│ To the extent possible under law, Justine Tunney has waived      │
│ all copyright and related or neighboring rights to this file,    │
│ as it is written in the following disclaimers:                   │
│   • http://unlicense.org/                                        │
│   • http://creativecommons.org/publicdomain/zero/1.0/            │
╚─────────────────────────────────────────────────────────────────*/
#endif
#include "libc/assert.h"
#include "libc/calls/calls.h"
#include "libc/calls/struct/sigaction.h"
#include "libc/errno.h"
#include "libc/limits.h"
#include "libc/runtime/runtime.h"
#include "libc/sock/struct/pollfd.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/f.h"
#include "libc/sysv/consts/limits.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/poll.h"
#include "libc/sysv/consts/sig.h"

volatile bool gotsig;

void SignalHandler(int sig) {
  // we don't need to do anything in our signal handler since the signal
  // delivery itself causes a visible state change, saying what happened
  const char *s = "SignalHandler() called\n";
  write(1, s, strlen(s));  // notice both functions are @asyncsignalsafe
  // however this will help if delivered asynchronously in cpubound code
  // it's also necessary to discern spurious interrupts from real signal
  gotsig = true;
}

int main(int argc, char *argv[]) {

  printf("echoing stdin until ctrl+c is pressed\n");

  // you need to set your signal handler using sigaction() rather than
  // signal(), since the latter uses .sa_flags=SA_RESTART, which means
  // read will restart itself after signals, rather than raising EINTR
  sigaction(SIGINT, &(struct sigaction){.sa_handler = SignalHandler}, 0);

  for (;;) {

    // some programs are blocked on cpu rather than i/o
    // such programs shall rely on asynchronous signals
    printf("doing cpu task...\n");
    for (volatile int i = 0; i < INT_MAX / 5; ++i) {
      if (gotsig) {
        printf("\rgot ctrl+c asynchronously\n");
        exit(0);
      }
    }

    // posix guarantees atomic i/o if you use pipe_buf sized buffers
    // that way we don't need to worry about things like looping and
    // we can also be assured that multiple actors wont have tearing
    char buf[4];

    // read data from standard input
    //
    // since this is a blocking operation and we're not performing a
    // cpu-bound operation it is almost with absolute certainty that
    // when the ctrl-c signal gets delivered, it'll happen in read()
    //
    // it's possible to be more precise if we were building library
    // code. for example, you can block signals using sigprocmask()
    // and then use pselect() to do the waiting.
    printf("doing read i/o task...\n");
    int got = read(0, buf, sizeof(buf));

    // check if the read operation failed
    // negative one is the *only* return value to indicate errors
    if (got == -1) {
      if (errno == EINTR) {
        // a signal handler was invoked during the read operation
        // since we have only one such signal handler it's sigint
        // if we didn't have any signal handlers in our app, then
        // we wouldn't need to check this errno. using SA_RESTART
        // is another great way to avoid having to worry about it
        // however EINTR is very useful, when we choose to use it
        // the \r character is needed so when the line is printed
        // it'll overwrite the ^C that got echo'd with the ctrl-c
        if (gotsig) {
          printf("\rgot ctrl+c via i/o eintr\n");
          exit(0);
        } else {
          printf("\rgot spurious eintr\n");
          continue;
        }
      } else {
        // log it in the unlikely event something else went wrong
        perror("<stdin>");
        exit(1);
      }
    }

    // check if the user typed ctrl-d which closes the input handle
    if (!got) {
      printf("got eof\n");
      exit(0);
    }

    // relay read data to standard output
    //
    // it's usually safe to ignore the return code of write. the
    // operating system will send SIGPIPE if there's any problem
    // which kills the process by default
    write(1, buf, got);
    write(1, "\n", 1);
  }
}
