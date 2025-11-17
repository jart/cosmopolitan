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

#include <assert.h>
#include <cosmo.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/resource.h>
#include <time.h>
#include <unistd.h>

////////////////////////////////////////////////////////////////////////////////
// testing

char16_t *rawmemchr16_naive(const char16_t *s, char16_t c) {
  for (;; ++s)
    if (*s == c)
      return (char16_t *)s;
}

char16_t rando() {
  return rand() & 0x0707;
}

void test() {
  int pagesz = sysconf(_SC_PAGESIZE);
  char16_t *map = (char16_t *)mmap(0, pagesz * 2, PROT_READ | PROT_WRITE,
                                   MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
  npassert(map != MAP_FAILED);
  npassert(!mprotect(map + pagesz / 2, pagesz, PROT_NONE));
  for (int haylen = 1; haylen < 128; ++haylen) {
    char16_t *hay = map + pagesz / 2 - (haylen + 1);
    for (int i = 0; i < haylen; ++i)
      hay[i] = rando();
    hay[haylen] = 0;
    for (int i = 0; i < 8; ++i) {
      char16_t c = hay[rand() % haylen];
      if (rawmemchr16_naive(hay, c) != rawmemchr16(hay, c)) {
        kprintf("rawmemchr16(%#.*hs, %d) returned wrong result\n", haylen, hay,
                c);
        exit(1);
      }
    }
  }
  munmap(map, pagesz * 2);
}

void *eat(const void *x) {
  return (void *)x;
}
void *(*pEat)(const void *x) = eat;

long tonanos(struct timespec x) {
  return x.tv_sec * 1000000000ul + x.tv_nsec;
}

struct timespec tub(struct timespec a, struct timespec b) {
  a.tv_sec -= b.tv_sec;
  if (a.tv_nsec < b.tv_nsec) {
    a.tv_nsec += 1000000000;
    a.tv_sec--;
  }
  a.tv_nsec -= b.tv_nsec;
  return a;
}

void bench(const char *name, void init(void), void func(void)) {
#define bench(func) bench(#func, func##_init, func)
  init();
  struct timespec beg, end;
  clock_gettime(CLOCK_MONOTONIC, &beg);
  int n = 10000;
  for (int i = 0; i < n; ++i)
    func();
  clock_gettime(CLOCK_MONOTONIC, &end);
  fprintf(stderr, "%s took %ld ns\n", name, tonanos(tub(end, beg)) / n);
}

void *Calloc(size_t n, size_t z) {
  return (char *)calloc(n + 1, z) + z;
}

char16_t *ab;
int n = 16 * 1024;

void rawmemchr16_single_char_search_16k_init(void) {
  ab = Calloc(n + 2, sizeof(char16_t));
  memset16(ab, 'a', n);
  ab[n] = 'b';
}
void rawmemchr16_single_char_search_16k(void) {
  pEat(rawmemchr16(ab, 'b'));
}

int main() {
  test();
  bench(rawmemchr16_single_char_search_16k);
}
