#include <assert.h>
#include <cosmo.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/resource.h>
#include <time.h>
#include <unistd.h>

size_t strspn_naive(const char *s, const char *accept) {
  size_t count = 0;
  while (*s) {
    const char *p = accept;
    int found = 0;
    while (*p) {
      if (*s == *p) {
        found = 1;
        break;
      }
      p++;
    }
    if (!found)
      break;
    count++;
    s++;
  }
  return count;
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
    for (int i = 0; i < 128; ++i) {
      char accept[33] = {0};
      for (int i = 0; i < 32; ++i)
        accept[i] = rand() & 31;
      if (strspn_naive(hay, accept) != strspn(hay, accept)) {
        kprintf("strspn(%#hhs, %#hhs) returned wrong result\n", hay, accept);
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
  return (char *)calloc(n + 1, z) + z;
}

char everything[256] = {
    1,   2,   3,   4,   5,   6,   7,   8,   9,   10,  11,  12,  13,  14,  15,
    16,  17,  18,  19,  20,  21,  22,  23,  24,  25,  26,  27,  28,  29,  30,
    31,  32,  33,  34,  35,  36,  37,  38,  39,  40,  41,  42,  43,  44,  45,
    46,  47,  48,  49,  50,  51,  52,  53,  54,  55,  56,  57,  58,  59,  60,
    61,  62,  63,  64,  65,  66,  67,  68,  69,  70,  71,  72,  73,  74,  75,
    76,  77,  78,  79,  80,  81,  82,  83,  84,  85,  86,  87,  88,  89,  90,
    91,  92,  93,  94,  95,  96,  97,  98,  99,  100, 101, 102, 103, 104, 105,
    106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120,
    121, 122, 123, 124, 125, 126, 127, 128, 129, 130, 131, 132, 133, 134, 135,
    136, 137, 138, 139, 140, 141, 142, 143, 144, 145, 146, 147, 148, 149, 150,
    151, 152, 153, 154, 155, 156, 157, 158, 159, 160, 161, 162, 163, 164, 165,
    166, 167, 168, 169, 170, 171, 172, 173, 174, 175, 176, 177, 178, 179, 180,
    181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191, 192, 193, 194, 195,
    196, 197, 198, 199, 200, 201, 202, 203, 204, 205, 206, 207, 208, 209, 210,
    211, 212, 213, 214, 215, 216, 217, 218, 219, 220, 221, 222, 223, 224, 225,
    226, 227, 228, 229, 230, 231, 232, 233, 234, 235, 236, 237, 238, 239, 240,
    241, 242, 243, 244, 245, 246, 247, 248, 249, 250, 251, 252, 253, 254, 255,
};

char *ab;
int n = 15 * 1024;

void strspn_single_char_search_15k_init(void) {
  ab = Calloc(n + 2, sizeof(char));
  memset(ab, 'a', n);
  ab[n] = 'b';
}
void strspn_single_char_search_15k(void) {
  npassert(strspn(ab, "a") == n);
}

void strspn_everything_15k_init(void) {
  ab = Calloc(n + 1, sizeof(char));
  for (int i = 0; i < n; ++i) {
    ab[i] = rand();
    if (!ab[i])
      ++ab[i];
  }
}
void strspn_everything_15k(void) {
  npassert(strspn(ab, everything) == n);
}

int main(int argc, char *argv[]) {
  bench(strspn_single_char_search_15k);
  bench(strspn_everything_15k);
  test();
}
