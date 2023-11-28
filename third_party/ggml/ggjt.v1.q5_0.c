/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:4;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=4 sts=4 sw=4 fenc=utf-8                                :vi│
╚──────────────────────────────────────────────────────────────────────────────╝
│                                                                              │
│  GGML                                                                        │
│  Copyright (c) 2023 Georgi Gerganov                                          │
│                                                                              │
│  Permission is hereby granted, free of charge, to any person obtaining       │
│  a copy of this software and associated documentation files (the             │
│  "Software"), to deal in the Software without restriction, including         │
│  without limitation the rights to use, copy, modify, merge, publish,         │
│  distribute, sublicense, and/or sell copies of the Software, and to          │
│  permit persons to whom the Software is furnished to do so, subject to       │
│  the following conditions:                                                   │
│                                                                              │
│  The above copyright notice and this permission notice shall be              │
│  included in all copies or substantial portions of the Software.             │
│                                                                              │
│  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,             │
│  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF          │
│  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.      │
│  IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY        │
│  CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,        │
│  TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE           │
│  SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                      │
│                                                                              │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "third_party/ggml/ggjt.v1.q5_0.h"
#include "libc/assert.h"
#include "libc/macros.internal.h"
#include "third_party/aarch64/arm_neon.internal.h"
#include "third_party/ggml/fp16.internal.h"
#include "third_party/ggml/ggjt.v1.internal.h"
#include "third_party/ggml/ggjt.v1.q8_0.h"

static_assert(sizeof(block_v1_q5_0) ==
                  sizeof(ggml_fp16_t) + sizeof(uint32_t) + V1_QK5_0 / 2,
              "wrong q5_0 block size/padding");

void quantize_row_v1_q5_0_reference(const float * restrict x, block_v1_q5_0 * restrict y, int k) {
    assert(k % V1_QK5_0 == 0);
    const int nb = k / V1_QK5_0;

    for (int i = 0; i < nb; i++) {
        float amax = 0.0f; // absolute max
        float max = 0.0f;

        for (int l = 0; l < V1_QK5_0; l++) {
            const float v = x[i*V1_QK5_0 + l];
            if (amax < fabsf(v)) {
                amax = fabsf(v);
                max = v;
            }
        }

        const float d = max / -16;
        const float id = d ? 1.0f/d : 0.0f;

        y[i].d = GGML_FP32_TO_FP16(d);

        uint32_t qh = 0;

        for (int l = 0; l < V1_QK5_0; l += 2) {
            const float v0 = x[i*V1_QK5_0 + l + 0]*id;
            const float v1 = x[i*V1_QK5_0 + l + 1]*id;

            const uint32_t vi0 = MIN(31, (int) (v0 + 16.5f));
            const uint32_t vi1 = MIN(31, (int) (v1 + 16.5f));

            y[i].qs[l/2] = (vi0 & 0x0F) | ((vi1 & 0x0F) << 4);

            // get the 5-th bit and store it in qh at the right position
            qh |= ((vi0 & 0x10) >> 4) << (l + 0);
            qh |= ((vi1 & 0x10) >> 4) << (l + 1);
        }

        memcpy(&y[i].qh, &qh, sizeof(y[i].qh));
    }
}

void quantize_row_v1_q5_0(const float * restrict x, void * restrict vy, int k) {
    assert(k % V1_QK5_0 == 0);

    block_v1_q5_0 * restrict y = vy;

    quantize_row_v1_q5_0_reference(x, y, k);
}

size_t ggml_quantize_v1_q5_0(const float * src, void * dst, int n, int k, int64_t * hist) {
    assert(k % V1_QK5_0 == 0);
    const int nb = k / V1_QK5_0;

    for (int j = 0; j < n; j += k) {
        block_v1_q5_0 * restrict y = (block_v1_q5_0 *)dst + j/V1_QK5_0;

        quantize_row_v1_q5_0_reference(src + j, y, k);

        for (int i = 0; i < nb; i++) {
            uint32_t qh;
            memcpy(&qh, &y[i].qh, sizeof(qh));

            for (int l = 0; l < V1_QK5_0; l += 2) {
                const uint8_t vh0 = ((qh & (1u << (l + 0))) >> (l + 0)) << 4;
                const uint8_t vh1 = ((qh & (1u << (l + 1))) >> (l + 1)) << 4;

                // cast to 16 bins
                const uint8_t vi0 = ((y[i].qs[l/2] & 0x0F) | vh0) / 2;
                const uint8_t vi1 = ((y[i].qs[l/2] >>   4) | vh1) / 2;

                hist[vi0]++;
                hist[vi1]++;
            }
        }
    }

    return (n/V1_QK5_0*sizeof(block_v1_q5_0));
}

void dequantize_row_v1_q5_0(const void * restrict vx, float * restrict y, int k) {
    assert(k % V1_QK5_0 == 0);
    const int nb = k / V1_QK5_0;

    const block_v1_q5_0 * restrict x = vx;

    for (int i = 0; i < nb; i++) {
        const float d = GGML_FP16_TO_FP32(x[i].d);

        const uint8_t * restrict pp = x[i].qs;

        uint32_t qh;
        memcpy(&qh, x[i].qh, sizeof(qh));

        for (int l = 0; l < V1_QK5_0; l += 2) {
            const uint8_t vi = pp[l/2];

            // extract the 5-th bit from qh
            const uint8_t vh0 = ((qh & (1u << (l + 0))) >> (l + 0)) << 4;
            const uint8_t vh1 = ((qh & (1u << (l + 1))) >> (l + 1)) << 4;

            const int8_t vi0 = (vi & 0x0F) | vh0;
            const int8_t vi1 = (vi >>   4) | vh1;

            const float v0 = (vi0 - 16)*d;
            const float v1 = (vi1 - 16)*d;

            y[i*V1_QK5_0 + l + 0] = v0;
            y[i*V1_QK5_0 + l + 1] = v1;

            assert(!isnan(y[i*V1_QK5_0 + l + 0]));
            assert(!isnan(y[i*V1_QK5_0 + l + 1]));
        }
    }
}

void ggml_vec_dot_v1_q5_0_q8_0(const int n, float * restrict s, const void * restrict vx, const void * restrict vy) {
    const int nb = n / V1_QK8_0;

    assert(n % V1_QK8_0 == 0);
    assert(nb % 2 == 0);
    assert(V1_QK8_0 == V1_QK5_0);

    const block_v1_q5_0 * restrict x = vx;
    const block_v1_q8_0 * restrict y = vy;

#if defined(__ARM_NEON)
    float32x4_t sumv = vdupq_n_f32(0.0f);

    uint64_t tmp[4];

    for (int i = 0; i < nb; ++i) {
        const block_v1_q5_0 * restrict x0 = &x[i];
        const block_v1_q8_0 * restrict y0 = &y[i];

        const uint8x16_t m4b  = vdupq_n_u8(0x0F);
        const int8x16_t  s16b = vdupq_n_s8(0x10);

        // extract the 5th bit
        uint32_t qh;
        memcpy(&qh, x0->qh, sizeof(qh));

        tmp[0] = table_b2b_0[(qh >>  0) & 0xFF];
        tmp[1] = table_b2b_0[(qh >>  8) & 0xFF];
        tmp[2] = table_b2b_0[(qh >> 16) & 0xFF];
        tmp[3] = table_b2b_0[(qh >> 24)       ];

        const int8x16_t qhl = vld1q_s8((const int8_t *)(tmp + 0));
        const int8x16_t qhh = vld1q_s8((const int8_t *)(tmp + 2));

        const uint8x16_t v0 = vld1q_u8(x0->qs);

        // 4-bit -> 8-bit
        const int8x16_t v0l = vreinterpretq_s8_u8(vandq_u8  (v0, m4b));
        const int8x16_t v0h = vreinterpretq_s8_u8(vshrq_n_u8(v0, 4));

        // interleave
        const int8x16_t v0lz = vzip1q_s8(v0l, v0h);
        const int8x16_t v0hz = vzip2q_s8(v0l, v0h);

        // add high bit and sub 16
        const int8x16_t v0lf = vsubq_s8(vorrq_s8(v0lz, qhl), s16b);
        const int8x16_t v0hf = vsubq_s8(vorrq_s8(v0hz, qhh), s16b);

        // load y
        const int8x16_t v1l = vld1q_s8(y0->qs);
        const int8x16_t v1h = vld1q_s8(y0->qs + 16);

        const float x0d = GGML_FP16_TO_FP32(x0->d);

#if defined(__ARM_FEATURE_DOTPROD)
        sumv = vmlaq_n_f32(sumv, vcvtq_f32_s32(vaddq_s32(
                        vdotq_s32(vdupq_n_s32(0), v0lf, v1l),
                        vdotq_s32(vdupq_n_s32(0), v0hf, v1h))), x0d*y0->d);
#else
        const int16x8_t pl0l = vmull_s8(vget_low_s8 (v0lf), vget_low_s8 (v1l));
        const int16x8_t pl0h = vmull_s8(vget_high_s8(v0lf), vget_high_s8(v1l));
        const int16x8_t ph0l = vmull_s8(vget_low_s8 (v0hf), vget_low_s8 (v1h));
        const int16x8_t ph0h = vmull_s8(vget_high_s8(v0hf), vget_high_s8(v1h));

        const int32x4_t pl0 = vaddq_s32(vpaddlq_s16(pl0l), vpaddlq_s16(pl0h));
        const int32x4_t ph0 = vaddq_s32(vpaddlq_s16(ph0l), vpaddlq_s16(ph0h));

        sumv = vmlaq_n_f32(sumv, vcvtq_f32_s32(vaddq_s32(pl0, ph0)), x0d*y0->d);
#endif
    }

    *s = vaddvq_f32(sumv);
#elif defined(__wasm_simd128__)
    v128_t sumv = wasm_f32x4_splat(0.0f);

    uint64_t tmp[4];

    for (int i = 0; i < nb; ++i) {
        const block_v1_q5_0 * restrict x0 = &x[i];
        const block_v1_q8_0 * restrict y0 = &y[i];

        const v128_t m4b  = wasm_i8x16_splat(0x0F);
        const v128_t s16b = wasm_i8x16_splat(0x10);

        // extract the 5th bit
        uint32_t qh;
        memcpy(&qh, x0->qh, sizeof(qh));

        tmp[0] = table_b2b_0[(qh >>  0) & 0xFF];
        tmp[1] = table_b2b_0[(qh >>  8) & 0xFF];
        tmp[2] = table_b2b_0[(qh >> 16) & 0xFF];
        tmp[3] = table_b2b_0[(qh >> 24)       ];

        const v128_t qhl = wasm_v128_load(tmp + 0);
        const v128_t qhh = wasm_v128_load(tmp + 2);

        const v128_t v0 = wasm_v128_load(x0->qs);

        // 4-bit -> 8-bit
        const v128_t v0l = wasm_v128_and (v0, m4b);
        const v128_t v0h = wasm_u8x16_shr(v0, 4);

        // interleave
        const v128_t v0lz = wasm_v8x16_shuffle(v0l, v0h,  0, 16,  1, 17,  2, 18,  3, 19,  4, 20,  5, 21,  6, 22,  7, 23);
        const v128_t v0hz = wasm_v8x16_shuffle(v0l, v0h,  8, 24,  9, 25, 10, 26, 11, 27, 12, 28, 13, 29, 14, 30, 15, 31);

        // add high bit and sub 16
        const v128_t v0lf = wasm_i8x16_sub(wasm_v128_or(v0lz, qhl), s16b);
        const v128_t v0hf = wasm_i8x16_sub(wasm_v128_or(v0hz, qhh), s16b);

        // load y
        const v128_t v1l = wasm_v128_load(y0->qs);
        const v128_t v1h = wasm_v128_load(y0->qs + 16);

        // int8x16 -> int16x8
        const v128_t v0lfl = wasm_i16x8_extend_low_i8x16 (v0lf);
        const v128_t v0lfh = wasm_i16x8_extend_high_i8x16(v0lf);
        const v128_t v0hfl = wasm_i16x8_extend_low_i8x16 (v0hf);
        const v128_t v0hfh = wasm_i16x8_extend_high_i8x16(v0hf);

        const v128_t v1ll = wasm_i16x8_extend_low_i8x16 (v1l);
        const v128_t v1lh = wasm_i16x8_extend_high_i8x16(v1l);
        const v128_t v1hl = wasm_i16x8_extend_low_i8x16 (v1h);
        const v128_t v1hh = wasm_i16x8_extend_high_i8x16(v1h);

        const float x0d = GGML_FP16_TO_FP32(x0->d);

        // dot product
        sumv = wasm_f32x4_add(sumv, wasm_f32x4_mul(wasm_f32x4_convert_i32x4(
                        wasm_i32x4_add(
                            wasm_i32x4_add(wasm_i32x4_dot_i16x8(v0lfl, v1ll),
                                           wasm_i32x4_dot_i16x8(v0lfh, v1lh)),
                            wasm_i32x4_add(wasm_i32x4_dot_i16x8(v0hfl, v1hl),
                                           wasm_i32x4_dot_i16x8(v0hfh, v1hh)))), wasm_f32x4_splat(x0d*y0->d)));
    }

    *s = wasm_f32x4_extract_lane(sumv, 0) + wasm_f32x4_extract_lane(sumv, 1) +
         wasm_f32x4_extract_lane(sumv, 2) + wasm_f32x4_extract_lane(sumv, 3);
#elif defined(__AVX2__)
    // Initialize accumulator with zeros
    __m256 acc = _mm256_setzero_ps();

    // Main loop
    for (int i = 0; i < nb; i++) {
        /* Compute combined scale for the block */
        const __m256 d = _mm256_mul_ps(_mm256_set1_ps(GGML_FP16_TO_FP32(x[i].d)), _mm256_broadcast_ss(&y[i].d));

        __m256i bx = bytes_from_nibbles_32(x[i].qs);
        __m256i bxhi = bytes_from_bits_32(x[i].qh);
        bxhi = _mm256_andnot_si256(bxhi, _mm256_set1_epi8((char)0xF0));
        bx = _mm256_or_si256(bx, bxhi);

        __m256i by = _mm256_loadu_si256((const __m256i *)y[i].qs);

        const __m256 q = mul_sum_i8_pairs_float(bx, by);

        /* Multiply q with scale and accumulate */
        acc = _mm256_fmadd_ps(d, q, acc);
    }

    *s = hsum_float_8(acc);
#else
    // scalar
    float sumf = 0.0;
    for (int i = 0; i < nb; i++) {
        const uint8_t * restrict x0 = x[i].qs;
        const  int8_t * restrict y0 = y[i].qs;

        uint32_t qh;
        memcpy(&qh, x[i].qh, sizeof(qh));

        const float d = GGML_FP16_TO_FP32(x[i].d);

        int sxy = 0;

        for (int j = 0; j < V1_QK8_0/2; j++) {
            const uint8_t v0 = x0[j];

            const int x0_0h = ((qh & (1u << (2*j + 0))) >> (2*j + 0)) << 4;
            const int x1_0h = ((qh & (1u << (2*j + 1))) >> (2*j + 1)) << 4;

            const int x0_0 = ((v0 & 0x0F) | x0_0h) - 16;
            const int x1_0 = ((v0 >>   4) | x1_0h) - 16;

            const int y0_0 = y0[2*j + 0];
            const int y1_0 = y0[2*j + 1];

            sxy += x0_0*y0_0 + x1_0*y1_0;
        }

        sumf += (d*sxy)*y[i].d;
    }
    *s = sumf;
#endif
}
