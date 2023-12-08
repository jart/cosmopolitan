/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2022 Justine Alexandra Roberts Tunney                              │
│                                                                              │
│ Permission to use, copy, modify, and/or distribute this software for         │
│ any purpose with or without fee is hereby granted, provided that the         │
│ above copyright notice and this permission notice appear in all copies.      │
│                                                                              │
│ THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL                │
│ WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED                │
│ WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE             │
│ AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL         │
│ DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR        │
│ PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER               │
│ TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR             │
│ PERFORMANCE OF THIS SOFTWARE.                                                │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "libc/fmt/conv.h"
#include "libc/limits.h"
#include "libc/log/log.h"
#include "libc/mem/mem.h"
#include "libc/runtime/internal.h"
#include "libc/runtime/symbols.internal.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#ifdef __x86_64__

int StackOverflow(int f(), int n) {
  if (n < INT_MAX) {
    return f(f, n + 1) - 1;
  } else {
    return INT_MAX;
  }
}

void FpuCrash(void) {
  typedef char xmm_t __attribute__((__vector_size__(16)));
  xmm_t v = {0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7,
             0x8, 0x9, 0xa, 0xb, 0xc, 0xd, 0xe, 0xf};
  volatile int x = 0;
  asm volatile("fldpi");
  asm volatile("mov\t%0,%%r15" : /* no outputs */ : "g"(0x3133731337));
  asm volatile("movaps\t%0,%%xmm15" : /* no outputs */ : "x"(v));
  fputc(7 / x, stdout);
}

char bss[10];
void BssOverrunCrash(int n) {
  int i;
  for (i = 0; i < n; ++i) {
    bss[i] = i;
  }
}

char data[10] = "abcdeabcde";
void DataOverrunCrash(int n) {
  int i;
  for (i = 0; i < n; ++i) {
    data[i] = i;
  }
}

const char rodata[10] = "abcdeabcde";
int RodataOverrunCrash(int i) {
  return rodata[i];
}

char *StackOverrunCrash(int n) {
  int i;
  char stack[10];
  bzero(stack, sizeof(stack));
  for (i = 0; i < n; ++i) {
    stack[i] = i;
  }
  return strdup(stack);
}

char *MemoryLeakCrash(void) {
  char *p = strdup("doge");
  CheckForMemoryLeaks();
  return p;
}

int NpeCrash(char *p) {
  asm("nop");  // xxx: due to backtrace addr-1 thing
  return *p;
}

void (*pFpuCrash)(void) = FpuCrash;
void (*pBssOverrunCrash)(int) = BssOverrunCrash;
void (*pDataOverrunCrash)(int) = DataOverrunCrash;
int (*pRodataOverrunCrash)(int) = RodataOverrunCrash;
char *(*pStackOverrunCrash)(int) = StackOverrunCrash;
char *(*pMemoryLeakCrash)(void) = MemoryLeakCrash;
int (*pNpeCrash)(char *) = NpeCrash;
int (*pStackOverflow)(int (*)(), int) = StackOverflow;

int main(int argc, char *argv[]) {
  ShowCrashReports();
  if (argc > 1) {
    switch (atoi(argv[1])) {
      case 0:
        break;
      case 1:
        pFpuCrash();
        exit(0);
      case 2:
        pBssOverrunCrash(10 + 1);
        exit(0);
      case 3:
        exit(pRodataOverrunCrash(10 + 1));
      case 4:
        pDataOverrunCrash(10 + 1);
        exit(0);
      case 5:
        exit((intptr_t)pStackOverrunCrash(10 + 10000));
      case 6:
        exit((intptr_t)pMemoryLeakCrash());
      case 7:
        exit(pNpeCrash(0));
      case 8:
        exit(pNpeCrash(0));
      case 9:
        exit(pStackOverflow(pStackOverflow, 0));
      default:
        fputs("error: unrecognized argument\n", stderr);
        exit(1);
    }
  } else {
    fputs("error: too few args\n", stderr);
    exit(1);
  }
}

#else

int main(int argc, char *argv[]) {
}

#endif /* __x86_64__ */
