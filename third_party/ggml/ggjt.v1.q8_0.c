/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2023 Justine Alexandra Roberts Tunney                              │
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
#include "third_party/ggml/ggjt.v1.q8_0.h"
#include "libc/assert.h"
#include "libc/macros.internal.h"
#include "third_party/aarch64/arm_neon.internal.h"
#include "third_party/ggml/ggjt.v1.internal.h"
#include "third_party/ggml/ggjt.v1.q8_0.h"
#include "third_party/intel/immintrin.internal.h"
#include "third_party/libcxx/math.h"

static_assert(sizeof(block_v1_q8_0) == sizeof(float) + V1_QK8_0,
              "wrong q8_0 block size/padding");

// reference implementation for deterministic creation of model files
void quantize_row_v1_q8_0_reference(const float * restrict x, block_v1_q8_0 * restrict y, int k) {
    assert(k % V1_QK8_0 == 0);
    const int nb = k / V1_QK8_0;

    for (int i = 0; i < nb; i++) {
        float amax = 0.0f; // absolute max

        for (int l = 0; l < V1_QK8_0; l++) {
            const float v = x[i*V1_QK8_0 + l];
            amax = MAX(amax, fabsf(v));
        }

        const float d = amax / ((1 << 7) - 1);
        const float id = d ? 1.0f/d : 0.0f;

        y[i].d = d;

        for (int l = 0; l < V1_QK8_0; ++l) {
            const float v0 = x[i*V1_QK8_0 + l]*id;

            y[i].qs[l] = roundf(v0);
        }
    }
}

void quantize_row_v1_q8_0(const float * restrict x, void * restrict vy, int k) {
    assert(V1_QK8_0 == 32);
    assert(k % V1_QK8_0 == 0);
    const int nb = k / V1_QK8_0;

    block_v1_q8_0 * restrict y = vy;

#if defined(__ARM_NEON)
    for (int i = 0; i < nb; i++) {
        float32x4_t srcv [8];
        float32x4_t asrcv[8];
        float32x4_t amaxv[8];

        for (int l = 0; l < 8; l++) srcv[l]  = vld1q_f32(x + i*32 + 4*l);
        for (int l = 0; l < 8; l++) asrcv[l] = vabsq_f32(srcv[l]);

        for (int l = 0; l < 4; l++) amaxv[2*l] = vmaxq_f32(asrcv[2*l], asrcv[2*l+1]);
        for (int l = 0; l < 2; l++) amaxv[4*l] = vmaxq_f32(amaxv[4*l], amaxv[4*l+2]);
        for (int l = 0; l < 1; l++) amaxv[8*l] = vmaxq_f32(amaxv[8*l], amaxv[8*l+4]);

        const float amax = vmaxvq_f32(amaxv[0]);

        const float d = amax / ((1 << 7) - 1);
        const float id = d ? 1.0f/d : 0.0f;

        y[i].d = d;

        for (int l = 0; l < 8; l++) {
            const float32x4_t v  = vmulq_n_f32(srcv[l], id);
            const int32x4_t   vi = vcvtnq_s32_f32(v);

            y[i].qs[4*l + 0] = vgetq_lane_s32(vi, 0);
            y[i].qs[4*l + 1] = vgetq_lane_s32(vi, 1);
            y[i].qs[4*l + 2] = vgetq_lane_s32(vi, 2);
            y[i].qs[4*l + 3] = vgetq_lane_s32(vi, 3);
        }
    }
#elif defined(__AVX2__) || defined(__AVX__)
    for (int i = 0; i < nb; i++) {
        // Load elements into 4 AVX vectors
        __m256 v0 = _mm256_loadu_ps( x );
        __m256 v1 = _mm256_loadu_ps( x + 8 );
        __m256 v2 = _mm256_loadu_ps( x + 16 );
        __m256 v3 = _mm256_loadu_ps( x + 24 );
        x += 32;

        // Compute max(abs(e)) for the block
        const __m256 signBit = _mm256_set1_ps( -0.0f );
        __m256 maxAbs = _mm256_andnot_ps( signBit, v0 );
        maxAbs = _mm256_max_ps( maxAbs, _mm256_andnot_ps( signBit, v1 ) );
        maxAbs = _mm256_max_ps( maxAbs, _mm256_andnot_ps( signBit, v2 ) );
        maxAbs = _mm256_max_ps( maxAbs, _mm256_andnot_ps( signBit, v3 ) );

        __m128 max4 = _mm_max_ps( _mm256_extractf128_ps( maxAbs, 1 ), _mm256_castps256_ps128( maxAbs ) );
        max4 = _mm_max_ps( max4, _mm_movehl_ps( max4, max4 ) );
        max4 = _mm_max_ss( max4, _mm_movehdup_ps( max4 ) );
        const float maxScalar = _mm_cvtss_f32( max4 );

        // Quantize these floats
        const float d = maxScalar / 127.f;
        y[i].d = d;
        const float id = ( maxScalar != 0.0f ) ? 127.f / maxScalar : 0.0f;
        const __m256 mul = _mm256_set1_ps( id );

        // Apply the multiplier
        v0 = _mm256_mul_ps( v0, mul );
        v1 = _mm256_mul_ps( v1, mul );
        v2 = _mm256_mul_ps( v2, mul );
        v3 = _mm256_mul_ps( v3, mul );

        // Round to nearest integer
        v0 = _mm256_round_ps( v0, _MM_ROUND_NEAREST );
        v1 = _mm256_round_ps( v1, _MM_ROUND_NEAREST );
        v2 = _mm256_round_ps( v2, _MM_ROUND_NEAREST );
        v3 = _mm256_round_ps( v3, _MM_ROUND_NEAREST );

        // Convert floats to integers
        __m256i i0 = _mm256_cvtps_epi32( v0 );
        __m256i i1 = _mm256_cvtps_epi32( v1 );
        __m256i i2 = _mm256_cvtps_epi32( v2 );
        __m256i i3 = _mm256_cvtps_epi32( v3 );

#if defined(__AVX2__)
        // Convert int32 to int16
        i0 = _mm256_packs_epi32( i0, i1 );	// 0, 1, 2, 3,  8, 9, 10, 11,  4, 5, 6, 7, 12, 13, 14, 15
        i2 = _mm256_packs_epi32( i2, i3 );	// 16, 17, 18, 19,  24, 25, 26, 27,  20, 21, 22, 23, 28, 29, 30, 31
                                            // Convert int16 to int8
        i0 = _mm256_packs_epi16( i0, i2 );	// 0, 1, 2, 3,  8, 9, 10, 11,  16, 17, 18, 19,  24, 25, 26, 27,  4, 5, 6, 7, 12, 13, 14, 15, 20, 21, 22, 23, 28, 29, 30, 31

        // We got our precious signed bytes, but the order is now wrong
        // These AVX2 pack instructions process 16-byte pieces independently
        // The following instruction is fixing the order
        const __m256i perm = _mm256_setr_epi32( 0, 4, 1, 5, 2, 6, 3, 7 );
        i0 = _mm256_permutevar8x32_epi32( i0, perm );

        _mm256_storeu_si256((__m256i *)y[i].qs, i0);
#else
        // Since we don't have in AVX some necessary functions,
        // we split the registers in half and call AVX2 analogs from SSE
        __m128i ni0 = _mm256_castsi256_si128( i0 );
        __m128i ni1 = _mm256_extractf128_si256( i0, 1);
        __m128i ni2 = _mm256_castsi256_si128( i1 );
        __m128i ni3 = _mm256_extractf128_si256( i1, 1);
        __m128i ni4 = _mm256_castsi256_si128( i2 );
        __m128i ni5 = _mm256_extractf128_si256( i2, 1);
        __m128i ni6 = _mm256_castsi256_si128( i3 );
        __m128i ni7 = _mm256_extractf128_si256( i3, 1);

        // Convert int32 to int16
        ni0 = _mm_packs_epi32( ni0, ni1 );
        ni2 = _mm_packs_epi32( ni2, ni3 );
        ni4 = _mm_packs_epi32( ni4, ni5 );
        ni6 = _mm_packs_epi32( ni6, ni7 );
        // Convert int16 to int8
        ni0 = _mm_packs_epi16( ni0, ni2 );
        ni4 = _mm_packs_epi16( ni4, ni6 );

        _mm_storeu_si128((__m128i *)(y[i].qs +  0), ni0);
        _mm_storeu_si128((__m128i *)(y[i].qs + 16), ni4);
#endif
    }
#else
    // scalar
    quantize_row_v1_q8_0_reference(x, y, k);
#endif
}

size_t ggml_quantize_v1_q8_0(const float * src, void * dst, int n, int k, int64_t * hist) {
    assert(k % V1_QK8_0 == 0);
    const int nb = k / V1_QK8_0;

    for (int j = 0; j < n; j += k) {
        block_v1_q8_0 * restrict y = (block_v1_q8_0 *)dst + j/V1_QK8_0;

        quantize_row_v1_q8_0_reference(src + j, y, k);

        for (int i = 0; i < nb; i++) {
            for (int l = 0; l < V1_QK8_0; ++l) {
                const int8_t vi = y[i].qs[l];

                hist[vi/16 + 8]++;
            }
        }
    }

    return (n/V1_QK8_0*sizeof(block_v1_q8_0));
}

void dequantize_row_v1_q8_0(const void * restrict vx, float * restrict y, int k) {
    assert(k % V1_QK8_0 == 0);
    const int nb = k / V1_QK8_0;

    const block_v1_q8_0 * restrict x = vx;

    for (int i = 0; i < nb; i++) {
        const float d = x[i].d;

        const int8_t * restrict pp = x[i].qs;

        for (int l = 0; l < V1_QK8_0; ++l) {
            y[i*V1_QK8_0 + l] = pp[l]*d;
        }
    }
}

void ggml_vec_dot_v1_q8_0_q8_0(const int n, float * restrict s, const void * restrict vx, const void * restrict vy) {
    const int nb = n / V1_QK8_0;

    assert(n % V1_QK8_0 == 0);
    assert(nb % 2 == 0);
    assert(V1_QK8_0 == V1_QK8_0);

    const block_v1_q8_0 * restrict x = vx;
    const block_v1_q8_0 * restrict y = vy;

#if defined(__ARM_NEON)
    float32x4_t sumv0 = vdupq_n_f32(0.0f);
    float32x4_t sumv1 = vdupq_n_f32(0.0f);

    for (int i = 0; i < nb; i += 2) {
        const block_v1_q8_0 * restrict x0 = &x[i + 0];
        const block_v1_q8_0 * restrict x1 = &x[i + 1];
        const block_v1_q8_0 * restrict y0 = &y[i + 0];
        const block_v1_q8_0 * restrict y1 = &y[i + 1];

        const int8x16_t x0_0 = vld1q_s8(x0->qs);
        const int8x16_t x0_1 = vld1q_s8(x0->qs + 16);
        const int8x16_t x1_0 = vld1q_s8(x1->qs);
        const int8x16_t x1_1 = vld1q_s8(x1->qs + 16);

        // load y
        const int8x16_t y0_0 = vld1q_s8(y0->qs);
        const int8x16_t y0_1 = vld1q_s8(y0->qs + 16);
        const int8x16_t y1_0 = vld1q_s8(y1->qs);
        const int8x16_t y1_1 = vld1q_s8(y1->qs + 16);

#if defined(__ARM_FEATURE_DOTPROD)
        sumv0 = vmlaq_n_f32(sumv0, vcvtq_f32_s32(vaddq_s32(
                        vdotq_s32(vdupq_n_s32(0), x0_0, y0_0),
                        vdotq_s32(vdupq_n_s32(0), x0_1, y0_1))), x0->d*y0->d);

        sumv1 = vmlaq_n_f32(sumv1, vcvtq_f32_s32(vaddq_s32(
                        vdotq_s32(vdupq_n_s32(0), x1_0, y1_0),
                        vdotq_s32(vdupq_n_s32(0), x1_1, y1_1))), x1->d*y1->d);

#else
        const int16x8_t p0_0 = vmull_s8(vget_low_s8 (x0_0), vget_low_s8 (y0_0));
        const int16x8_t p0_1 = vmull_s8(vget_high_s8(x0_0), vget_high_s8(y0_0));
        const int16x8_t p0_2 = vmull_s8(vget_low_s8 (x0_1), vget_low_s8 (y0_1));
        const int16x8_t p0_3 = vmull_s8(vget_high_s8(x0_1), vget_high_s8(y0_1));

        const int16x8_t p1_0 = vmull_s8(vget_low_s8 (x1_0), vget_low_s8 (y1_0));
        const int16x8_t p1_1 = vmull_s8(vget_high_s8(x1_0), vget_high_s8(y1_0));
        const int16x8_t p1_2 = vmull_s8(vget_low_s8 (x1_1), vget_low_s8 (y1_1));
        const int16x8_t p1_3 = vmull_s8(vget_high_s8(x1_1), vget_high_s8(y1_1));

        const int32x4_t p0 = vaddq_s32(vpaddlq_s16(p0_0), vpaddlq_s16(p0_1));
        const int32x4_t p1 = vaddq_s32(vpaddlq_s16(p0_2), vpaddlq_s16(p0_3));
        const int32x4_t p2 = vaddq_s32(vpaddlq_s16(p1_0), vpaddlq_s16(p1_1));
        const int32x4_t p3 = vaddq_s32(vpaddlq_s16(p1_2), vpaddlq_s16(p1_3));

        sumv0 = vmlaq_n_f32(sumv0, vcvtq_f32_s32(vaddq_s32(p0, p1)), x0->d*y0->d);
        sumv1 = vmlaq_n_f32(sumv1, vcvtq_f32_s32(vaddq_s32(p2, p3)), x1->d*y1->d);
#endif
    }

    *s = vaddvq_f32(sumv0) + vaddvq_f32(sumv1);
#elif defined(__AVX2__)
    // Initialize accumulator with zeros
    __m256 acc = _mm256_setzero_ps();

    // Main loop
    for (int i = 0; i < nb; ++i) {
        // Compute combined scale for the block
        const __m256 d = _mm256_mul_ps( _mm256_broadcast_ss( &x[i].d ), _mm256_broadcast_ss( &y[i].d ) );
        __m256i bx = _mm256_loadu_si256((const __m256i *)x[i].qs);
        __m256i by = _mm256_loadu_si256((const __m256i *)y[i].qs);

        const __m256 q = mul_sum_i8_pairs_float(bx, by);

        // Multiply q with scale and accumulate
        acc = _mm256_fmadd_ps( d, q, acc );
    }

    *s = hsum_float_8(acc);
#else
    // scalar
    float sumf = 0.0;

    for (int i = 0; i < nb; i++) {
        const int8_t * restrict x0 = x[i].qs;
        const int8_t * restrict y0 = y[i].qs;

        int sumi = 0;

        for (int j = 0; j < V1_QK8_0; j++) {
            const int v0 = x0[j];
            const int v1 = y0[j];

            sumi += v0*v1;
        }

        sumf += (x[i].d*y[i].d)*sumi;
    }

    *s = sumf;
#endif
}
