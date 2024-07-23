/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2023 Justine Alexandra Roberts Tunney                              │
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
#include <cosmo.h>
#include <limits.h>
#include <pthread.h>
#include <signal.h>
#include <unistd.h>

/**
 * stack overflow recovery technique #5
 * use the cosmo posix threads extensions
 */

sig_atomic_t smashed_stack;

void CrashHandler(int sig) {
  smashed_stack = true;
  pthread_exit(0);
}

int StackOverflow(int d) {
  char A[8];
  for (int i = 0; i < sizeof(A); i++)
    A[i] = d + i;
  if (__veil("r", d))
    return StackOverflow(d + 1) + A[d % sizeof(A)];
  return 0;
}

void *MyPosixThread(void *arg) {
  exit(StackOverflow(0));
  return 0;
}

int main() {

  // choose the most dangerously small size possible
  size_t sigstacksize = sysconf(_SC_MINSIGSTKSZ) + 2048;

  // setup signal handler
  struct sigaction sa;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = SA_ONSTACK;
  sa.sa_handler = CrashHandler;
  if (sigaction(SIGBUS, &sa, 0))
    return 1;
  if (sigaction(SIGSEGV, &sa, 0))
    return 2;

  // create thread with signal stack
  pthread_t id;
  pthread_attr_t attr;
  if (pthread_attr_init(&attr))
    return 3;
  if (pthread_attr_setguardsize(&attr, getpagesize()))
    return 4;
  if (pthread_attr_setsigaltstacksize_np(&attr, sigstacksize))
    return 5;
  if (pthread_create(&id, &attr, MyPosixThread, 0))
    return 6;
  if (pthread_attr_destroy(&attr))
    return 7;
  if (pthread_join(id, 0))
    return 8;
  if (!smashed_stack)
    return 9;

  CheckForMemoryLeaks();
}
