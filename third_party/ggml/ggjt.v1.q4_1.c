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
#include "third_party/ggml/ggjt.v1.q4_1.h"
#include "libc/assert.h"
#include "libc/macros.internal.h"
#include "libc/str/str.h"
#include "third_party/aarch64/arm_neon.internal.h"
#include "third_party/ggml/ggjt.v1.internal.h"
#include "third_party/ggml/ggjt.v1.q4_1.h"
#include "third_party/ggml/ggjt.v1.q8_1.h"
#include "third_party/intel/immintrin.internal.h"
#include "third_party/libcxx/math.h"

// quantization for the ggjt.v1.q4_1 file format

static_assert(sizeof(block_v1_q4_1) == 2 * sizeof(float) + V1_QK4_1 / 2,
              "wrong q4_1 block size/padding");
static_assert(sizeof(block_v1_q8_1) == 3*sizeof(float) + V1_QK8_1,
              "wrong q8_1 block size/padding");

void quantize_row_v1_q4_1_reference(const float * restrict x, void * restrict vy, int k) {
    assert(k % V1_QK4_1 == 0);
    const int nb = k / V1_QK4_1;

    block_v1_q4_1 * restrict y = vy;

    uint8_t pp[V1_QK4_1/2];

    for (int i = 0; i < nb; i++) {
        float min = FLT_MAX;
        float max = -FLT_MAX;

        for (int l = 0; l < V1_QK4_1; l++) {
            const float v = x[i*V1_QK4_1 + l];
            if (v < min) min = v;
            if (v > max) max = v;
        }

        const float d = (max - min) / ((1 << 4) - 1);
        const float id = d ? 1.0f/d : 0.0f;

        y[i].d = d;
        y[i].m = min;

        for (int l = 0; l < V1_QK4_1; l += 2) {
            const float v0 = (x[i*V1_QK4_1 + l + 0] - min)*id;
            const float v1 = (x[i*V1_QK4_1 + l + 1] - min)*id;

            const uint8_t vi0 = roundf(v0);
            const uint8_t vi1 = roundf(v1);

            assert(vi0 < 16);
            assert(vi1 < 16);

            pp[l/2] = vi0 | (vi1 << 4);
        }

        memcpy(y[i].qs, pp, sizeof(pp));
    }
}

void quantize_row_v1_q4_1(const float * restrict x, void * restrict vy, int k) {
    assert(k % V1_QK4_1 == 0);

    const int nb = k / V1_QK4_1;

    block_v1_q4_1 * restrict y = vy;

#if defined(__AVX2__)
    for (int i = 0; i < nb; i++) {
        // Load elements into 4 AVX vectors
        __m256 v0 = _mm256_loadu_ps( x );
        __m256 v1 = _mm256_loadu_ps( x + 8 );
        __m256 v2 = _mm256_loadu_ps( x + 16 );
        __m256 v3 = _mm256_loadu_ps( x + 24 );
        x += 32;

        // Compute max for the block
        __m256 vmax;
        vmax = _mm256_max_ps( v0, v1 );
        vmax = _mm256_max_ps( vmax, v2 );
        vmax = _mm256_max_ps( vmax, v3 );

        __m128 max4 = _mm_max_ps( _mm256_extractf128_ps( vmax, 1 ), _mm256_castps256_ps128( vmax ) );
        max4 = _mm_max_ps( max4, _mm_movehl_ps( max4, max4 ) );
        max4 = _mm_max_ss( max4, _mm_movehdup_ps( max4 ) );
        const float maxScalar = _mm_cvtss_f32( max4 );

        // Compute min for the block
        __m256 vmin;
        vmin = _mm256_min_ps( v0, v1 );
        vmin = _mm256_min_ps( vmin, v2 );
        vmin = _mm256_min_ps( vmin, v3 );

        __m128 min4 = _mm_min_ps( _mm256_extractf128_ps( vmin, 1 ), _mm256_castps256_ps128( vmin ) );
        min4 = _mm_min_ps( min4, _mm_movehl_ps( min4, min4 ) );
        min4 = _mm_min_ss( min4, _mm_movehdup_ps( min4 ) );
        const float minScalar = _mm_cvtss_f32( min4 );

        // Quantize these floats
        const float d = (maxScalar - minScalar) / ((1 << 4) - 1);
        const float id = d ? 1.0f/d : 0.0f;

        y[i].m = minScalar;
        y[i].d = d;

        // x = (x-min)*id
        const __m256 mul = _mm256_set1_ps( id );
        const __m256 off = _mm256_set1_ps( minScalar );
        v0 = _mm256_mul_ps( _mm256_sub_ps( v0, off ), mul );
        v1 = _mm256_mul_ps( _mm256_sub_ps( v1, off ), mul );
        v2 = _mm256_mul_ps( _mm256_sub_ps( v2, off ), mul );
        v3 = _mm256_mul_ps( _mm256_sub_ps( v3, off ), mul );

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

        // Compress the vector into 4 bit/value, and store
        __m128i res = packNibbles( i0 );
        _mm_storeu_si128( ( __m128i* )y[i].qs, res );
    }
#elif __ARM_NEON
    for (int i = 0; i < nb; i++) {
        float32x4_t srcv[8];
        float32x4_t minv[8];
        float32x4_t maxv[8];

        for (int l = 0; l < 8; l++) srcv[l] = vld1q_f32(x + i*V1_QK4_1 + 4*l);

        for (int l = 0; l < 4; l++) minv[2*l] = vminq_f32(srcv[2*l], srcv[2*l + 1]);
        for (int l = 0; l < 2; l++) minv[4*l] = vminq_f32(minv[4*l], minv[4*l + 2]);
        for (int l = 0; l < 1; l++) minv[8*l] = vminq_f32(minv[8*l], minv[8*l + 4]);

        for (int l = 0; l < 4; l++) maxv[2*l] = vmaxq_f32(srcv[2*l], srcv[2*l + 1]);
        for (int l = 0; l < 2; l++) maxv[4*l] = vmaxq_f32(maxv[4*l], maxv[4*l + 2]);
        for (int l = 0; l < 1; l++) maxv[8*l] = vmaxq_f32(maxv[8*l], maxv[8*l + 4]);

        const float min = vminvq_f32(minv[0]);
        const float max = vmaxvq_f32(maxv[0]);

        const float d = (max - min) / ((1 << 4) - 1);
        const float id = d ? 1.0f/d : 0.0f;

        y[i].d = d;
        y[i].m = min;

        const float32x4_t minv0 = vdupq_n_f32(min);

        for (int l = 0; l < 8; l++) {
            const float32x4_t v  = vmulq_n_f32(vsubq_f32(srcv[l], minv0), id);
            const float32x4_t vf = vaddq_f32(v, vdupq_n_f32(0.5f)); // needed to round to nearest
            const int32x4_t   vi = vcvtq_s32_f32(vf);

            y[i].qs[2*l + 0] = vgetq_lane_s32(vi, 0) | (vgetq_lane_s32(vi, 1) << 4);
            y[i].qs[2*l + 1] = vgetq_lane_s32(vi, 2) | (vgetq_lane_s32(vi, 3) << 4);
        }
    }
#else
    // scalar
    quantize_row_v1_q4_1_reference(x, vy, k);
#endif
}

size_t ggml_quantize_v1_q4_1(const float * src, void * dst, int n, int k, int64_t * hist) {
    assert(k % V1_QK4_1 == 0);
    const int nb = k / V1_QK4_1;

    for (int j = 0; j < n; j += k) {
        block_v1_q4_1 * restrict y = (block_v1_q4_1 *)dst + j/V1_QK4_1;

        quantize_row_v1_q4_1_reference(src + j, y, k);

        for (int i = 0; i < nb; i++) {
            for (int l = 0; l < V1_QK4_1; l += 2) {
                const uint8_t vi0 = y[i].qs[l/2] & 0x0F;
                const uint8_t vi1 = y[i].qs[l/2] >> 4;

                hist[vi0]++;
                hist[vi1]++;
            }
        }
    }

    return (n/V1_QK4_1*sizeof(block_v1_q4_1));
}

void dequantize_row_v1_q4_1(const void * restrict vx, float * restrict y, int k) {
    assert(k % V1_QK4_1 == 0);
    const int nb = k / V1_QK4_1;

    const block_v1_q4_1 * restrict x = vx;

#if defined(__AVX2__)
    for (int i = 0; i < nb; i++) {
        const __m256 d_v = _mm256_broadcast_ss(&x[i].d);
        const __m256 d_m = _mm256_broadcast_ss(&x[i].m);

        const uint8_t * restrict pp = x[i].qs;

        for (int l = 0; l < V1_QK4_1; l += 32) {
            // Load 32x4-bit integers into 32x8-bit integers
            __m256i vx8 = bytes_from_nibbles_32(pp+l/2);

            // Convert to 16-bit int
            const __m256i vx16_lo = _mm256_cvtepi8_epi16(_mm256_extracti128_si256(vx8, 0));
            const __m256i vx16_hi = _mm256_cvtepi8_epi16(_mm256_extracti128_si256(vx8, 1));

            // Convert to 32-bit int -> float 32
            const __m256 vf[4] = {
                _mm256_cvtepi32_ps(_mm256_cvtepi16_epi32(_mm256_extracti128_si256(vx16_lo, 0))),
                _mm256_cvtepi32_ps(_mm256_cvtepi16_epi32(_mm256_extracti128_si256(vx16_lo, 1))),
                _mm256_cvtepi32_ps(_mm256_cvtepi16_epi32(_mm256_extracti128_si256(vx16_hi, 0))),
                _mm256_cvtepi32_ps(_mm256_cvtepi16_epi32(_mm256_extracti128_si256(vx16_hi, 1)))
            };

            // Scale, add m and store
            for (int j = 0; j < 4; j++) {
                const __m256 result = _mm256_add_ps(_mm256_mul_ps(vf[j], d_v), d_m);
                _mm256_storeu_ps(y + i * V1_QK4_1 + l + j*8, result);
            }
        }
    }
#elif defined(__ARM_NEON)
    for (int i = 0; i < nb; i++) {
        const float32x4_t vd = vdupq_n_f32(x[i].d);
        const float32x4_t vm = vdupq_n_f32(x[i].m);

        const uint8_t * restrict pp = x[i].qs;

        for (int l = 0; l < V1_QK4_1; l += 16) {
            // Load 16x4-bit integers into 8x8-bit integers
            const uint8x8_t v8 = vld1_u8(pp + l/2);

            // Expand 4-bit qs to 8-bit bytes
            const uint8x8_t v0 = vand_u8(v8, vdup_n_u8(0x0F));
            const uint8x8_t v1 = vshr_n_u8(v8, 4);

            // Interleave and combine
            const uint8x8_t vx_0 = vzip1_u8(v0, v1);
            const uint8x8_t vx_1 = vzip2_u8(v0, v1);

            const uint8x16_t vq = vcombine_u8(vx_0, vx_1);

            // convert to 2x uint16x8_t
            const uint16x8_t vi_0 = vmovl_u8(vget_low_u8 (vq));
            const uint16x8_t vi_1 = vmovl_u8(vget_high_u8(vq));

            // convert to 4x float32x4_t
            const float32x4_t vf_0 = vcvtq_f32_u32(vmovl_u16(vget_low_u16 (vi_0)));
            const float32x4_t vf_1 = vcvtq_f32_u32(vmovl_u16(vget_high_u16(vi_0)));
            const float32x4_t vf_2 = vcvtq_f32_u32(vmovl_u16(vget_low_u16 (vi_1)));
            const float32x4_t vf_3 = vcvtq_f32_u32(vmovl_u16(vget_high_u16(vi_1)));

            // multiply by d and add m
            const float32x4_t r0 = vmlaq_f32(vm, vf_0, vd);
            const float32x4_t r1 = vmlaq_f32(vm, vf_1, vd);
            const float32x4_t r2 = vmlaq_f32(vm, vf_2, vd);
            const float32x4_t r3 = vmlaq_f32(vm, vf_3, vd);

            // Store
            vst1q_f32(y + i*V1_QK4_1 + l +  0, r0);
            vst1q_f32(y + i*V1_QK4_1 + l +  4, r1);
            vst1q_f32(y + i*V1_QK4_1 + l +  8, r2);
            vst1q_f32(y + i*V1_QK4_1 + l + 12, r3);
        }
    }
#else
    for (int i = 0; i < nb; i++) {
        const float d = x[i].d;
        const float m = x[i].m;

        const uint8_t * restrict pp = x[i].qs;

        for (int l = 0; l < V1_QK4_1; l += 2) {
            const uint8_t vi = pp[l/2];

            const int8_t vi0 = vi & 0x0F;
            const int8_t vi1 = vi >> 4;

            const float v0 = vi0*d + m;
            const float v1 = vi1*d + m;

            y[i*V1_QK4_1 + l + 0] = v0;
            y[i*V1_QK4_1 + l + 1] = v1;

            assert(!isnan(y[i*V1_QK4_1 + l + 0]));
            assert(!isnan(y[i*V1_QK4_1 + l + 1]));
        }
    }
#endif
}

void ggml_vec_dot_v1_q4_1_q8_1(const int n, float * restrict s, const void * restrict vx, const void * restrict vy) {
    const int nb = n / V1_QK8_1;

    assert(n % V1_QK8_1 == 0);
    assert(nb % 2 == 0);

    const block_v1_q4_1 * restrict x = vx;
    const block_v1_q8_1 * restrict y = vy;

    // TODO: add AVX / WASM SIMD / etc
#if defined(__ARM_NEON)
    float32x4_t sumv0 = vdupq_n_f32(0.0f);
    float32x4_t sumv1 = vdupq_n_f32(0.0f);

    float summs = 0;

    for (int i = 0; i < nb; i += 2) {
        const block_v1_q4_1 * restrict x0 = &x[i + 0];
        const block_v1_q4_1 * restrict x1 = &x[i + 1];
        const block_v1_q8_1 * restrict y0 = &y[i + 0];
        const block_v1_q8_1 * restrict y1 = &y[i + 1];

        summs += x0->m * (y0->s0 + y0->s1) + x1->m * (y1->s0 + y1->s1);

        const uint8x16_t m4b = vdupq_n_u8(0x0F);

        const uint8x16_t v0_0 = vld1q_u8(x0->qs);
        const uint8x16_t v0_1 = vld1q_u8(x1->qs);

        // 4-bit -> 8-bit
        const int8x16_t v0_0l = vreinterpretq_s8_u8(vandq_u8  (v0_0, m4b));
        const int8x16_t v0_0h = vreinterpretq_s8_u8(vshrq_n_u8(v0_0, 4));
        const int8x16_t v0_1l = vreinterpretq_s8_u8(vandq_u8  (v0_1, m4b));
        const int8x16_t v0_1h = vreinterpretq_s8_u8(vshrq_n_u8(v0_1, 4));

        // interleave
        const int8x16_t v0_0lz = vzip1q_s8(v0_0l, v0_0h);
        const int8x16_t v0_0hz = vzip2q_s8(v0_0l, v0_0h);
        const int8x16_t v0_1lz = vzip1q_s8(v0_1l, v0_1h);
        const int8x16_t v0_1hz = vzip2q_s8(v0_1l, v0_1h);

        // load y
        const int8x16_t v1_0l = vld1q_s8(y0->qs);
        const int8x16_t v1_0h = vld1q_s8(y0->qs + 16);
        const int8x16_t v1_1l = vld1q_s8(y1->qs);
        const int8x16_t v1_1h = vld1q_s8(y1->qs + 16);

#if defined(__ARM_FEATURE_DOTPROD)
        // dot product into int32x4_t
        const int32x4_t p_0 = vdotq_s32(vdotq_s32(vdupq_n_s32(0), v0_0lz, v1_0l), v0_0hz, v1_0h);
        const int32x4_t p_1 = vdotq_s32(vdotq_s32(vdupq_n_s32(0), v0_1lz, v1_1l), v0_1hz, v1_1h);

        sumv0 = vmlaq_n_f32(sumv0, vcvtq_f32_s32(p_0), x0->d*y0->d);
        sumv1 = vmlaq_n_f32(sumv1, vcvtq_f32_s32(p_1), x1->d*y1->d);
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

        sumv0 = vmlaq_n_f32(sumv0, vcvtq_f32_s32(vaddq_s32(pl0, ph0)), x0->d*y0->d);
        sumv1 = vmlaq_n_f32(sumv1, vcvtq_f32_s32(vaddq_s32(pl1, ph1)), x1->d*y1->d);
#endif
    }

    *s = vaddvq_f32(sumv0) + vaddvq_f32(sumv1) + summs;
#elif defined(__AVX2__)
    // Initialize accumulator with zeros
    __m256 acc = _mm256_setzero_ps();

    float summs = 0;

    // Main loop
    for (int i = 0; i < nb; ++i) {
        const float * d0 = &x[i].d;
        const float * d1 = &y[i].d;

        summs += x[i].m * (y[i].s0 + y[i].s1);

        const __m256 d0v = _mm256_broadcast_ss( d0 );
        const __m256 d1v = _mm256_broadcast_ss( d1 );

        // Compute combined scales
        const __m256 d0d1 = _mm256_mul_ps( d0v, d1v );

        // Load 16 bytes, and unpack 4 bit fields into bytes, making 32 bytes
        const __m256i bx = bytes_from_nibbles_32(x[i].qs);
        const __m256i by = _mm256_loadu_si256( (const __m256i *)y[i].qs );

        const __m256 xy = mul_sum_i8_pairs_float(bx, by);

        // Accumulate d0*d1*x*y
        acc = _mm256_fmadd_ps( d0d1, xy, acc );
    }

    *s = hsum_float_8(acc) + summs;
#else
    // scalar
    float sumf = 0.0;
    for (int i = 0; i < nb; i++) {
        const float d0 = x[i].d;
        const float m0 = x[i].m;
        const float d1 = y[i].d;

        const uint8_t * restrict p0 = x[i].qs;
        const  int8_t * restrict p1 = y[i].qs;

        // TODO: this is very slow ..
        for (int j = 0; j < V1_QK8_1/2; j++) {
            const uint8_t v0 = p0[j];

            const float f0 = d0*(v0 & 0x0F) + m0;
            const float f1 = d0*(v0 >>   4) + m0;

            const float f2 = d1*p1[2*j + 0];
            const float f3 = d1*p1[2*j + 1];

            sumf += f0*f2 + f1*f3;
        }
    }
    *s = sumf;
#endif
}
