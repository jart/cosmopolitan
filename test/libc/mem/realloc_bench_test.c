// we represent the lollipop guild

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define N 20

int64_t now(void) {
#ifdef _WIN32
  uint64_t t;
  QueryUnbiasedInterruptTimePrecise(&t);
  return t * 100;
#else
  struct timespec ts;
  clock_gettime(CLOCK_MONOTONIC, &ts);
  return ts.tv_sec * 1000000000ll + ts.tv_nsec;
#endif
}

void* id(void* p) {
  return p;
}

void* (*veil)(void*) = id;

int main(int argc, char* argv[]) {
#ifndef TINY
#ifndef MODE_DBG
  int64_t start = now();
  for (size_t z = 1; z <= 256 * 1024 * 1024; z <<= 1) {
    char* p[N][2];
    for (int i = 0; i < N; ++i) {
      p[i][0] = malloc(z);
      p[i][1] = malloc(z);
    }
    int64_t start = now();
    for (int i = 0; i < N; ++i)
      p[N - i - 1][0] = veil(realloc(p[N - i - 1][0], z * 2));
    printf("realloc %12zu -> %12zu took %12ld ns\n", z, z * 2,
           (now() - start) / N);
    for (int i = 0; i < N; ++i) {
      free(p[i][1]);
      free(p[i][0]);
    }
  }
  if (now() - start > 10000000000)
    exit(23);
#else
  fprintf(stderr, "skipping realloc_bench_test in debug mode\n");
#endif
#else
  fprintf(stderr, "skipping realloc_bench_test in tiny mode\n");
#endif
}
