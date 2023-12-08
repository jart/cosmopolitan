/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:4;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=4 sts=4 sw=4 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright (c) 2008-2016 Stefan Krah. All rights reserved.                    │
│                                                                              │
│ Redistribution and use in source and binary forms, with or without           │
│ modification, are permitted provided that the following conditions           │
│ are met:                                                                     │
│                                                                              │
│ 1. Redistributions of source code must retain the above copyright            │
│    notice, this list of conditions and the following disclaimer.             │
│                                                                              │
│ 2. Redistributions in binary form must reproduce the above copyright         │
│    notice, this list of conditions and the following disclaimer in           │
│    the documentation and/or other materials provided with the                │
│    distribution.                                                             │
│                                                                              │
│ THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS "AS IS" AND         │
│ ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE        │
│ IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR           │
│ PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS         │
│ BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY,          │
│ OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT         │
│ OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR           │
│ BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,        │
│ WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE         │
│ OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,            │
│ EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.                           │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "third_party/python/Modules/_decimal/libmpdec/fourstep.h"
#include "third_party/python/Modules/_decimal/libmpdec/mpdecimal.h"
#include "third_party/python/Modules/_decimal/libmpdec/numbertheory.h"
#include "third_party/python/Modules/_decimal/libmpdec/sixstep.h"
#include "third_party/python/Modules/_decimal/libmpdec/transpose.h"
#include "third_party/python/Modules/_decimal/libmpdec/umodarith.h"

asm(".ident\t\"\\n\\n\
libmpdec (BSD-2)\\n\
Copyright 2008-2016 Stefan Krah\"");
asm(".include \"libc/disclaimer.inc\"");

/*
                 Cache Efficient Matrix Fourier Transform
                        for arrays of form 3×2ⁿ


The Matrix Fourier Transform
════════════════════════════

In libmpdec, the Matrix Fourier Transform [1] is called four-step
transform after a variant that appears in [2]. The algorithm requires
that the input array can be viewed as an R*C matrix.

All operations are done modulo p. For readability, the proofs drop all
instances of (mod p).


Algorithm four-step (forward transform)
───────────────────────────────────────

  a := input array
  d := len(a) = R * C
  p := prime
  w := primitive root of unity of the prime field
  r := w**((p-1)/d)
  A := output array

  1) Apply a length R FNT to each column.

  2) Multiply each matrix element (addressed by j*C+m) by r**(j*m).

  3) Apply a length C FNT to each row.

  4) Transpose the matrix.


Proof (forward transform)
─────────────────────────

  The algorithm can be derived starting from the regular definition of
  the finite-field transform of length d:

            d-1
           ,────
           \
    A[k] =  |  a[l]  × r**(k × l)
           /
           `────
           l = 0


  The sum can be rearranged into the sum of the sums of columns:

            C-1     R-1
           ,────   ,────
           \       \
         =  |       |  a[i × C + j] × r**(k × (i × C + j))
           /       /
           `────   `────
           j = 0   i = 0


  Extracting a constant from the inner sum:

            C-1           R-1
           ,────         ,────
           \             \
         =  |  rᵏ×j    ×  |  a[i × C + j] × r**(k × i × C)
           /             /
           `────         `────
           j = 0         i = 0


  Without any loss of generality, let k = n × R + m,
  where n < C and m < R:

                C-1                          R-1
               ,────                        ,────
               \                            \
    A[n×R+m] =  |  r**(R×n×j) × r**(m×j)  ×  |  a[i×C+j] × r**(R×C×n×i) × r**(C×m×i)
               /                            /
               `────                        `────
               j = 0                        i = 0


  Since r = w ** ((p-1) / (R×C)):

     a) r**(R×C×n×i) = w**((p-1)×n×i) = 1

     b) r**(C×m×i) = w**((p-1) / R) ** (m×i) = r_R ** (m×i)

     c) r**(R×n×j) = w**((p-1) / C) ** (n×j) = r_C ** (n×j)

     r_R := root of the subfield of length R.
     r_C := root of the subfield of length C.


                C-1                             R-1
               ,────                           ,────
               \                               \
    A[n×R+m] =  |  r_C**(n×j) × [ r**(m×j)  ×   |  a[i×C+j] × r_R**(m×i) ]
               /                     ^         /
               `────                 |         `────    1) transform the columns
               j = 0                 |         i = 0
                 ^                   |
                 |                   `-- 2) multiply
                 |
                 `-- 3) transform the rows


   Note that the entire RHS is a function of n and m and that the results
   for each pair (n, m) are stored in Fortran order.

   Let the term in square brackets be 𝑓(m, j). Step 1) and 2) precalculate
   the term for all (m, j). After that, the original matrix is now a lookup
   table with the mth element in the jth column at location m × C + j.

   Let the complete RHS be g(m, n). Step 3) does an in-place transform of
   length n on all rows. After that, the original matrix is now a lookup
   table with the mth element in the nth column at location m × C + n.

   But each (m, n) pair should be written to location n × R + m. Therefore,
   step 4) transposes the result of step 3).



Algorithm four-step (inverse transform)
───────────────────────────────────────

  A  := input array
  d  := len(A) = R × C
  p  := prime
  d′ := d⁽ᵖ⁻²⁾               # inverse of d
  w  := primitive root of unity of the prime field
  r  := w**((p-1)/d)         # root of the subfield
  r′ := w**((p-1) - (p-1)/d) # inverse of r
  a  := output array

  0) View the matrix as a C×R matrix.

  1) Transpose the matrix, producing an R×C matrix.

  2) Apply a length C FNT to each row.

  3) Multiply each matrix element (addressed by i×C+n) by r**(i×n).

  4) Apply a length R FNT to each column.


Proof (inverse transform)
─────────────────────────

  The algorithm can be derived starting from the regular definition of
  the finite-field inverse transform of length d:

                  d-1
                 ,────
                 \
    a[k] =  d′ ×  |  A[l]  × r′ ** (k × l)
                 /
                 `────
                 l = 0


  The sum can be rearranged into the sum of the sums of columns. Note
  that at this stage we still have a C*R matrix, so C denotes the number
  of rows:

                  R-1     C-1
                 ,────   ,────
                 \       \
         =  d′ ×  |       |  a[j × R + i] × r′ ** (k × (j × R + i))
                 /       /
                 `────   `────
                 i = 0   j = 0


  Extracting a constant from the inner sum:

                  R-1                C-1
                 ,────              ,────
                 \                  \
         =  d′ ×  |  r′ ** (k×i)  ×  |  a[j × R + i] × r′ ** (k × j × R)
                 /                  /
                 `────              `────
                 i = 0              j = 0


  Without any loss of generality, let k = m * C + n,
  where m < R and n < C:

                     R-1                                  C-1
                    ,────                                ,────
                    \                                    \
    A[m×C+n] = d′ ×  |  r′ ** (C×m×i) ×  r′ ** (n×i)   ×  |  a[j×R+i] × r′ ** (R×C×m×j) × r′ ** (R×n×j)
                    /                                    /
                    `────                                `────
                    i = 0                                j = 0


  Since r′ = w**((p-1) - (p-1)/d) and d = R×C:

     a) r′ ** (R×C×m×j) = w**((p-1)×R×C×m×j - (p-1)×m×j) = 1

     b) r′ ** (C×m×i) = w**((p-1)×C - (p-1)/R) ** (m×i) = r_R′ ** (m×i)

     c) r′ ** (R×n×j) = r_C′ ** (n×j)

     d) d′ = d⁽ᵖ⁻²⁾ = (R×C)⁽ᵖ⁻²⁾ = R⁽ᵖ⁻²⁾ × C⁽ᵖ⁻²⁾ = R′ × C′

     r_R′ := inverse of the root of the subfield of length R.
     r_C′ := inverse of the root of the subfield of length C.
     R′   := inverse of R
     C′   := inverse of C


                     R-1                                      C-1
                    ,────                                    ,────  2) transform the rows of a^T
                    \                                        \
    A[m×C+n] = R′ ×  |  r_R′ ** (m×i) × [ r′ ** (n×i) × C′ ×  |  a[j×R+i] × r_C′ ** (n×j) ]
                    /                           ^            /       ^
                    `────                       |            `────   |
                    i = 0                       |            j = 0   |
                      ^                         |                    `── 1) Transpose input matrix
                      |                         `── 3) multiply             to address elements by
                      |                                                     i × C + j
                      `── 3) transform the columns



   Note that the entire RHS is a function of m and n and that the results
   for each pair (m, n) are stored in C order.

   Let the term in square brackets be 𝑓(n, i). Without step 1), the sum
   would perform a length C transform on the columns of the input matrix.
   This is a) inefficient and b) the results are needed in C order, so
   step 1) exchanges rows and columns.

   Step 2) and 3) precalculate 𝑓(n, i) for all (n, i). After that, the
   original matrix is now a lookup table with the ith element in the nth
   column at location i × C + n.

   Let the complete RHS be g(m, n). Step 4) does an in-place transform of
   length m on all columns. After that, the original matrix is now a lookup
   table with the mth element in the nth column at location m × C + n,
   which means that all A[k] = A[m × C + n] are in the correct order.


──

  [1] Joerg Arndt: "Matters Computational"
      http://www.jjj.de/fxt/
  [2] David H. Bailey: FFTs in External or Hierarchical Memory
      http://crd.lbl.gov/~dhbailey/dhbpapers/
*/

static inline void
std_size3_ntt(mpd_uint_t *x1, mpd_uint_t *x2, mpd_uint_t *x3,
              mpd_uint_t w3table[3], mpd_uint_t umod)
{
    mpd_uint_t r1, r2;
    mpd_uint_t w;
    mpd_uint_t s, tmp;
    /* k = 0 -> w = 1 */
    s = *x1;
    s = addmod(s, *x2, umod);
    s = addmod(s, *x3, umod);
    r1 = s;
    /* k = 1 */
    s = *x1;
    w = w3table[1];
    tmp = MULMOD(*x2, w);
    s = addmod(s, tmp, umod);
    w = w3table[2];
    tmp = MULMOD(*x3, w);
    s = addmod(s, tmp, umod);
    r2 = s;
    /* k = 2 */
    s = *x1;
    w = w3table[2];
    tmp = MULMOD(*x2, w);
    s = addmod(s, tmp, umod);
    w = w3table[1];
    tmp = MULMOD(*x3, w);
    s = addmod(s, tmp, umod);
    *x3 = s;
    *x2 = r2;
    *x1 = r1;
}

/* forward transform, sign = -1; transform length = 3 * 2**n */
int
four_step_fnt(mpd_uint_t *a, mpd_size_t n, int modnum)
{
    mpd_size_t R = 3; /* number of rows */
    mpd_size_t C = n / 3; /* number of columns */
    mpd_uint_t w3table[3];
    mpd_uint_t kernel, w0, w1, wstep;
    mpd_uint_t *s, *p0, *p1, *p2;
    mpd_uint_t umod;
    mpd_size_t i, k;
    assert(n >= 48);
    assert(n <= 3*MPD_MAXTRANSFORM_2N);
    /* Length R transform on the columns. */
    SETMODULUS(modnum);
    _mpd_init_w3table(w3table, -1, modnum);
    for (p0=a, p1=p0+C, p2=p0+2*C; p0<a+C; p0++,p1++,p2++) {
        SIZE3_NTT(p0, p1, p2, w3table);
    }
    /* Multiply each matrix element (addressed by i*C+k) by r**(i*k). */
    kernel = _mpd_getkernel(n, -1, modnum);
    for (i = 1; i < R; i++) {
        w0 = 1;                  /* r**(i*0): initial value for k=0 */
        w1 = POWMOD(kernel, i);  /* r**(i*1): initial value for k=1 */
        wstep = MULMOD(w1, w1);  /* r**(2*i) */
        for (k = 0; k < C-1; k += 2) {
            mpd_uint_t x0 = a[i*C+k];
            mpd_uint_t x1 = a[i*C+k+1];
            MULMOD2(&x0, w0, &x1, w1);
            MULMOD2C(&w0, &w1, wstep);  /* r**(i*(k+2)) = r**(i*k) * r**(2*i) */
            a[i*C+k] = x0;
            a[i*C+k+1] = x1;
        }
    }
    /* Length C transform on the rows. */
    for (s = a; s < a+n; s += C) {
        if (!six_step_fnt(s, C, modnum)) {
            return 0;
        }
    }
#if 0
    /* An unordered transform is sufficient for convolution. */
    /* Transpose the matrix. */
    transpose_3xpow2(a, R, C);
#endif
    return 1;
}

/* backward transform, sign = 1; transform length = 3 * 2**n */
int
inv_four_step_fnt(mpd_uint_t *a, mpd_size_t n, int modnum)
{
    mpd_size_t R = 3; /* number of rows */
    mpd_size_t C = n / 3; /* number of columns */
    mpd_uint_t w3table[3];
    mpd_uint_t kernel, w0, w1, wstep;
    mpd_uint_t *s, *p0, *p1, *p2;
    mpd_uint_t umod;
    mpd_size_t i, k;
    assert(n >= 48);
    assert(n <= 3*MPD_MAXTRANSFORM_2N);
#if 0
    /* An unordered transform is sufficient for convolution. */
    /* Transpose the matrix, producing an R*C matrix. */
    transpose_3xpow2(a, C, R);
#endif
    /* Length C transform on the rows. */
    for (s = a; s < a+n; s += C) {
        if (!inv_six_step_fnt(s, C, modnum)) {
            return 0;
        }
    }
    /* Multiply each matrix element (addressed by i*C+k) by r**(i*k). */
    SETMODULUS(modnum);
    kernel = _mpd_getkernel(n, 1, modnum);
    for (i = 1; i < R; i++) {
        w0 = 1;
        w1 = POWMOD(kernel, i);
        wstep = MULMOD(w1, w1);
        for (k = 0; k < C; k += 2) {
            mpd_uint_t x0 = a[i*C+k];
            mpd_uint_t x1 = a[i*C+k+1];
            MULMOD2(&x0, w0, &x1, w1);
            MULMOD2C(&w0, &w1, wstep);
            a[i*C+k] = x0;
            a[i*C+k+1] = x1;
        }
    }
    /* Length R transform on the columns. */
    _mpd_init_w3table(w3table, 1, modnum);
    for (p0=a, p1=p0+C, p2=p0+2*C; p0<a+C; p0++,p1++,p2++) {
        SIZE3_NTT(p0, p1, p2, w3table);
    }
    return 1;
}
