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
#include "third_party/python/Modules/_decimal/libmpdec/bits.h"
#include "third_party/python/Modules/_decimal/libmpdec/difradix2.h"
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
                          for arrays of form 2ⁿ


The Six Step Transform
══════════════════════

In libmpdec, the six-step transform is the Matrix Fourier Transform in
disguise. It is called six-step transform after a variant that appears
in [1]. The algorithm requires that the input array can be viewed as an
R×C matrix.


Algorithm six-step (forward transform)
──────────────────────────────────────

  1a) Transpose the matrix.

  1b) Apply a length R FNT to each row.

  1c) Transpose the matrix.

  2) Multiply each matrix element (addressed by j×C+m) by r**(j×m).

  3) Apply a length C FNT to each row.

  4) Transpose the matrix.

Note that steps 1a) - 1c) are exactly equivalent to step 1) of the Matrix
Fourier Transform. For large R, it is faster to transpose twice and do
a transform on the rows than to perform a column transpose directly.


Algorithm six-step (inverse transform)
──────────────────────────────────────

  0) View the matrix as a C×R matrix.

  1) Transpose the matrix, producing an R×C matrix.

  2) Apply a length C FNT to each row.

  3) Multiply each matrix element (addressed by i×C+n) by r**(i×n).

  4a) Transpose the matrix.

  4b) Apply a length R FNT to each row.

  4c) Transpose the matrix.

Again, steps 4a) - 4c) are equivalent to step 4) of the Matrix Fourier
Transform.


──

  [1] David H. Bailey: FFTs in External or Hierarchical Memory
      http://crd.lbl.gov/~dhbailey/dhbpapers/
*/

/* forward transform with sign = -1 */
int
six_step_fnt(mpd_uint_t *a, mpd_size_t n, int modnum)
{
    struct fnt_params *tparams;
    mpd_size_t log2n, C, R;
    mpd_uint_t kernel;
    mpd_uint_t umod;
    mpd_uint_t *x, w0, w1, wstep;
    mpd_size_t i, k;
    assert(ispower2(n));
    assert(n >= 16);
    assert(n <= MPD_MAXTRANSFORM_2N);
    log2n = mpd_bsr(n);
    C = ((mpd_size_t)1) << (log2n / 2);  /* number of columns */
    R = ((mpd_size_t)1) << (log2n - (log2n / 2)); /* number of rows */
    /* Transpose the matrix. */
    if (!transpose_pow2(a, R, C)) {
        return 0;
    }
    /* Length R transform on the rows. */
    if ((tparams = _mpd_init_fnt_params(R, -1, modnum)) == NULL) {
        return 0;
    }
    for (x = a; x < a+n; x += R) {
        fnt_dif2(x, R, tparams);
    }
    /* Transpose the matrix. */
    if (!transpose_pow2(a, C, R)) {
        mpd_free(tparams);
        return 0;
    }
    /* Multiply each matrix element (addressed by i*C+k) by r**(i*k). */
    SETMODULUS(modnum);
    kernel = _mpd_getkernel(n, -1, modnum);
    for (i = 1; i < R; i++) {
        w0 = 1;                  /* r**(i*0): initial value for k=0 */
        w1 = POWMOD(kernel, i);  /* r**(i*1): initial value for k=1 */
        wstep = MULMOD(w1, w1);  /* r**(2*i) */
        for (k = 0; k < C; k += 2) {
            mpd_uint_t x0 = a[i*C+k];
            mpd_uint_t x1 = a[i*C+k+1];
            MULMOD2(&x0, w0, &x1, w1);
            MULMOD2C(&w0, &w1, wstep);  /* r**(i*(k+2)) = r**(i*k) * r**(2*i) */
            a[i*C+k] = x0;
            a[i*C+k+1] = x1;
        }
    }
    /* Length C transform on the rows. */
    if (C != R) {
        mpd_free(tparams);
        if ((tparams = _mpd_init_fnt_params(C, -1, modnum)) == NULL) {
            return 0;
        }
    }
    for (x = a; x < a+n; x += C) {
        fnt_dif2(x, C, tparams);
    }
    mpd_free(tparams);
#if 0
    /* An unordered transform is sufficient for convolution. */
    /* Transpose the matrix. */
    if (!transpose_pow2(a, R, C)) {
        return 0;
    }
#endif
    return 1;
}

/* reverse transform, sign = 1 */
int
inv_six_step_fnt(mpd_uint_t *a, mpd_size_t n, int modnum)
{
    struct fnt_params *tparams;
    mpd_size_t log2n, C, R;
    mpd_uint_t kernel;
    mpd_uint_t umod;
    mpd_uint_t *x, w0, w1, wstep;
    mpd_size_t i, k;
    assert(ispower2(n));
    assert(n >= 16);
    assert(n <= MPD_MAXTRANSFORM_2N);
    log2n = mpd_bsr(n);
    C = ((mpd_size_t)1) << (log2n / 2); /* number of columns */
    R = ((mpd_size_t)1) << (log2n - (log2n / 2)); /* number of rows */
#if 0
    /* An unordered transform is sufficient for convolution. */
    /* Transpose the matrix, producing an R*C matrix. */
    if (!transpose_pow2(a, C, R)) {
        return 0;
    }
#endif
    /* Length C transform on the rows. */
    if ((tparams = _mpd_init_fnt_params(C, 1, modnum)) == NULL) {
        return 0;
    }
    for (x = a; x < a+n; x += C) {
        fnt_dif2(x, C, tparams);
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
    /* Transpose the matrix. */
    if (!transpose_pow2(a, R, C)) {
        mpd_free(tparams);
        return 0;
    }
    /* Length R transform on the rows. */
    if (R != C) {
        mpd_free(tparams);
        if ((tparams = _mpd_init_fnt_params(R, 1, modnum)) == NULL) {
            return 0;
        }
    }
    for (x = a; x < a+n; x += R) {
        fnt_dif2(x, R, tparams);
    }
    mpd_free(tparams);
    /* Transpose the matrix. */
    if (!transpose_pow2(a, C, R)) {
        return 0;
    }
    return 1;
}
