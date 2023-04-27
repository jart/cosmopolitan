#if 0
/*─────────────────────────────────────────────────────────────────╗
│ To the extent possible under law, Justine Tunney has waived      │
│ all copyright and related or neighboring rights to this file,    │
│ as it is written in the following disclaimers:                   │
│   • http://unlicense.org/                                        │
│   • http://creativecommons.org/publicdomain/zero/1.0/            │
╚─────────────────────────────────────────────────────────────────*/
#endif
#include "third_party/vqsort/vqsort.h"
#include "libc/macros.internal.h"
#include "libc/stdio/stdio.h"
#include "third_party/vqsort/vqsort.h"

// how to sort one gigabyte of 64-bit integers per second

int main(int argc, char *argv[]) {
  int64_t A[] = {9, 3, -3, 5, 23, 7};
  vqsort_int64(A, ARRAYLEN(A));
  for (int i = 0; i < ARRAYLEN(A); ++i) {
    if (i) printf(" ");
    printf("%ld", A[i]);
  }
  printf("\n");
}
