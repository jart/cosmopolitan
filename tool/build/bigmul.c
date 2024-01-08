/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2021 Justine Alexandra Roberts Tunney                              │
│                                                                              │
│ Permission to use, copy, modify, and/or distribute this software for         │
│ any purpose with or without fee is hereby granted, provided that the         │
│ above copyright notice and this permission notice appear in all copies.      │
│                                                                              │
│ THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL                │
│ WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED                │
│ WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE             │
│ AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL         │
│ DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR        │
│ PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER               │
│ TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR             │
│ PERFORMANCE OF THIS SOFTWARE.                                                │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "libc/assert.h"
#include "libc/fmt/conv.h"
#include "libc/log/log.h"
#include "libc/macros.internal.h"
#include "libc/mem/gc.h"
#include "libc/mem/mem.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"

/**
 * @fileoverview cryptographic multiplication kernel generator
 */

void PrintMultiplyKernel(int n, int m) {
  bool cf, of;
  bool *Rs, *Ra;
  int j, i, k1, k2, g;
  printf("\
/**\n\
 * Computes %d-bit product of %d-bit and %d-bit numbers.\n\
 *\n\
 * @param C receives %d quadword result\n\
 * @param A is left hand side which must have %d quadwords\n\
 * @param B is right hand side which must have %d quadwords\n\
 * @note words are host endian while array is little endian\n\
 * @mayalias\n\
 */\n\
void Multiply%dx%d(uint64_t C[%d], const uint64_t A[%d], const uint64_t B[%d]) {\n\
  static bool cf,of;\n\
  uint64_t z,h,l;\n\
  uint64_t ",
         (n + m) * 64, n * 64, m * 64, n + m, n, m, n, m, n + m, n, m);
  Rs = gc(calloc(sizeof(*Rs), n + m + 1));
  Ra = gc(calloc(sizeof(*Ra), n + m + 1));
  for (j = 0; j < n; ++j) {
    if (j) printf(", ");
    printf("H%d", j);
  }
  printf(";\n");
  printf("  uint64_t ");
  for (j = 0; j < n + m; ++j) {
    if (j) printf(", ");
    printf("R%d", j);
  }
  printf(";\n");
  for (j = 0; j < m; ++j) {
    (printf)("\
  asm(\"xorl\\t%%k0,%%k0\" : \"=r\"(z), \"+m\"(cf), \"+m\"(of));\n",
             j);
    for (cf = of = false, i = 0; i < n; ++i) {
      if (!i) {
        if (!Rs[i + j] && !Rs[i + j + 1]) {
          assert(!cf);
          assert(!of);
          Rs[i + j + 0] = true;
          Rs[i + j + 1] = true;
          (printf)("\
  asm(\"mulx\\t%%2,%%1,%%0\" : \"=r\"(R%d), \"=r\"(R%d) : \"rm\"(A[%d]), \"d\"(B[%d]));\n",
                   i + j + 1, i + j, i, j);
        } else if (!Rs[i + j + 1]) {
          of = true;
          assert(!cf);
          Ra[i + j + 0] = true;
          Rs[i + j + 1] = true;
          (printf)("\
  asm(\"mulx\\t%%2,%%1,%%0\" : \"=r\"(R%d), \"=r\"(l) : \"rm\"(A[%d]), \"d\"(B[%d]));\n\
  asm(\"adox\\t%%2,%%0\" : \"+r\"(R%d), \"+m\"(of) : \"r\"(l));\n",
                   i + j + 1, i, j, i + j);
        } else {
          cf = true;
          of = true;
          assert(Rs[i + j]);
          Ra[i + j + 0] = true;
          Ra[i + j + 1] = true;
          (printf)("\
  asm(\"mulx\\t%%2,%%1,%%0\" : \"=r\"(h), \"=r\"(l) : \"rm\"(A[%d]), \"d\"(B[%d]));\n\
  asm(\"adox\\t%%2,%%0\" : \"+r\"(R%d), \"+m\"(of) : \"r\"(l));\n\
  asm(\"adcx\\t%%2,%%0\" : \"+r\"(R%d), \"+m\"(cf) : \"r\"(h));\n",
                   i, j, i + j, i + j + 1);
        }
      } else {
        assert(Rs[i + j]);
        if (!Rs[i + j + 1]) {
          if (cf) {
            of = true;
            cf = false;
            Ra[i + j + 0] = true;
            Rs[i + j + 1] = true;
            Ra[i + j + 1] = false;
            (printf)("\
  asm(\"mulx\\t%%2,%%1,%%0\" : \"=r\"(R%d), \"=r\"(l) : \"rm\"(A[%d]), \"d\"(B[%d]));\n\
  asm(\"adox\\t%%2,%%0\" : \"+r\"(R%d), \"+m\"(of) : \"r\"(l));\n\
  asm(\"adcx\\t%%2,%%0\" : \"+r\"(R%d), \"+m\"(cf) : \"r\"(z));\n",
                     i + j + 1, i, j, i + j, i + j + 1);
          } else {
            of = true;
            Ra[i + j + 0] = true;
            Rs[i + j + 1] = true;
            Ra[i + j + 1] = false;
            (printf)("\
  asm(\"mulx\\t%%2,%%1,%%0\" : \"=r\"(R%d), \"=r\"(l) : \"rm\"(A[%d]), \"d\"(B[%d]));\n\
  asm(\"adox\\t%%2,%%0\" : \"+r\"(R%d), \"+m\"(of) : \"r\"(l));\n",
                     i + j + 1, i, j, i + j);
          }
        } else {
          of = true;
          cf = true;
          Ra[i + j + 0] = true;
          Ra[i + j + 1] = true;
          (printf)("\
  asm(\"mulx\\t%%2,%%1,%%0\" : \"=r\"(h), \"=r\"(l) : \"rm\"(A[%d]), \"d\"(B[%d]));\n\
  asm(\"adox\\t%%2,%%0\" : \"+r\"(R%d), \"+m\"(of) : \"r\"(l));\n\
  asm(\"adcx\\t%%2,%%0\" : \"+r\"(R%d), \"+m\"(cf) : \"rm\"(h));\n",
                   i, j, i + j, i + j + 1);
        }
      }
    }
    k1 = 0;
    if (of) {
      for (;; ++k1) {
        (printf)("\
  asm(\"adox\\t%%2,%%0\" : \"+r\"(R%d), \"+m\"(of) : \"r\"(z));\n",
                 i + j + k1);
        if (!Rs[i + j + k1]) {
          break;
        }
        if (!Ra[i + j + k1]) {
          break;
        }
      }
    }
    k2 = 0;
    if (cf) {
      for (;; ++k2) {
        (printf)("\
  asm(\"adcx\\t%%2,%%0\" : \"+r\"(R%d), \"+m\"(cf) : \"r\"(z));\n",
                 i + j + k2);
        if (!Rs[i + j + k2]) {
          break;
        }
        if (!Ra[i + j + k2]) {
          break;
        }
      }
    }
    for (g = 0; g < MAX(k1, k2); ++g) {
      Rs[i + j + g] = true;
    }
  }
  for (j = 0; j < n + m; ++j) {
    printf("  C[%d] = R%d;\n", j, j);
  }
  printf("}\n");
  fflush(stdout);
}

int main(int argc, char *argv[]) {
  int n, m;
  if (argc != 3 || (n = atoi(argv[1])) <= 0 || (m = atoi(argv[2])) <= 0) {
    fprintf(stderr, "Usage: %s LHS-LIMBS RHS-LIMBS\n", argv[0]);
    return 1;
  }
  PrintMultiplyKernel(n, m);
  return 0;
}
