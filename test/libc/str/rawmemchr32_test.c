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

char32_t *rawmemchr32_naive(const char32_t *s, char32_t c) {
  for (;; ++s)
    if (*s == c)
      return (char32_t *)s;
}

void test() {
  int pagesz = sysconf(_SC_PAGESIZE);
  char32_t *map = (char32_t *)mmap(0, pagesz * 2, PROT_READ | PROT_WRITE,
                                   MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
  npassert(map != MAP_FAILED);
  npassert(!mprotect(map + pagesz / 4, pagesz, PROT_NONE));
  for (int haylen = 1; haylen < 256; ++haylen) {
    char32_t *hay = map + pagesz / 4 - (haylen + 1);
    for (int i = 0; i < haylen; ++i)
      hay[i] = rand() & 0x03030303;
    hay[haylen] = 0;
    for (int i = 0; i < 8; ++i) {
      char32_t c = hay[rand() % haylen];
      if (rawmemchr32_naive(hay, c) != rawmemchr32(hay, c)) {
        kprintf("rawmemchr32(%#.*hs, %d) returned wrong result\n", haylen, hay,
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

char32_t *ab;
int n = 15 * 1024;

void rawmemchr32_single_char_search_15k_init(void) {
  ab = Calloc(n + 2, sizeof(char32_t));
  wmemset((wchar_t *)ab, 'a', n);
  ab[n] = 'b';
}
void rawmemchr32_single_char_search_15k(void) {
  pEat(rawmemchr32(ab, 'b'));
}

int main() {
  test();
  bench(rawmemchr32_single_char_search_15k);
}
