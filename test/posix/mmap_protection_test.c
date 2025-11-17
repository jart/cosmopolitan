// Copyright 2025 Justine Alexandra Roberts Tunney
//
// Permission to use, copy, modify, and/or distribute this software for
// any purpose with or without fee is hereby granted, provided that the
// above copyright notice and this permission notice appear in all copies.
//
// THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL
// WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE
// AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL
// DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
// PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
// TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
// PERFORMANCE OF THIS SOFTWARE.

#include <errno.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>

volatile int got_segv = 0;

void segv_handler(int sig) {
  got_segv = 1;
}

int main() {
  void *ptr;
  struct sigaction sa;
  size_t pagesize = getpagesize();

  sa.sa_handler = segv_handler;
  sa.sa_flags = SA_NODEFER;
  sigemptyset(&sa.sa_mask);
  if (sigaction(SIGSEGV, &sa, NULL) != 0)
    return 1;

  ptr = mmap(NULL, pagesize, PROT_READ | PROT_WRITE,
             MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
  if (ptr == MAP_FAILED)
    return 2;

  *(int *)ptr = 42;
  if (*(int *)ptr != 42)
    return 3;

  if (mprotect(ptr, pagesize, PROT_READ) != 0)
    return 4;

  if (*(int *)ptr != 42)
    return 5;

  if (mprotect(ptr, pagesize, PROT_WRITE) != 0) {
    if (errno == ENOTSUP) {
      munmap(ptr, pagesize);
      return 0;
    }
    return 6;
  }

  *(int *)ptr = 24;
  if (*(int *)ptr != 24)
    return 7;

  if (munmap(ptr, pagesize) != 0)
    return 8;

  if (mmap(NULL, 0, PROT_READ, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0) !=
      MAP_FAILED)
    return 9;

  if (errno != EINVAL)
    return 10;

  return 0;
}