#include <assert.h>
#include <cosmo.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/resource.h>
#include <time.h>
#include <unistd.h>

size_t wcscspn_naive(const wchar_t *s, const wchar_t *accept) {
  size_t count = 0;
  while (*s) {
    const wchar_t *p = accept;
    int found = 0;
    while (*p) {
      if (*s == *p) {
        found = 1;
        break;
      }
      p++;
    }
    if (found)
      break;
    count++;
    s++;
  }
  return count;
}

void print(const wchar_t *s) {
  while (*s)
    fprintf(stderr, " %#x", *s++);
}

void test() {
  int pagesz = sysconf(_SC_PAGESIZE);
  wchar_t *map = (wchar_t *)mmap(0, pagesz * 2, PROT_READ | PROT_WRITE,
                                 MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
  npassert(map != MAP_FAILED);
  npassert(!mprotect(map + pagesz / sizeof(wchar_t), pagesz, PROT_NONE));
  for (int haylen = 1; haylen < 128; ++haylen) {
    wchar_t *hay = map + pagesz / sizeof(wchar_t) - (haylen + 1);
    for (int i = 0; i < haylen; ++i)
      hay[i] = lemur64();
    hay[haylen] = 0;
    for (int i = 0; i < 128; ++i) {
      wchar_t accept[33] = {0};
      for (int i = 0; i < 32; ++i)
        accept[i] = lemur64();
      size_t want = wcscspn_naive(hay, accept);
      size_t got = wcscspn(hay, accept);
      if (got != want) {
        fprintf(stderr, "error: got %zu but wanted %zu\n", got, want);
        fprintf(stderr, "haystk:");
        print(hay);
        fprintf(stderr, "\naccept:");
        print(accept);
        fprintf(stderr, "\n");
        exit(1);
      }
    }
  }
  munmap(map, pagesz * 2);
}

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
  return calloc(n + 1, z) + z;
}

#define HARD 512

wchar_t *ab;
int n = 15 * 1024;
wchar_t everything[HARD + 1];

void wcscspn_single_char_search_15k_init(void) {
  ab = Calloc(n + 2, sizeof(wchar_t));
  wmemset(ab, 'a', n);
  ab[n] = 'b';
}
void wcscspn_single_char_search_15k(void) {
  npassert(wcscspn(ab, L"b") == n);
}

void wcscspn_everything_15k_init(void) {
  for (int i = 0; i < HARD; ++i)
    if (!(everything[i] = rand() % HARD))
      everything[i] = 1;
  ab = Calloc(n + 1, sizeof(wchar_t));
  for (int i = 0; i < n; ++i)
    ab[i] = -everything[rand() % HARD];
}
void wcscspn_everything_15k(void) {
  npassert(wcscspn(ab, everything) == n);
}

int main(int argc, char *argv[]) {
  test();
  bench(wcscspn_single_char_search_15k);
  bench(wcscspn_everything_15k);
}
