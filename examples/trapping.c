#include <fenv.h>
#include <math.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <ucontext.h>
#include <unistd.h>
#include "libc/calls/struct/aarch64.internal.h"

/*
   Do you put lots of assert(!isnan(x)) in your code??
   Your microprocessor has a feature to automate this.

       Uncaught SIGFPE (FPE_FLTINV)
       __math_invalidf at libc/tinymath/math_errf.c:88
       logf at libc/tinymath/logf.c:100
       main at examples/trapping.c:29
       cosmo at libc/runtime/cosmo.S:105
       _start at libc/crt/crt.S:116

   This file shows how to use floating point exception
   trapping with Cosmopolitan Libc.
*/

#define TRAPS (FE_INVALID | FE_DIVBYZERO | FE_OVERFLOW | FE_UNDERFLOW)

void spring_trap(int sig, siginfo_t *si, void *arg) {

  // print signal safely
  const char *msg;
  int sic = si->si_code;
  if (sic == FPE_INTDIV)
    msg = "FPE_INTDIV: ";  // integer divide by zero
  else if (sic == FPE_INTOVF)
    msg = "FPE_INTOVF: ";  // integer overflow
  else if (sic == FPE_FLTDIV)
    msg = "FPE_FLTDIV: ";  // floating point divide by zero
  else if (sic == FPE_FLTOVF)
    msg = "FPE_FLTOVF: ";  // floating point overflow
  else if (sic == FPE_FLTUND)
    msg = "FPE_FLTUND: ";  // floating point underflow
  else if (sic == FPE_FLTRES)
    msg = "FPE_FLTRES: ";  // floating point inexact
  else if (sic == FPE_FLTINV)
    msg = "FPE_FLTINV: ";  // invalid floating point operation
  else if (sic == FPE_FLTSUB)
    msg = "FPE_FLTSUB: ";  // subscript out of range
  else
    msg = "SIGFPE: ";
  write(1, msg, strlen(msg));

  // recover from trap so that execution may resume
  // without this the same signal will just keep getting raised
  ucontext_t *ctx = arg;
#ifdef __x86_64__
  if (ctx->uc_mcontext.fpregs) {
    ctx->uc_mcontext.fpregs->mxcsr |= TRAPS << 7;  // disable traps
    ctx->uc_mcontext.fpregs->mxcsr &= ~TRAPS;      // clear cages
    return;
  }
#elif defined(__aarch64__)
  struct _aarch64_ctx *ac;
  for (ac = (struct _aarch64_ctx *)ctx->uc_mcontext.__reserved; ac->magic;
       ac = (struct _aarch64_ctx *)((char *)ac + ac->size)) {
    if (ac->magic == FPSIMD_MAGIC) {
      struct fpsimd_context *sm = (struct fpsimd_context *)ac;
      sm->fpcr &= ~(TRAPS << 8);  // disable traps
      sm->fpsr &= ~TRAPS;         // clear cages
      return;
    }
  }
#endif

  // exit if we can't recover execution
  msg = "cannot recover from signal\n";
  write(1, msg, strlen(msg));
  _exit(1);
}

void setup_trap(void) {
  struct sigaction sa;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = SA_SIGINFO;
  sa.sa_sigaction = spring_trap;
  sigaction(SIGFPE, &sa, 0);
}

void activate_trap(void) {
  feclearexcept(TRAPS);
  if (feenableexcept(TRAPS)) {
    static bool once;
    if (!once) {
      fprintf(stderr, "warning: trapping math isn't supported on this cpu\n");
      once = true;
    }
  }
}

float ident(float x) {
  return x;
}
float (*veil)(float) = ident;

int main(int argc, char *argv[]) {
  float x;
  setup_trap();

  // test illegal math
  activate_trap();
  x = 0 / veil(0);
  printf("0/0 = %g\n", x);

  // test divide by zero
  activate_trap();
  x = 1 / veil(0);
  printf("1/0 = %g\n", x);

  // test divide by zero again
  activate_trap();
  x = -1 / veil(0);
  printf("-1/0 = %g\n", x);

  // test domain error
  activate_trap();
  x = logf(veil(-1));
  printf("log(-1) = %g\n", x);

  // test imaginary number
  activate_trap();
  x = sqrtf(veil(-1));
  printf("sqrt(-1) = %g\n", x);

  // test overflow
  activate_trap();
  x = expf(veil(88.8));
  printf("expf(88.8) = %g\n", x);

  // test underflow
  activate_trap();
  x = expf(veil(-104));
  printf("expf(-104) = %g\n", x);
}
