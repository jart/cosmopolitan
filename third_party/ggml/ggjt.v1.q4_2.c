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
#include "third_party/ggml/ggjt.v1.q4_2.h"
#include "libc/assert.h"
#include "libc/macros.internal.h"
#include "libc/str/str.h"
#include "third_party/aarch64/arm_neon.internal.h"
#include "third_party/ggml/fp16.internal.h"
#include "third_party/ggml/ggjt.v1.internal.h"
#include "third_party/ggml/ggjt.v1.q8_0.h"
#include "third_party/intel/immintrin.internal.h"
#include "third_party/libcxx/math.h"

static_assert(sizeof(block_v1_q4_2) == sizeof(ggml_fp16_t) + V1_QK4_2 / 2,
              "wrong q4_2 block size/padding");

// reference implementation for deterministic creation of model files
void quantize_row_v1_q4_2_reference(const float * restrict x, block_v1_q4_2 * restrict y, int k) {
    assert(k % V1_QK4_2 == 0);

    const int nb = k / V1_QK4_2;

    for (int i = 0; i < nb; i++) {
        float amax = 0.0f; // absolute max
        float max = 0.0f;

        for (int l = 0; l < V1_QK4_2; l++) {
            const float v = x[i*V1_QK4_2 + l];
            if (amax < fabsf(v)) {
                amax = fabsf(v);
                max = v;
            }
        }

        const float d = max / -8;

        const float id = d ? 1.0f/d : 0.0f;

        y[i].d = GGML_FP32_TO_FP16(d);

        for (int l = 0; l < V1_QK4_2; l += 2) {
            const float v0 = x[i*V1_QK4_2 + l + 0]*id;
            const float v1 = x[i*V1_QK4_2 + l + 1]*id;

            const uint8_t vi0 = MIN(15, (uint8_t)(v0 + 8.5f));
            const uint8_t vi1 = MIN(15, (uint8_t)(v1 + 8.5f));

            assert(vi0 < 16);
            assert(vi1 < 16);

            y[i].qs[l/2] = vi0 | (vi1 << 4);
        }
    }
}

void quantize_row_v1_q4_2(const float * restrict x, void * restrict vy, int k) {
    assert(k % V1_QK4_2 == 0);

    block_v1_q4_2 * restrict y = vy;

    quantize_row_v1_q4_2_reference(x, y, k);
}

size_t ggml_quantize_v1_q4_2(const float * src, void * dst, int n, int k, int64_t * hist) {
    assert(k % V1_QK4_2 == 0);
    const int nb = k / V1_QK4_2;

    for (int j = 0; j < n; j += k) {
        block_v1_q4_2 * restrict y = (block_v1_q4_2 *)dst + j/V1_QK4_2;

        quantize_row_v1_q4_2_reference(src + j, y, k);

        for (int i = 0; i < nb; i++) {
            for (int l = 0; l < V1_QK4_2; l += 2) {
                const uint8_t vi0 = y[i].qs[l/2] & 0x0F;
                const uint8_t vi1 = y[i].qs[l/2] >> 4;

                hist[vi0]++;
                hist[vi1]++;
            }
        }
    }

    return (n/V1_QK4_2*sizeof(block_v1_q4_2));
}

void dequantize_row_v1_q4_2(const void * restrict vx, float * restrict y, int k) {
    assert(k % V1_QK4_2 == 0);
    const int nb = k / V1_QK4_2;

    const block_v1_q4_2 * restrict x = vx;

    for (int i = 0; i < nb; i++) {
        const float d = GGML_FP16_TO_FP32(x[i].d);

        const uint8_t * restrict pp = x[i].qs;

        for (int l = 0; l < V1_QK4_2; l += 2) {
            const uint8_t vi = pp[l/2];

            const int8_t vi0 = vi & 0x0F;
            const int8_t vi1 = vi >> 4;

            const float v0 = (vi0 - 8)*d;
            const float v1 = (vi1 - 8)*d;

            y[i*V1_QK4_2 + l + 0] = v0;
            y[i*V1_QK4_2 + l + 1] = v1;

            assert(!isnan(y[i*V1_QK4_2 + l + 0]));
            assert(!isnan(y[i*V1_QK4_2 + l + 1]));
        }
    }
}

void ggml_vec_dot_v1_q4_2_q8_0(const int n, float * restrict s, const void * restrict vx, const void * restrict vy) {
    const int nb = n / V1_QK8_0;

    assert(n % V1_QK8_0 == 0);
    assert(nb % 2 == 0);
    assert(V1_QK8_0 == 2*V1_QK4_2);

    const block_v1_q4_2 * restrict x = vx;
    const block_v1_q8_0 * restrict y = vy;

#if defined(__ARM_NEON)
    float32x4_t sumv0 = vdupq_n_f32(0.0f);
    float32x4_t sumv1 = vdupq_n_f32(0.0f);

    for (int i = 0; i < nb; i += 2) {
        const block_v1_q4_2 * restrict x0_0 = &x[2*(i + 0) + 0];
        const block_v1_q4_2 * restrict x0_1 = &x[2*(i + 0) + 1];
        const block_v1_q4_2 * restrict x1_0 = &x[2*(i + 1) + 0];
        const block_v1_q4_2 * restrict x1_1 = &x[2*(i + 1) + 1];

        const block_v1_q8_0 * restrict y0 = &y[i + 0];
        const block_v1_q8_0 * restrict y1 = &y[i + 1];

        const uint8x16_t m4b   = vdupq_n_u8(0x0F);
        const int8x16_t  s8b   = vdupq_n_s8(0x8);

        const uint8x16_t v0_0 = vcombine_u8(vld1_u8(x0_0->qs), vld1_u8(x0_1->qs));
        const uint8x16_t v0_1 = vcombine_u8(vld1_u8(x1_0->qs), vld1_u8(x1_1->qs));

        // 4-bit -> 8-bit
        const int8x16_t v0_0l = vreinterpretq_s8_u8(vandq_u8  (v0_0, m4b));
        const int8x16_t v0_0h = vreinterpretq_s8_u8(vshrq_n_u8(v0_0, 4));
        const int8x16_t v0_1l = vreinterpretq_s8_u8(vandq_u8  (v0_1, m4b));
        const int8x16_t v0_1h = vreinterpretq_s8_u8(vshrq_n_u8(v0_1, 4));

        // sub 8
        const int8x16_t v0_0ls = vsubq_s8(v0_0l, s8b);
        const int8x16_t v0_0hs = vsubq_s8(v0_0h, s8b);
        const int8x16_t v0_1ls = vsubq_s8(v0_1l, s8b);
        const int8x16_t v0_1hs = vsubq_s8(v0_1h, s8b);

        // interleave
        const int8x16_t v0_0lz = vzip1q_s8(v0_0ls, v0_0hs);
        const int8x16_t v0_0hz = vzip2q_s8(v0_0ls, v0_0hs);
        const int8x16_t v0_1lz = vzip1q_s8(v0_1ls, v0_1hs);
        const int8x16_t v0_1hz = vzip2q_s8(v0_1ls, v0_1hs);

        // load y
        const int8x16_t v1_0l = vld1q_s8(y0->qs);
        const int8x16_t v1_0h = vld1q_s8(y0->qs + 16);
        const int8x16_t v1_1l = vld1q_s8(y1->qs);
        const int8x16_t v1_1h = vld1q_s8(y1->qs + 16);

#if defined(__ARM_FEATURE_DOTPROD)
        sumv0 = vmlaq_n_f32(sumv0, vaddq_f32(
                vmulq_n_f32(vcvtq_f32_s32(vdotq_s32(vdupq_n_s32(0), v0_0lz, v1_0l)), GGML_FP16_TO_FP32(x0_0->d)),
                vmulq_n_f32(vcvtq_f32_s32(vdotq_s32(vdupq_n_s32(0), v0_0hz, v1_0h)), GGML_FP16_TO_FP32(x0_1->d))), y0->d);

        sumv1 = vmlaq_n_f32(sumv1, vaddq_f32(
                vmulq_n_f32(vcvtq_f32_s32(vdotq_s32(vdupq_n_s32(0), v0_1lz, v1_1l)), GGML_FP16_TO_FP32(x1_0->d)),
                vmulq_n_f32(vcvtq_f32_s32(vdotq_s32(vdupq_n_s32(0), v0_1hz, v1_1h)), GGML_FP16_TO_FP32(x1_1->d))), y1->d);
#else
        const int16x8_t pl0l = vmull_s8(vget_low_s8 (v0_0lz), vget_low_s8 (v1_0l));
        const int16x8_t pl0h = vmull_s8(vget_high_s8(v0_0lz), vget_high_s8(v1_0l));
        const int16x8_t ph0l = vmull_s8(vget_low_s8 (v0_0hz), vget_low_s8 (v1_0h));
        const int16x8_t ph0h = vmull_s8(vget_high_s8(v0_0hz), vget_high_s8(v1_0h));

        const int16x8_t pl1l = vmull_s8(vget_low_s8 (v0_1lz), vget_low_s8 (v1_1l));
        const int16x8_t pl1h = vmull_s8(vget_high_s8(v0_1lz), vget_high_s8(v1_1l));
        const int16x8_t ph1l = vmull_s8(vget_low_s8 (v0_1hz), vget_low_s8 (v1_1h));
        const int16x8_t ph1h = vmull_s8(vget_high_s8(v0_1hz), vget_high_s8(v1_1h));

        const int32x4_t pl0 = vaddq_s32(vpaddlq_s16(pl0l), vpaddlq_s16(pl0h));
        const int32x4_t ph0 = vaddq_s32(vpaddlq_s16(ph0l), vpaddlq_s16(ph0h));
        const int32x4_t pl1 = vaddq_s32(vpaddlq_s16(pl1l), vpaddlq_s16(pl1h));
        const int32x4_t ph1 = vaddq_s32(vpaddlq_s16(ph1l), vpaddlq_s16(ph1h));

        sumv0 = vmlaq_n_f32(sumv0, vaddq_f32(
                vmulq_n_f32(vcvtq_f32_s32(pl0), GGML_FP16_TO_FP32(x0_0->d)),
                vmulq_n_f32(vcvtq_f32_s32(ph0), GGML_FP16_TO_FP32(x0_1->d))), y0->d);

        sumv1 = vmlaq_n_f32(sumv1, vaddq_f32(
                vmulq_n_f32(vcvtq_f32_s32(pl1), GGML_FP16_TO_FP32(x1_0->d)),
                vmulq_n_f32(vcvtq_f32_s32(ph1), GGML_FP16_TO_FP32(x1_1->d))), y1->d);
#endif
    }

    *s = vaddvq_f32(sumv0) + vaddvq_f32(sumv1);
#elif defined(__AVX2__)
    // Initialize accumulator with zeros
    __m256 acc = _mm256_setzero_ps();

    // Main loop
    for (int i = 0; i < nb; i++) {
        /* Compute combined scale for the block */
        const __m128 d0 = _mm_set1_ps(GGML_FP16_TO_FP32(x[2*i + 0].d));
        const __m128 d1 = _mm_set1_ps(GGML_FP16_TO_FP32(x[2*i + 1].d));
        const __m256 d = _mm256_mul_ps(_mm256_set_m128(d1, d0), _mm256_broadcast_ss(&y[i].d));

        __m128i bx0 = bytes_from_nibbles_16(x[2*i + 0].qs);
        __m128i bx1 = bytes_from_nibbles_16(x[2*i + 1].qs);
        __m256i bx = _mm256_set_m128i(bx1, bx0);

        // Now we have a vector with bytes in [ 0 .. 15 ] interval. Offset them into [ -8 .. +7 ] interval.
        const __m256i off = _mm256_set1_epi8(8);
        bx = _mm256_sub_epi8(bx, off);

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
        const uint8_t * restrict x0 = x[2*i + 0].qs;
        const uint8_t * restrict x1 = x[2*i + 1].qs;
        const  int8_t * restrict y0 = y[i].qs;

        const float d0 = GGML_FP16_TO_FP32(x[2*i + 0].d);
        const float d1 = GGML_FP16_TO_FP32(x[2*i + 1].d);

        int sumi_0 = 0;
        int sumi_1 = 0;

        for (int j = 0; j < V1_QK8_0/4; j++) {
            const uint8_t v0 = x0[j];
            const uint8_t v1 = x1[j];

            const int i0_0 = (int8_t) (v0 & 0x0F) - 8;
            const int i1_0 = (int8_t) (v0 >>   4) - 8;

            const int i0_1 = (int8_t) (v1 & 0x0F) - 8;
            const int i1_1 = (int8_t) (v1 >>   4) - 8;

            const int i2_0 = y0[2*j + 0];
            const int i3_0 = y0[2*j + 1];

            const int i2_1 = y0[2*(j + V1_QK8_0/4) + 0];
            const int i3_1 = y0[2*(j + V1_QK8_0/4) + 1];

            sumi_0 += i0_0*i2_0 + i1_0*i3_0;
            sumi_1 += i0_1*i2_1 + i1_1*i3_1;
        }

        sumf += (d0 * y[i].d) * sumi_0;
        sumf += (d1 * y[i].d) * sumi_1;
    }
    *s = sumf;
#endif
}
