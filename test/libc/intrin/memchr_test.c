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

const char *memchr_golden(const char *s, unsigned char c, size_t n) {
  size_t i;
  for (i = 0; i < n; ++i)
    if (s[i] == c)
      return s + i;
  return 0;
}

void test() {
  int pagesz = sysconf(_SC_PAGESIZE);
  char *map = (char *)mmap(0, pagesz * 2, PROT_READ | PROT_WRITE,
                           MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
  npassert(map != MAP_FAILED);
  npassert(!mprotect(map + pagesz, pagesz, PROT_NONE));
  for (int haylen = 1; haylen < 128; ++haylen) {
    char *hay = map + pagesz - (haylen + 1);
    for (int i = 0; i < haylen; ++i)
      hay[i] = rand() & 31;
    hay[haylen] = 0;
    for (int i = 0; i < 8; ++i) {
      int c = rand() & 31;
      const char *x = memchr_golden(hay, c, haylen);
      const char *y = memchr(hay, c, haylen);
      if (x != y) {
        kprintf("memchr(%#.*hhs, %d) returned wrong result\n", haylen, hay, c);
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
int n = 16 * 1024;

void memchr_single_char_search_16k_init(void) {
  ab = Calloc(n + 2, 1);
  memset(ab, 'a', n);
  ab[n] = 'b';
}
void memchr_single_char_search_16k(void) {
  pEat(memchr(ab, 'b', n));
}

void whoa_test(void) {
  __expropriate(memchr((void *)__veil("r", (void *)1), 0, 0));
}

void overflow_test(void) {
  const char *s = "hello hello hello hello";
  if (memchr(s, 0, -1) != s + 23)
    exit(77);
}

int main() {
  test();
  whoa_test();
  overflow_test();
  bench(memchr_single_char_search_16k);
}
