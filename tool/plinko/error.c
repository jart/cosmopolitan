#/*─────────────────────────────────────────────────────────────────╗
 │ To the extent possible under law, Justin Tunney has waived       │
 │ all copyright and related or neighboring rights to this file,    │
 │ as it is written in the following disclaimers:                   │
 │   • http://unlicense.org/                                        │
 │   • http://creativecommons.org/publicdomain/zero/1.0/            │
 ╚─────────────────────────────────────────────────────────────────*/
#include "error.h"
#include <stdarg.h>
#include "plinko.h"
#include "printf.h"
#include "stack.h"

[[noreturn]] void Raise(int x) {
  fflush(stdout);
  fflush(stderr);
  longjmp(crash, ~x);
}

[[noreturn]] void Error(const wchar_t *f, ...) {
  int n;
  va_list va;
  fflush(stdout);
  fflush(stderr);
  va_start(va, f);
  n = Fprintf(2, L"\033[1;31merror\033[0m: ");
  n = Vfnprintf(f, va, 2, n);
  Fprintf(2, L"%n");
  va_end(va);
  Raise(kError);
}

[[noreturn]] void OutOfMemory(void) {
  Error(L"out of memory");
}

[[noreturn]] void StackOverflow(void) {
  Error(L"stack overflow");
}

[[noreturn]] void React(int e, int x, int k) {
  if (!sp || e != LO(GetCurrentFrame()))
    Push(e);
  Push(x);
  Raise(k);
}
