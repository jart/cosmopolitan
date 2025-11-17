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

wchar_t *wmemchr_naive(const wchar_t *s, wchar_t c, size_t n) {
  const wchar_t *e = s + n;
  for (; s < e; ++s)
    if (*s == c)
      return (wchar_t *)s;
  return 0;
}

void test() {
  int pagesz = sysconf(_SC_PAGESIZE);
  wchar_t *map = (wchar_t *)mmap(0, pagesz * 2, PROT_READ | PROT_WRITE,
                                 MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
  npassert(map != MAP_FAILED);
  npassert(!mprotect(map + pagesz / 4, pagesz, PROT_NONE));
  for (int haylen = 1; haylen < 256; ++haylen) {
    wchar_t *hay = map + pagesz / 4 - (haylen + 1);
    for (int i = 0; i < haylen; ++i)
      hay[i] = rand() & 0x03030303;
    hay[haylen] = 0;
    for (int i = 0; i < 8; ++i) {
      wchar_t c = rand() & 0x03030303;
      if (wmemchr_naive(hay, c, haylen) != wmemchr(hay, c, haylen)) {
        kprintf("wmemchr(%#.*hhs, %#x) returned wrong result\n", haylen * 4,
                hay, c);
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
  int n = 1000;
  for (int i = 0; i < n; ++i)
    func();
  clock_gettime(CLOCK_MONOTONIC, &end);
  fprintf(stderr, "%s took %ld ns\n", name, tonanos(tub(end, beg)) / n);
}

void *Calloc(size_t n, size_t z) {
  return (char *)calloc(n + 1, z) + z;
}

wchar_t *ab;
int n = 4 * 1024;

void wmemchr_single_char_search_4k_init(void) {
  ab = Calloc(n + 2, 4);
  wmemset(ab, 'a', n);
  ab[n] = 'b';
}
void wmemchr_single_char_search_4k(void) {
  pEat(wmemchr(ab, 'b', n));
}

int main() {
  test();
  bench(wmemchr_single_char_search_4k);
}
