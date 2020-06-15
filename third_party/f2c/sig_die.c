#include "libc/calls/calls.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/stdio.h"
#include "libc/sysv/consts/sig.h"
#include "third_party/f2c/internal.h"

void sig_die(const char *s, int kill) {
  /* print error message, then clear buffers */
  fprintf(stderr, "%s\n", s);
  if (kill) {
    fflush(stderr);
    f_exit();
    fflush(stderr);
    /* now get a core */
    signal(SIGIOT, SIG_DFL);
    abort();
  } else {
    f_exit();
    exit(1);
  }
}
