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
#include "libc/calls/ucontext.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/exit.h"

/**
 * @fileoverview swapcontext() and makecontext() example
 * @note adapted from the Linux man-pages project
 */

static ucontext_t uctx_main;
static ucontext_t uctx_func1;
static ucontext_t uctx_func2;

#define say(s) write(1, s, strlen(s))
#define handle_error(msg) \
  do {                    \
    perror(msg);          \
    exit(EXIT_FAILURE);   \
  } while (0)

static void func1(void) {
  say("func1: started\n");
  say("func1: swapcontext(&uctx_func1, &uctx_func2)\n");
  if (swapcontext(&uctx_func1, &uctx_func2) == -1) {
    handle_error("swapcontext");
  }
  say("func1: returning\n");
}

static void func2(void) {
  say("func2: started\n");
  say("func2: swapcontext(&uctx_func2, &uctx_func1)\n");
  if (swapcontext(&uctx_func2, &uctx_func1) == -1) {
    handle_error("swapcontext");
  }
  say("func2: returning\n");
}

int main(int argc, char *argv[]) {
  char func1_stack[8192];
  char func2_stack[8192];

  if (getcontext(&uctx_func1) == -1) {
    handle_error("getcontext");
  }
  uctx_func1.uc_stack.ss_sp = func1_stack;
  uctx_func1.uc_stack.ss_size = sizeof(func1_stack);
  uctx_func1.uc_link = &uctx_main;
  makecontext(&uctx_func1, func1, 0);

  if (getcontext(&uctx_func2) == -1) {
    handle_error("getcontext");
  }
  uctx_func2.uc_stack.ss_sp = func2_stack;
  uctx_func2.uc_stack.ss_size = sizeof(func2_stack);
  /* Successor context is f1(), unless argc > 1 */
  uctx_func2.uc_link = (argc > 1) ? NULL : &uctx_func1;
  makecontext(&uctx_func2, func2, 0);

  say("main: swapcontext(&uctx_main, &uctx_func2)\n");
  if (swapcontext(&uctx_main, &uctx_func2) == -1) {
    handle_error("swapcontext");
  }

  say("main: exiting\n");
  exit(EXIT_SUCCESS);
}
