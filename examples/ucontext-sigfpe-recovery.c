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
#include "libc/calls/struct/siginfo.h"
#include "libc/calls/ucontext.h"
#include "libc/stdio/stdio.h"
#include "libc/sysv/consts/sa.h"
#include "libc/sysv/consts/sig.h"
#include "third_party/xed/x86.h"
#ifdef __x86_64__

/**
 * @fileoverview How to change CPU state on signal delivery
 *
 * This program redefines division by zero so that it has a definition.
 * The definition is the meaning of life, the universe, and everything.
 * Normally crash signals like `SIGSEGV`, `SIGILL`, and `SIGFPE` aren't
 * recoverable. This example shows how it actually can be done with Xed
 * and this example should work on all supported platforms even Windows
 */

void handler(int sig, siginfo_t *si, void *vctx) {
  struct XedDecodedInst xedd;
  struct ucontext *ctx = vctx;
  xed_decoded_inst_zero_set_mode(&xedd, XED_MACHINE_MODE_LONG_64);
  xed_instruction_length_decode(&xedd, (void *)ctx->uc_mcontext.rip, 15);
  ctx->uc_mcontext.rip += xedd.length;
  ctx->uc_mcontext.rax = 42;  // set the DIV result registers rdx:rax
  ctx->uc_mcontext.rdx = 0;
}

int main(int argc, char *argv[]) {
  struct sigaction saint = {.sa_sigaction = handler, .sa_flags = SA_SIGINFO};
  sigaction(SIGFPE, &saint, NULL);
  volatile long x = 0;
  printf("123/0 = %ld\n", 123 / x);
  return 0;
}

#endif /* __x86_64__ */
