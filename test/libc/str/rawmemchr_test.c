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

char *rawmemchr_naive(const char *s, char c) {
  for (;; ++s)
    if ((*s & 255) == (c & 255))
      return (char *)s;
}

void test() {
  int pagesz = sysconf(_SC_PAGESIZE);
  char *map = (char *)mmap(0, pagesz * 2, PROT_READ | PROT_WRITE,
                           MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
  npassert(map != MAP_FAILED);
  npassert(!mprotect(map + pagesz / 1, pagesz, PROT_NONE));
  for (int haylen = 1; haylen < 128; ++haylen) {
    char *hay = map + pagesz / 1 - (haylen + 1);
    for (int i = 0; i < haylen; ++i)
      hay[i] = rand() & 31;
    hay[haylen] = 0;
    for (int i = 0; i < 8; ++i) {
      char c = hay[rand() % haylen];
      if (rawmemchr_naive(hay, c) != rawmemchr(hay, c)) {
        kprintf("rawmemchr(%#.*hhs, %d) returned wrong result\n", haylen, hay,
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

char *ab;
int n = 15 * 1024;

void rawmemchr_single_char_search_15k_init(void) {
  ab = Calloc(n + 2, sizeof(char));
  memset(ab, 'a', n);
  ab[n] = 'b';
}
void rawmemchr_single_char_search_15k(void) {
  pEat(rawmemchr(ab, 'b'));
}

int main() {
  test();
  bench(rawmemchr_single_char_search_15k);
}
